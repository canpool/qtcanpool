/**
 * Copyright (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxdock/dockwindow.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockpanel.h"
#include "qxdock/dockcontainer.h"
#include "qxdock/dockmanager.h"
#include "qxdock/dockautohidecontainer.h"
#include "qxdock/dockfloatingcontainer.h"
#include "qxdock/docksidebar.h"
#include "qxdock/docksidetab.h"
#include "qxdock/dockstylesheet.h"
#include "qxdock/docktab.h"

#include <QAbstractButton>
#include <QLabel>
#include <QDebug>
#include <QFile>
#include <QPointer>
#include <QSettings>
#include <QStringList>
#include <QTemporaryDir>
#include <QApplication>
#include <QImage>
#include <QTest>

QX_DOCK_USE_NAMESPACE

namespace
{
/*!
 * The state document with the geometry of the floating windows taken out.
 *
 * A floating container stores what saveGeometry() reports, and what that
 * reports is the platform's answer about a window's frame rather than anything
 * the docking layout decided. Measured in this test run on the offscreen
 * platform: a plain QWidget saved at (2, 2) and restored comes back at
 * (2, 25), with the size untouched - which is Qt's contract, not a defect here,
 * and would make a byte comparison fail on every platform in some other way.
 *
 * What the element holds is still covered: the floating container itself, the
 * widget inside it and the size of the window all appear elsewhere in the
 * document and are compared as they are.
 */
QByteArray withoutFloatingWindowGeometry(const QByteArray &state)
{
    QByteArray document = state.startsWith("<?xml") ? state : qUncompress(state);
    const QByteArray open = QByteArrayLiteral("<Geometry>");
    const QByteArray close = QByteArrayLiteral("</Geometry>");
    int from = document.indexOf(open);
    while (from >= 0) {
        const int to = document.indexOf(close, from);
        if (to < 0) {
            break;
        }
        document.remove(from, to + close.size() - from);
        from = document.indexOf(open);
    }
    return document;
}

/*!
 * The contents of a file, or an empty array if it cannot be read. Used where the
 * test wants to look at what was written rather than at what the library says
 * it wrote.
 */
QByteArray contentsOf(const QString &fileName)
{
    QFile file(fileName);
    return file.open(QIODevice::ReadOnly) ? file.readAll() : QByteArray();
}

/*!
 * The configuration flags are global, and a failed QCOMPARE returns from the
 * test function on the spot - so a test that changes them has to put them back
 * on the way out instead of at the end of its own body, or every test that runs
 * after a failure inherits whatever the failing one had set.
 */
class ConfigFlagsGuard
{
public:
    ConfigFlagsGuard()
        : m_saved(DockManager::configFlags())
    {
    }

    ~ConfigFlagsGuard()
    {
        DockManager::setConfigFlags(m_saved);
    }

    ConfigFlagsGuard(const ConfigFlagsGuard &) = delete;
    ConfigFlagsGuard &operator=(const ConfigFlagsGuard &) = delete;
private:
    DockManager::ConfigFlags m_saved;
};

/*!
 * The application's palette and sheet, put back on the way out - for the same
 * reason ConfigFlagsGuard exists: a failed QVERIFY returns from the function,
 * and a look this test installed would otherwise be the look every test after
 * it measures.
 */
class ApplicationLookGuard
{
public:
    ApplicationLookGuard()
        : m_palette(QGuiApplication::palette())
        , m_sheet(qApp->styleSheet())
    {
    }

    ~ApplicationLookGuard()
    {
        qApp->setStyleSheet(m_sheet);
        QGuiApplication::setPalette(m_palette);
    }

    ApplicationLookGuard(const ApplicationLookGuard &) = delete;
    ApplicationLookGuard &operator=(const ApplicationLookGuard &) = delete;
private:
    QPalette m_palette;
    QString m_sheet;
};

/*!
 * The color covering most of \a area in \a image.
 *
 * The dominant color rather than a sample or an average: a button's icon and
 * its label sit in the middle of it, so a single pixel says more about the
 * glyph than about the surface the state changed, and an average blends the two
 * into a color neither of them has.
 */
QRgb dominantColour(const QImage &image, const QRect &area)
{
    QHash<QRgb, int> counts;
    const QRect clipped = area.intersected(image.rect());
    for (int y = clipped.top(); y <= clipped.bottom(); ++y) {
        for (int x = clipped.left(); x <= clipped.right(); ++x) {
            ++counts[image.pixel(x, y)];
        }
    }
    QRgb best = 0;
    int bestCount = -1;
    for (auto it = counts.constBegin(); it != counts.constEnd(); ++it) {
        if (it.value() > bestCount) {
            bestCount = it.value();
            best = it.key();
        }
    }
    return best;
}

/*!
 * How far apart two colors are, in the channel that moved furthest.
 *
 * The largest channel rather than a distance: the states here are washes of one
 * hue, and a color that shifted in red alone is as visible as one that shifted
 * in all three.
 */
int channelDelta(QRgb a, QRgb b)
{
    const QColor first(a);
    const QColor second(b);
    return qMax(qAbs(first.red() - second.red()),
                qMax(qAbs(first.green() - second.green()), qAbs(first.blue() - second.blue())));
}

/*!
 * What a button paints at rest, in the hover state, and while held down.
 *
 * \a surface is what gets captured and \a target is what the states are put
 * into: a state the sheet paints as transparent has to be captured with
 * whatever is behind it, or there is nothing for the transparency to be
 * against.
 *
 * The state is entered the way a pointer enters it, with QTest::mouseMove, and
 * that is also the one thing that limits this to the offscreen platform: a
 * synthesized move hovers a widget there, and on the Windows platform it leaves
 * the button at rest even with its under-mouse attribute set - so the stylesheet
 * looks at more than the attribute, and a platform with real windows does not
 * hand a test the hover it made. The suites and CI run on offscreen, which is
 * where this is asserted; see the caller for the skip elsewhere.
 *
 * The release lands outside the button on purpose. A release inside it is a
 * click, and these are the dock's close buttons - clicking one closes what the
 * rest of the function is about to measure.
 */
struct StateReading {
    QRgb rest = 0;
    QRgb hover = 0;
    QRgb pressed = 0;
    bool sampled = false;
};

StateReading readStates(QWidget *surface, QWidget *target)
{
    StateReading reading;
    const QRect area(target->mapTo(surface, QPoint(0, 0)), target->size());
    if (!surface->rect().contains(area)) {
        return reading;
    }
    reading.rest = dominantColour(surface->grab().toImage(), area);

    QTest::mouseMove(target, target->rect().center());
    QTest::qWait(20);
    reading.hover = dominantColour(surface->grab().toImage(), area);

    QTest::mousePress(target, Qt::LeftButton, Qt::NoModifier, target->rect().center());
    QTest::qWait(20);
    reading.pressed = dominantColour(surface->grab().toImage(), area);

    // Off the button, so the press is not also a click.
    QTest::mouseRelease(target, Qt::LeftButton, Qt::NoModifier, QPoint(-10, -10));
    QTest::qWait(10);
    reading.sampled = true;
    return reading;
}
}   // namespace

class tst_DockWindow : public QObject
{
    Q_OBJECT
private slots:
    void addDockWidget_other();
    void addDockWidget();
    void addDockWidgetTab();
    void addDockWidgetToContainer();
    void removeDockWidget();
    void addAutoHideDockWidget();
    void addDockWidgetFloating();
    void centralWidget();
    void lockDockWidget();
    void state();
    void stateFile();
    void perspective();
    void perspectiveReturnsToTheSavedBytes();
    void persistenceRoutesAgreeWithEachOther();
    void stateFormatIsOnlyARenderingChoice();
    void framelessHost();
    void carriesNoStyleSheetOfItsOwn();
    void styleSheetIsReadFromTheApplication();
    void hoverAndPressAreVisibleInBothSchemes();
};

/*!
 * The window a DockWindow lives in may be frameless - the main window this
 * release was asked to work with is one. Nothing in the docking machinery asks
 * the top level for a frame, and that is what this holds on to: docking, closing
 * and reopening, floating, and the state round trip all behave the same when the
 * DockWindow is a child of a frameless window.
 *
 * The frame itself is the host's business: moving the window and resizing it by
 * the edges takes a title bar and a hit test that docking has no opinion about,
 * and examples/qxdock/frameless shows the host side of that arrangement.
 */
