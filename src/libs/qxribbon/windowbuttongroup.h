/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxribbon_global.h"
#include <QPushButton>
#include <QWidget>

QX_RIBBON_BEGIN_NAMESPACE

class WindowButtonGroupPrivate;

class WindowButtonGroup : public QWidget
{
    Q_OBJECT
public:
    explicit WindowButtonGroup(QWidget *parent, Qt::WindowFlags flags = Qt::WindowFlags());
    ~WindowButtonGroup();

    void setupMinimizeButton(bool on);
    void setupMaximizeButton(bool on);
    void setupCloseButton(bool on);

    void updateWindowFlags(Qt::WindowFlags flags);

    void setButtonWidthStretch(int close = 4, int max = 3, int min = 3);
    void setIconScale(qreal iconscale = 0.5);
    void setWindowStates(Qt::WindowStates s);

    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
protected:
    virtual bool eventFilter(QObject *watched, QEvent *e) Q_DECL_OVERRIDE;
    virtual void parentResize();
    virtual void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
protected slots:
    Q_SLOT void closeWindow();
    Q_SLOT void minimizeWindow();
    Q_SLOT void maximizeWindow();
private:
    QX_DECLARE_PRIVATE(WindowButtonGroup)
};

class WindowToolButton : public QPushButton
{
    Q_OBJECT
public:
    explicit WindowToolButton(QWidget *p = Q_NULLPTR);
};

QX_RIBBON_END_NAMESPACE
