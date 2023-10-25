/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "extensionbutton.h"

#include <QEvent>
#include <QStyle>
#include <QStyleOption>
#include <QStylePainter>

QX_WIDGET_BEGIN_NAMESPACE

class ExtensionButtonPrivate
{
    QX_DECLARE_PUBLIC(ExtensionButton)
public:
    Qt::Orientation m_orientation;
};

ExtensionButton::ExtensionButton(QWidget *parent)
    : QToolButton(parent)
{
    QX_INIT_PRIVATE(ExtensionButton)
    Q_D(ExtensionButton);
    d->m_orientation = Qt::Horizontal;

    setObjectName(QLatin1String("qx_ExtensionButton"));
    setAutoRaise(true);
    setOrientation(d->m_orientation);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setPopupMode(QToolButton::InstantPopup);
}

ExtensionButton::~ExtensionButton()
{
    QX_FINI_PRIVATE()
}

QSize ExtensionButton::sizeHint() const
{
    int ext = style()->pixelMetric(QStyle::PM_ToolBarExtensionExtent);
    return QSize(ext, ext);
}

void ExtensionButton::setOrientation(Qt::Orientation o)
{
    Q_D(ExtensionButton);
    QStyleOption opt;
    opt.initFrom(this);
    if (o == Qt::Horizontal) {
        setIcon(style()->standardIcon(QStyle::SP_ToolBarHorizontalExtensionButton, &opt));
    } else {
        setIcon(style()->standardIcon(QStyle::SP_ToolBarVerticalExtensionButton, &opt));
    }
    d->m_orientation = o;
}

void ExtensionButton::paintEvent(QPaintEvent *)
{
    QStylePainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    // We do not need to draw both extension arrows
    opt.features &= ~QStyleOptionToolButton::HasMenu;
    p.drawComplexControl(QStyle::CC_ToolButton, opt);
}

bool ExtensionButton::event(QEvent *event)
{
    Q_D(ExtensionButton);
    switch (event->type()) {
    case QEvent::LayoutDirectionChange:
        setOrientation(d->m_orientation);
        break;
    default:
        break;
    }
    return QToolButton::event(event);
}

QX_WIDGET_END_NAMESPACE
