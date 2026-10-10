/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxextensionbutton.h"

#include <QEvent>
#include <QStyle>
#include <QStyleOption>
#include <QStylePainter>

QX_APP_BEGIN_NAMESPACE

class QxExtensionButtonPrivate
{
    QX_DECLARE_PUBLIC(QxExtensionButton)
public:
    Qt::Orientation m_orientation;
};

QxExtensionButton::QxExtensionButton(QWidget *parent)
    : QToolButton(parent)
{
    QX_INIT_PRIVATE(QxExtensionButton)
    Q_D(QxExtensionButton);
    d->m_orientation = Qt::Horizontal;

    setObjectName(QLatin1String("qtc_qxextensionbutton"));
    setAutoRaise(true);
    setOrientation(d->m_orientation);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setPopupMode(QToolButton::InstantPopup);
}

QxExtensionButton::~QxExtensionButton(){QX_FINI_PRIVATE()}

QSize QxExtensionButton::sizeHint() const
{
    int ext = style()->pixelMetric(QStyle::PM_ToolBarExtensionExtent);
    return QSize(ext, ext);
}

void QxExtensionButton::setOrientation(Qt::Orientation o)
{
    Q_D(QxExtensionButton);
    QStyleOption opt;
    opt.initFrom(this);
    if (o == Qt::Horizontal) {
        setIcon(style()->standardIcon(QStyle::SP_ToolBarHorizontalExtensionButton, &opt));
    } else {
        setIcon(style()->standardIcon(QStyle::SP_ToolBarVerticalExtensionButton, &opt));
    }
    d->m_orientation = o;
}

void QxExtensionButton::paintEvent(QPaintEvent *)
{
    QStylePainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    // We do not need to draw both extension arrows
    opt.features &= ~QStyleOptionToolButton::HasMenu;
    p.drawComplexControl(QStyle::CC_ToolButton, opt);
}

bool QxExtensionButton::event(QEvent *event)
{
    Q_D(QxExtensionButton);
    switch (event->type()) {
    case QEvent::LayoutDirectionChange:
        setOrientation(d->m_orientation);
        break;
    default:
        break;
    }
    return QToolButton::event(event);
}

QX_APP_END_NAMESPACE
