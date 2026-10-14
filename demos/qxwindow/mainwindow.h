/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qxwindow/qxwindow_global.h"
#include <QtWidgets/QMainWindow>

QT_BEGIN_NAMESPACE
class QLabel;
class QMenuBar;
QT_END_NAMESPACE

QX_WINDOW_BEGIN_NAMESPACE
class WindowAgentWidget;
QX_WINDOW_END_NAMESPACE

class TitleButton;

/*
 * The window the agent is installed on, with a title bar of the demo's own.
 *
 * The title bar is a plain QFrame: the agent is told which widget it is, and
 * everything in it - a menu bar, a title label, four buttons - is built here.
 * That is the whole shape of the API, and this demo exists to show it, because
 * no other sample in the tree draws its own bar (RibbonAppWindow brings the
 * ribbon's, and everything else opens a RibbonMainWindow or a QxAppShell).
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
protected:
    bool event(QEvent *event) override;
private:
    enum Theme {
        Dark,
        Light,
    };

    void installWindowAgent();
    void buildTitleBar();
    QMenuBar *buildMenuBar();
    void loadStyleSheet(Theme theme);

    Theme m_theme = Dark;

    QX_WINDOW_PREPEND_NAMESPACE(WindowAgentWidget) *m_windowAgent = nullptr;

    QWidget *m_titleBar = nullptr;
    QMenuBar *m_menuBar = nullptr;
    QLabel *m_titleLabel = nullptr;
    TitleButton *m_iconButton = nullptr;
    TitleButton *m_minButton = nullptr;
    TitleButton *m_maxButton = nullptr;
    TitleButton *m_closeButton = nullptr;
};

#endif   // MAINWINDOW_H
