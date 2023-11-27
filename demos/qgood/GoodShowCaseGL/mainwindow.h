/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2018-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qgood/qgoodcentralwidget.h"
#include "qgood/qgoodwindow.h"

#include "glwidget.h"

class MainWindow : public QGoodWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void closeEvent(QCloseEvent *event);
private:
    QGoodCentralWidget *m_goodCentralWidget;
};

#endif   // MAINWINDOW_H
