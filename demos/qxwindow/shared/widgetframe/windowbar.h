/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
**/
#ifndef WINDOWBAR_H
#define WINDOWBAR_H

#include <QAbstractButton>
#include <QFrame>
#include <QLabel>
#include <QMenuBar>
#include <qxwindow/qxwindow_global.h>

QX_WINDOW_BEGIN_NAMESPACE

class WindowBarPrivate;

class WindowBar : public QFrame
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WindowBar)
public:
    explicit WindowBar(QWidget *parent = nullptr);
    ~WindowBar();
public:
    QMenuBar *menuBar() const;
    QLabel *titleLabel() const;
    QAbstractButton *iconButton() const;
    QAbstractButton *minButton() const;
    QAbstractButton *maxButton() const;
    QAbstractButton *closeButton() const;

    void setMenuBar(QMenuBar *menuBar);
    void setTitleLabel(QLabel *label);
    void setIconButton(QAbstractButton *btn);
    void setMinButton(QAbstractButton *btn);
    void setMaxButton(QAbstractButton *btn);
    void setCloseButton(QAbstractButton *btn);

    QMenuBar *takeMenuBar();
    QLabel *takeTitleLabel();
    QAbstractButton *takeIconButton();
    QAbstractButton *takeMinButton();
    QAbstractButton *takeMaxButton();
    QAbstractButton *takeCloseButton();

    QWidget *hostWidget() const;
    void setHostWidget(QWidget *w);

    bool titleFollowWindow() const;
    void setTitleFollowWindow(bool value);

    bool iconFollowWindow() const;
    void setIconFollowWindow(bool value);

Q_SIGNALS:
    void minimizeRequested();
    void maximizeRequested(bool max = false);
    void closeRequested();
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

    virtual void titleChanged(const QString &text);
    virtual void iconChanged(const QIcon &icon);
protected:
    WindowBar(WindowBarPrivate &d, QWidget *parent = nullptr);

    QScopedPointer<WindowBarPrivate> d_ptr;
};

QX_WINDOW_END_NAMESPACE

#endif   // WINDOWBAR_H
