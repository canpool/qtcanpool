/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbontheme.h"

#include <QApplication>
#include <QWidget>
#include <QFile>

QX_RIBBON_BEGIN_NAMESPACE

void RibbonTheme::loadTheme(const QString &themeFile, QWidget *widget)
{
    QFile file(themeFile);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QString css = QString::fromUtf8(file.readAll());
    if (widget) {
        widget->setStyleSheet(css);
    } else {
        qApp->setStyleSheet(css);
    }

    file.close();
}

void RibbonTheme::setTheme(int themeStyle, QWidget *widget)
{
    switch (themeStyle) {
    case LightOffice2013Theme:
        loadTheme(":/qxribbon/res/stylesheets/light_office2013.css", widget);
        break;
    case LightClassicTheme:
        loadTheme(":/qxribbon/res/stylesheets/light_classic.css", widget);
        break;
    case LightFancyTheme:
        loadTheme(":/qxribbon/res/stylesheets/light_fancy.css", widget);
        break;
    case DarkWpsTheme:
        loadTheme(":/qxribbon/res/stylesheets/dark_wps.css", widget);
        break;
    case DarkOfficePlusTheme:
        loadTheme(":/qxribbon/res/stylesheets/dark_officeplus.css", widget);
        break;
    case LightYellowTheme:
        loadTheme(":/qxribbon/res/stylesheets/light_yellow.css", widget);
        break;
    case CustomTheme:
        // "No widget means the application" is what loadTheme() above promises, and what
        // the nullptr default of the widget argument says. Dereferencing the pointer
        // unconditionally turned the documented way of calling this - setTheme(CustomTheme)
        // with no widget - into a crash on a null pointer.
        if (widget) {
            widget->setStyleSheet("");
        } else {
            qApp->setStyleSheet("");
        }
        break;
    default:
        break;
    }
}

QX_RIBBON_END_NAMESPACE
