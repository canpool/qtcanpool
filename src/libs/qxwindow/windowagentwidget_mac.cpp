/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
**/
#include "windowagentwidget_p.h"
#include <QtGui/QtEvents>

QX_WINDOW_BEGIN_NAMESPACE

static inline QRect getWidgetSceneRect(QWidget *widget)
{
    return {widget->mapTo(widget->window(), QPoint()), widget->size()};
}

class SystemButtonAreaWidgetEventFilter : public QObject
{
public:
    SystemButtonAreaWidgetEventFilter(QWidget *widget, WindowContext *ctx, QObject *parent = nullptr)
        : QObject(parent), widget(widget), ctx(ctx)
    {
        widget->installEventFilter(this);
        ctx->setSystemButtonAreaCallback([widget](const QSize &) {
            return getWidgetSceneRect(widget);   //
        });
    }
    ~SystemButtonAreaWidgetEventFilter() override = default;
protected:
    bool eventFilter(QObject *obj, QEvent *event) override
    {
        Q_UNUSED(obj)
        switch (event->type()) {
        case QEvent::Move:
        case QEvent::Resize: {
            ctx->virtual_hook(WindowContext::SystemButtonAreaChangedHook, nullptr);
            break;
        }

        default:
            break;
        }
        return false;
    }
protected:
    QWidget *widget;
    WindowContext *ctx;
};

/*!
    Returns the widget that acts as the system button area.
*/
QWidget *WindowAgentWidget::systemButtonArea() const
{
    Q_D(const WindowAgentWidget);
    return d->systemButtonAreaWidget;
}

/*!
    Sets the widget that acts as the system button area. The system button will be centered in
    its area, it is recommended to place the widget in a layout and set a fixed size policy.

    The system button will be visible in the system title bar area.
*/
void WindowAgentWidget::setSystemButtonArea(QWidget *widget)
{
    Q_D(WindowAgentWidget);
    if (d->systemButtonAreaWidget == widget)
        return;

    auto ctx = d->context.get();
    d->systemButtonAreaWidget = widget;
    if (!widget) {
        d->context->setSystemButtonAreaCallback({});
        d->systemButtonAreaWidgetEventFilter.reset();
        return;
    }
    d->systemButtonAreaWidgetEventFilter = std::make_unique<SystemButtonAreaWidgetEventFilter>(widget, ctx);
}

/*!
    Returns the the system button area callback.
*/
ScreenRectCallback WindowAgentWidget::systemButtonAreaCallback() const
{
    Q_D(const WindowAgentWidget);
    return d->systemButtonAreaWidget ? nullptr : d->context->systemButtonAreaCallback();
}

/*!
    Sets the the system button area callback, the \c size of the callback is the native title
    bar size.

    The system button position will be updated when the window resizes.
*/
void WindowAgentWidget::setSystemButtonAreaCallback(const ScreenRectCallback &callback)
{
    Q_D(WindowAgentWidget);
    setSystemButtonArea(nullptr);
    d->context->setSystemButtonAreaCallback(callback);
}

QX_WINDOW_END_NAMESPACE
