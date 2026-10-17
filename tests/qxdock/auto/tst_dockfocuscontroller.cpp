/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

/*!
 * \file
 *
 * DockFocusController is the object that answers "which dock widget has focus"
 * for a DockWindow. It is constructed by the window, never by a caller, and its
 * whole state is one guarded pointer - so the test is about the two ends of that
 * pointer: nothing is focused before anything is, and asking for the focused
 * widget after the dock widget it pointed at is gone must not hand back a
 * dangling one.
 *
 * The second half is the reason this class holds a QPointer: a dock widget can be
 * closed, or dragged into another window, while it is still the focused one.
 */
#include "tst_global.h"

#include "qxdock/dockfocuscontroller.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockwindow.h"

QX_DOCK_USE_NAMESPACE

class tst_DockFocusController : public QObject
{
    Q_OBJECT

private slots:
    void nothingIsFocusedToBeginWith();
    void focusedWidgetDoesNotOutliveTheDockWidget();
};

void tst_DockFocusController::nothingIsFocusedToBeginWith()
{
    DockWindow window;
    DockFocusController controller(&window);

    QVERIFY(!controller.focusedDockWidget());
}

void tst_DockFocusController::focusedWidgetDoesNotOutliveTheDockWidget()
{
    DockWindow window;
    DockFocusController controller(&window);

    DockWidget *dw = new DockWidget(QStringLiteral("focused"));
    QVERIFY(window.addDockWidget(Qx::CenterDockWidgetArea, dw));

    controller.setDockWidgetFocused(dw);
    QCOMPARE(controller.focusedDockWidget(), dw);

    delete dw;

    // A QPointer, so this is null rather than a pointer into freed memory.
    QVERIFY(!controller.focusedDockWidget());
}

TEST_ADD(tst_DockFocusController)

#include "tst_dockfocuscontroller.moc"
