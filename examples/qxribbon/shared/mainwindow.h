/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qxribbon/ribbonmainwindow.h"

class MainWindow : public QxRibbon::RibbonMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif   // MAINWINDOW_H
