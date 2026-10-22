/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "tst_probe.h"

#include "qxcore/qxsettings.h"
#include "qxdock/docksplitter.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockwindow.h"
#include "qxtheme/qxtheme.h"
#include "qxtheme/qxthememanager.h"

#include <QApplication>
#include <QBoxLayout>
#include <QList>
#include <QPalette>
#include <QSplitter>
#include <QTemporaryDir>
#include <QTest>

QX_DOCK_USE_NAMESPACE
QX_THEME_USE_NAMESPACE
QX_CORE_USE_NAMESPACE

namespace
{
const char *const kShort = "chrome-short";
const char *const kTall = "chrome-tall";
const char *const kGrey = "chrome-grey";
const char *const kBlue = "chrome-blue";
const char *const kPaneShort = "pane-short";
const char *const kPaneTall = "pane-tall";

QxThemeInfo makeTheme(const QString &id, const QString &name, const QString &css)
{
    QxThemeInfo info(id, name, false);
    info.styleSheet = css;
    return info;
}

/*! The root splitter is protected, so the test reaches it the way a subclass
 *  would - the same door the layout and sequence scenes use. */
class SplitterWindow : public DockWindow
{
public:
    using DockWindow::DockWindow;

    QSplitter *root() const
    {
        return rootSplitter();
    }
};

/*! The arrangement the screenshot suite compares: a left panel holding most of
 *  the docks, a right panel beside it, and one row of two below both. The two
 *  rows of it are what the root splitter divides, and they are not the same
 *  kind of widget - which is what makes the split depend on the chrome. */
void buildTheComparedArrangement(SplitterWindow &dock)
{
    DockWidget *w = new DockWidget(QStringLiteral("dock1"), &dock);
    w->setWidget(new QWidget(&dock));
    DockPanel *panel = dock.addDockWidget(Qx::LeftDockWidgetArea, w);

    w = new DockWidget(QStringLiteral("dock2"), &dock);
    w->setWidget(new QWidget(&dock));
    dock.addDockWidget(Qx::RightDockWidgetArea, w, panel);

    w = new DockWidget(QStringLiteral("longdock"), &dock);
    w->setWidget(new QWidget(&dock), DockWidget::ForceScrollArea);
    dock.addDockWidget(Qx::RightDockWidgetArea, w, panel);

    w = new DockWidget(QStringLiteral("dock4"), &dock);
    dock.addDockWidget(Qx::CenterDockWidgetArea, w, panel);

    w = new DockWidget(QStringLiteral("dock5"), &dock);
    dock.addDockWidgetTab(Qx::LeftDockWidgetArea, w);

    w = new DockWidget(QStringLiteral("dock3"), &dock);
    panel = dock.addDockWidget(Qx::BottomDockWidgetArea, w);

    w = new DockWidget(QStringLiteral("dock6"), &dock);
    w->setWidget(new QWidget(&dock), DockWidget::ForceNoScrollArea);
    dock.addDockWidgetTab(w, panel);
}

/*! The row the root splitter divides, in a window built while @p themeId was
 *  in force; the window is the caller's to keep alive, and @p dockOut is how
 *  the caller gets at the splitter afterwards. */
QList<int> rowBuiltUnder(const QString &themeId, QWidget *host, SplitterWindow **dockOut = nullptr)
{
    QxThemeManager::instance()->setTheme(themeId);

    host->setLayout(new QVBoxLayout);
    auto *dock = new SplitterWindow(host);
    host->layout()->addWidget(dock);
    host->resize(1280, 800);
    buildTheComparedArrangement(*dock);
    host->show();
    Probe::settle(host);
    if (dockOut) {
        *dockOut = dock;
    }
    return dock->root()->sizes();
}
}   // namespace

/*!
 * A theme switch is the one change a dock cannot answer with a single number:
 * the chrome is painted from a sheet the application builds on every switch,
 * while the panes are holding sizes the user arranged and were measured once.
 *
 * These two scenes say which of those follows the switch and which does not,
 * in-process, where the screenshot suite has to say it with a pixel budget
 * over a whole window. That budget is the only reason the 4.6 defect - the
 * panes keeping sizes measured against a chrome that had since shrunk - was
 * filed as expected rather than fixed: re-dividing the row would also throw
 * away the arrangement, which is a worse thing to do to a user than leaving
 * 17 pixels behind. So the trade is pinned here rather than changed.
 */
class tst_DockTheme : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void theChromeFollowsTheSwitchWhileTheRowKeepsTheSizesItWasGiven();
    void theRowIsDividedWhileTheWindowIsBuiltAndReDividingItIsNotTheAnswer();
    void theChromeIsRepaintedWhenTheThemeChanges();
private:
    void registerThemes();

    QPalette m_palette;
    QString m_sheet;
    QTemporaryDir m_dir;
};

