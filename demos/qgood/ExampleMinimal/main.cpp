/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2018-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
**/

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QGoodWindow::setup();

    QApplication app(argc, argv);

    MainWindow mw;
    mw.show();

    return app.exec();
}
