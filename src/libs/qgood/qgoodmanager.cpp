/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * SPDX-License-Identifier: MIT
 **/
#include "qgoodmanager.h"
#include "qgoodstyle.h"
#include <QApplication>

void QGoodManager::setAppDarkTheme()
{
    qApp->setStyle(new QGoodStyle(QGoodStyle::DarkStyle));
    qApp->style()->setObjectName("fusion");
}

void QGoodManager::setAppLightTheme()
{
    qApp->setStyle(new QGoodStyle(QGoodStyle::LightStyle));
    qApp->style()->setObjectName("fusion");
}
