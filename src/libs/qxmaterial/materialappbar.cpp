/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#include "materialappbar.h"
#include "materialappbar_p.h"
#include "materialstyle.h"
#include <QPainter>
#include <QtWidgets/QGraphicsDropShadowEffect>

QX_MATERIAL_BEGIN_NAMESPACE

/*!
 *  \class MaterialAppBarPrivate
 *  \internal
 */

/*!
 *  \internal
 */
MaterialAppBarPrivate::MaterialAppBarPrivate(MaterialAppBar *q) : q_ptr(q)
{
}

/*!
 *  \internal
 */
MaterialAppBarPrivate::~MaterialAppBarPrivate()
{
}

/*!
 *  \internal
 */
void MaterialAppBarPrivate::init()
{
    Q_Q(MaterialAppBar);

    useThemeColors = true;

    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
    effect->setBlurRadius(11);
    effect->setColor(QColor(0, 0, 0, 50));
    effect->setOffset(0, 3);

    q->setGraphicsEffect(effect);

    QHBoxLayout *layout = new QHBoxLayout;
    q->setLayout(layout);
}

/*!
 *  \class MaterialAppBar
 */

MaterialAppBar::MaterialAppBar(QWidget *parent) : QWidget(parent), d_ptr(new MaterialAppBarPrivate(this))
{
    d_func()->init();
}

MaterialAppBar::~MaterialAppBar()
{
}

QSize MaterialAppBar::sizeHint() const
{
    return QSize(-1, 64);
}

void MaterialAppBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    painter.fillRect(rect(), backgroundColor());
}

void MaterialAppBar::setUseThemeColors(bool value)
{
    Q_D(MaterialAppBar);

    if (d->useThemeColors == value) {
        return;
    }

    d->useThemeColors = value;
    update();
}

bool MaterialAppBar::useThemeColors() const
{
    Q_D(const MaterialAppBar);

    return d->useThemeColors;
}

void MaterialAppBar::setForegroundColor(const QColor &color)
{
    Q_D(MaterialAppBar);

    d->foregroundColor = color;

    if (d->useThemeColors == true) {
        d->useThemeColors = false;
    }
    update();
}

QColor MaterialAppBar::foregroundColor() const
{
    Q_D(const MaterialAppBar);

    if (d->useThemeColors || !d->foregroundColor.isValid()) {
        return MaterialStyle::instance().themeColor(MK_primary1);
    } else {
        return d->foregroundColor;
    }
}

void MaterialAppBar::setBackgroundColor(const QColor &color)
{
    Q_D(MaterialAppBar);

    d->backgroundColor = color;

    if (d->useThemeColors == true) {
        d->useThemeColors = false;
    }
    update();
}

QColor MaterialAppBar::backgroundColor() const
{
    Q_D(const MaterialAppBar);

    if (d->useThemeColors || !d->backgroundColor.isValid()) {
        return MaterialStyle::instance().themeColor(MK_primary1);
    } else {
        return d->backgroundColor;
    }
}

QX_MATERIAL_END_NAMESPACE
