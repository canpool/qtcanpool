/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef TITLEBUTTON_H
#define TITLEBUTTON_H

#include <QtWidgets/QToolButton>

/*
 * A button of the demo's title bar.
 *
 * Minimize, maximize and close only ever need clicked(), which QToolButton
 * already has. The window icon needs one thing more - the system menu on a
 * single click and close on a double click - and a double click is only reported
 * by a class that overrides mouseDoubleClickEvent(). This adds that signal and
 * nothing else, which is also the point it makes about the agent: a system button
 * is any QWidget, so a demo is free to bring its own.
 */
class TitleButton : public QToolButton
{
    Q_OBJECT
public:
    explicit TitleButton(QWidget *parent = nullptr);

Q_SIGNALS:
    void doubleClicked();
protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;
};

#endif   // TITLEBUTTON_H