void tst_DockWindow::framelessHost()
{
    QWidget host;
    host.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    host.resize(800, 600);

    DockWindow *wd = new DockWindow(&host);
    QCOMPARE(wd->window(), &host);
    QCOMPARE(wd->isWindow(), false);

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");
    DockPanel *p1 = wd->addDockWidget(Qx::LeftDockWidgetArea, dw1);
    DockPanel *p2 = wd->addDockWidget(Qx::CenterDockWidgetArea, dw2);

    QCOMPARE(p1->currentDockWidget(), dw1);
    QCOMPARE(p2->currentDockWidget(), dw2);

    // Closing and reopening still works
    dw1->toggleView(false);
    QCOMPARE(dw1->isClosed(), true);
    dw1->toggleView(true);
    QCOMPARE(dw1->isClosed(), false);

    // A dock widget still floats, in a window of its own
    DockFloatingContainer *fc = wd->addDockWidgetFloating(dw2);
    QCOMPARE(fc->topLevelDockWidget(), dw2);
    QCOMPARE(fc->isWindow(), true);

    // And the layout still survives a save and restore. The panels are rebuilt
    // by a restore, so this asks where the dock widgets ended up rather than
    // whether the old panel objects are still around.
    const QByteArray state = wd->saveState();
    QCOMPARE(wd->restoreState(state), true);

    QCOMPARE(wd->dockWidgetsMap().count(), 2);
    QCOMPARE(wd->findDockWidget("dw1"), dw1);
    QVERIFY(dw1->dockPanel() != nullptr);
    QCOMPARE(dw1->dockPanel()->dockContainer(), wd);
    QCOMPARE(dw2->dockFloatingContainer(), fc);
}

/*!
 * The window carries no style sheet of its own, and an application-level one
 * reaches the dock through it.
 *
 * The first half is the change: DockWindow used to read a sheet out of the
 * library's own resources while being constructed, which put the dock's
 * appearance beyond the application's reach - a widget-level sheet outranks the
 * application-level one, so a dark theme the application had set still left a
 * DockWindow the library had already styled. That is the same shape of problem
 * the ribbon window had, and it was fixed the same way (see qxdock::dockStyleSheet
 * for who installs the dock look now).
 *
 * The second half is the part that makes the first worth anything: a sheet set
 * on the application has to arrive at the widgets inside the dock. Asserting
 * only that the window's own sheet is empty would pass just as well with the
 * dock unable to be styled at all, which is not what was asked for.
 *
 * "Arrived" is measured by whether the sheet changed what gets painted, not by
 * what styleSheet() returns: Qt keeps an application-level sheet in the
 * internal QStyleSheetStyle and styleSheet() reports only a widget's own, so a
 * QCOMPARE against the accessor would fail on a dock that is being styled
 * perfectly well. A border wide enough to colour the edge is what tells the
 * two apart.
 */
void tst_DockWindow::carriesNoStyleSheetOfItsOwn()
{
    DockWindow window;

    QVERIFY2(window.styleSheet().isEmpty(), "the dock window still styles itself");

    DockWidget *w = new DockWidget("dw");
    DockPanel *panel = window.addDockWidget(Qx::CenterDockWidgetArea, w);
    QVERIFY(panel != nullptr);
    DockContainer *container = panel->dockContainer();
    QVERIFY(container != nullptr);

    // The dock's own widgets, not the window: the sheet has to have reached
    // something a DockWindow is made of. DockContainer is the outermost of them.
    container->resize(200, 120);
    container->show();
    QVERIFY(QTest::qWaitForWindowExposed(container));

    const QString applicationSheet = QStringLiteral("QFrame { border: 7px solid red; }");
    qApp->setStyleSheet(applicationSheet);

    // Left edge, first seven columns: the sheet asks for a border there and the
    // rest of the widget is not painted that colour by anything else.
    const QImage painted = container->grab().toImage();
    int redEdge = 0;
    for (int y = 0; y < painted.height(); ++y) {
        for (int x = 0; x < 7 && x < painted.width(); ++x) {
            const QColor pixel = painted.pixelColor(x, y);
            if (pixel.red() > 120 && pixel.green() < 90 && pixel.blue() < 90) {
                ++redEdge;
            }
        }
    }
    QVERIFY2(redEdge > 0, "the application style sheet did not reach the dock: a DockWindow that nothing can "
                          "style is not what removing the built-in sheet was supposed to leave behind");

    qApp->setStyleSheet(QString());
}

/*!
 * The dock style sheet is the application's to build in, and the library only
 * reads it - so what this pins is that reading it is possible at all.
 *
 * The test links qxdock and builds res/dockstyle.qrc in, exactly as an application
 * does; see tests/qxdock/CMakeLists.txt. Without that resource the library has
 * nothing to hand out, which is the case this guards against going unnoticed:
 * a DockWindow would simply look unstyled, and nothing anywhere would fail.
 */
void tst_DockWindow::styleSheetIsReadFromTheApplication()
{
    const QString plain = dockStyleSheet(false);

    QVERIFY2(!plain.isEmpty(), "res/dockstyle.qrc is not built in, or the sheet is empty");
    QVERIFY(plain.contains(QStringLiteral("QxDock--DockSplitter::handle")));

    // The focused treatment is no longer a fragment the flag appends, so both
    // values are the same sheet - what the flag does now is decide at match
    // time, through properties the dock sets only when it is on.
    QCOMPARE(dockStyleSheet(true), plain);

    // ...which is only true while every one of those rules asks for a property.
    // A rule that fired on the class alone would dress an application that
    // never asked for focus highlighting in the focused look.
    QVERIFY(plain.contains(QStringLiteral("QxDock--DockTab[focused=\"true\"]")));
    QVERIFY(plain.contains(QStringLiteral("QxDock--DockTitleBar[focusHighlighting=\"true\"]")));
    // The line-anchored form, because "#autoHideDockContainer QxDock--DockTitleBar"
    // ends in the same characters and is a rule of its own.
    QVERIFY2(!plain.contains(QStringLiteral("\nQxDock--DockTitleBar {")),
             "the focused title bar rule is not guarded by a property");

    // Every placeholder in the fragments is resolved: an unresolved one makes
    // Qt drop the declaration without a word, which is the one failure this
    // sheet cannot show by looking at it.
    QVERIFY2(!plain.contains(QStringLiteral("{{")), "a dock sheet placeholder was left unresolved");

    // A field the caller sets reaches the sheet, and one left invalid falls
    // back to the palette rather than becoming an empty declaration.
    DockStyleColors colors;
    colors.accent = QColor(1, 2, 3);
    const QString custom = dockStyleSheet(colors, true);
    QVERIFY(custom.contains(QStringLiteral("#010203")));
    QVERIFY(!custom.contains(QStringLiteral(": ;")));
    QVERIFY(!custom.contains(QStringLiteral(":;")));
    QVERIFY(custom.contains(QStringLiteral("rgba(")));

    // Two values fromPalette() does not take straight from a role: the text on
    // an accent fill, which a style sheet cannot ask for, and the low contrast
    // step, which has to flip direction on a dark window to stay visible.
    QPalette dark;
    dark.setColor(QPalette::Window, QColor(41, 41, 41));
    dark.setColor(QPalette::Light, QColor(64, 64, 64));
    dark.setColor(QPalette::Dark, QColor(16, 16, 16));
    dark.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
    QCOMPARE(DockStyleColors::fromPalette(dark).subtle, QColor(255, 255, 255, 48));
    QCOMPARE(DockStyleColors::fromPalette(dark).onAccent, QColor(255, 255, 255));
    // The separator steps away from the surface, whichever way that is.
    QCOMPARE(DockStyleColors::fromPalette(dark).divider, QColor(64, 64, 64));

    QPalette light;
    light.setColor(QPalette::Window, QColor(245, 245, 245));
    light.setColor(QPalette::Light, QColor(255, 255, 255));
    light.setColor(QPalette::Dark, QColor(208, 208, 208));
    QCOMPARE(DockStyleColors::fromPalette(light).subtle, QColor(0, 0, 0, 48));
    QCOMPARE(DockStyleColors::fromPalette(light).divider, QColor(208, 208, 208));
}

/*!
 * What a hover and a press paint has to be readable on the surface they are
 * painted on - on a light window and on a dark one.
 *
 * This is the check the hover and press washes were missing rather than the
 * washes themselves. They were rgba(0, 0, 0, 16/24/48), a black wash: 9 percent
 * of black over the #202020 of a dark window is three or four levels, which is
 * the same as nothing. They are tokens now - the accent mixed towards the window
 * for hover, the palette's Mid for press - and this holds them to the property
 * that made the old ones wrong in the first place: the difference from rest has
 * to be visible in both schemes.
 *
 * Measured here, the black wash comes out as no change whatsoever: an
 * alpha-only background on a face that is itself transparent does not reach the
 * window this is captured from. A reader sees the same nothing, reached a
 * different way - which is why the old values fail this rather than scrape past
 * the threshold on a light scheme and only lose the dark one.
 *
 * It measures what gets painted, not what the sheet says. Reading the sheet
 * back would pass on a rule Qt never matches, which is how the states went
 * unnoticed: a wash the base style paints instead is still a wash, and the
 * question is whether it can be seen.
 *
 * The pointer is moved with QTest::mouseMove, so the state is reached the way
 * a pointer reaches it rather than by setting a widget attribute. Measured
 * here: offscreen does route it, and grab() renders the state correctly.
 */
