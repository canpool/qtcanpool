/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
**/
#include "windowcontext_p.h"
#include "windowkit_p.h"
#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtGui/QScreen>
#include <memory>

QX_WINDOW_BEGIN_NAMESPACE

namespace {

class WinIdChangeEventFilter : public QObject
{
public:
    explicit WinIdChangeEventFilter(QObject *widget, WindowContext *ctx, QObject *parent = nullptr)
        : QObject(parent), ctx(ctx)
    {
        widget->installEventFilter(this);
    }
protected:
    bool eventFilter(QObject *obj, QEvent *event) override
    {
        Q_UNUSED(obj)
        if (event->type() == QEvent::WinIdChange) {
            ctx->notifyWinIdChange();
        }
        return false;
    }
protected:
    WindowContext *ctx;
};

class WindowEventFilter : public QObject
{
public:
    explicit WindowEventFilter(QWindow *window, WindowContext *ctx, QObject *parent = nullptr)
        : QObject(parent), ctx(ctx), window(window)
    {
        window->installEventFilter(this);
    }
protected:
    bool eventFilter(QObject *obj, QEvent *event) override
    {
        return ctx->sharedDispatch(obj, event);
    }
protected:
    WindowContext *ctx;
    QWindow *window;
};

}

WindowContext::WindowContext() = default;

WindowContext::~WindowContext() = default;

void WindowContext::setup(QObject *host, WindowItemDelegate *delegate)
{
    if (m_host || !host || !delegate) {
        return;
    }
    m_host = host;
    m_delegate.reset(delegate);
    m_winIdChangeEventFilter = std::make_unique<WinIdChangeEventFilter>(host, this);

    m_windowHandle = m_delegate->hostWindow(m_host);
    if (m_windowHandle) {
        winIdChanged();
        m_windowEventFilter = std::make_unique<WindowEventFilter>(m_windowHandle, this);
    }
}

bool WindowContext::setHitTestVisible(const QObject *obj, bool visible)
{
    Q_ASSERT(obj);
    if (!obj) {
        return false;
    }

    if (visible) {
        m_hitTestVisibleItems.insert(obj);
    } else {
        m_hitTestVisibleItems.remove(obj);
    }
    return true;
}

bool WindowContext::setSystemButton(WindowAgentBase::SystemButton button, QObject *obj)
{
    Q_ASSERT(button != WindowAgentBase::Unknown);
    if (button == WindowAgentBase::Unknown) {
        return false;
    }

    if (m_systemButtons[button] == obj) {
        return false;
    }
    m_systemButtons[button] = obj;
    return true;
}

bool WindowContext::setTitleBar(QObject *item)
{
    Q_ASSERT(item);
    if (m_titleBar == item) {
        return false;
    }

    if (m_titleBar) {
        // Since the title bar is changed, all items inside it should be dereferenced right away
        for (auto &button : m_systemButtons) {
            button = nullptr;
        }
        m_hitTestVisibleItems.clear();
    }

    m_titleBar = item;
    return true;
}

#ifdef Q_OS_MAC
void WindowContext::setSystemButtonAreaCallback(const ScreenRectCallback &callback)
{
    m_systemButtonAreaCallback = callback;
    virtual_hook(SystemButtonAreaChangedHook, nullptr);
}
#endif

bool WindowContext::isInSystemButtons(const QPoint &pos, WindowAgentBase::SystemButton *button) const
{
    *button = WindowAgentBase::Unknown;
    for (int i = WindowAgentBase::WindowIcon; i <= WindowAgentBase::Close; ++i) {
        auto currentButton = m_systemButtons[i];
        if (!currentButton || !m_delegate->isVisible(currentButton) || !m_delegate->isEnabled(currentButton)) {
            continue;
        }
        if (m_delegate->mapGeometryToScene(currentButton).contains(pos)) {
            *button = static_cast<WindowAgentBase::SystemButton>(i);
            return true;
        }
    }
    return false;
}

bool WindowContext::isInTitleBarDraggableArea(const QPoint &pos) const
{
    if (!m_titleBar) {
        // There's no title bar at all, the mouse will always be in the client area.
        return false;
    }
    if (!m_delegate->isVisible(m_titleBar) || !m_delegate->isEnabled(m_titleBar)) {
        // The title bar is hidden or disabled for some reason, treat it as there's
        // no title bar.
        return false;
    }
    QRect windowRect = {QPoint(0, 0), m_windowHandle->size()};
    QRect titleBarRect = m_delegate->mapGeometryToScene(m_titleBar);
    if (!titleBarRect.intersects(windowRect)) {
        // The title bar is totally outside the window for some reason,
        // also treat it as there's no title bar.
        return false;
    }

    if (!titleBarRect.contains(pos)) {
        return false;
    }

    for (int i = WindowAgentBase::WindowIcon; i <= WindowAgentBase::Close; ++i) {
        auto currentButton = m_systemButtons[i];
        if (currentButton && m_delegate->isVisible(currentButton) && m_delegate->isEnabled(currentButton) &&
            m_delegate->mapGeometryToScene(currentButton).contains(pos)) {
            return false;
        }
    }

    for (auto widget : m_hitTestVisibleItems) {
        if (widget && m_delegate->isVisible(widget) && m_delegate->isEnabled(widget) &&
            m_delegate->mapGeometryToScene(widget).contains(pos)) {
            return false;
        }
    }
    if (!m_captionClassNameList.isEmpty()) {
        if (auto widget = m_delegate->childAt(m_titleBar, pos)) {
            if (!isCaptionClassName(widget->metaObject()->className())) {
                return false;
            }
        }
    }

    return true;
}

