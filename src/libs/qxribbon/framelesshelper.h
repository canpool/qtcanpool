/**
 * Copyleft (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include <QObject>
#include <QAbstractNativeEventFilter>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#define QTRESULT qintptr
#else
#define QTRESULT long
#endif   // QTRESULT

class QWidget;
class FramelessHelperPrivate;

/*
    Deprecated in 4.2 (K27). FramelessHelper is the frameless backend of
    RibbonWindow and has no other user in this repository.

    It exists on purpose: it is what lets qxribbon offer a frameless window
    without depending on qxwindow, and that layering is deliberate - qxribbon
    links Qt and nothing else (see doc/pages/architecture.md). qxwindow is the
    better implementation, and the answer for it is RibbonAppWindow in qxapp:
    it is a sibling of RibbonWindow - the same RibbonMainWindow base, qxwindow's
    WindowAgentWidget instead of this class - which keeps the dependency out of
    qxribbon while still shipping a frameless ribbon window.

    What this class builds on is Qt::FramelessWindowHint; the hit testing, the
    move and the resize are simulated on top of it. On Windows with Qt 5 that is
    backed by the native path (FRAMELESS_USE_NATIVE); everywhere else - and on
    every Qt 6 platform - the simulation is what runs, and it is the incomplete
    half: a window shown maximised for the first time cannot be restored, and on
    Linux the geometry Qt reports is not usable. See doc/pages/limitations.md,
    "a frameless window on Windows, Qt 6, maximised on first show" and "frameless
    windows on Linux".

    So it stays as long as Qt 5 does. New code, and every Qt 6 application,
    should use QxApp::RibbonAppWindow. RibbonWindow is RibbonMainWindow plus this
    backend and nothing else, so the two go together - what that means for
    callers is worked out in doc/pages/migration.md, section "4.2 起弃用".
    Removal needs at least one release of warning first (K26), so the marker is
    the whole change here.
*/
#define QX_RIBBON_FRAMELESSHELPER_DEPRECATED                                                                           \
    QT_DEPRECATED_X("FramelessHelper is the Qt 5 frameless backend of RibbonWindow; use QxApp::RibbonAppWindow on Qt 6")

class QX_RIBBON_FRAMELESSHELPER_DEPRECATED FramelessHelper : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit FramelessHelper(QObject *parent);
    ~FramelessHelper();

    void addWidget(QWidget *w);
    void removeWidget(QWidget *w);

    bool widgetResizable() const;
    void setWidgetResizable(bool resizable);

    bool widgetMovable() const;
    void setWidgetMovable(bool movable);

    int titleHeight() const;
    void setTitleHeight(int height);

    int borderWidth() const;
    void setBorderWidth(int width);

    void addCaptionClassName(const QString &name);

Q_SIGNALS:
    void windowIconChanged(const QIcon &icon);
    void windowTitleChanged(const QString &title);
    void windowStateChanged(Qt::WindowStates state);
protected:
    virtual bool eventFilter(QObject *object, QEvent *event);
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, QTRESULT *result);
private:
    FramelessHelperPrivate *d;
};