void tst_DockWindow::hoverAndPressAreVisibleInBothSchemes()
{
    const ApplicationLookGuard look;

    struct Scheme {
        const char *name;
        QColor window;
        QColor text;
        QColor light;
        QColor mid;
        QColor placeholder;
    };
    // The two surfaces the dock is asked to work on: a white window, and the
    // #202020 a dark theme gives it. Both state an accent - the hover wash is
    // mixed from it towards the window, so a scheme without one has no hover.
    const Scheme schemes[] = {
        {"light", QColor(255, 255, 255), QColor(32, 31, 30), QColor(243, 242, 241), QColor(214, 212, 210),
         QColor(96, 94, 92)},
        {"dark", QColor(32, 32, 32), QColor(243, 242, 241), QColor(43, 43, 43), QColor(72, 72, 72),
         QColor(161, 159, 157)},
    };
    // The wash is a color step, not a shade: 16 levels in one channel is a
    // state the eye picks up without being told where to look. The black wash
    // this replaced is 3 to 4 levels on a dark window by arithmetic, and the
    // washes in place now measured 34 to 64 across the two schemes - so the
    // threshold has room on both sides rather than being set to whatever passes
    // today.
    const int kMinStateDelta = 16;

    // A hover is carried by the pointer, and a test's pointer reaches a widget
    // on the platform that routes it without a window manager. Measured here:
    // offscreen hovers the button and captures it, the Windows platform leaves
    // it at rest however the pointer is moved. Both the test preset and CI run
    // offscreen, so this is skipped rather than failed where it cannot reach.
    if (QGuiApplication::platformName() != QLatin1String("offscreen")) {
        QSKIP("a hover is delivered by the pointer, which only offscreen routes to a test");
    }

    for (const Scheme &scheme : schemes) {
        const QString name = QString::fromLatin1(scheme.name);

        QPalette palette;
        palette.setColor(QPalette::Window, scheme.window);
        palette.setColor(QPalette::WindowText, scheme.text);
        palette.setColor(QPalette::Light, scheme.light);
        palette.setColor(QPalette::Mid, scheme.mid);
        palette.setColor(QPalette::PlaceholderText, scheme.placeholder);
        palette.setColor(QPalette::Highlight, QColor(0, 120, 212));
        palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));

        // A sheet takes its place in the application's look, the way an
        // application installs it; a sheet set on one widget would be styled
        // against that widget's palette and not this one.
        qApp->setPalette(palette);
        qApp->setStyleSheet(dockStyleSheet(DockStyleColors::fromPalette(palette), false));

        enum Target {
            TabClose,
            TitleBarClose
        };
        const struct {
            Target which;
            const char *what;
        } targets[] = {
            {TabClose, "the tab's close button"},
            {TitleBarClose, "the title bar's close button"},
        };

        for (const auto &target : targets) {
            // A window per button, because the pointer is left where the last
            // reading put it: sharing one window, a hover still in force would
            // be the next button's rest, and the reading would report that the
            // title bar had been re-coloured rather than that it was hovered.
            DockWindow window;
            DockWidget *dw = new DockWidget(QStringLiteral("dw"));
            DockPanel *panel = window.addDockWidget(Qx::CenterDockWidgetArea, dw);
            QVERIFY(panel != nullptr);
            window.resize(420, 260);
            window.show();
            QVERIFY(QTest::qWaitForWindowExposed(&window));

            // The two dock buttons a pointer reaches without opening anything.
            // They are the rules the hover and press tokens are spelled in.
            QAbstractButton *button = nullptr;
            if (target.which == TabClose) {
                if (dw->tab()) {
                    button = dw->tab()->findChild<QAbstractButton *>(QStringLiteral("dockTabCloseButton"));
                }
            } else {
                button = panel->titleBarButton(Qx::TitleBarButtonClose);
            }
            QVERIFY2(button != nullptr, target.which == TabClose ? "the tab has no close button to measure"
                                                                 : "the title bar has no close button to measure");
            QVERIFY2(button->isVisible(), "a close button is not on screen, so nothing it paints is");

            const StateReading reading = readStates(&window, button);
            QVERIFY2(reading.sampled, "a close button is not inside the window it is painted on");

            QVERIFY2(channelDelta(reading.rest, reading.hover) >= kMinStateDelta,
                     qPrintable(QStringLiteral("%1 scheme: a hover on %2 is %3 levels off rest (rest #%4, hover "
                                               "#%5) - that is not feedback")
                                    .arg(name, QString::fromLatin1(target.what))
                                    .arg(channelDelta(reading.rest, reading.hover))
                                    .arg(QColor(reading.rest).name(), QColor(reading.hover).name())));
            QVERIFY2(channelDelta(reading.hover, reading.pressed) >= kMinStateDelta,
                     qPrintable(QStringLiteral("%1 scheme: a press on %2 is %3 levels off the hover it follows "
                                               "(hover #%4, pressed #%5) - the press would read as nothing")
                                    .arg(name, QString::fromLatin1(target.what))
                                    .arg(channelDelta(reading.hover, reading.pressed))
                                    .arg(QColor(reading.hover).name(), QColor(reading.pressed).name())));
            QVERIFY2(channelDelta(reading.rest, reading.pressed) >= kMinStateDelta,
                     qPrintable(QStringLiteral("%1 scheme: a press on %2 is %3 levels off rest (rest #%4, "
                                               "pressed #%5)")
                                    .arg(name, QString::fromLatin1(target.what))
                                    .arg(channelDelta(reading.rest, reading.pressed))
                                    .arg(QColor(reading.rest).name(), QColor(reading.pressed).name())));
        }
    }
}

void tst_DockWindow::addDockWidget_other()
{
    DockWindow window;

    DockWidget *w = new DockWidget("Label 1");
    QLabel *l = new QLabel();
    l->setWordWrap(true);
    l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    l->setText("I am happy to join with you today in what will go down in history as the greatest demonstration "
               "for freedom in the history of our nation.");
    w->setWidget(l);
    DockPanel *centerPanel = window.addDockWidget(Qx::CenterDockWidgetArea, w);

    QCOMPARE(centerPanel, w->dockPanel());
    QCOMPARE(centerPanel->dockWidgetsCount(), 1);
    QCOMPARE(centerPanel->openDockWidgetsCount(), 1);
    QCOMPARE(centerPanel->currentDockWidget(), w);
    QCOMPARE(centerPanel->currentDockWidget(), centerPanel->dockWidget(centerPanel->currentIndex()));

    DockContainer *container = centerPanel->dockContainer();
    QCOMPARE(container, w->dockContainer());
    QCOMPARE(container->dockPanelCount(), 1);

    DockPanel *centerPanel_2 = window.addDockWidget(Qx::CenterDockWidgetArea, w);
    QVERIFY(centerPanel_2 != centerPanel);
    QCOMPARE(centerPanel->dockWidgetsCount(), 0);   // w was removed from centerPanel
    QVERIFY(centerPanel != w->dockPanel());
    QCOMPARE(centerPanel->dockContainer(), nullptr);   // centerPanel was removed from it's container
    QCOMPARE(centerPanel_2, w->dockPanel());
    QCOMPARE(centerPanel_2->currentDockWidget(), w);

    // WARNING: centerPanel called deleteLater but not deleted.
    // The centerPanel can no longer be used and may become a wild pointer

    DockWidget *w2 = new DockWidget("empty");
    DockPanel *centerPanel_3 = window.addDockWidget(Qx::CenterDockWidgetArea, w2, centerPanel);
    QVERIFY(centerPanel_3 != centerPanel);
    QCOMPARE(centerPanel->dockContainer(), nullptr);
    QCOMPARE(window.dockWidgetsMap().count(), 2);
    QCOMPARE(window.dockWidgetsMap().value("Label 1"), w);

    DockPanel *topPanel = window.addDockWidget(Qx::TopDockWidgetArea, w2, centerPanel);
    QCOMPARE(centerPanel_3->dockWidgetsCount(), 0);   // w2 was removed from centerPanel_3
    QVERIFY(topPanel != centerPanel);
    QVERIFY(topPanel != centerPanel_3);
    QCOMPARE(topPanel->currentDockWidget(), w2);
}

