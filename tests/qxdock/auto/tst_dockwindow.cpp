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

#include <QLabel>
#include <QDebug>
#include <QFile>
#include <QTemporaryDir>
#include <QApplication>
#include <QImage>
#include <QTest>

QX_DOCK_USE_NAMESPACE

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
    void framelessHost();
    void carriesNoStyleSheetOfItsOwn();
    void styleSheetIsReadFromTheApplication();
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

    QCOMPARE(p1->dockContainer(), p2->dockContainer());
    QCOMPARE(p1->currentDockWidget(), dw1);
    QCOMPARE(p2->currentDockWidget(), dw2);
    QCOMPARE(ahc1->sideBarArea(), Qx::DockSideBarTop);
    QCOMPARE(fc1->topLevelDockWidget(), dw4);

    // switch t2
    wd.openPerspective("t2");

    QCOMPARE(p1->dockContainer(), p2->dockContainer());
    QCOMPARE(p1->currentDockWidget(), dw1);
    QCOMPARE(p2->currentDockWidget(), dw2);
    QCOMPARE(ahc1->sideBarArea(), Qx::DockSideBarBottom);
    QCOMPARE(fc1->topLevelDockWidget(), dw4);
}

TEST_ADD(tst_DockWindow)

#include "tst_dockwindow.moc"
