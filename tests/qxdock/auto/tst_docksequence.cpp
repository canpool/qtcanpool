/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "tst_probe.h"

#include "qxdock/dockcontainer.h"
#include "qxdock/dockdrag.h"
#include "qxdock/dockmanager.h"
#include "qxdock/docksplitter.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockwindow.h"

#include <QApplication>
#include <QBoxLayout>
#include <QMimeData>
#include <QScopedPointer>
#include <QSplitter>
#include <QTest>

QX_DOCK_USE_NAMESPACE

namespace
{
/*! The root splitter is protected - a container's own business - so the test
 *  reaches it the way a subclass would. */
class SplitterWindow : public DockWindow
{
public:
    using DockWindow::DockWindow;

    QSplitter *root() const
    {
        return rootSplitter();
    }
};

/*! And the door a drop comes in by is protected too: a QDropEvent handed to
 *  sendEvent() never reaches dropEvent(), so what a drop does is what
 *  handleDockWidgetDrop() does, and this is how the sequence reaches it. */
class DropWindow : public DockWindow
{
public:
    using DockWindow::DockWindow;

    bool drop(const QMimeData *payload, const QPoint &pos)
    {
        return handleDockWidgetDrop(payload, pos);
    }

    QSplitter *root() const
    {
        return rootSplitter();
    }
};
}   // namespace

/*!
 * The sequences a user actually runs, rather than the calls they are made of.
 *
 * Every dock widget here is asked for one thing at a time by the tests that
 * cover its class - close it, float it, pin it - and each of those answers is
 * right on its own. What was never covered is what happens when they are run in
 * the order a user runs them: a pane goes away, something else takes its room,
 * and then it comes back and the room has to be given back. Each sequence below
 * ends by reading the row again, because a row that is no longer measured
 * against what it is holding is the one thing every step of these can get
 * wrong, and the one thing a single-step test cannot see.
 */
class tst_DockSequence : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void aClosedPaneComesBackIntoARowThatIsMeasuredAgain();
    void anAutoHiddenPaneComesBackIntoARowThatIsMeasuredAgain();
    void aPaneThatWentFloatingComesBackIntoARowThatIsMeasuredAgain();
    void aPaneDraggedBackIntoTheRowIsGivenItsRoomAgain();
private:
    DockManager::AutoHideFlags m_autoHide;
};

void tst_DockSequence::init()
{
    // Pinning a pane to a side bar is a feature an application turns on, and it
    // is off until it is - so the sequence has to ask for it before it can run.
    m_autoHide = DockManager::autoHideConfigFlags();
    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);
}

void tst_DockSequence::cleanup()
{
    DockManager::setAutoHideConfigFlags(m_autoHide);
}

/*! Closing a pane gives its room to the panes next to it; opening it again
 *  takes it back. A row that only ever measured itself on the way down keeps a
 *  gap on the way up. */
void tst_DockSequence::aClosedPaneComesBackIntoARowThatIsMeasuredAgain()
{
    QWidget host;
    host.setLayout(new QVBoxLayout);
    SplitterWindow dock(&host);
    host.layout()->addWidget(&dock);
    host.resize(800, 600);

    DockWidget left(QStringLiteral("left"));
    DockWidget center(QStringLiteral("center"));
    dock.addDockWidget(Qx::LeftDockWidgetArea, &left);
    dock.addDockWidget(Qx::CenterDockWidgetArea, &center);
    host.show();
    Probe::settle(&host);

    QSplitter *root = dock.root();
    QVERIFY(root);
    QVERIFY2(Probe::fillsItself(root), qPrintable(Probe::describe(root)));
    const int panes = Probe::lines(Probe::tree(&host), QStringLiteral("QxDock::DockWidget ")).count();

    left.toggleView(false);
    Probe::settle(&host);
    QVERIFY(left.isClosed());
    // A closed pane is still in the tree - it is the panel that goes, and the
    // dock widget inside it is simply not showing.
    QVERIFY(!left.isVisible());
    QVERIFY2(Probe::fillsItself(dock.root()), qPrintable(Probe::describe(dock.root())));

    left.toggleView(true);
    Probe::settle(&host);
    QVERIFY(!left.isClosed());
    QVERIFY(left.isVisible());
    QCOMPARE(Probe::lines(Probe::tree(&host), QStringLiteral("QxDock::DockWidget ")).count(), panes);
    QVERIFY2(Probe::fillsItself(dock.root()), qPrintable(Probe::describe(dock.root())));
}

/*! An auto hidden pane leaves the row for the side bar, and comes back out of
 *  it - which is the same exchange of room, with a container in the middle. */