void tst_DockWindow::addDockWidget()
{
    // add dock widget
    {
        DockWindow wd;

        int i = 0;
        connect(&wd, &DockWindow::dockWidgetAdded, this, [&i](DockWidget *) {
            ++i;
        });

        int j = 0;
        connect(&wd, &DockWindow::dockAreasAdded, this, [&j]() {
            // dock panel added
            ++j;
        });

        DockWidget *dw1 = new DockWidget("dw1");
        DockWidget *dw2 = new DockWidget("dw2");
        DockWidget *dw3 = new DockWidget("dw3");
        DockWidget *dw4 = new DockWidget("dw4");
        DockWidget *dw5 = new DockWidget("dw5");
        DockWidget *dw6 = new DockWidget("dw6");
        DockWidget *dw7 = new DockWidget("dw7");

        DockPanel *p1 = wd.addDockWidget(Qx::NoDockWidgetArea, dw1);   // same as TopDockWidgetArea
        DockPanel *p2 = wd.addDockWidget(Qx::LeftDockWidgetArea, dw2);
        DockPanel *p3 = wd.addDockWidget(Qx::RightDockWidgetArea, dw3);
        DockPanel *p4 = wd.addDockWidget(Qx::TopDockWidgetArea, dw4);
        DockPanel *p5 = wd.addDockWidget(Qx::BottomDockWidgetArea, dw5);
        DockPanel *p6 = wd.addDockWidget(Qx::CenterDockWidgetArea, dw6);
        DockPanel *p7 = wd.addDockWidget(Qx::LeftAutoHideArea, dw7);   // same as TopDockWidgetArea

        QCOMPARE(dw1->dockPanel(), p1);
        QCOMPARE(dw2->dockPanel(), p2);
        QCOMPARE(dw3->dockPanel(), p3);
        QCOMPARE(dw4->dockPanel(), p4);
        QCOMPARE(dw5->dockPanel(), p5);
        QCOMPARE(dw6->dockPanel(), p6);
        QCOMPARE(dw7->dockPanel(), p7);

        QCOMPARE(wd.findDockWidget("dw1"), dw1);
        QCOMPARE(wd.findDockWidget("dw2"), dw2);
        QCOMPARE(wd.findDockWidget("dw3"), dw3);
        QCOMPARE(wd.findDockWidget("dw4"), dw4);
        QCOMPARE(wd.findDockWidget("dw5"), dw5);
        QCOMPARE(wd.findDockWidget("dw6"), dw6);
        QCOMPARE(wd.findDockWidget("dw7"), dw7);

        QCOMPARE(wd.dockContainers().count(), 1);
        QCOMPARE(wd.dockWidgetsMap().count(), 7);
        QCOMPARE(i, 7);
        QCOMPARE(j, 7);
        QCOMPARE(wd.centralWidget(), nullptr);
    }

    // add dock widget to panel
    {
        DockWindow wd;

        DockWidget *dw1 = new DockWidget("dw1");
        DockWidget *dw2 = new DockWidget("dw2");
        DockWidget *dw3 = new DockWidget("dw3");
        DockWidget *dw4 = new DockWidget("dw4");
        DockWidget *dw5 = new DockWidget("dw5");
        DockWidget *dw6 = new DockWidget("dw6");
        DockWidget *dw7 = new DockWidget("dw7");

        DockPanel *p1 = wd.addDockWidget(Qx::NoDockWidgetArea, dw1);
        DockPanel *p2 = wd.addDockWidget(Qx::LeftDockWidgetArea, dw2, p1);
        DockPanel *p3 = wd.addDockWidget(Qx::LeftDockWidgetArea, dw3, p1);
        DockPanel *p4 = wd.addDockWidget(Qx::LeftDockWidgetArea, dw4, p1, 0);
        DockPanel *p5 = wd.addDockWidget(Qx::CenterDockWidgetArea, dw5, p1);
        DockPanel *p6 = wd.addDockWidget(Qx::CenterDockWidgetArea, dw6, p1);
        DockPanel *p7 = wd.addDockWidget(Qx::CenterDockWidgetArea, dw7, p1, 0);

        QVERIFY(p1 != p2);
        QVERIFY(p2 != p3);
        QVERIFY(p2 != p4);
        QVERIFY(p3 != p4);

        QCOMPARE(p1, p5);
        QCOMPARE(p1, p6);
        QCOMPARE(p1, p7);

        QCOMPARE(wd.openedDockPanels().count(), 4);   // p1,p2,p3,p4
        QCOMPARE(p1->dockWidgets().count(), 4);

        QString text = "";
        for (DockWidget *w : p1->dockWidgets()) {
            text += w->objectName() + ",";
        }
        QCOMPARE(text, "dw7,dw1,dw5,dw6,");
    }
}

void tst_DockWindow::addDockWidgetTab()
{
    // add dock widget
    {
        DockWindow wd;

        int j = 0;
        connect(&wd, &DockWindow::dockAreasAdded, this, [&j]() {
            // dock panel added
            ++j;
        });

        DockWidget *dw1 = new DockWidget("dw1");
        DockWidget *dw2 = new DockWidget("dw2");
        DockWidget *dw3 = new DockWidget("dw3");
        DockWidget *dw4 = new DockWidget("dw4");
        DockWidget *dw5 = new DockWidget("dw5");
        DockWidget *dw6 = new DockWidget("dw6");
        DockWidget *dw7 = new DockWidget("dw7");

        // widget same as TopDockWidgetArea, panel same as CenterDockWidgetArea
        DockPanel *p1 = wd.addDockWidgetTab(Qx::NoDockWidgetArea, dw1);
        DockPanel *p2 = wd.addDockWidgetTab(Qx::LeftDockWidgetArea, dw2);
        DockPanel *p3 = wd.addDockWidgetTab(Qx::RightDockWidgetArea, dw3);
        DockPanel *p4 = wd.addDockWidgetTab(Qx::TopDockWidgetArea, dw4);
        DockPanel *p5 = wd.addDockWidgetTab(Qx::BottomDockWidgetArea, dw5);
        DockPanel *p6 = wd.addDockWidgetTab(Qx::CenterDockWidgetArea, dw6);
        // widget same as TopDockWidgetArea, panel same as CenterDockWidgetArea
        DockPanel *p7 = wd.addDockWidgetTab(Qx::LeftAutoHideArea, dw7);

        QCOMPARE(dw1->dockPanel(), p1);
        QCOMPARE(dw2->dockPanel(), p2);
        QCOMPARE(dw3->dockPanel(), p3);
        QCOMPARE(dw4->dockPanel(), p4);
        QCOMPARE(dw5->dockPanel(), p5);
        QCOMPARE(dw6->dockPanel(), p6);
        QCOMPARE(dw7->dockPanel(), p7);

        QCOMPARE(p1, p6);
        QCOMPARE(p6, p7);

        QCOMPARE(wd.findDockWidget("dw1"), dw1);
        QCOMPARE(wd.findDockWidget("dw2"), dw2);
        QCOMPARE(wd.findDockWidget("dw3"), dw3);
        QCOMPARE(wd.findDockWidget("dw4"), dw4);
        QCOMPARE(wd.findDockWidget("dw5"), dw5);
        QCOMPARE(wd.findDockWidget("dw6"), dw6);
        QCOMPARE(wd.findDockWidget("dw7"), dw7);

        QCOMPARE(wd.dockContainers().count(), 1);
        QCOMPARE(wd.dockWidgetsMap().count(), 7);
        QCOMPARE(j, 5);   // p1,p2,p3,p4,p5
        QCOMPARE(wd.centralWidget(), nullptr);
    }

    // add dock widget to panel
    {
        DockWindow wd;

        DockWidget *dw1 = new DockWidget("dw1");
        DockWidget *dw2 = new DockWidget("dw2");
        DockWidget *dw3 = new DockWidget("dw3");
        DockWidget *dw4 = new DockWidget("dw4");

        DockPanel *p1 = wd.addDockWidgetTab(dw1, nullptr);
        DockPanel *p2 = wd.addDockWidgetTab(dw2, p1);
        DockPanel *p3 = wd.addDockWidgetTab(dw3, p1, 0);
        DockPanel *p4 = wd.addDockWidgetTab(dw4, p1, 10);

        QCOMPARE(p1, p2);
        QCOMPARE(p2, p3);
        QCOMPARE(p3, p4);

        QCOMPARE(wd.openedDockPanels().count(), 1);
        QCOMPARE(p1->dockWidgets().count(), 4);

        QString text = "";
        for (DockWidget *w : p1->dockWidgets()) {
            text += w->objectName() + ",";
        }
        QCOMPARE(text, "dw3,dw1,dw2,dw4,");
    }
}

