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
#include "qcanpool.h"
#include <QColor>
#include <QTextCodec>

#if defined(Q_OS_WIN32)
#include <Windows.h>
#include <ShellAPI.h>
#else
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#endif

QColor QCanpool::argbToColor(const QString &argb)
{
    QColor color;
    bool ok = true;
    QRgb rgba = argb.toLongLong(&ok, 16);
    if(ok){
        color = QColor::fromRgba(rgba);
    }else{
        color = QColor(0,0,0);
    }
    return color;
}

QString QCanpool::colorToArgb(const QColor &color)
{
    QString argb =  QString("%1%2%3%4")
            .arg(color.alpha(), 2, 16, QLatin1Char('0'))
            .arg(color.red(), 2, 16, QLatin1Char('0'))
            .arg(color.green(), 2, 16, QLatin1Char('0'))
            .arg(color.blue(), 2, 16, QLatin1Char('0'));
    return argb;
}

void QCanpool::showInExplorer(QString fileName)
{
    Q_UNUSED(fileName);
#if defined(Q_OS_WIN32)
#if !defined(Q_CC_MSVC)
        QString argsFile = QString("/select, %1").arg(fileName.replace("/","\\"));
        QTextCodec *codec = QTextCodec::codecForName("GB18030");
        ShellExecuteA(0,"open","explorer.exe",codec->fromUnicode(argsFile).constData(),NULL,SW_SHOWDEFAULT);
#endif
#else
        QDesktopServices::openUrl(QUrl(QFileInfo(fileName).absolutePath(), QUrl::TolerantMode));
#endif
}

