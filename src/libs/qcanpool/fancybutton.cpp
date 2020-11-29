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
#include "fancybutton.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QAction>
#include "qcanpool.h"

QCANPOOL_BEGIN_NAMESPACE

class FancyButtonPrivate
{
public:
    FancyButtonPrivate();
    ~FancyButtonPrivate();

public:
    void painterInfo(QColor &color);
    bool actionHasMenu();

public:
    FancyButton *q;

    QColor m_hoverColor;
    QColor m_pressColor;
    QColor m_textColor;
    QColor m_normalColor;

    bool m_bMouseHover;
    bool m_bMousePress;
    bool m_hasBorder;
    bool m_bRound;
    bool m_hasMenu;
};

FancyButtonPrivate::FancyButtonPrivate()
    : m_hoverColor(QColor(255, 255, 255, 50))
    , m_pressColor(QColor(0, 0, 0, 50))
    , m_textColor(QColor(255, 255, 255))
    , m_normalColor(QColor())
    , m_bMouseHover(false)
    , m_bMousePress(false)
    , m_hasBorder(true)
    , m_bRound(false)
    , m_hasMenu(false)
{
}

FancyButtonPrivate::~FancyButtonPrivate()
{
}

void FancyButtonPrivate::painterInfo(QColor &color)
{
    QPainter painter(q);
    QPen pen(Qt::NoBrush, 1);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(color);

    if (m_bRound) {
        int r = qMin(q->width(), q->height()) / 2;
        painter.drawEllipse(QPointF(q->width() / 2, q->height() / 2), r, r);
    } else {
        painter.drawRect(q->rect());
    }
}

bool FancyButtonPrivate::actionHasMenu()
{
    QAction *action = q->defaultAction();
    if (action && action->menu()) {
        return true;
    }

    return false;
}

FancyButton::FancyButton(QWidget *parent)
    : QToolButton(parent)
    , d(new FancyButtonPrivate())
{
    d->q = this;

    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setAutoRaise(true);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setColor(Qt::white);
}

FancyButton::~FancyButton()
{
    delete d;
}

void FancyButton::select(bool selected)
{
    d->m_bMousePress = selected;
    if (!selected) {
        d->m_bMouseHover = false;
    }
    update();
}

void FancyButton::setHoverColor(const QColor &color)
{
    d->m_hoverColor = color;
}

QColor FancyButton::hoverColor() const
{
    return d->m_hoverColor;
}

void FancyButton::setPressColor(const QColor &color)
{
    d->m_pressColor = color;
}

QColor FancyButton::pressColor() const
{
    return d->m_pressColor;
}

void FancyButton::setTextColor(const QColor &color)
{
    d->m_textColor = color;
}

QColor FancyButton::textColor() const
{
    return d->m_textColor;
}

void FancyButton::setColor(const QColor &color)
{
    if (d->m_hasBorder) {
        setStyleSheet(QString("QToolButton::menu-indicator { image: None; }"
                              "QToolButton{background-color: transparent;color: #%1;}")
                      .arg(QCanpool::colorToArgb(color)));
    } else {
        setStyleSheet(QString("QToolButton::menu-indicator { image: None; }"
                              "QToolButton{background-color: transparent;color: #%1;border:none;}")
                      .arg(QCanpool::colorToArgb(color)));
    }
}

void FancyButton::setNormalColor(const QColor &color)
{
    d->m_normalColor = color;
}

QColor FancyButton::normalColor() const
{
    return d->m_normalColor;
}

void FancyButton::setHasMenu(bool has)
{
    d->m_hasMenu = has;
}

bool FancyButton::hasMenu()
{
    return d->m_hasMenu;
}

void FancyButton::setHasBorder(bool has)
{
    d->m_hasBorder = has;
    setColor(d->m_textColor);
    update();
}

void FancyButton::setRound(bool round)
{
    d->m_bRound = round;
}

void FancyButton::enterEvent(QEvent *event)
{
    Q_UNUSED(event);

    if (!isEnabled()) {
        return;
    }

    d->m_bMouseHover = true;
}

void FancyButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    d->m_bMouseHover = false;
}

void FancyButton::mousePressEvent(QMouseEvent *event)
{
    if (!isEnabled()) {
        return;
    }

    if (event->button() == Qt::LeftButton) {
        d->m_bMousePress = true;

        if (d->m_hasMenu || d->actionHasMenu()) {
            emit menuTriggered(event);
        }

        update();
    }
}

void FancyButton::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (!isEnabled()) {
        return;
    }

    if (event->button() == Qt::LeftButton) {
        emit doubleClicked();
    }
}

void FancyButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        d->m_bMousePress = false;
        update();

        // don't emit when move out
        if (this->rect().contains(event->pos())) {
            emit clicked();
        }
    }
}

void FancyButton::paintEvent(QPaintEvent *event)
{
    if (d->m_bMousePress) {
        d->painterInfo(d->m_pressColor);
    } else if (d->m_bMouseHover) {
        d->painterInfo(d->m_hoverColor);
    } else {
        if (d->m_normalColor.isValid()) {
            d->painterInfo(d->m_normalColor);
        }
    }

    QToolButton::paintEvent(event);
}

QCANPOOL_END_NAMESPACE