void tst_DockWindow::addDockWidgetToContainer()
{
    DockWindow wd;

    DockContainer *c = new DockContainer(&wd);

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");

    DockPanel *p1 = wd.addDockWidgetToContainer(Qx::CenterDockWidgetArea, dw1, c);
    DockPanel *p2 = wd.addDockWidgetToContainer(Qx::CenterDockWidgetArea, dw2, &wd);

    QVERIFY(p1 != p2);

    QCOMPARE(wd.dockContainers().count(), 2);
    QCOMPARE(wd.dockWidgetsMap().count(), 2);

    QCOMPARE(wd.openedDockWidgets().count(), 1);
    QCOMPARE(wd.openedDockPanels().count(), 1);

    QCOMPARE(c->openedDockWidgets().count(), 1);
    QCOMPARE(c->openedDockPanels().count(), 1);
}

void tst_DockWindow::removeDockWidget()
{
    DockWindow wd;

    int i = 0;
    connect(&wd, &DockWindow::dockWidgetAboutToBeRemoved, this, [&i](DockWidget *) {
        ++i;
    });
    int j = 0;
    connect(&wd, &DockWindow::dockWidgetRemoved, this, [&j](DockWidget *) {
        ++j;
    });

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");
    DockWidget *dw3 = new DockWidget("dw3");

    DockPanel *p1 = wd.addDockWidget(Qx::CenterDockWidgetArea, dw1);
    DockPanel *p2 = wd.addDockWidgetTab(Qx::CenterDockWidgetArea, dw2);
    DockPanel *p3 = wd.addDockWidgetTab(dw3, p2);

    QCOMPARE(p1, p2);
    QCOMPARE(p2, p3);

    QCOMPARE(p1->dockWidgets().count(), 3);

    wd.removeDockWidget(dw1);
    QCOMPARE(i, 1);
    QCOMPARE(j, 1);

    QCOMPARE(p1->dockWidgets().count(), 2);
}

void tst_DockWindow::addAutoHideDockWidget()
{
    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);

    DockWindow wd;

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");
    DockWidget *dw3 = new DockWidget("dw3");
    DockWidget *dw4 = new DockWidget("dw4");

    DockAutoHideContainer *ahc = wd.addAutoHideDockWidget(Qx::DockSideBarNone, dw1);
    QCOMPARE(ahc, nullptr);

    DockAutoHideContainer *ahc1 = wd.addAutoHideDockWidget(Qx::DockSideBarTop, dw1);
    DockAutoHideContainer *ahc2 = wd.addAutoHideDockWidget(Qx::DockSideBarLeft, dw2);
    DockAutoHideContainer *ahc3 = wd.addAutoHideDockWidget(Qx::DockSideBarRight, dw3);
    DockAutoHideContainer *ahc4 = wd.addAutoHideDockWidget(Qx::DockSideBarBottom, dw4);

    QCOMPARE(ahc1->sideBarArea(), Qx::DockSideBarTop);
    QCOMPARE(ahc2->sideBarArea(), Qx::DockSideBarLeft);
    QCOMPARE(ahc3->sideBarArea(), Qx::DockSideBarRight);
    QCOMPARE(ahc4->sideBarArea(), Qx::DockSideBarBottom);

    QCOMPARE(ahc1->dockWidget(), dw1);
    QCOMPARE(ahc2->dockWidget(), dw2);
    QCOMPARE(ahc3->dockWidget(), dw3);
    QCOMPARE(ahc4->dockWidget(), dw4);

    QCOMPARE(wd.dockWidgetsMap().count(), 4);
}

void tst_DockWindow::addDockWidgetFloating()
{
    DockWindow wd;

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");
    DockWidget *dw3 = new DockWidget("dw3");
    DockWidget *dw4 = new DockWidget("dw4");

    DockFloatingContainer *fc1 = wd.addDockWidgetFloating(dw1);
    DockFloatingContainer *fc2 = wd.addDockWidgetFloating(dw2);
    DockFloatingContainer *fc3 = wd.addDockWidgetFloating(dw3);

    QCOMPARE(fc1->dockContainer(), dw1->dockContainer());
    QCOMPARE(fc2->dockContainer(), dw2->dockContainer());
    QCOMPARE(fc3->dockContainer(), dw3->dockContainer());

    QCOMPARE(dw1->dockFloatingContainer(), fc1);
    QCOMPARE(dw2->dockFloatingContainer(), fc2);
    QCOMPARE(dw3->dockFloatingContainer(), fc3);

    QCOMPARE(fc1->hasTopLevelDockWidget(), true);
    QCOMPARE(fc2->hasTopLevelDockWidget(), true);
    QCOMPARE(fc3->hasTopLevelDockWidget(), true);

    QCOMPARE(fc1->topLevelDockWidget(), dw1);
    QCOMPARE(fc2->topLevelDockWidget(), dw2);
    QCOMPARE(fc3->topLevelDockWidget(), dw3);

    QCOMPARE(wd.dockWidgetsMap().count(), 3);

    QCOMPARE(dw1->isFloating(), true);
    QCOMPARE(dw2->isInFloatingContainer(), true);
    QCOMPARE(dw3->isInFloatingContainer(), true);
    QCOMPARE(dw3->isFloating(), true);

    fc3->dockContainer()->addDockWidget(Qx::CenterDockWidgetArea, dw4);
    QCOMPARE(dw3->isInFloatingContainer(), true);
    QCOMPARE(dw3->isFloating(), false);
    QCOMPARE(dw4->isInFloatingContainer(), true);
    QCOMPARE(dw4->isFloating(), false);
    QCOMPARE(fc3->dockWidgets().count(), 2);

    // It is still 3, because dw4 was not added via DockWindow
    QCOMPARE(wd.dockWidgetsMap().count(), 3);

    QVERIFY(dw3->dockContainer() == dw4->dockContainer());
    QVERIFY(dw3->dockPanel() != dw4->dockPanel());
}

void tst_DockWindow::centralWidget()
{
    DockWindow wd;

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");

    QCOMPARE(wd.centralWidget(), nullptr);

    DockPanel *p1 = wd.addDockWidget(Qx::CenterDockWidgetArea, dw1);
    QCOMPARE(dw1->dockPanel(), p1);

    // the central widget need to be the first dock widget that is added to the dock window
    DockPanel *p2 = wd.setCentralWidget(dw2);
    QCOMPARE(wd.centralWidget(), nullptr);
    QCOMPARE(p2, nullptr);

    wd.removeDockWidget(dw1);
    QCOMPARE(dw1->dockPanel(), nullptr);

    p2 = wd.setCentralWidget(dw2);
    QCOMPARE(dw2->dockPanel(), p2);
    QCOMPARE(wd.centralWidget(), dw2);

    // Setting a central widget not possible because there is already a central widget
    DockPanel *p3 = wd.setCentralWidget(dw1);
    QCOMPARE(wd.centralWidget(), dw2);   // central widget is still dw2
    QCOMPARE(p3, nullptr);

    // clear central widget
    wd.setCentralWidget(nullptr);
    wd.removeDockWidget(dw2);

    QCOMPARE(wd.centralWidget(), nullptr);

    wd.setCentralWidget(dw1);
    QCOMPARE(wd.centralWidget(), dw1);
}

