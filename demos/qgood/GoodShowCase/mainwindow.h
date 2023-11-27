/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2022-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qgood/qgoodcentralwidget.h"
#include "qgood/qgoodwindow.h"
#include "ui_centralwidget.h"
#include <QMainWindow>

class CentralWidget : public QMainWindow
{
    Q_OBJECT
public:
    explicit CentralWidget(QWidget *parent = nullptr) : QMainWindow(parent), ui(new Ui::CentralWidget)
    {
        ui->setupUi(this);
    }

    ~CentralWidget()
    {
        delete ui;
    }

    Ui::CentralWidget *ui;
};

class MainWindow : public QGoodWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private Q_SLOTS:
    void themeChange();
private:
    bool nativeEvent(const QByteArray &eventType, void *message, qgoodintptr *result);
    void closeEvent(QCloseEvent *event);
private:
    QGoodCentralWidget *m_goodCentralWidget;
    CentralWidget *m_centralWidget;
};

#endif   // MAINWINDOW_H
