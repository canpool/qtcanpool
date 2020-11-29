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
#include "fancywindow.h"
#include "fancybar.h"
#include "fancyscreen.h"
#include "qcanpool_p.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QStatusBar>
#include <QSettings>
#include <QPainter>

QCANPOOL_BEGIN_NAMESPACE

class FancyWindowPrivate
{
public:
    FancyWindowPrivate();

    FancyBar *fancyBar;
};


FancyWindowPrivate::FancyWindowPrivate()
{
    fancyBar = nullptr;
}

FancyWindow::FancyWindow(QWidget *parent)
    : QMainWindow(parent), d(new FancyWindowPrivate())
{
    QMainWindow::setWindowFlags(Qt::FramelessWindowHint
                   | Qt::WindowSystemMenuHint
                   | Qt::WindowMinimizeButtonHint
                   | Qt::WindowMaximizeButtonHint
                   | Qt::Window
                  );
    d->fancyBar = new FancyBar(this);
    connect(d->fancyBar, SIGNAL(maximizationChanged(bool)), this, SIGNAL(resizable(bool)));
    setMenuWidget(d->fancyBar);
    setMouseTracking(true);
    QRect geom = FancyScreen::normalRect();
    resize(geom.width(), geom.height());
    raise();
    activateWindow();
}

FancyWindow::~FancyWindow()
{
    delete d;
}

FancyBar *FancyWindow::fancyBar() const
{
    return d->fancyBar;
}

void FancyWindow::setFixedSize(const QSize &s)
{
    setFixedSize(s.width(), s.height());
}

void FancyWindow::setFixedSize(int w, int h)
{
    d->fancyBar->setWidgetResizable(false);
    QWidget::setFixedSize(w, h);
}

void FancyWindow::setFixedWidth(int w)
{
    setFixedSize(w, this->height());
}

void FancyWindow::setFixedHeight(int h)
{
    setFixedSize(this->width(), h);
}

void FancyWindow::setWindowFlags(Qt::WindowFlags type)
{
    QMainWindow::setWindowFlags(type);
    d->fancyBar->updateWidgetFlags();
}

void FancyWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QSettings settings(QCanpoolPrivate::g_settingsOrganization,
                       QCanpoolPrivate::g_settingsApplication);
    QString skinName = settings.value("skin").toString();

    if (skinName.isEmpty()) {
        skinName = QString(":/skin/default");
    }

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::gray);
    painter.drawPixmap(QRect(0, 0, this->width(), this->height()),
                       QPixmap(skinName));
}

QCANPOOL_END_NAMESPACE