void tst_DockWindow::lockDockWidget()
{
    DockWindow wd;

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");
    DockWidget *dw3 = new DockWidget("dw3");
    DockWidget *dw4 = new DockWidget("dw4");

    wd.addDockWidget(Qx::LeftDockWidgetArea, dw1);
    wd.addDockWidget(Qx::CenterDockWidgetArea, dw2);
    wd.addDockWidget(Qx::RightDockWidgetArea, dw3);
    wd.addDockWidget(Qx::TopDockWidgetArea, dw4);

    QCOMPARE(dw1->features() & DockWidget::DockWidgetClosable, DockWidget::DockWidgetClosable);
    QCOMPARE(dw2->features() & DockWidget::DockWidgetMovable, DockWidget::DockWidgetMovable);
    QCOMPARE(dw3->features() & DockWidget::DockWidgetFloatable, DockWidget::DockWidgetFloatable);
    QCOMPARE(dw4->features() & DockWidget::DockWidgetPinnable, DockWidget::DockWidgetPinnable);

    wd.lockDockWidgetFeaturesGlobally();

    QCOMPARE(dw1->features() & DockWidget::DockWidgetClosable, DockWidget::NoDockWidgetFeatures);
    QCOMPARE(dw2->features() & DockWidget::DockWidgetMovable, DockWidget::NoDockWidgetFeatures);
    QCOMPARE(dw3->features() & DockWidget::DockWidgetFloatable, DockWidget::NoDockWidgetFeatures);
    QCOMPARE(dw4->features() & DockWidget::DockWidgetPinnable, DockWidget::NoDockWidgetFeatures);

    wd.lockDockWidgetFeaturesGlobally(DockWidget::NoDockWidgetFeatures);   // unlock

    QCOMPARE(dw1->features() & DockWidget::DockWidgetClosable, DockWidget::DockWidgetClosable);
    QCOMPARE(dw2->features() & DockWidget::DockWidgetMovable, DockWidget::DockWidgetMovable);
    QCOMPARE(dw3->features() & DockWidget::DockWidgetFloatable, DockWidget::DockWidgetFloatable);
    QCOMPARE(dw4->features() & DockWidget::DockWidgetPinnable, DockWidget::DockWidgetPinnable);
}

void tst_DockWindow::state()
{
    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);

    DockWindow wd;

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");
    DockWidget *dw3 = new DockWidget("dw3");
    DockWidget *dw4 = new DockWidget("dw4");

    DockPanel *p1 = wd.addDockWidget(Qx::LeftDockWidgetArea, dw1);
    DockPanel *p2 = wd.addDockWidget(Qx::RightDockWidgetArea, dw2, p1);
    DockAutoHideContainer *ahc1 = wd.addAutoHideDockWidget(Qx::DockSideBarTop, dw3);
    DockFloatingContainer *fc1 = wd.addDockWidgetFloating(dw4);

    QCOMPARE(p1->dockContainer(), p2->dockContainer());
    QCOMPARE(p1->currentDockWidget(), dw1);
    QCOMPARE(p2->currentDockWidget(), dw2);
    QCOMPARE(ahc1->sideBarArea(), Qx::DockSideBarTop);
    QCOMPARE(fc1->topLevelDockWidget(), dw4);

    QCOMPARE(wd.dockWidgetsMap().count(), 4);

    QByteArray state = wd.saveState();
    QCOMPARE(state.startsWith("<?xml"), false);
    QCOMPARE(qUncompress(state).startsWith("<?xml"), true);

    QCOMPARE(wd.restoreState(state), true);

    QCOMPARE(wd.dockWidgetsMap().count(), 4);

    // A restore rebuilds the panels and only schedules the old ones for
    // deletion, so the pointers taken before it must not be used again - they
    // still address a dead panel and would answer with its leftover state. The
    // panels are asked for again from the widgets that sit in them.
    p1 = dw1->dockPanel();
    p2 = dw2->dockPanel();
    QVERIFY(p1);
    QVERIFY(p2);

    QCOMPARE(p1->dockContainer(), p2->dockContainer());
    QCOMPARE(p1->currentDockWidget(), dw1);
    QCOMPARE(p2->currentDockWidget(), dw2);
    QCOMPARE(ahc1->sideBarArea(), Qx::DockSideBarTop);
    QCOMPARE(fc1->topLevelDockWidget(), dw4);
}

void tst_DockWindow::stateFile()
{
    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);

    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString fileName = dir.filePath("dock-state.xml");

    // Empty / unreadable paths must fail cleanly instead of crashing or writing.
    {
        DockWindow wd;
        QCOMPARE(wd.saveStateToFile(QString()), false);
        QCOMPARE(wd.restoreStateFromFile(QString()), false);
        QCOMPARE(wd.restoreStateFromFile(dir.filePath("does-not-exist.xml")), false);
    }

    DockWindow wd;

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");
    DockWidget *dw3 = new DockWidget("dw3");
    DockWidget *dw4 = new DockWidget("dw4");

    DockPanel *p1 = wd.addDockWidget(Qx::LeftDockWidgetArea, dw1);
    DockPanel *p2 = wd.addDockWidget(Qx::RightDockWidgetArea, dw2, p1);
    DockAutoHideContainer *ahc1 = wd.addAutoHideDockWidget(Qx::DockSideBarTop, dw3);
    DockFloatingContainer *fc1 = wd.addDockWidgetFloating(dw4);

    QCOMPARE(wd.dockWidgetsMap().count(), 4);

    QVERIFY(wd.saveStateToFile(fileName));

    // The file must hold plain, human readable XML - never the compressed blob
    // that saveState() returns, so that a saved layout can be inspected by hand.
    QFile file(fileName);
    QVERIFY(file.open(QIODevice::ReadOnly));
    const QByteArray contents = file.readAll();
    file.close();
    QVERIFY(contents.startsWith("<?xml"));
    QVERIFY(contents.contains("<QxDock"));

    QCOMPARE(wd.restoreStateFromFile(fileName), true);
    QCOMPARE(wd.dockWidgetsMap().count(), 4);

    p1 = dw1->dockPanel();
    p2 = dw2->dockPanel();
    QVERIFY(p1);
    QVERIFY(p2);

    QCOMPARE(p1->dockContainer(), p2->dockContainer());
    QCOMPARE(p1->currentDockWidget(), dw1);
    QCOMPARE(p2->currentDockWidget(), dw2);
    QCOMPARE(ahc1->sideBarArea(), Qx::DockSideBarTop);
    QCOMPARE(fc1->topLevelDockWidget(), dw4);

    // A version mismatch must be rejected without touching the current state.
    QCOMPARE(wd.restoreStateFromFile(fileName, 12345), false);
}

void tst_DockWindow::perspective()
{
    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);

    DockWindow wd;

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");
    DockWidget *dw3 = new DockWidget("dw3");
    DockWidget *dw4 = new DockWidget("dw4");

    DockPanel *p1 = wd.addDockWidget(Qx::LeftDockWidgetArea, dw1);
    DockPanel *p2 = wd.addDockWidget(Qx::RightDockWidgetArea, dw2, p1);
    DockAutoHideContainer *ahc1 = wd.addAutoHideDockWidget(Qx::DockSideBarTop, dw3);
    DockFloatingContainer *fc1 = wd.addDockWidgetFloating(dw4);

    QCOMPARE(p1->dockContainer(), p2->dockContainer());
    QCOMPARE(p1->currentDockWidget(), dw1);
    QCOMPARE(p2->currentDockWidget(), dw2);
    QCOMPARE(ahc1->sideBarArea(), Qx::DockSideBarTop);
    QCOMPARE(fc1->topLevelDockWidget(), dw4);

    QCOMPARE(wd.dockWidgetsMap().count(), 4);

    QCOMPARE(dw3->isAutoHide(), true);
    QCOMPARE(dw3->autoHideContainer(), ahc1);
    QCOMPARE(ahc1->autoHideSideBar()->sideBarArea(), Qx::DockSideBarTop);
    QCOMPARE(ahc1->autoHideTab()->sideBarArea(), Qx::DockSideBarTop);

    wd.addPerspective("t1");

    ahc1->moveToNewSideBarArea(Qx::DockSideBarBottom);
    QCOMPARE(ahc1->autoHideSideBar()->sideBarArea(), Qx::DockSideBarBottom);
    QCOMPARE(ahc1->autoHideTab()->sideBarArea(), Qx::DockSideBarBottom);

    wd.addPerspective("t2");

    QCOMPARE(p1->dockContainer(), p2->dockContainer());
    QCOMPARE(p1->currentDockWidget(), dw1);
    QCOMPARE(p2->currentDockWidget(), dw2);
    QCOMPARE(ahc1->sideBarArea(), Qx::DockSideBarBottom);
    QCOMPARE(fc1->topLevelDockWidget(), dw4);

    // switch t1
    wd.openPerspective("t1");

    // Opening a perspective restores a state, which rebuilds the panels - the
    // pointers above belong to the previous arrangement and are taken again
    // below before anything is asked of them.
    p1 = dw1->dockPanel();
    p2 = dw2->dockPanel();
    QVERIFY(p1);
    QVERIFY(p2);

    QCOMPARE(p1->dockContainer(), p2->dockContainer());
    QCOMPARE(p1->currentDockWidget(), dw1);
    QCOMPARE(p2->currentDockWidget(), dw2);
    QCOMPARE(ahc1->sideBarArea(), Qx::DockSideBarTop);
    QCOMPARE(fc1->topLevelDockWidget(), dw4);

    // switch t2
    wd.openPerspective("t2");

    p1 = dw1->dockPanel();
    p2 = dw2->dockPanel();
    QVERIFY(p1);
    QVERIFY(p2);

    QCOMPARE(p1->dockContainer(), p2->dockContainer());
    QCOMPARE(p1->currentDockWidget(), dw1);
    QCOMPARE(p2->currentDockWidget(), dw2);
    QCOMPARE(ahc1->sideBarArea(), Qx::DockSideBarBottom);
    QCOMPARE(fc1->topLevelDockWidget(), dw4);
}

