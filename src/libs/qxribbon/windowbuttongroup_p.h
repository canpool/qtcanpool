/**
 * Copyleft (C) 2023-2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include <QToolButton>

QX_RIBBON_BEGIN_NAMESPACE

class WindowButtonGroup;

class WindowButton : public QToolButton
{
    Q_OBJECT
public:
    explicit WindowButton(QWidget *p = Q_NULLPTR);
};

class WindowButtonGroupPrivate : public QObject
{
    Q_OBJECT
    QX_DECLARE_PUBLIC(WindowButtonGroup)
public:
    WindowButtonGroupPrivate();

    void init();
    void updateButton(Qt::WindowFlags flags);
    void updateButtonIconSize();
    QSize sizeHint() const;
    int groupHeight() const;

public Q_SLOTS:
    void buttonClicked();

public:
    WindowButton *m_closeButton;
    WindowButton *m_minimizeButton;
    WindowButton *m_maximizeButton;
    qreal m_iconScale;
    bool m_signalEnabled;
};

QX_RIBBON_END_NAMESPACE

