/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxwidget_global.h"

#include <QObject>
#include <QIcon>

class QToolBar;
class QAction;

QX_WIDGET_BEGIN_NAMESPACE

class WindowToolBar;

class WindowToolBarPrivate : public QObject
{
    Q_OBJECT
    QX_DECLARE_PUBLIC(WindowToolBar)
public:
    WindowToolBarPrivate();

    void init();
    void windowStateChange(QObject *obj);
    void setWindowStates(Qt::WindowStates states);

public slots:
    void buttonClicked();
public:
    QToolBar *m_toolBar;
    QAction *m_minimizeAction;
    QAction *m_maximizeAction;
    QAction *m_closeAction;
    QIcon m_maximizeIcon;
    QIcon m_normalIcon;
    QString m_maximizeTip;
    QString m_normalTip;
};

QX_WIDGET_END_NAMESPACE
