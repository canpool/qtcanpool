/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
 **/
#include "windowcontext_p.h"
#include "windowgeometry_p.h"
#include "windowkit_p.h"
#include "windowsystem_p.h"
#include <QtCore/QDebug>
#include <QScreen>

QX_WINDOW_BEGIN_NAMESPACE

// The arithmetic itself lives in windowgeometry_p.h, where it can be tested with
// nothing but a QSize. What is left here are the two conditions that are about
// the window rather than the geometry: macOS deliberately has no resize cursor,
// and a window that is not in the windowed state (full screen, hidden) is not
// resized from its border either. Keeping the guards here keeps the observable
// behaviour identical to the single-file version this was split out of.
static Qt::CursorShape calculateCursorShape(const QWindow *window, const QPoint &pos)
{
#ifdef Q_OS_MACOS
    Q_UNUSED(window);
    Q_UNUSED(pos);
    return Qt::ArrowCursor;
#else
    Q_ASSERT(window);
    if (!window || window->visibility() != QWindow::Windowed) {
        return Qt::ArrowCursor;
    }
    return resizeCursorFor(window->size(), pos);
#endif
}

static inline Qt::Edges calculateWindowEdges(const QWindow *window, const QPoint &pos)
{
#ifdef Q_OS_MACOS
    Q_UNUSED(window);
    Q_UNUSED(pos);
    return {};
#else
    Q_ASSERT(window);
    if (!window || window->visibility() != QWindow::Windowed) {
        return {};
    }
    return resizeEdgesFor(window->size(), pos);
#endif
}

class QtWindowEventFilter : public WindowSharedEventFilter
{
public:
    explicit QtWindowEventFilter(WindowContext *context);
    ~QtWindowEventFilter() override;

    enum WindowStatus {
        Idle,
        WaitingRelease,
        PreparingMove,
        Moving,
        Resizing,
    };
protected:
    bool sharedEventFilter(QObject *object, QEvent *event) override;
private:
    WindowContext *m_context;
    bool m_cursorShapeChanged;
    WindowStatus m_windowStatus;
};

QtWindowEventFilter::QtWindowEventFilter(WindowContext *context)
    : m_context(context)
    , m_cursorShapeChanged(false)
    , m_windowStatus(Idle)
{
    m_context->installSharedEventFilter(this);
}

QtWindowEventFilter::~QtWindowEventFilter() = default;

bool QtWindowEventFilter::sharedEventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)

    auto host = m_context->host();
    auto window = m_context->window();
    auto delegate = m_context->delegate();

    auto type = event->type();
#ifdef Q_OS_WINDOWS
    if (type == QEvent::WindowStateChange) {
        Qt::WindowStates windowState = delegate->getWindowState(host);
        if (windowState & Qt::WindowMaximized) {
            // Prevents the window from becoming full screen
            window->setGeometry(window->screen()->availableGeometry());
            return true;
        }
    }
#endif
    if (type < QEvent::MouseButtonPress || type > QEvent::MouseMove) {
        return false;
    }
    auto me = static_cast<const QMouseEvent *>(event);
    bool fixedSize = delegate->isHostSizeFixed(host);

    QPoint scenePos = getMouseEventScenePos(me);
    QPoint globalPos = getMouseEventGlobalPos(me);

    bool inTitleBar = m_context->isInTitleBarDraggableArea(scenePos);
    switch (type) {
    case QEvent::MouseButtonPress: {
        switch (me->button()) {
        case Qt::LeftButton: {
            if (!fixedSize) {
                Qt::Edges edges = calculateWindowEdges(window, scenePos);
                if (edges != Qt::Edges()) {
                    m_windowStatus = Resizing;
                    startSystemResize(window, edges);
                    event->accept();
                    return true;
                }
            }
            if (inTitleBar) {
                // If we call startSystemMove() now but release the mouse without actual
                // movement, there will be no MouseReleaseEvent, so we defer it when the
                // mouse is actually moving for the first time
                m_windowStatus = PreparingMove;
                event->accept();
                return true;
            }
            break;
        }
        case Qt::RightButton: {
            m_context->showSystemMenu(globalPos);
            break;
        }
        default:
            break;
        }
        m_windowStatus = WaitingRelease;
        break;
    }

    case QEvent::MouseButtonRelease: {
        switch (m_windowStatus) {
        case PreparingMove:
        case Moving:
        case Resizing: {
            m_windowStatus = Idle;
            event->accept();
            return true;
        }
        case WaitingRelease: {
            m_windowStatus = Idle;
            break;
        }
        default: {
            if (inTitleBar) {
                event->accept();
                return true;
            }
            break;
        }
        }
        break;
    }

    case QEvent::MouseMove: {
        switch (m_windowStatus) {
        case Moving: {
            return true;
        }
        case PreparingMove: {
            m_windowStatus = Moving;
            startSystemMove(window);
            event->accept();
            return true;
        }
        case Idle: {
            if (!fixedSize) {
                const Qt::CursorShape shape = calculateCursorShape(window, scenePos);
                if (shape == Qt::ArrowCursor) {
                    if (m_cursorShapeChanged) {
                        delegate->restoreCursorShape(host);
                        m_cursorShapeChanged = false;
                    }
                } else {
                    delegate->setCursorShape(host, shape);
                    m_cursorShapeChanged = true;
                }
            }
            break;
        }
        default:
            break;
        }
        break;
    }

    case QEvent::MouseButtonDblClick: {
        if (me->button() == Qt::LeftButton && inTitleBar && !fixedSize) {
            Qt::WindowStates windowState = delegate->getWindowState(host);
            if (!(windowState & Qt::WindowFullScreen)) {
                if (windowState & Qt::WindowMaximized) {
                    delegate->setWindowState(host, windowState & ~Qt::WindowMaximized);
                } else {
                    delegate->setWindowState(host, windowState | Qt::WindowMaximized);
                }
                event->accept();
                return true;
            }
        }
        break;
    }

    default:
        break;
    }
    return false;
}

WindowContextQt::WindowContextQt()
    : WindowContext()
{
    qtWindowEventFilter = std::make_unique<QtWindowEventFilter>(this);
}

WindowContextQt::~WindowContextQt() = default;

QString WindowContextQt::key() const
{
    return QStringLiteral("qt");
}

void WindowContextQt::virtual_hook(int id, void *data)
{
    WindowContext::virtual_hook(id, data);
}

void WindowContextQt::winIdChanged()
{
    if (!m_windowHandle) {
        m_delegate->setWindowFlags(m_host, m_delegate->getWindowFlags(m_host) & ~Qt::FramelessWindowHint);
        return;
    }

    // Allocate new resources
    m_delegate->setWindowFlags(m_host, m_delegate->getWindowFlags(m_host) | Qt::FramelessWindowHint);
}

QX_WINDOW_END_NAMESPACE
