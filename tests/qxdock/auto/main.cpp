/**
 * Copyright (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include <QApplication>

#include "tst_global.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Returns the number of failed test objects, so CTest fails when a test does.
    TEST_RUN_ALL();
}
