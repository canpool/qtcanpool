/***************************************************************************
 **
 **  Copyright (C) 2018-2020 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **           https://gitee.com/icanpool
 **
 **  GNU Lesser General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU Lesser
 **  General Public License version 3 as published by the Free Software
 **  Foundation and appearing in the file LICENSE.LGPL3 included in the
 **  packaging of this file. Please review the following information to
 **  ensure the GNU Lesser General Public License version 3 requirements
 **  will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
 **
 **  GNU General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU
 **  General Public License version 2.0 or (at your option) the GNU General
 **  Public license version 3 or any later version approved by the KDE Free
 **  Qt Foundation. The licenses are as published by the Free Software
 **  Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
 **  included in the packaging of this file. Please review the following
 **  information to ensure the GNU General Public License requirements will
 **  be met: https://www.gnu.org/licenses/gpl-2.0.html and
 **  https://www.gnu.org/licenses/gpl-3.0.html.
 **
****************************************************************************/
#include "qcanpool.h"
#include "qcanpool_p.h"
#include <QColor>
#include <QTextCodec>

#if defined(Q_OS_WIN32) && !defined(Q_CC_MSVC)
#include <Windows.h>
#include <shellapi.h>
#else
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#endif

namespace QCanpool {

QColor argbToColor(const QString &argb)
{
    QColor color;
    bool ok = true;
    QRgb rgba = argb.toUInt(&ok, 16);

    if (ok) {
        color = QColor::fromRgba(rgba);
    } else {
        color = QColor(0, 0, 0);
    }

    return color;
}

QString colorToArgb(const QColor &color)
{
    QString argb =  QString("%1%2%3%4")
                    .arg(color.alpha(), 2, 16, QLatin1Char('0'))
                    .arg(color.red(), 2, 16, QLatin1Char('0'))
                    .arg(color.green(), 2, 16, QLatin1Char('0'))
                    .arg(color.blue(), 2, 16, QLatin1Char('0'));
    return argb;
}

void showInExplorer(QString fileName)
{
    Q_UNUSED(fileName);
#if defined(Q_OS_WIN32) && !defined(Q_CC_MSVC)
    QString argsFile = QString("/select, %1").arg(fileName.replace("/", "\\"));
    QTextCodec *codec = QTextCodec::codecForName("GB18030");
    ShellExecuteA(nullptr, "open", "explorer.exe", codec->fromUnicode(argsFile).constData(), nullptr, SW_SHOWDEFAULT);
#else
    QDesktopServices::openUrl(QUrl(QFileInfo(fileName).absolutePath(), QUrl::TolerantMode));
#endif
}

void setSysSettings(const QString &organization, const QString &application)
{
    QCanpoolPrivate::g_settingsOrganization = organization;
    QCanpoolPrivate::g_settingsApplication  = application;
}

} // namespace QCanpool
