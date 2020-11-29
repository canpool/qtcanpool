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
#include "fancytab.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QAction>

QCANPOOL_BEGIN_NAMESPACE

class FancyTabPrivate
{
public:
    FancyTabPrivate();
    ~FancyTabPrivate();

    void init(void);
    void painterInfo(const QColor &color);
    void painterArrow(const QColor &color);

public:
    FancyTab *q;

    QColor m_selectedTextColor;
    bool m_bMouseHover;
    bool m_bMousePress;
    bool m_bSelected;

    FancyTab::TabType m_type;
};

FancyTabPrivate::FancyTabPrivate()
    : m_selectedTextColor(QColor(255, 255, 255))
    , m_bMouseHover(false)
    , m_bMousePress(false)
    , m_bSelected(false)
    , m_type(FancyTab::Mode)
{
}

FancyTabPrivate::~FancyTabPrivate()
{
}


void FancyTabPrivate::init()
{
    q->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    q->setIconSize(QSize(24, 24));
    q->setAutoRaise(true);

    q->setPressColor(QColor(0, 0, 0, 100));
    q->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    q->setStyleSheet("QToolButton{background-color: transparent;color: white;}");
}

void FancyTabPrivate::painterInfo(const QColor &color)
{
    QPainter painter(q);
    QPen pen(Qt::NoBrush, 1);
    painter.setPen(pen);
    painter.setBrush(color);
    painter.drawRect(q->rect());
}

void FancyTabPrivate::painterArrow(const QColor &color)
{
    QPainter painter(q);
    painter.setBrush(QBrush(color));
    QPolygonF polygonf;
    QPointF pt;
    QPoint pos = q->rect().center();
    pos.setX(pos.x() + q->rect().width() / 2);
    pt = QPointF(pos.x() - 6, pos.y());
    polygonf.append(pt);
    pt = QPointF(pos.x() - 10, pos.y() + 4);
    polygonf.append(pt);
    pt = QPointF(pos.x() - 10, pos.y() - 4);
    polygonf.append(pt);
    painter.drawPolygon(polygonf);
}

FancyTab::FancyTab(QWidget *parent)
    : FancyButton(parent)
    , d(new FancyTabPrivate())
{
    d->q = this;
    d->init();
}

FancyTab::FancyTab(const QIcon &icon, QWidget *parent)
    : FancyButton(parent)
    , d(new FancyTabPrivate())
{
    d->q = this;
    d->init();
    setIcon(icon);
}

FancyTab::~FancyTab()
{
}

void FancyTab::select(bool selected)
{
    d->m_bMousePress = selected;
    if (!selected) {
        d->m_bMouseHover = false;
    }
    update();
}

bool FancyTab::isSelected()
{
    return d->m_bSelected; // was set in update()
}

void FancyTab::setType(FancyTab::TabType type)
{
    d->m_type = type;
}

FancyTab::TabType FancyTab::type() const
{
    return d->m_type;
}

void FancyTab::setSelectedTextColor(const QColor &color)
{
    d->m_selectedTextColor = color;
}

void FancyTab::setColor(const QColor &color)
{
    setStyleSheet(QString("QToolButton{background-color: transparent;color: rgba(%1,%2,%3,%4);}")
                  .arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha())
                  );
}

void FancyTab::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    if (!isEnabled()) {
        return;
    }
    d->m_bMouseHover = true;
}

void FancyTab::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    d->m_bMouseHover = false;
}

void FancyTab::mousePressEvent(QMouseEvent *event)
{
    if (!isEnabled()) {
        return;
    }
    if (event->button() == Qt::LeftButton) {
        if (d->m_type == Action) {
            d->m_bMousePress = true; // add for action
            update();
            if (hasMenu()) {
                emit menuTriggered(event);
            }
        } else {
            if (hasMenu()) {
                d->m_bMousePress = true;
                update();
                emit menuTriggered(event);
            } else {
                emit clicked();
            }
        }
    }
}

void FancyTab::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (d->m_type == Action) {
            d->m_bMousePress = false; // add for action
            update();
            // don't emit when move out
            if (rect().contains(event->pos()) && !hasMenu()) {
                emit clicked();
            }
        }
    }
}

void FancyTab::paintEvent(QPaintEvent *event)
{
    if (d->m_bMousePress) {
        d->painterInfo(pressColor());
    } else if (d->m_bMouseHover) {
        d->painterInfo(hoverColor());
    }

    if (hasMenu() && d->m_type != Action) {
        if (d->m_bMousePress || d->m_bMouseHover) {
            d->painterArrow(d->m_selectedTextColor);
        } else {
            d->painterArrow(textColor());
        }
    }

    QToolButton::paintEvent(event);
}

void FancyTab::update()
{
    QToolButton::update();
    if (d->m_bMousePress || d->m_bMouseHover) {
        setColor(d->m_selectedTextColor);
    } else {
        setColor(textColor());
    }
    d->m_bSelected = d->m_bMousePress;
}

QCANPOOL_END_NAMESPACE
