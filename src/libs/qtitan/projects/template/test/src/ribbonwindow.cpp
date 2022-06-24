/***************************************************************************
 **
 **  Copyright (C) 2022 MaMinJie <canpool@163.com>
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
#include <QApplication>
#include <QDesktopWidget>
#include <QSettings>
#include "ribbonwindow.h"


/* RibbonWindow */
RibbonWindow::RibbonWindow(QWidget* parent)
    : Qtitan::RibbonMainWindow(parent)
{
    setWindowTitle("Qtitan Project template");
    RibbonBar* ribbonBar = new RibbonBar(this);
    setMenuBar(ribbonBar);
    ribbonBar->addPage(tr("&Buttons_1"));
    ribbonBar->addPage(tr("&Buttons_2"));

    ribbonBar->setFrameThemeEnabled();

    QRect geom = QApplication::desktop()->availableGeometry(this);
    resize(2 * geom.width() / 3, 2 * geom.height() / 3);

    OfficeStyle* style = new OfficeStyle();
    style->setTheme(OfficeStyle::Office2010Blue);
    qApp->setStyle(style);
}

RibbonWindow::~RibbonWindow()
{

}


