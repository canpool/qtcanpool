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
#include "skinitem.h"
#include <QMouseEvent>
#include <QPainter>

QCANPOOL_BEGIN_NAMESPACE

#ifndef SHADOW_WIDTH
#define SHADOW_WIDTH    5
#endif
#define BORDER_COLOR    QColor(255, 102, 0)

class SkinItemPrivate
{
public:
    SkinItemPrivate();

    QString m_pixmapName;
    QPixmap m_fitPixmap;

    bool m_bMousePress;
    bool m_bMouseEnter;

    bool m_isSelected;
};

SkinItemPrivate::SkinItemPrivate()
    : m_bMousePress(false),
      m_bMouseEnter(false),
      m_isSelected(false)
{
}

SkinItem::SkinItem(QWidget *parent)
    : QWidget(parent), d(new SkinItemPrivate())
{
}

SkinItem::~SkinItem()
{
    delete d;
}

void SkinItem::setPixmapName(const QString &name)
{
    if (d->m_pixmapName == name) {
        return;
    }
    d->m_pixmapName = name;
    QPixmap pixmap(name);
    d->m_fitPixmap = pixmap.scaled(300, 200, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

QString SkinItem::pixmapName() const
{
    return d->m_pixmapName;
}

void SkinItem::select(bool selected)
{
    d->m_isSelected  = selected;
    update();
}

void SkinItem::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        d->m_bMousePress = true;
    }
}

void SkinItem::mouseReleaseEvent(QMouseEvent *event)
{
    if (d->m_bMousePress && (this->rect()).contains(event->pos())) {
        emit clicked();
    }

    d->m_bMousePress = false;
}

void SkinItem::enterEvent(QEvent *)
{
    d->m_bMouseEnter = true;
    update();
}

void SkinItem::leaveEvent(QEvent *)
{
    d->m_bMouseEnter = false;
    update();
}

void SkinItem::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::gray);
    painter.drawPixmap(QRect(SHADOW_WIDTH, SHADOW_WIDTH,
                             this->width() - 2 * SHADOW_WIDTH,
                             this->height() - 2 * SHADOW_WIDTH),
                       d->m_fitPixmap);
    painter.restore();
    painter.save();

    if (d->m_bMouseEnter || d->m_isSelected) {
        QPen pen(BORDER_COLOR);
        pen.setWidth(5);
        painter.setPen(pen);
        painter.drawRoundedRect(0, 0, this->width() - 1, this->height() - 1, 5, 5);

        if (d->m_isSelected) {
            QString state = QString(":/skin/state_ok");
            QPixmap pixmap(state);
            painter.drawPixmap(QRect(this->width() - pixmap.width() - 10,
                                     this->height() - pixmap.height() - 10,
                                     pixmap.width(),
                                     pixmap.height()),
                               pixmap);
        }
    }

    painter.restore();
}

QCANPOOL_END_NAMESPACE
