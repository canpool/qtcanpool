/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * SPDX-License-Identifier: MIT
 **/
#ifndef QGOODMANAGER_H
#define QGOODMANAGER_H

#include "qgood_global.h"

class QGOOD_SHARED_EXPORT QGoodManager
{
public:
    /** Set the app theme to the dark theme. */
    static void setAppDarkTheme();

    /** Set the app theme to the light theme. */
    static void setAppLightTheme();
};

#endif   // QGOODMANAGER_H
