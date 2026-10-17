/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include <QCoreApplication>

#include "tst_global.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // Returns the number of failed test objects, so CTest fails when a test does.
    TEST_RUN_ALL();
}
