/***************************************************************************
 **
 **  Copyright (C) 2018 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **
 **  This program is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  This program is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with this program.  If not, see http://www.gnu.org/licenses/.
 **
****************************************************************************/
#include "theme.h"
#include "qcanpool/qcanpool.h"
#include <QSettings>
#include <QFile>
#include "explorer.h"

QColor Theme::g_color;
QColor Theme::g_oddColor;
QColor Theme::g_evenColor;
QColor Theme::g_checkColor;
QColor Theme::g_modifiedColor;

QColor Theme::g_fancyBarHoverColor = FANCYBAR_HOVER_COLOR;
QColor Theme::g_fancyBarPressColor = FANCYBAR_PRESS_COLOR;
QColor Theme::g_fancyBarTextColor = FANCYBAR_TEXT_COLOR;
QColor Theme::g_fancyBarBackgroundColor = FANCYBAR_BACKGROUND_COLOR;
// FancyTab
QColor Theme::g_fancyTabHoverColor = FANCYTAB_HOVER_COLOR;
QColor Theme::g_fancyTabPressColor = FANCYTAB_PRESS_COLOR;
QColor Theme::g_fancyTabTextColor = FANCYTAB_TEXT_COLOR;
QColor Theme::g_fancyTabSelectedTextColor = FANCYTAB_SELECTED_TEXT_COLOR;
QColor Theme::g_fancyTabBackgroundColor = FANCYTAB_BACKGROUND_COLOR;
// TinyTab
QColor Theme::g_tinyTabHoverColor = TINYTAB_HOVER_COLOR;
QColor Theme::g_tinyTabPressColor = TINYTAB_PRESS_COLOR;
QColor Theme::g_tinyTabTextColor = TINYTAB_TEXT_COLOR;
QColor Theme::g_tinyTabSelectedTextColor = TINYTAB_SELECTED_TEXT_COLOR;
QColor Theme::g_tinyTabBackgroundColor = TINYTAB_BACKGROUND_COLOR;
QColor Theme::g_tinyTabNormalColor = TINYTAB_NORMAL_COLOR;
QColor Theme::g_tinyTabSlideColor = TINYTAB_SLIDE_COLOR;

void Theme::setClassicTheme()
{
    g_color = CLASSIC_COLOR;
    g_evenColor = CLASSIC_EVEN_COLOR;
    g_oddColor = CLASSIC_ODD_COLOR;
    g_checkColor = CLASSIC_CHECK_COLOR;
    g_modifiedColor = CLASSIC_MODIFIED_COLOR;
}

void Theme::setFlatdarkTheme()
{
    g_color = FLATDARK_COLOR;
    g_evenColor = FLATDARK_EVEN_COLOR;
    g_oddColor = FLATDARK_ODD_COLOR;
    g_checkColor = FLATDARK_CHECK_COLOR;
    g_modifiedColor = FLATDARK_MODIFIED_COLOR;
}

void Theme::init()
{
    QString theme = themeFileName();
    if(theme.isEmpty()){
        QFileInfoList themeFiles = Explorer::getThemeFiles();
        foreach (QFileInfo info, themeFiles) {
            if(info.baseName() == "default"){
                theme = info.absoluteFilePath();
            }
        }
    }
    loadTheme(theme);
}

QString Theme::themeFileName()
{
    QSettings settings("Canpool", "qtcanpool");
    QString theme = settings.value("theme").toString();
    return theme;
}

QString Theme::themeName(const QString &fileName)
{
    if(!QFile::exists(fileName)) return QString("");

    QSettings settings(fileName, QSettings::IniFormat);
    settings.beginGroup("Name");
    QString themeName = settings.value("ThemeName").toString();
    settings.endGroup();
    return themeName;
}

void Theme::setTheme(const QString &fileName)
{
    QSettings settings("Canpool", "qtcanpool");
    settings.setValue("theme", fileName);
    loadTheme(fileName);
}

void Theme::loadTheme(const QString &fileName)
{
    if(!QFile::exists(fileName)) return;

    QSettings settings(fileName, QSettings::IniFormat);
    settings.beginGroup("Colors");

    g_fancyBarHoverColor = QCanpool::argbToColor(settings.value("FancyBarHoverColor").toString());
    g_fancyBarPressColor = QCanpool::argbToColor(settings.value("FancyBarPressColor").toString());
    g_fancyBarTextColor = QCanpool::argbToColor(settings.value("FancyBarTextColor").toString());
    g_fancyBarBackgroundColor = QCanpool::argbToColor(settings.value("FancyBarBackgroundColor").toString());

    g_fancyTabHoverColor = QCanpool::argbToColor(settings.value("FancyTabHoverColor").toString());
    g_fancyTabPressColor = QCanpool::argbToColor(settings.value("FancyTabPressColor").toString());
    g_fancyTabTextColor = QCanpool::argbToColor(settings.value("FancyTabTextColor").toString());
    g_fancyTabSelectedTextColor = QCanpool::argbToColor(settings.value("FancyTabSelectedTextColor").toString());
    g_fancyTabBackgroundColor = QCanpool::argbToColor(settings.value("FancyTabBackgroundColor").toString());

    g_tinyTabHoverColor = QCanpool::argbToColor(settings.value("TinyTabHoverColor").toString());
    g_tinyTabPressColor = QCanpool::argbToColor(settings.value("TinyTabPressColor").toString());
    g_tinyTabTextColor = QCanpool::argbToColor(settings.value("TinyTabTextColor").toString());
    g_tinyTabSelectedTextColor = QCanpool::argbToColor(settings.value("TinyTabSelectedTextColor").toString());
    g_tinyTabBackgroundColor = QCanpool::argbToColor(settings.value("TinyTabBackgroundColor").toString());
    g_tinyTabNormalColor = QCanpool::argbToColor(settings.value("TinyTabNormalColor").toString());
    g_tinyTabSlideColor = QCanpool::argbToColor(settings.value("TinyTabSlideColor").toString());
    settings.endGroup();
}
