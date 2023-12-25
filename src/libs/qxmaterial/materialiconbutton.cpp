/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#include "materialiconbutton.h"
#include "materialiconbutton_p.h"
#include "materialrippleoverlay.h"
#include "materialstyle.h"
#include <QEvent>
#include <QPainter>

QX_MATERIAL_BEGIN_NAMESPACE

/*!
 *  \class MaterialIconButtonPrivate
 *  \internal
 */

MaterialIconButtonPrivate::MaterialIconButtonPrivate(MaterialIconButton *q)
    : q_ptr(q)
{
}

MaterialIconButtonPrivate::~MaterialIconButtonPrivate()
{
}

void MaterialIconButtonPrivate::init()
{
    Q_Q(MaterialIconButton);

    rippleOverlay = new MaterialRippleOverlay(q->parentWidget());
    useThemeColors = true;

    rippleOverlay->installEventFilter(q);

    q->setStyle(&MaterialStyle::instance());

    QSizePolicy policy;
    policy.setWidthForHeight(true);
    q->setSizePolicy(policy);
}

void MaterialIconButtonPrivate::updateRipple()
{
    Q_Q(MaterialIconButton);

    QRect r(q->rect());
    r.setSize(QSize(q->width() * 2, q->height() * 2));
    r.moveCenter(q->geometry().center());
    rippleOverlay->setGeometry(r);
}

/*!
 *  \class MaterialIconButton
 */

MaterialIconButton::MaterialIconButton(const QIcon &icon, QWidget *parent)
    : QAbstractButton(parent), d_ptr(new MaterialIconButtonPrivate(this))
{
    d_func()->init();

    setIcon(icon);
}

MaterialIconButton::~MaterialIconButton()
{
}

/*!
 *  \reimp
 */
QSize MaterialIconButton::sizeHint() const
{
    return iconSize();
}

void MaterialIconButton::setUseThemeColors(bool value)
{
    Q_D(MaterialIconButton);

    if (d->useThemeColors == value) {
        return;
    }

    d->useThemeColors = value;
    update();
}

bool MaterialIconButton::useThemeColors() const
{
    Q_D(const MaterialIconButton);

    return d->useThemeColors;
}

void MaterialIconButton::setColor(const QColor &color)
{
    Q_D(MaterialIconButton);

    d->color = color;

    MATERIAL_DISABLE_THEME_COLORS
    update();
}

QColor MaterialIconButton::color() const
{
    Q_D(const MaterialIconButton);

    if (d->useThemeColors || !d->color.isValid()) {
        return MaterialStyle::instance().themeColor("text");
    }
    return d->color;
}

void MaterialIconButton::setDisabledColor(const QColor &color)
{
    Q_D(MaterialIconButton);

    d->disabledColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    update();
}

QColor MaterialIconButton::disabledColor() const
{
    Q_D(const MaterialIconButton);

    if (d->useThemeColors || !d->disabledColor.isValid()) {
        return MaterialStyle::instance().themeColor("disabled");
    }
    return d->disabledColor;
}

MaterialIconButton::MaterialIconButton(MaterialIconButtonPrivate &d, QWidget *parent)
    : QAbstractButton(parent), d_ptr(&d)
{
    d_func()->init();
}

/*!
 *  \reimp
 */
bool MaterialIconButton::event(QEvent *event)
{
    Q_D(MaterialIconButton);

    switch (event->type()) {
    case QEvent::Move:
    case QEvent::Resize:
        d->updateRipple();
        break;
    case QEvent::ParentChange: {
        QWidget *widget;
        if ((widget = parentWidget())) {
            d->rippleOverlay->setParent(widget);
        }
        break;
    }
    default:
        break;
    }
    return QAbstractButton::event(event);
}

/*!
 *  \reimp
 */
bool MaterialIconButton::eventFilter(QObject *obj, QEvent *event)
{
    if (QEvent::Resize == event->type()) {
        Q_D(MaterialIconButton);

        d->updateRipple();
    }
    return QAbstractButton::eventFilter(obj, event);
}

/*!
 *  \reimp
 */
void MaterialIconButton::mousePressEvent(QMouseEvent *event)
{
    Q_D(MaterialIconButton);

    d->rippleOverlay->addRipple(QPoint(d->rippleOverlay->width(), d->rippleOverlay->height()) / 2, iconSize().width());
    emit clicked();

    QAbstractButton::mousePressEvent(event);
}

/*!
 *  \reimp
 */
void MaterialIconButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    QPixmap pixmap = icon().pixmap(iconSize());
    QPainter icon(&pixmap);
    icon.setCompositionMode(QPainter::CompositionMode_SourceIn);
    icon.fillRect(pixmap.rect(), isEnabled() ? color() : disabledColor());

    QRect r(rect());
    const qreal w = pixmap.width();
    const qreal h = pixmap.height();
    painter.drawPixmap(QRect((r.width() - w) / 2, (r.height() - h) / 2, w, h), pixmap);
}

QX_MATERIAL_END_NAMESPACE