/*!
 * Going back to a perspective must give back the moment it was taken, and the
 * strongest way to say that is to serialize again and compare the documents: a
 * restore that drops the closed widget, forgets which tab was in front or drags
 * an auto hide container to the wrong bar writes a different document, while a
 * hand written list of properties only asks about the ones somebody thought of.
 *
 * The scene differs from perspective() in exactly the axes that a restore is
 * likely to lose: a closed widget, two tabs with an order between them, an auto
 * hide container that has moved, and a floating widget that has to come back
 * with the widget inside it.
 *
 * One value in the document is left out of the comparison for the reason
 * withoutFloatingWindowGeometry() gives: it reports what the platform says
 * about a window rather than what the docking decided. The splitter sizes are
 * the other provisional value, and they are kept - by letting the pending
 * layout run before saving, so that both documents describe the same
 * arrangement instead of one describing an arrangement that has moved on.
 */
void tst_DockWindow::perspectiveReturnsToTheSavedBytes()
{
    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);

    DockWindow wd;
    wd.resize(1000, 700);
    wd.show();

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");
    DockWidget *dw3 = new DockWidget("dw3");
    DockWidget *dw4 = new DockWidget("dw4");
    DockWidget *dw5 = new DockWidget("dw5");

    DockPanel *p1 = wd.addDockWidget(Qx::LeftDockWidgetArea, dw1);
    wd.addDockWidget(Qx::RightDockWidgetArea, dw2, p1);
    // dw5 joins the panel dw1 is in, so the pair has a tab order to keep.
    wd.addDockWidgetTab(Qx::LeftDockWidgetArea, dw5);
    DockAutoHideContainer *ahc1 = wd.addAutoHideDockWidget(Qx::DockSideBarTop, dw3);
    DockFloatingContainer *fc1 = wd.addDockWidgetFloating(dw4);

    // Two of the axes are states rather than positions: one widget is closed and
    // one panel has a tab in front of the other.
    dw2->toggleView(false);
    QCOMPARE(dw2->isClosed(), true);
    p1->setCurrentDockWidget(dw5);
    QCOMPARE(p1->currentDockWidget(), dw5);

    // The document records the sizes the splitters have, not the sizes they
    // ought to have: with dw2 closed but the layout not yet re-run, it stores
    // the 498/498 the closing left behind, and restoring that document lays out
    // to what the sizes really are (1000/0) - two documents for one state. The
    // pending layout requests are let through first, which is also what a caller
    // saving from a menu gets, since the event loop has run by then.
    QCoreApplication::sendPostedEvents(nullptr, QEvent::LayoutRequest);
    const QByteArray saved = wd.saveState();

    wd.addPerspective("A");

    // Move everything that can move, so that "still the saved state" cannot be
    // true by accident: the bar to the other side, the closed widget back, the
    // other tab in front.
    ahc1->moveToNewSideBarArea(Qx::DockSideBarBottom);
    dw2->toggleView(true);
    QCOMPARE(dw2->isClosed(), false);
    p1->setCurrentDockWidget(dw1);
    QCOMPARE(p1->currentDockWidget(), dw1);

    wd.addPerspective("B");
    QVERIFY(withoutFloatingWindowGeometry(saved) != withoutFloatingWindowGeometry(wd.saveState()));

    // A restore rebuilds every panel, and the ones it replaces are only
    // scheduled for deletion - so a pointer taken before it addresses memory
    // that has not been freed yet, and reading through it answers with a dead
    // panel instead of failing. The deferred deletes are run here to state that
    // as a fact rather than a suspicion, and the panel is then asked for again
    // from a widget that sits in it.
    const QPointer<DockPanel> leftPanelBeforeRestore(p1);
    wd.openPerspective("A");
    QCoreApplication::sendPostedEvents(nullptr, QEvent::DeferredDelete);
    QVERIFY2(leftPanelBeforeRestore.isNull(), "restoring a state must rebuild the panels");
    p1 = dw1->dockPanel();
    QVERIFY(p1);

    // Asked one by one as well as by document: when the bytes do differ, these
    // say which axis came back wrong instead of leaving a diff to be read.
    QCOMPARE(ahc1->sideBarArea(), Qx::DockSideBarTop);
    QCOMPARE(dw2->isClosed(), true);
    QCOMPARE(p1->currentDockWidget(), dw5);
    QCOMPARE(fc1->topLevelDockWidget(), dw4);
    QCOMPARE(withoutFloatingWindowGeometry(wd.saveState()), withoutFloatingWindowGeometry(saved));
}

/*!
 * A layout can be written down four ways - a compressed blob from saveState(), a
 * file from saveStateToFile(), a value an application puts in a QSettings file
 * itself, and a named perspective carried by savePerspectives() - and all four
 * have to give back the same layout. This walks each route separately: disturb
 * the arrangement, restore through that one route, and compare the document
 * that comes out with the one that went in.
 *
 * Every disturbance is first shown to be visible in the document, so that a
 * route which quietly does nothing cannot pass by leaving the layout alone.
 *
 * The QSettings route is taken twice because there are two things to hold on
 * to. One is the encoding: an ini file escapes its values, and a compressed
 * blob is full of the bytes that need escaping, so the round trip has to be
 * byte for byte - and the same has to hold for the plain document, which
 * contains '=' in every attribute. The other is the library's own array API,
 * restored into a second window, which is what an application does on its next
 * start.
 */
