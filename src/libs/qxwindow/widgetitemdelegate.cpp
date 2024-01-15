/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
**/
#include "widgetitemdelegate_p.h"
#include <QtGui/QMouseEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

extern Q_DECL_IMPORT QWidget *qt_button_down;

QX_WINDOW_BEGIN_NAMESPACE

WidgetItemDelegate::WidgetItemDelegate() = default;

WidgetItemDelegate::~WidgetItemDelegate() = default;

QWindow *WidgetItemDelegate::window(const QObject *obj) const
{
    return static_cast<const QWidget *>(obj)->windowHandle();
}

bool WidgetItemDelegate::isEnabled(const QObject *obj) const
{
    return static_cast<const QWidget *>(obj)->isEnabled();
}

bool WidgetItemDelegate::isVisible(const QObject *obj) const
{
    return static_cast<const QWidget *>(obj)->isVisible();
}

QRect WidgetItemDelegate::mapGeometryToScene(const QObject *obj) const
{
    auto widget = static_cast<const QWidget *>(obj);
    const QPoint originPoint = widget->mapTo(widget->window(), QPoint(0, 0));
    const QSize size = widget->size();
    return {originPoint, size};
}

QObject *WidgetItemDelegate::childAt(const QObject *obj, const QPoint &pos) const
{
    auto widget = static_cast<const QWidget *>(obj);
    if (widget) {
        return widget->childAt(pos);
    }
    return nullptr;
}

QWindow *WidgetItemDelegate::hostWindow(const QObject *host) const
{
    return static_cast<const QWidget *>(host)->windowHandle();
}

bool WidgetItemDelegate::isHostSizeFixed(const QObject *host) const
{
    const auto widget = static_cast<const QWidget *>(host);
    // "Qt::MSWindowsFixedSizeDialogHint" is used cross-platform actually.
    if (widget->windowFlags() & Qt::MSWindowsFixedSizeDialogHint) {
        return true;
    }
    // Caused by setFixedWidth/Height/Size().
    const QSize minSize = widget->minimumSize();
    const QSize maxSize = widget->maximumSize();
    if (!minSize.isEmpty() && !maxSize.isEmpty() && (minSize == maxSize)) {
        return true;
    }
    return false;
}

bool WidgetItemDelegate::isWindowActive(const QObject *host) const
{
    return static_cast<const QWidget *>(host)->isActiveWindow();
}

void WidgetItemDelegate::resetQtGrabbedControl(QObject *host) const
{
    Q_UNUSED(host);
    if (!qt_button_down) {
        return;
    }
    static constexpr const auto invalidPos =
        QPoint{std::numeric_limits<int>::lowest(), std::numeric_limits<int>::lowest()};
    const auto event =
        new QMouseEvent(QEvent::MouseButtonRelease, invalidPos, invalidPos, invalidPos, Qt::LeftButton,
                        QGuiApplication::mouseButtons() ^ Qt::LeftButton, QGuiApplication::keyboardModifiers());
    QCoreApplication::postEvent(qt_button_down, event);
    qt_button_down = nullptr;
}

Qt::WindowStates WidgetItemDelegate::getWindowState(const QObject *host) const
{
    return static_cast<const QWidget *>(host)->windowState();
}

void WidgetItemDelegate::setWindowState(QObject *host, Qt::WindowStates state) const
{
    static_cast<QWidget *>(host)->setWindowState(state);
}

void WidgetItemDelegate::setCursorShape(QObject *host, Qt::CursorShape shape) const
{
    static_cast<QWidget *>(host)->setCursor(QCursor(shape));
}

void WidgetItemDelegate::restoreCursorShape(QObject *host) const
{
    static_cast<QWidget *>(host)->unsetCursor();
}

Qt::WindowFlags WidgetItemDelegate::getWindowFlags(const QObject *host) const
{
    return static_cast<const QWidget *>(host)->windowFlags();
}

void WidgetItemDelegate::setWindowFlags(QObject *host, Qt::WindowFlags flags) const
{
    static_cast<QWidget *>(host)->setWindowFlags(flags);
}

void WidgetItemDelegate::setWindowVisible(QObject *host, bool visible) const
{
    static_cast<QWidget *>(host)->setVisible(visible);
}

void WidgetItemDelegate::bringWindowToTop(QObject *host) const
{
    static_cast<QWidget *>(host)->raise();
}

QX_WINDOW_END_NAMESPACE
