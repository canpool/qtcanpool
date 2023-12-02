/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qgoodbye/qgoodcentralwidget.h"
#include "qgood/qgoodwindow.h"

class QSettings;
class QLabel;

class MainWindow : public QGoodWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public Q_SLOTS:
    void show();

private Q_SLOTS:
    void adjustSizeLabel();
private:
    bool event(QEvent *event);
    void closeEvent(QCloseEvent *event);
private:
    QSettings *m_settings;
    QGoodCentralWidget *m_goodCentralWidget;
    QLabel *m_label;
};

#endif   // MAINWINDOW_H
