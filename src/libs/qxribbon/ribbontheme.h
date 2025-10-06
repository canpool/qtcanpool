/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "qxribbon_global.h"
#include <QObject>

class QWidget;

QX_RIBBON_BEGIN_NAMESPACE

class QX_RIBBON_EXPORT RibbonTheme : public QObject
{
    Q_OBJECT
public:
    enum ThemeStyle {
        LightYellowTheme,
        LightOffice2013Theme,
        LightClassicTheme,
        LightFancyTheme,
        DarkWpsTheme,
        DarkOfficePlusTheme,
        CustomTheme,
    };
    Q_ENUM(ThemeStyle)
public:
    /** If the widget is nullptr, css/qss will be applied to QApplication */
    static void loadTheme(const QString &themeFile, QWidget *widget = nullptr);

    /** If you have a custom theme, please use loadTheme */
    static void setTheme(int themeStyle, QWidget *widget = nullptr);
};

QX_RIBBON_END_NAMESPACE