bool WindowContext::isCaptionClassName(const char *name) const
{
    foreach (const QString &cn, m_captionClassNameList) {
        if (cn.compare(QLatin1String(name)) == 0) {
            return true;
        }
    }
    return false;
}

void WindowContext::addCaptionClassName(const QString &name)
{
    if (!m_captionClassNameList.contains(name)) {
        m_captionClassNameList.append(name);
    }
}

QString WindowContext::key() const
{
    return {};
}

QWK_USED static constexpr const struct {
    const quint32 activeLight = MAKE_RGBA_COLOR(210, 233, 189, 226);
    const quint32 activeDark = MAKE_RGBA_COLOR(177, 205, 190, 240);
    const quint32 inactiveLight = MAKE_RGBA_COLOR(193, 195, 211, 203);
    const quint32 inactiveDark = MAKE_RGBA_COLOR(240, 240, 250, 255);
} kSampleColorSet;

void WindowContext::virtual_hook(int id, void *data)
{
    switch (id) {
    case CentralizeHook: {
        if (!m_windowHandle)
            return;

        QRect screenGeometry = m_windowHandle->screen()->geometry();
        int x = (screenGeometry.width() - m_windowHandle->width()) / 2;
        int y = (screenGeometry.height() - m_windowHandle->height()) / 2;
        QPoint pos(x, y);
        pos += screenGeometry.topLeft();
        m_windowHandle->setPosition(pos);
        return;
    }

    case RaiseWindowHook: {
        if (!m_windowHandle)
            return;

        m_delegate->setWindowVisible(m_host, true);
        Qt::WindowStates state = m_delegate->getWindowState(m_host);
        if (state & Qt::WindowMinimized) {
            m_delegate->setWindowState(m_host, state & ~Qt::WindowMinimized);
        }
        m_delegate->bringWindowToTop(m_host);
        return;
    }

    case DefaultColorsHook: {
        auto &map = *static_cast<QMap<QString, QColor> *>(data);
        map.clear();
        map.insert(QStringLiteral("activeLight"), kSampleColorSet.activeLight);
        map.insert(QStringLiteral("activeDark"), kSampleColorSet.activeDark);
        map.insert(QStringLiteral("inactiveLight"), kSampleColorSet.inactiveLight);
        map.insert(QStringLiteral("inactiveDark"), kSampleColorSet.inactiveDark);
        return;
    }

    default:
        break;
    }
}

void WindowContext::showSystemMenu(const QPoint &pos)
{
    virtual_hook(ShowSystemMenuHook, &const_cast<QPoint &>(pos));
}

void WindowContext::notifyWinIdChange()
{
    auto oldWindow = m_windowHandle;
    m_windowHandle = m_delegate->hostWindow(m_host);
    if (oldWindow == m_windowHandle)
        return;
    m_windowEventFilter.reset();
    winIdChanged();
    if (m_windowHandle) {
        m_windowEventFilter = std::make_unique<WindowEventFilter>(m_windowHandle, this);

        // Refresh window attributes
        auto attributes = m_windowAttributes;
        m_windowAttributes.clear();
        for (auto it = attributes.begin(); it != attributes.end(); ++it) {
            if (!windowAttributeChanged(it.key(), it.value(), {})) {
                continue;
            }
            m_windowAttributes.insert(it.key(), it.value());
        }
    }
}

QVariant WindowContext::windowAttribute(const QString &key) const
{
    return m_windowAttributes.value(key);
}

bool WindowContext::setWindowAttribute(const QString &key, const QVariant &attribute)
{
    auto it = m_windowAttributes.find(key);
    if (it == m_windowAttributes.end()) {
        if (!attribute.isValid()) {
            return true;
        }
        if (!m_windowHandle || !windowAttributeChanged(key, attribute, {})) {
            return false;
        }
        m_windowAttributes.insert(key, attribute);
        return true;
    }

    if (it.value() == attribute)
        return true;
    if (!m_windowHandle || !windowAttributeChanged(key, attribute, it.value())) {
        return false;
    }

    if (attribute.isValid()) {
        it.value() = attribute;
    } else {
        m_windowAttributes.erase(it);
    }
    return true;
}

bool WindowContext::windowAttributeChanged(const QString &key, const QVariant &attribute,
                                                   const QVariant &oldAttribute)
{
    Q_UNUSED(key);
    Q_UNUSED(attribute);
    Q_UNUSED(oldAttribute);
    return false;
}

QX_WINDOW_END_NAMESPACE
