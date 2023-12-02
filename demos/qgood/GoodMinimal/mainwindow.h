/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2018-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qgood/qgoodwindow.h"

class MainWindow : public QGoodWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};

#endif   // MAINWINDOW_H
