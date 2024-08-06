/**
 * Copyleft (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include <QPushButton>

QX_RIBBON_BEGIN_NAMESPACE

class WindowButtonGroup;

class WindowButton : public QPushButton
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

    void setupMinimizeButton(bool on);
    void setupMaximizeButton(bool on);
    void setupCloseButton(bool on);
    void updateSize();
    void resize(QSize size);
    QSize sizeHint() const;
    int groupHeight() const;
public slots:
    void buttonClicked();
public:
    WindowButton *m_closeButton;
    WindowButton *m_minimizeButton;
    WindowButton *m_maximizeButton;
    int m_closeStretch;
    int m_maxStretch;
    int m_minStretch;
    int m_windowBorder;
    qreal m_iconScale;
    bool m_signalEnabled;
};

QX_RIBBON_END_NAMESPACE

