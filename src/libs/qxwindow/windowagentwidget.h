/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
**/
#ifndef WINDOWAGENTWIDGET_H
#define WINDOWAGENTWIDGET_H

#include "windowagentbase.h"
#include "qxwindow_global.h"
#include <QtWidgets/QWidget>

QX_WINDOW_BEGIN_NAMESPACE

class WindowAgentWidgetPrivate;

class QX_WINDOW_EXPORT WindowAgentWidget : public WindowAgentBase
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WindowAgentWidget)
public:
    explicit WindowAgentWidget(QObject *parent = nullptr);
    ~WindowAgentWidget() override;
public:
    bool setup(QWidget *w);

    QWidget *titleBar() const;
    void setTitleBar(QWidget *w);

    QWidget *systemButton(SystemButton button) const;
    void setSystemButton(SystemButton button, QWidget *w);
    void removeSystemButton(SystemButton button);

#ifdef Q_OS_MAC
    // The system button area APIs are experimental, very likely to change in the future.
    QWidget *systemButtonArea() const;
    void setSystemButtonArea(QWidget *widget);

    ScreenRectCallback systemButtonAreaCallback() const;
    void setSystemButtonAreaCallback(const ScreenRectCallback &callback);
#endif

    bool isHitTestVisible(const QWidget *w) const;
    void setHitTestVisible(const QWidget *w, bool visible = true);

    void addCaptionClassName(const QString &name);

Q_SIGNALS:
    void titleBarChanged(const QWidget *w);
    void systemButtonChanged(SystemButton button, const QWidget *w);
protected:
    WindowAgentWidget(WindowAgentWidgetPrivate &d, QObject *parent = nullptr);
};

QX_WINDOW_END_NAMESPACE

#endif   // WINDOWAGENTWIDGET_H
