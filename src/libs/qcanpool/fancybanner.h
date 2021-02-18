/***************************************************************************
 **
 **  Copyright (C) 2018-2021 MaMinJie <canpool@163.com>
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
#ifndef FANCYBANNER_H
#define FANCYBANNER_H

#include <QWidget>
#include "qcanpool_global.h"

QCANPOOL_BEGIN_NAMESPACE

class FancyBannerPrivate;
class QCANPOOL_SHARED_EXPORT FancyBanner : public QWidget
{
    Q_OBJECT
public:
    explicit FancyBanner(QWidget *parent = nullptr);

    void addPage(QPixmap pixmap);
    void addPages(QList<QPixmap> pixmaps);

    void startAutoPlay();
    void stopAutoPlay();
    void setDelayTime(int msec, bool start = false);

    void setIndicatorFrontColor(QColor color);
    void setIndicatorBackColor(QColor color);

    void setPageSize(int w, int h);

signals:
    void currentClicked(int index);

protected:
    void paintEvent(QPaintEvent *event);

private:
    FancyBannerPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYBANNER_H
