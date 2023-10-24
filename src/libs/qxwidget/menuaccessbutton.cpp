/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "menuaccessbutton.h"

#include <QStyle>

QX_BEGIN_NAMESPACE

class MenuAccessButtonPrivate
{
    QX_DECLARE_PUBLIC(MenuAccessButton)
public:
    Qt::Orientation m_orientation;
};

MenuAccessButton::MenuAccessButton(QWidget *parent)
    : QToolButton(parent)
{
    QX_INIT_PRIVATE(MenuAccessButton)
    Q_D(MenuAccessButton);
    d->m_orientation = Qt::Horizontal;

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setPopupMode(QToolButton::InstantPopup);
}

MenuAccessButton::~MenuAccessButton()
{
    QX_FINI_PRIVATE()
}

QSize MenuAccessButton::sizeHint() const
{
    Q_D(const MenuAccessButton);
    QSize sz = QToolButton::sizeHint();
    int w = style()->pixelMetric(QStyle::PM_MenuButtonIndicator, Q_NULLPTR, this);
    if (d->m_orientation == Qt::Horizontal) {
        return QSize(qMax(sz.width() / 2, w), sz.height());
    } else {
        return QSize(sz.width(), qMax(sz.height() / 2, w));
    }
}

void MenuAccessButton::setOrientation(Qt::Orientation orientation)
{
    Q_D(MenuAccessButton);
    if (d->m_orientation == orientation) {
        return;
    }
    d->m_orientation = orientation;
    updateGeometry();
}

QX_END_NAMESPACE
