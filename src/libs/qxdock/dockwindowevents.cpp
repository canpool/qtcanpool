/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "dockwindow.h"
#include "dockwindow_p.h"
#include "dockwidget.h"
#include "dockfloatingcontainer.h"
#include "dockdrag.h"
#include "dockutils.h"

#if QX_DOCK_X11
#include "linux/dockfloatingtitlebar.h"
#endif

#include <QWindowStateChangeEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
// Only the X11 block below asks for the platform name, and that block is
// compiled by the Linux legs and by nothing on Windows or macOS - the same way
// the split's missing includes stayed invisible on both local legs.
#include <QGuiApplication>

QX_DOCK_BEGIN_NAMESPACE

#if QX_DOCK_X11
bool DockWindow::eventFilter(QObject *obj, QEvent *e)
{
    Q_D(DockWindow);
    // Emulate Qt:Tool behaviour.
    // Required because on some WMs Tool windows can't be maximized.

    // Window always on top of the MainWindow.
    if (e->type() == QEvent::WindowActivate) {
        for (auto _window : d->m_floatingContainers) {
            if (!_window->isVisible() || window()->isMinimized()) {
                continue;
            }
            // setWindowFlags(Qt::WindowStaysOnTopHint) will hide the window and thus requires a show call.
            // This then leads to flickering and a nasty endless loop (also buggy behaviour on Ubuntu).
            // So we just do it ourself.
            if (QGuiApplication::platformName() == QLatin1String("xcb")) {
                internal::xcb_update_prop(true, _window->window()->winId(), "_NET_WM_STATE", "_NET_WM_STATE_ABOVE",
                                          "_NET_WM_STATE_STAYS_ON_TOP");
            } else {
                _window->setWindowFlag(Qt::WindowStaysOnTopHint, true);
            }
        }
    } else if (e->type() == QEvent::WindowDeactivate) {
        for (auto _window : d->m_floatingContainers) {
            if (!_window->isVisible() || window()->isMinimized()) {
                continue;
            }

            if (QGuiApplication::platformName() == QLatin1String("xcb")) {
                internal::xcb_update_prop(false, _window->window()->winId(), "_NET_WM_STATE", "_NET_WM_STATE_ABOVE",
                                          "_NET_WM_STATE_STAYS_ON_TOP");
            } else {
                _window->setWindowFlag(Qt::WindowStaysOnTopHint, false);
            }
            _window->raise();
        }
    }

    // Sync minimize with MainWindow
    if (e->type() == QEvent::WindowStateChange) {
        for (auto _window : d->m_floatingContainers) {
            if (!_window->isVisible()) {
                continue;
            }

            if (window()->isMinimized()) {
                _window->showMinimized();
            } else {
                _window->setWindowState(_window->windowState() & (~Qt::WindowMinimized));
            }
        }
        if (!window()->isMinimized()) {
            // QApplication::setActiveWindow() is deprecated as of Qt 6.12; it
            // asked for the same thing this does, activation of the window that
            // contains this widget.
            window()->activateWindow();
        }
    }
    return Super::eventFilter(obj, e);
}
#else
bool DockWindow::eventFilter(QObject *obj, QEvent *e)
{
    Q_D(DockWindow);
    if (e->type() == QEvent::WindowStateChange) {
        QWindowStateChangeEvent *ev = static_cast<QWindowStateChangeEvent *>(e);
        if (ev->oldState().testFlag(Qt::WindowMinimized)) {
            d->m_isLeavingMinimized = true;
            QMetaObject::invokeMethod(this, "endLeavingMinimizedState", Qt::QueuedConnection);
        }
    }
    return Super::eventFilter(obj, e);
}
#endif

void DockWindow::showEvent(QShowEvent *e)
{
    Q_D(DockWindow);
    Super::showEvent(e);

    restoreHiddenFloatingWidgets();
    if (d->m_uninitializedFloatingWidgets.empty()) {
        return;
    }

    for (auto floatingWidget : d->m_uninitializedFloatingWidgets) {
        // Check, if someone closed a floating dock widget before the dock window is shown
        if (floatingWidget->dockContainer()->hasOpenDockPanels()) {
            floatingWidget->show();
        }
    }
    d->m_uninitializedFloatingWidgets.clear();
}

/**
 * The native drag entry points. Only a payload of ours is accepted: every other
 * drag is ignored so that the widget under the cursor - not the window - gets to
 * decide, and the in-process mouse dragging, which never produces a QDrag, is
 * unaffected either way.
 */
void DockWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (!DockDrag::canDecode(e->mimeData())) {
        e->ignore();
        return;
    }
    e->setDropAction(Qt::MoveAction);
    e->accept();
}

void DockWindow::dragMoveEvent(QDragMoveEvent *e)
{
    if (!DockDrag::canDecode(e->mimeData())) {
        e->ignore();
        return;
    }
    e->setDropAction(Qt::MoveAction);
    e->accept();
}

void DockWindow::dragLeaveEvent(QDragLeaveEvent *e)
{
    e->accept();
}

void DockWindow::dropEvent(QDropEvent *e)
{
    // Qt 6 moved the drop position to QPointF; Qt 5 has only the integer one.
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QPoint dropPos = e->position().toPoint();
#else
    const QPoint dropPos = e->pos();
#endif

    if (!handleDockWidgetDrop(e->mimeData(), dropPos)) {
        e->ignore();
        return;
    }

    e->setDropAction(Qt::MoveAction);
    e->accept();
}

bool DockWindow::handleDockWidgetDrop(const QMimeData *data, const QPoint &dropPos)
{
    if (!DockDrag::canDecode(data)) {
        return false;
    }

    const QString name = DockDrag::dockWidgetName(data);

    // A dock widget this window owns may have been closed; the payload names it,
    // so bringing it back needs no decision from the application. A dock widget
    // that is not here - the drag came from another process - can only be built
    // by the application, which is what the signal below is for.
    if (DockWidget *w = findDockWidget(name)) {
        if (w->isClosed()) {
            w->toggleView(true);
        }
    }

    Q_EMIT dockWidgetDropped(name, mapToGlobal(dropPos));
    return true;
}

QX_DOCK_END_NAMESPACE
