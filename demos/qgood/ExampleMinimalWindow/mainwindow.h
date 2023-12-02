/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2022-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qgoodbye/qgoodcentralwidget.h"
#include "qgood/qgoodwindow.h"

class QLabel;

class MainWindow : public QGoodWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    QGoodCentralWidget *m_goodCentralWidget;
    QLabel *m_label;
};

#endif   // MAINWINDOW_H
