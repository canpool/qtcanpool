/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
**/
#ifndef WINDOWCONTEXT_P_H
#define WINDOWCONTEXT_P_H

//
//  W A R N I N G !!!
//  -----------------
//
// This file is not part of the QxWindow API. It is used purely as an
// implementation detail. This header file may change from version to
// version without notice, or may even be removed.
//

#include "qxwindow_global.h"
#include "windowevent_p.h"
#include "windowitemdelegate_p.h"
#include "windowagentbase.h"
#include <QtCore/QPointer>
#include <QtCore/QSet>
#include <QtGui/QRegion>
#include <QtGui/QWindow>
#include <array>
#include <memory>

QX_WINDOW_BEGIN_NAMESPACE

class QX_WINDOW_EXPORT WindowContext : public QObject, public WindowNativeEventDispatcher,
                                       public WindowSharedEventDispatcher
{
    Q_OBJECT
public:
    WindowContext();
    ~WindowContext() override;
public:
    void setup(QObject *host, WindowItemDelegate *delegate);

    inline QObject *host() const;
    inline QWindow *window() const;
    inline WindowItemDelegate *delegate() const;

    inline bool isHitTestVisible(const QObject *obj) const;
    bool setHitTestVisible(const QObject *obj, bool visible);

    inline QObject *systemButton(WindowAgentBase::SystemButton button) const;
    bool setSystemButton(WindowAgentBase::SystemButton button, QObject *obj);

    inline QObject *titleBar() const;
    bool setTitleBar(QObject *obj);

#ifdef Q_OS_MAC
    inline ScreenRectCallback systemButtonAreaCallback() const;
    void setSystemButtonAreaCallback(const ScreenRectCallback &callback);
#endif

    bool isInSystemButtons(const QPoint &pos, WindowAgentBase::SystemButton *button) const;
    bool isInTitleBarDraggableArea(const QPoint &pos) const;

    bool isCaptionClassName(const char *name) const;
    void addCaptionClassName(const QString &name);

    virtual QString key() const;

    enum WindowContextHook {
        CentralizeHook = 1,
        RaiseWindowHook,
        ShowSystemMenuHook,
        DefaultColorsHook,
        DrawWindows10BorderHook,       // Only works on Windows 10, emulated workaround
        DrawWindows10BorderHook2,      // Only works on Windows 10, native workaround
        SystemButtonAreaChangedHook,   // Only works on Mac
    };
    virtual void virtual_hook(int id, void *data);

    void showSystemMenu(const QPoint &pos);
    void notifyWinIdChange();

    virtual QVariant windowAttribute(const QString &key) const;
    virtual bool setWindowAttribute(const QString &key, const QVariant &attribute);
protected:
    virtual void winIdChanged() = 0;
    virtual bool windowAttributeChanged(const QString &key, const QVariant &attribute, const QVariant &oldAttribute);
protected:
    QObject *m_host{};
    std::unique_ptr<WindowItemDelegate> m_delegate;
    QWindow *m_windowHandle{};

    QSet<const QObject *> m_hitTestVisibleItems;
    QList<QString> m_captionClassNameList;
#ifdef Q_OS_MAC
    ScreenRectCallback m_systemButtonAreaCallback;
#endif

    QObject *m_titleBar{};
    std::array<QObject *, WindowAgentBase::Close + 1> m_systemButtons{};

    QVariantHash m_windowAttributes;

    std::unique_ptr<QObject> m_windowEventFilter;
    std::unique_ptr<QObject> m_winIdChangeEventFilter;
};

inline QObject *WindowContext::host() const
{
    return m_host;
}

inline QWindow *WindowContext::window() const
{
    return m_windowHandle;
}

inline WindowItemDelegate *WindowContext::delegate() const
{
    return m_delegate.get();
}

inline bool WindowContext::isHitTestVisible(const QObject *obj) const
{
    return m_hitTestVisibleItems.contains(obj);
}

inline QObject *WindowContext::systemButton(WindowAgentBase::SystemButton button) const
{
    return m_systemButtons[button];
}

inline QObject *WindowContext::titleBar() const
{
    return m_titleBar;
}

#ifdef Q_OS_MAC
inline ScreenRectCallback WindowContext::systemButtonAreaCallback() const
{
    return m_systemButtonAreaCallback;
}
#endif

QX_WINDOW_END_NAMESPACE

#if defined(Q_OS_WINDOWS) && defined(QX_WINDOW_NATIVE)

#include "windowkit_win.h"

QX_WINDOW_BEGIN_NAMESPACE

class WindowContextWin : public WindowContext
{
    Q_OBJECT
public:
    WindowContextWin();
    ~WindowContextWin() override;

    enum WindowPart {
        Outside,
        ClientArea,
        ChromeButton,
        ResizeBorder,
        FixedBorder,
        TitleBar,
    };

    QString key() const override;
    void virtual_hook(int id, void *data) override;

    QVariant windowAttribute(const QString &key) const override;
protected:
    void winIdChanged() override;
    bool windowAttributeChanged(const QString &key, const QVariant &attribute, const QVariant &oldAttribute) override;
public:
    bool windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *result);

    bool systemMenuHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *result);

    // In order to perfectly apply Windows 11 Snap Layout into the Qt window, we need to
    // intercept and emulate most of the  mouse events, so that the processing logic
    // is quite complex. Simultaneously, in order to make the handling code of other
    // Windows messages clearer, we have separated them into this function.
    bool snapLayoutHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *result);

    bool customWindowHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *result);

    bool nonClientCalcSizeHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT *result);
protected:
    WId windowId = 0;

    // The last hit test result, helpful to handle WM_MOUSEMOVE and WM_NCMOUSELEAVE.
    WindowPart lastHitTestResult = WindowPart::Outside;

    // Whether the last mouse leave message is blocked, mainly for handling the unexpected
    // WM_MOUSELEAVE.
    bool mouseLeaveBlocked = false;

    bool initialCentered = false;
};

QX_WINDOW_END_NAMESPACE

#elif defined(Q_OS_MAC) && defined(QX_WINDOW_NATIVE)

QX_WINDOW_BEGIN_NAMESPACE

class WindowContextCocoa : public WindowContext
{
    Q_OBJECT
public:
    WindowContextCocoa();
    ~WindowContextCocoa() override;

    QString key() const override;
    void virtual_hook(int id, void *data) override;

    QVariant windowAttribute(const QString &key) const override;
protected:
    void winIdChanged() override;
    bool windowAttributeChanged(const QString &key, const QVariant &attribute, const QVariant &oldAttribute) override;
protected:
    WId windowId = 0;

    std::unique_ptr<WindowSharedEventFilter> cocoaWindowEventFilter;
};

QX_WINDOW_END_NAMESPACE

#else
QX_WINDOW_BEGIN_NAMESPACE

class WindowContextQt : public WindowContext
{
    Q_OBJECT
public:
    WindowContextQt();
    ~WindowContextQt() override;

    QString key() const override;
    void virtual_hook(int id, void *data) override;
protected:
    void winIdChanged() override;
protected:
    std::unique_ptr<WindowSharedEventFilter> qtWindowEventFilter;
};

QX_WINDOW_END_NAMESPACE

#endif

#endif   // WINDOWCONTEXT_P_H
