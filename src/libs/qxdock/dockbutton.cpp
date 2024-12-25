/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "dockbutton.h"

#include <QPainter>
#include <QStyleOptionButton>
#include <QStylePainter>

QX_DOCK_BEGIN_NAMESPACE

class DockButtonPrivate
{
public:
    QX_DECLARE_PUBLIC(DockButton)
public:
    DockButtonPrivate();
public:
    DockButton::Orientation m_orientation = DockButton::Horizontal;
};

DockButtonPrivate::DockButtonPrivate()
{
}

DockButton::DockButton(QWidget *parent)
    : QPushButton(parent)
{
    QX_INIT_PRIVATE(DockButton);
}

DockButton::~DockButton()
{
    QX_FINI_PRIVATE();
}

QSize DockButton::sizeHint() const
{
    Q_D(const DockButton);
    QSize sh = QPushButton::sizeHint();

    if (d->m_orientation != DockButton::Horizontal) {
        // Swaps the width and height values
        sh.transpose();
    }

    return sh;
}

DockButton::Orientation DockButton::buttonOrientation() const
{
    Q_D(const DockButton);
    return d->m_orientation;
}

void DockButton::setButtonOrientation(DockButton::Orientation orientation)
{
    Q_D(DockButton);
    d->m_orientation = orientation;
    updateGeometry();
}

void DockButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    Q_D(DockButton);

    QStylePainter painter(this);
    QStyleOptionButton option;
    initStyleOption(&option);

    if (d->m_orientation == DockButton::VerticalTopToBottom) {
        painter.rotate(90);
        painter.translate(0, -1 * width());
        option.rect = option.rect.transposed();
    } else if (d->m_orientation == DockButton::VerticalBottomToTop) {
        painter.rotate(-90);
        painter.translate(-1 * height(), 0);
        option.rect = option.rect.transposed();
    }

    painter.drawControl(QStyle::CE_PushButton, option);
}

QX_DOCK_END_NAMESPACE
