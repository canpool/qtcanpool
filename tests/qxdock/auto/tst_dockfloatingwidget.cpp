/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

/*!
 * \file
 *
 * DockFloatingWidget is the interface behind both kinds of undocking: the opaque
 * one creates a real DockFloatingContainer, the non-opaque one a
 * DockFloatingPreview that follows the cursor. Nothing in the docking code names
 * a concrete class while a drag is in progress - it holds a DockFloatingWidget
 * and calls startFloating(), moveFloating() and finishDragging() on it.
 *
 * That is why it is worth testing at all: it has no implementation of its own,
 * so the only thing that can go wrong is the contract - a signature that drifts,
 * or a call that reaches the wrong override. The stub below implements exactly
 * the three virtuals and counts them through a base pointer, which is what a drag
 * does.
 */
#include "tst_global.h"

#include "qxdock/dockfloatingpreview.h"
#include "qxdock/dockfloatingwidget.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockwindow.h"

#include <QWidget>

QX_DOCK_USE_NAMESPACE

namespace
{

/*
 * Deliberately not a QWidget: the two concrete floating widgets are, but the
 * interface itself is not a QObject, and keeping the stub free of QObject is
 * what makes the test about dispatch rather than about the moc.
 */
class FloatingStub : public DockFloatingWidget
{
public:
    void startFloating(const QPoint &dragStartMousePos, const QSize &size, Qx::DockDragState dragState,
                       QWidget *mouseEventHandler) override
    {
        m_startPos = dragStartMousePos;
        m_size = size;
        m_state = dragState;
        m_handler = mouseEventHandler;
        ++m_startCount;
    }

    void moveFloating() override
    {
        ++m_moveCount;
    }

    void finishDragging() override
    {
        ++m_finishCount;
    }

    QPoint m_startPos;
    QSize m_size;
    Qx::DockDragState m_state = Qx::DockDraggingInactive;
    QWidget *m_handler = nullptr;
    int m_startCount = 0;
    int m_moveCount = 0;
    int m_finishCount = 0;
};

}   // namespace

class tst_DockFloatingWidget : public QObject
{
    Q_OBJECT

private slots:
    void theThreeCallsReachTheImplementation();
};

class tst_DockFloatingPreview : public QObject
{
    Q_OBJECT

private slots:
    void takesOverTheTitleOfItsContent();
};

void tst_DockFloatingWidget::theThreeCallsReachTheImplementation()
{
    FloatingStub stub;
    QWidget handler;

    DockFloatingWidget *floating = &stub;
    floating->startFloating(QPoint(3, 4), QSize(10, 20), Qx::DockDraggingMousePressed, &handler);
    floating->moveFloating();
    floating->moveFloating();
    floating->finishDragging();

    QCOMPARE(stub.m_startCount, 1);
    QCOMPARE(stub.m_moveCount, 2);
    QCOMPARE(stub.m_finishCount, 1);

    // Everything the drag needs later is handed over in one call.
    QCOMPARE(stub.m_startPos, QPoint(3, 4));
    QCOMPARE(stub.m_size, QSize(10, 20));
    QCOMPARE(stub.m_state, Qx::DockDraggingMousePressed);
    QCOMPARE(stub.m_handler, &handler);
}

void tst_DockFloatingPreview::takesOverTheTitleOfItsContent()
{
    DockWindow window;
    DockWidget *dw = new DockWidget(QStringLiteral("preview content"));
    QVERIFY(window.addDockWidget(Qx::CenterDockWidgetArea, dw));

    // The content has to be docked first: the preview asks it for its window and
    // its panel, and both are null while it is floating free.
    DockFloatingPreview preview(dw);
    QCOMPARE(preview.windowTitle(), dw->windowTitle());

    // It is shown by the drag, not by being constructed.
    QVERIFY(!preview.isVisible());
}

TEST_ADD(tst_DockFloatingWidget)
TEST_ADD(tst_DockFloatingPreview)

#include "tst_dockfloatingwidget.moc"
