/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
**/
#ifndef WIDGETWINDOWAGENT_H
#define WIDGETWINDOWAGENT_H

#include "windowagentbase.h"
#include "qxwindow_global.h"
#include <QtWidgets/QWidget>

QX_WINDOW_BEGIN_NAMESPACE

class WidgetWindowAgentPrivate;

class QX_WINDOW_EXPORT WidgetWindowAgent : public WindowAgentBase
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WidgetWindowAgent)
public:
    explicit WidgetWindowAgent(QObject *parent = nullptr);
    ~WidgetWindowAgent() override;
public:
    bool setup(QWidget *w);

    QWidget *titleBar() const;
    void setTitleBar(QWidget *w);

    QWidget *systemButton(SystemButton button) const;
    void setSystemButton(SystemButton button, QWidget *w);

#ifdef Q_OS_MAC
    // The system button area APIs are experimental, very likely to change in the future.
    QWidget *systemButtonArea() const;
    void setSystemButtonArea(QWidget *widget);

    ScreenRectCallback systemButtonAreaCallback() const;
    void setSystemButtonAreaCallback(const ScreenRectCallback &callback);
#endif

    bool isHitTestVisible(const QWidget *w) const;
    void setHitTestVisible(const QWidget *w, bool visible = true);

Q_SIGNALS:
    void titleBarChanged(const QWidget *w);
    void systemButtonChanged(SystemButton button, const QWidget *w);
protected:
    WidgetWindowAgent(WidgetWindowAgentPrivate &d, QObject *parent = nullptr);
};

QX_WINDOW_END_NAMESPACE

#endif   // WIDGETWINDOWAGENT_H