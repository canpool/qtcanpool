/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
**/
#ifndef WINDOWSYSTEM_P_H
#define WINDOWSYSTEM_P_H

//
//  W A R N I N G !!!
//  -----------------
//
// This file is not part of the QxWindow API. It is used purely as an
// implementation detail. This header file may change from version to
// version without notice, or may even be removed.
//

#include "windowkit_p.h"
#include <QtGui/QMouseEvent>
#include <QtGui/QWindow>

// Don't include this header in any header files.

QX_WINDOW_BEGIN_NAMESPACE

// Anonymous namespace
namespace {

class WindowMoveManipulator : public QObject
{
public:
    explicit WindowMoveManipulator(QWindow *targetWindow)
        : QObject(targetWindow), target(targetWindow), initialMousePosition(QCursor::pos()),
          initialWindowPosition(targetWindow->position())
    {
        target->installEventFilter(this);
    }
protected:
    bool eventFilter(QObject *obj, QEvent *event) override
    {
        switch (event->type()) {
        case QEvent::MouseMove: {
            auto mouseEvent = static_cast<QMouseEvent *>(event);
            QPoint delta = getMouseEventGlobalPos(mouseEvent) - initialMousePosition;
            target->setPosition(initialWindowPosition + delta);
            return true;
        }

        case QEvent::MouseButtonRelease: {
            if (target->y() < 0) {
                target->setPosition(target->x(), 0);
            }
            target->removeEventFilter(this);
            deleteLater();
        }

        default:
            break;
        }
        return false;
    }
private:
    QWindow *target;
    QPoint initialMousePosition;
    QPoint initialWindowPosition;
};

class WindowResizeManipulator : public QObject
{
public:
    WindowResizeManipulator(QWindow *targetWindow, Qt::Edges edges)
        : QObject(targetWindow), target(targetWindow), resizeEdges(edges), initialMousePosition(QCursor::pos()),
          initialWindowRect(target->geometry())
    {
        target->installEventFilter(this);
    }
protected:
    bool eventFilter(QObject *obj, QEvent *event) override
    {
        switch (event->type()) {
        case QEvent::MouseMove: {
            auto mouseEvent = static_cast<QMouseEvent *>(event);
            QPoint globalMousePos = getMouseEventGlobalPos(mouseEvent);
            QRect windowRect = initialWindowRect;

            if (resizeEdges & Qt::LeftEdge) {
                int delta = globalMousePos.x() - initialMousePosition.x();
                windowRect.setLeft(initialWindowRect.left() + delta);
            }
            if (resizeEdges & Qt::RightEdge) {
                int delta = globalMousePos.x() - initialMousePosition.x();
                windowRect.setRight(initialWindowRect.right() + delta);
            }
            if (resizeEdges & Qt::TopEdge) {
                int delta = globalMousePos.y() - initialMousePosition.y();
                windowRect.setTop(initialWindowRect.top() + delta);
            }
            if (resizeEdges & Qt::BottomEdge) {
                int delta = globalMousePos.y() - initialMousePosition.y();
                windowRect.setBottom(initialWindowRect.bottom() + delta);
            }

            target->setGeometry(windowRect);
            return true;
        }

        case QEvent::MouseButtonRelease: {
            target->removeEventFilter(this);
            deleteLater();
        }

        default:
            break;
        }
        return false;
    }
private:
    QWindow *target;
    QPoint initialMousePosition;
    QRect initialWindowRect;
    Qt::Edges resizeEdges;
};

}

// QWindow::startSystemMove() and QWindow::startSystemResize() is first supported at Qt 5.15
// QWindow::startSystemResize() returns false on macOS
// QWindow::startSystemMove() and QWindow::startSystemResize() returns false on Linux Unity DE

// When the new API fails, we emulate the window actions using the classical API.

static inline void startSystemMove(QWindow *window)
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    std::ignore = new WindowMoveManipulator(window);
#elif defined(Q_OS_LINUX)
    if (window->startSystemMove()) {
        return;
    }
    std::ignore = new WindowMoveManipulator(window);
#else
    window->startSystemMove();
#endif
}

static inline void startSystemResize(QWindow *window, Qt::Edges edges)
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    std::ignore = new WindowResizeManipulator(window, edges);
#elif defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    if (window->startSystemResize(edges)) {
        return;
    }
    std::ignore = new WindowResizeManipulator(window, edges);
#else
    window->startSystemResize(edges);
#endif
}

QX_WINDOW_END_NAMESPACE

#endif   // WINDOWSYSTEM_P_H