void tst_DockTheme::init()
{
    m_palette = qApp->palette();
    m_sheet = qApp->styleSheet();
    QVERIFY(m_dir.isValid());

    // The manager is a process-wide singleton that reads back the last choice
    // on start, so every case hands it a settings file of its own before it
    // registers anything. What a case leaves behind is what the next one would
    // otherwise inherit.
    QxThemeManager *manager = QxThemeManager::instance();
    manager->setSettings(new QxSettings(m_dir.filePath(QStringLiteral("theme.ini")), QSettings::IniFormat));
    registerThemes();
}

void tst_DockTheme::cleanup()
{
    QxThemeManager *manager = QxThemeManager::instance();
    for (const QString &id : manager->themeIds()) {
        manager->unregisterTheme(id);
    }
    qApp->setPalette(m_palette);
    qApp->setStyleSheet(m_sheet);
}

void tst_DockTheme::registerThemes()
{
    QxThemeManager *manager = QxThemeManager::instance();
    for (const QString &id : manager->themeIds()) {
        manager->unregisterTheme(id);
    }
    // The dock's chrome is a number the sheet states, which is why a theme is
    // the honest way to change it: it is what an application actually hands
    // over on a switch.
    manager->registerTheme(makeTheme(QString::fromLatin1(kShort), QStringLiteral("chrome short"),
                                     QStringLiteral("QFrame { min-height: 24px; background-color: #404040; }")));
    manager->registerTheme(makeTheme(QString::fromLatin1(kTall), QStringLiteral("chrome tall"),
                                     QStringLiteral("QFrame { min-height: 40px; background-color: #404040; }")));
    manager->registerTheme(makeTheme(QString::fromLatin1(kGrey), QStringLiteral("chrome grey"),
                                     QStringLiteral("QFrame { background-color: #404040; }")));
    manager->registerTheme(makeTheme(QString::fromLatin1(kBlue), QStringLiteral("chrome blue"),
                                     QStringLiteral("QFrame { background-color: #305090; }")));
    // What the panes sit under is measured from the title bar, so a sheet on
    // the title bar is what moves a row without any widget being dragged.
    manager->registerTheme(makeTheme(QString::fromLatin1(kPaneShort), QStringLiteral("pane short"),
                                     QStringLiteral("QxDock--DockTitleBar { min-height: 24px; }")));
    manager->registerTheme(makeTheme(QString::fromLatin1(kPaneTall), QStringLiteral("pane tall"),
                                     QStringLiteral("QxDock--DockTitleBar { min-height: 48px; }")));
}

/*! The chrome is measured from the sheet on every switch, and what it takes it
 *  takes out of the pane under it. The row the panes sit in is a different
 *  thing: it was divided up once, and a switch does not divide it again - which
 *  is a choice, not an oversight, and it is pinned here so that a change to it
 *  has to be made on purpose. */
void tst_DockTheme::theChromeFollowsTheSwitchWhileTheRowKeepsTheSizesItWasGiven()
{
    QxThemeManager::instance()->setTheme(QString::fromLatin1(kShort));

    QWidget host;
    host.setLayout(new QVBoxLayout);
    SplitterWindow dock(&host);
    host.layout()->addWidget(&dock);
    host.resize(800, 600);

    DockWidget first(QStringLiteral("first"));
    QWidget content;
    first.setWidget(&content);
    dock.addDockWidget(Qx::LeftDockWidgetArea, &first);
    host.show();
    Probe::settle(&host);

    const int tabBarBefore = Probe::heightOf(Probe::tree(&host), QStringLiteral("DockTabBar"));
    const int contentBefore = content.height();
    const QList<int> sizesBefore = dock.root()->sizes();
    QVERIFY(tabBarBefore > 0);
    QVERIFY(contentBefore > 0);
    QVERIFY(!sizesBefore.isEmpty());

    QxThemeManager::instance()->setTheme(QString::fromLatin1(kTall));
    Probe::settle(&host);

    const int tabBarAfter = Probe::heightOf(Probe::tree(&host), QStringLiteral("DockTabBar"));
    QVERIFY2(tabBarAfter > tabBarBefore,
             qPrintable(QStringLiteral("the tab bar stayed %1 tall across the switch").arg(tabBarBefore)));
    // What the chrome took, the pane under it gave up - that is the part a
    // switch does recompute.
    QCOMPARE(contentBefore - content.height(), tabBarAfter - tabBarBefore);
    // And this is the part it does not: the row keeps the sizes it was given,
    // which is why a switched window and a fresh one are not the same shape.
    QCOMPARE(dock.root()->sizes(), sizesBefore);
}