void tst_DockSequence::anAutoHiddenPaneComesBackIntoARowThatIsMeasuredAgain()
{
    QWidget host;
    host.setLayout(new QVBoxLayout);
    SplitterWindow dock(&host);
    host.layout()->addWidget(&dock);
    host.resize(800, 600);

    DockWidget left(QStringLiteral("left"));
    DockWidget center(QStringLiteral("center"));
    dock.addDockWidget(Qx::LeftDockWidgetArea, &left);
    dock.addDockWidget(Qx::CenterDockWidgetArea, &center);
    host.show();
    Probe::settle(&host);

    QSplitter *root = dock.root();
    QVERIFY(root);
    QVERIFY2(Probe::fillsItself(root), qPrintable(Probe::describe(root)));

    left.setAutoHide(true, Qx::DockSideBarLeft);
    Probe::settle(&host);
    QVERIFY(left.isAutoHide());
    QVERIFY2(Probe::fillsItself(dock.root()), qPrintable(Probe::describe(dock.root())));

    left.setAutoHide(false);
    Probe::settle(&host);
    QVERIFY(!left.isAutoHide());
    QVERIFY(left.isVisible());
    QVERIFY2(Probe::fillsItself(dock.root()), qPrintable(Probe::describe(dock.root())));
}

/*! A pane that went floating took its room with it, and the row closed behind
 *  it; when it is docked again the row has to open again. */
void tst_DockSequence::aPaneThatWentFloatingComesBackIntoARowThatIsMeasuredAgain()
{
    QWidget host;
    host.setLayout(new QVBoxLayout);
    SplitterWindow dock(&host);
    host.layout()->addWidget(&dock);
    host.resize(800, 600);

    DockWidget left(QStringLiteral("left"));
    DockWidget center(QStringLiteral("center"));
    dock.addDockWidget(Qx::LeftDockWidgetArea, &left);
    dock.addDockWidget(Qx::CenterDockWidgetArea, &center);
    host.show();
    Probe::settle(&host);

    QSplitter *root = dock.root();
    QVERIFY(root);
    QVERIFY2(Probe::fillsItself(root), qPrintable(Probe::describe(root)));
    const int panes = Probe::lines(Probe::tree(&host), QStringLiteral("QxDock::DockWidget ")).count();

    left.setFloating();
    Probe::settle(&host);
    QVERIFY(left.isFloating());
    QVERIFY2(Probe::fillsItself(dock.root()), qPrintable(Probe::describe(dock.root())));

    dock.addDockWidget(Qx::LeftDockWidgetArea, &left);
    Probe::settle(&host);
    QVERIFY(!left.isFloating());
    QCOMPARE(Probe::lines(Probe::tree(&host), QStringLiteral("QxDock::DockWidget ")).count(), panes);
    QVERIFY2(Probe::fillsItself(dock.root()), qPrintable(Probe::describe(dock.root())));
}

/*! A pane that was dragged out and dropped back does not come back through a
 *  call on the pane: it comes back as a payload naming it, which the window
 *  has to recognise before the pane is even open again. The room is given back
 *  after that, and by then the row has already been divided up without it. */
void tst_DockSequence::aPaneDraggedBackIntoTheRowIsGivenItsRoomAgain()
{
    DropWindow dock;
    dock.resize(800, 600);

    DockWidget left(QStringLiteral("left"));
    DockWidget center(QStringLiteral("center"));
    dock.addDockWidget(Qx::LeftDockWidgetArea, &left);
    dock.addDockWidget(Qx::CenterDockWidgetArea, &center);
    dock.show();
    Probe::settle(&dock);

    QSplitter *root = dock.root();
    QVERIFY(root);
    QVERIFY2(Probe::fillsItself(root), qPrintable(Probe::describe(root)));

    left.toggleView(false);
    Probe::settle(&dock);
    QVERIFY(left.isClosed());
    QVERIFY2(Probe::fillsItself(dock.root()), qPrintable(Probe::describe(dock.root())));

    const QScopedPointer<QMimeData> payload(DockDrag::createMimeData(QStringLiteral("left"), QStringLiteral("left")));
    QVERIFY(dock.drop(payload.data(), QPoint(10, 10)));
    Probe::settle(&dock);

    QVERIFY(!left.isClosed());
    QVERIFY(left.isVisible());
    QVERIFY2(Probe::fillsItself(dock.root()), qPrintable(Probe::describe(dock.root())));
}

TEST_ADD(tst_DockSequence)

#include "tst_docksequence.moc"