void tst_DockWindow::persistenceRoutesAgreeWithEachOther()
{
    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);
    ConfigFlagsGuard flags;
    DockManager::setConfigFlag(DockManager::XmlAutoFormattingEnabled, false);
    DockManager::setConfigFlag(DockManager::XmlCompressionEnabled, true);

    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString stateFile = dir.filePath("dock-state.xml");
    const QString settingsFile = dir.filePath("dock-layout.ini");

    DockWindow wd;
    wd.resize(1000, 700);
    wd.show();

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");
    DockWidget *dw3 = new DockWidget("dw3");
    DockWidget *dw4 = new DockWidget("dw4");
    DockWidget *dw5 = new DockWidget("dw5");

    DockPanel *p1 = wd.addDockWidget(Qx::LeftDockWidgetArea, dw1);
    wd.addDockWidget(Qx::RightDockWidgetArea, dw2, p1);
    wd.addDockWidgetTab(Qx::LeftDockWidgetArea, dw5);
    DockAutoHideContainer *ahc1 = wd.addAutoHideDockWidget(Qx::DockSideBarTop, dw3);
    DockFloatingContainer *fc1 = wd.addDockWidgetFloating(dw4);

    dw2->toggleView(false);
    p1->setCurrentDockWidget(dw5);
    QCoreApplication::sendPostedEvents(nullptr, QEvent::LayoutRequest);

    const QByteArray recorded = withoutFloatingWindowGeometry(wd.saveState());

    // The four written forms of that one state.
    const QByteArray blob = wd.saveState();
    QVERIFY2(!blob.startsWith("<?xml"), "this route is meant to walk the compressed blob");
    QVERIFY(wd.saveStateToFile(stateFile));

    wd.addPerspective("A");

    // Compression off for a moment, to have the plain document as well.
    DockManager::setConfigFlag(DockManager::XmlCompressionEnabled, false);
    const QByteArray plainBlob = wd.saveState();
    QVERIFY2(plainBlob.startsWith("<?xml"), "with compression off saveState() writes the document");
    DockManager::setConfigFlag(DockManager::XmlCompressionEnabled, true);

    {
        QSettings settings(settingsFile, QSettings::IniFormat);
        settings.setValue("Layout", blob);
        settings.setValue("PlainLayout", plainBlob);
        wd.savePerspectives(settings);
        settings.sync();
    }

    // Reading through a fresh QSettings object, the way the next start would.
    const QByteArray blobFromIni = [&settingsFile]() {
        QSettings settings(settingsFile, QSettings::IniFormat);
        return settings.value("Layout").toByteArray();
    }();
    QVERIFY2(blobFromIni == blob, "a compressed state must survive the ini encoding byte for byte");

    const QByteArray plainFromIni = [&settingsFile]() {
        QSettings settings(settingsFile, QSettings::IniFormat);
        return settings.value("PlainLayout").toByteArray();
    }();
    QVERIFY2(plainFromIni == plainBlob, "a plain document must survive the ini escaping as well");

    // Move everything that can move, then read the document back and compare.
    // The panel is asked for again each time: a restore rebuilds the panels and
    // only schedules the old ones for deletion, as perspective() states.
    const auto disturb = [&]() -> bool {
        ahc1->moveToNewSideBarArea(Qx::DockSideBarBottom);
        dw2->toggleView(true);
        DockPanel *panel = dw1->dockPanel();
        if (!panel) {
            return false;
        }
        panel->setCurrentDockWidget(dw1);
        QCoreApplication::sendPostedEvents(nullptr, QEvent::LayoutRequest);
        return true;
    };
    const auto document = [&]() {
        return withoutFloatingWindowGeometry(wd.saveState());
    };

    QVERIFY(disturb());
    QVERIFY2(document() != recorded, "the disturbance must show in the document, or a restore proves nothing");
    QVERIFY(wd.restoreState(blob));
    QCOMPARE(document(), recorded);

    // Asked one by one as well, so that a failing comparison says which axis
    // came back wrong instead of leaving a diff to be read.
    QCOMPARE(ahc1->sideBarArea(), Qx::DockSideBarTop);
    QCOMPARE(dw2->isClosed(), true);
    QCOMPARE(dw1->dockPanel()->currentDockWidget(), dw5);
    QCOMPARE(fc1->topLevelDockWidget(), dw4);

    QVERIFY(disturb());
    QVERIFY2(document() != recorded, "the disturbance must show in the document, or a restore proves nothing");
    QVERIFY(wd.restoreStateFromFile(stateFile));
    QCOMPARE(document(), recorded);

    QVERIFY(disturb());
    QVERIFY2(document() != recorded, "the disturbance must show in the document, or a restore proves nothing");
    QVERIFY(wd.restoreState(blobFromIni));
    QCOMPARE(document(), recorded);

    QVERIFY(disturb());
    QVERIFY2(document() != recorded, "the disturbance must show in the document, or a restore proves nothing");
    QVERIFY(wd.restoreState(plainFromIni));
    QCOMPARE(document(), recorded);

    // The same document, restored into a second window that only knows the
    // perspective by the name it was saved under - the route an application
    // takes when it starts again.
    DockWindow reopened;
    reopened.resize(1000, 700);
    reopened.show();

    DockWidget *r1 = new DockWidget("dw1");
    DockWidget *r2 = new DockWidget("dw2");
    DockWidget *r3 = new DockWidget("dw3");
    DockWidget *r4 = new DockWidget("dw4");
    DockWidget *r5 = new DockWidget("dw5");

    DockPanel *rp = reopened.addDockWidget(Qx::LeftDockWidgetArea, r1);
    reopened.addDockWidget(Qx::RightDockWidgetArea, r2, rp);
    reopened.addDockWidgetTab(Qx::LeftDockWidgetArea, r5);
    reopened.addAutoHideDockWidget(Qx::DockSideBarTop, r3);
    reopened.addDockWidgetFloating(r4);

    {
        QSettings settings(settingsFile, QSettings::IniFormat);
        reopened.loadPerspectives(settings);
    }
    QCOMPARE(reopened.perspectiveNames(), QStringList{QStringLiteral("A")});
    reopened.openPerspective("A");

    // The second window has just been filled for the first time; let its layout
    // run, or the comparison is between two different moments.
    QCoreApplication::sendPostedEvents(nullptr, QEvent::LayoutRequest);
    QCOMPARE(withoutFloatingWindowGeometry(reopened.saveState()), recorded);
}

/*!
 * The format flags decide how a layout is written down, never what it says.
 * Three things follow, and this states each of them against the public API.
 *
 * Automatic formatting adds line breaks and indentation, and a document written
 * either way restores to the same layout. Compression changes saveState() from
 * the document to a qCompress() blob, and it is a writer's choice: both forms
 * are read back with the flag in either position, because restoreState() looks
 * at what the bytes are and not at what the flag currently says. And a file
 * written by saveStateToFile() is always the plain document, because a file on
 * disk is meant to be looked at by a person.
 *
 * The version number is the other half of the same subject: it is not a format
 * detail but a contract, so a document saved as version 7 must be refused by a
 * caller asking for version 8, and refusing has to leave the layout exactly as
 * it was.
 */
void tst_DockWindow::stateFormatIsOnlyARenderingChoice()
{
    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);
    ConfigFlagsGuard flags;

    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    DockWindow wd;
    wd.resize(800, 600);
    wd.show();

    DockWidget *dw1 = new DockWidget("dw1");
    DockWidget *dw2 = new DockWidget("dw2");
    DockPanel *p1 = wd.addDockWidget(Qx::LeftDockWidgetArea, dw1);
    wd.addDockWidget(Qx::RightDockWidgetArea, dw2, p1);
    dw2->toggleView(false);
    QCoreApplication::sendPostedEvents(nullptr, QEvent::LayoutRequest);

    const QString compactFile = dir.filePath("compact.xml");
    const QString prettyFile = dir.filePath("pretty.xml");

    DockManager::setConfigFlag(DockManager::XmlAutoFormattingEnabled, false);
    QVERIFY(wd.saveStateToFile(compactFile));
    DockManager::setConfigFlag(DockManager::XmlAutoFormattingEnabled, true);
    QVERIFY(wd.saveStateToFile(prettyFile));

    const QByteArray compact = contentsOf(compactFile);
    const QByteArray pretty = contentsOf(prettyFile);

    QVERIFY(compact.startsWith("<?xml"));
    QVERIFY(pretty.startsWith("<?xml"));
    QVERIFY2(pretty.count('\n') > compact.count('\n'), "automatic formatting is what puts the line breaks in");

    // Both renderings describe the same layout, so both restore to the same one.
    const QString afterCompactFile = dir.filePath("after-compact.xml");
    const QString afterPrettyFile = dir.filePath("after-pretty.xml");

    QVERIFY(wd.restoreState(compact));
    QVERIFY(wd.saveStateToFile(afterCompactFile));
    const QByteArray afterCompact = contentsOf(afterCompactFile);

    QVERIFY(wd.restoreState(pretty));
    QVERIFY(wd.saveStateToFile(afterPrettyFile));
    const QByteArray afterPretty = contentsOf(afterPrettyFile);

    QCOMPARE(afterPretty, afterCompact);

    // Compression is a writer's choice, not a reader's: each form is read back
    // with the flag in the position it was not written in.
    DockManager::setConfigFlag(DockManager::XmlCompressionEnabled, true);
    const QByteArray compressed = wd.saveState();
    QVERIFY2(!compressed.startsWith("<?xml"), "saveState() compresses when the flag asks it to");
    QVERIFY(qUncompress(compressed).startsWith("<?xml"));

    const QString plainFile = dir.filePath("plain.xml");
    QVERIFY(wd.saveStateToFile(plainFile));
    const QByteArray plain = contentsOf(plainFile);

    QVERIFY2(plain.startsWith("<?xml"), "a file on disk is never the compressed blob");
    QCOMPARE(plain, contentsOf(prettyFile));

    DockManager::setConfigFlag(DockManager::XmlCompressionEnabled, false);
    QVERIFY2(wd.restoreState(compressed), "a compressed document must be readable with compression off");
    DockManager::setConfigFlag(DockManager::XmlCompressionEnabled, true);
    QVERIFY2(wd.restoreState(plain), "a plain document must be readable with compression on");

    // A version mismatch is refused, and refusing does not touch the layout.
    const QString versionedFile = dir.filePath("version-7.xml");
    QVERIFY(wd.saveStateToFile(versionedFile, 7));
    const QByteArray versioned = contentsOf(versionedFile);

    dw2->toggleView(true);
    QCOMPARE(dw2->isClosed(), false);
    QCoreApplication::sendPostedEvents(nullptr, QEvent::LayoutRequest);

    const QString disturbedFile = dir.filePath("disturbed.xml");
    const QString refusedFile = dir.filePath("refused.xml");
    QVERIFY(wd.saveStateToFile(disturbedFile));
    const QByteArray disturbed = contentsOf(disturbedFile);

    QCOMPARE(wd.restoreState(versioned, 8), false);
    QVERIFY(wd.saveStateToFile(refusedFile));
    QCOMPARE(contentsOf(refusedFile), disturbed);

    QVERIFY(wd.restoreState(versioned, 7));
    QCOMPARE(dw2->isClosed(), true);
}

TEST_ADD(tst_DockWindow)

#include "tst_dockwindow.moc"
