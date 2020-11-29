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
#include "fancynavbutton.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QAction>

QCANPOOL_USE_NAMESPACE

FancyNavButton::FancyNavButton(QWidget *parent)
    : QToolButton(parent)
{
    setToolButtonStyle(Qt::ToolButtonTextOnly);
    setAutoRaise(false);
    setArrowType(Qt::NoArrow);
    setFocusPolicy(Qt::NoFocus);
    setIconSize(QSize(16, 16));
    m_isMouseOver = false;
    m_isMousePress = false;
    m_type = Button;
    m_hoverColor = QColor(255, 255, 255, 50);
    m_pressColor = QColor(0, 0, 0, 100);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setStyleSheet("QToolButton{background-color: transparent;border:none;color: white;}");
}

void FancyNavButton::select(bool selected)
{
    m_isMousePress = selected;
    if (!selected) {
        this->m_isMouseOver = false;
    }
    update();
}

void FancyNavButton::setTextColor(const QColor &color)
{
    setStyleSheet(QString("QToolButton{background-color: transparent;border:none;color: rgba(%1,%2,%3,%4);}")
                  .arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha())
                 );
}

void FancyNavButton::enterEvent(QEvent *event)
{
    Q_UNUSED(event);

    if (!isEnabled()) {
        return;
    }
    m_isMouseOver = true;
}

void FancyNavButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_isMouseOver = false;
}

void FancyNavButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_type == Action) {
            m_isMousePress = true;
            update();
        } else {
            if (this->isCheckable()) {
                if (this->isChecked()) {
                    m_isMousePress = false;
                } else {
                    m_isMousePress = true;
                }

                this->setChecked(m_isMousePress);
                update();
            } else {
                m_isMousePress = true;
                emit clicked();
            }
        }
    }
}

void FancyNavButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_type == Action) {
            m_isMousePress = false; // add for action
            update();

            if (this->rect().contains(event->pos())) { // don't emit when move out
                emit clicked();
            }
        }
    }
}

void FancyNavButton::paintEvent(QPaintEvent *event)
{
    if (m_isMousePress) {
        painterInfo(m_pressColor);
    } else if (m_isMouseOver) {
        painterInfo(m_hoverColor);
    }

    QToolButton::paintEvent(event);
}

void FancyNavButton::painterInfo(QColor &color)
{
    QPainter painter(this);
    QPen pen(Qt::NoBrush, 1);
    painter.setPen(pen);
    painter.setBrush(color);
    painter.drawRect(rect());
}
