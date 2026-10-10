/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxmenuaccessbutton.h"

#include <QStyle>

QX_APP_BEGIN_NAMESPACE

class QxMenuAccessButtonPrivate
{
    QX_DECLARE_PUBLIC(QxMenuAccessButton)
public:
    Qt::Orientation m_orientation;
};

QxMenuAccessButton::QxMenuAccessButton(QWidget *parent)
    : QToolButton(parent)
{
    QX_INIT_PRIVATE(QxMenuAccessButton)
    Q_D(QxMenuAccessButton);
    d->m_orientation = Qt::Horizontal;

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setPopupMode(QToolButton::InstantPopup);
    setObjectName(QLatin1String("qtc_qxmenuaccessbutton"));
}

QxMenuAccessButton::~QxMenuAccessButton(){QX_FINI_PRIVATE()}

QSize QxMenuAccessButton::sizeHint() const
{
    Q_D(const QxMenuAccessButton);
    QSize sz = QToolButton::sizeHint();
    int w = style()->pixelMetric(QStyle::PM_MenuButtonIndicator, Q_NULLPTR, this);
    if (d->m_orientation == Qt::Horizontal) {
        return QSize(qMax(sz.width() / 2, w), sz.height());
    } else {
        return QSize(sz.width(), qMax(sz.height() / 2, w));
    }
}

void QxMenuAccessButton::setOrientation(Qt::Orientation orientation)
{
    Q_D(QxMenuAccessButton);
    if (d->m_orientation == orientation) {
        return;
    }
    d->m_orientation = orientation;
    updateGeometry();
}

QX_APP_END_NAMESPACE