/*! Why the row is left alone, said in numbers rather than in the pixel budget
 *  over a whole window.
 *
 *  The row is divided while the window is being built, and what it is divided
 *  by is the chrome of the theme in force at that moment - so two windows with
 *  one arrangement in them, one built in a scheme and one built in its
 *  counterpart, do not start from the same split. A switch does not divide the
 *  row again, which is the choice the case above pins.
 *
 *  What this case adds is the third way that was left open when the 17 pixels
 *  were filed as expected: re-dividing the row on a switch. It is not a way.
 *  A row divided again is an even row, and an even row is what neither theme's
 *  own start gives - so re-dividing would move a switched window further from
 *  a fresh one than leaving it alone does, and would still throw away an
 *  arrangement the user had made. Measured before it was written down, so that
 *  nobody has to try it again. */
void tst_DockTheme::theRowIsDividedWhileTheWindowIsBuiltAndReDividingItIsNotTheAnswer()
{
    QWidget shortHost;
    const QList<int> shortSplit = rowBuiltUnder(QString::fromLatin1(kPaneShort), &shortHost);
    QWidget tallHost;
    const QList<int> tallSplit = rowBuiltUnder(QString::fromLatin1(kPaneTall), &tallHost);
    QCOMPARE(shortSplit.size(), tallSplit.size());
    QVERIFY(shortSplit.size() >= 2);
    QVERIFY2(shortSplit != tallSplit,
             qPrintable(QStringLiteral("a window built in either scheme started the row the same way (%1) - "
                                       "the chrome that decides the split was not measured")
                            .arg(shortSplit.value(1))));

    // Built in one and switched to the other: the split is the one the window
    // was built with, and no change of size divides the row again either.
    QWidget host;
    SplitterWindow *dock = nullptr;
    const QList<int> builtInShort = rowBuiltUnder(QString::fromLatin1(kPaneShort), &host, &dock);
    QCOMPARE(builtInShort, shortSplit);
    QVERIFY(dock);
    QxThemeManager::instance()->setTheme(QString::fromLatin1(kPaneTall));
    Probe::settle(&host);
    QCOMPARE(dock->root()->sizes(), shortSplit);
    host.resize(1280, 700);
    Probe::settle(&host);
    host.resize(1280, 800);
    Probe::settle(&host);
    QCOMPARE(dock->root()->sizes(), shortSplit);

    // And the way out that was not taken: an even row, which is neither.
    QSplitter *root = dock->root();
    const int usable = root->height() - (shortSplit.size() - 1) * root->handleWidth();
    QList<int> evenSplit;
    for (int i = 0; i < shortSplit.size(); ++i) {
        evenSplit.append(usable / shortSplit.size());
    }
    root->setSizes(evenSplit);
    Probe::settle(&host);
    const QList<int> redone = root->sizes();
    QVERIFY2(redone != shortSplit && redone != tallSplit,
             qPrintable(QStringLiteral("re-dividing the row landed on %1, which is what one of the two "
                                       "schemes starts with - the third way would have worked after all")
                            .arg(redone.value(1))));
}

/*! The other half of what a switch has to do: the chrome that was already up
 *  has to be painted in the new colours, not only measured in the new metrics.
 *  A surface that kept the old one is what the screenshot suite's pixel budget
 *  is really looking for, and this is the same question asked of one widget. */
void tst_DockTheme::theChromeIsRepaintedWhenTheThemeChanges()
{
    QxThemeManager::instance()->setTheme(QString::fromLatin1(kGrey));

    QWidget host;
    host.setLayout(new QVBoxLayout);
    SplitterWindow dock(&host);
    host.layout()->addWidget(&dock);
    host.resize(800, 600);

    DockWidget first(QStringLiteral("first"));
    dock.addDockWidget(Qx::LeftDockWidgetArea, &first);
    host.show();
    Probe::settle(&host);

    // The title bar is the chrome the dock paints from the sheet; a tree is
    // what says where it is, so the widget is found the way a capture would
    // find it rather than by looking the class up in the private headers.
    const QString before = Probe::tree(&host);
    const QString titleBefore = Probe::lines(before, QStringLiteral("DockTitleBar")).value(0);
    QVERIFY2(!titleBefore.isEmpty(), qPrintable(before));

    QxThemeManager::instance()->setTheme(QString::fromLatin1(kBlue));
    Probe::settle(&host);

    const QString after = Probe::tree(&host);
    const QString titleAfter = Probe::lines(after, QStringLiteral("DockTitleBar")).value(0);
    QVERIFY2(!titleAfter.isEmpty(), qPrintable(after));
    // What it painted, and not where it is: a tree line carries both, and a
    // chrome that moved is not a chrome that was repainted.
    const QString inkBefore = titleBefore.section(QLatin1Char('|'), 1).trimmed();
    const QString inkAfter = titleAfter.section(QLatin1Char('|'), 1).trimmed();
    QVERIFY2(inkAfter != inkBefore,
             qPrintable(QStringLiteral("the title bar painted %1 both before and after the switch").arg(inkBefore)));
}

TEST_ADD(tst_DockTheme)

#include "tst_docktheme.moc"
