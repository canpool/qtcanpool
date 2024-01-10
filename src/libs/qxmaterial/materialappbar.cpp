/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#include "materialappbar.h"
#include "materialappbar_p.h"
#include "materialstyle_p.h"
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
MaterialAppBarPrivate::MaterialAppBarPrivate()
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

    m_layout = new QHBoxLayout(q);
}

/*!
 *  \class MaterialAppBar
 */

MaterialAppBar::MaterialAppBar(QWidget *parent) : QWidget(parent)
{
    QX_INIT_PRIVATE(MaterialAppBar)
    Q_D(MaterialAppBar);
    d->init();
}

MaterialAppBar::~MaterialAppBar()
{
    QX_FINI_PRIVATE()
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

    MATERIAL_DISABLE_THEME_COLORS
    update();
}

QColor MaterialAppBar::foregroundColor() const
{
    Q_D(const MaterialAppBar);

    if (d->useThemeColors || !d->foregroundColor.isValid()) {
        return MaterialStyle::instance().themeColor(MK_textAlt);
    } else {
        return d->foregroundColor;
    }
}

void MaterialAppBar::setBackgroundColor(const QColor &color)
{
    Q_D(MaterialAppBar);

    d->backgroundColor = color;

    MATERIAL_DISABLE_THEME_COLORS
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

QHBoxLayout *MaterialAppBar::appBarLayout() const
{
    Q_D(const MaterialAppBar);
    return d->m_layout;
}

QX_MATERIAL_END_NAMESPACE
