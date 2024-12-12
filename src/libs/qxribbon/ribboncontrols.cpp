/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "ribboncontrols.h"

#include <QPainter>

QX_RIBBON_BEGIN_NAMESPACE

/* RibbonControl */
RibbonControl::RibbonControl(QWidget *parent) : QWidget(parent)
{

}

/* RibbonCheckBox */
RibbonCheckBox::RibbonCheckBox(QWidget *parent) : QCheckBox(parent)
{
}

/* RibbonComboBox */
RibbonComboBox::RibbonComboBox(QWidget *parent) : QComboBox(parent)
{
}

/* RibbonControlButton */
RibbonControlButton::RibbonControlButton(QWidget *parent) : QToolButton(parent)
{
}

/* RibbonLineEdit */
RibbonLineEdit::RibbonLineEdit(QWidget *parent) : QLineEdit(parent)
{
}

/* RibbonSeparator */
RibbonSeparator::RibbonSeparator(int height, QWidget *parent)
    : RibbonSeparator(parent)
{
    setFixedHeight(height);
}

RibbonSeparator::RibbonSeparator(QWidget *parent)
    : QWidget(parent)
    , m_topMargin(4)
    , m_bottomMargin(4)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setFixedWidth(6);
}

QSize RibbonSeparator::sizeHint() const
{
    return QSize(6, height());
}

void RibbonSeparator::setTopBottomMargins(int top, int bottom)
{
    m_topMargin = top;
    m_bottomMargin = bottom;
}

void RibbonSeparator::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    painter.setPen(palette().window().color().darker(114));
    int x1 = rect().center().x();

    painter.drawLine(QPoint(x1, rect().top() + m_topMargin),
                     QPoint(x1, rect().bottom() - m_bottomMargin));
}

QX_RIBBON_END_NAMESPACE
