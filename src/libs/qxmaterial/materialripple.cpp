/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#include "materialripple.h"
#include "materialrippleoverlay.h"

QX_MATERIAL_BEGIN_NAMESPACE

/*!
 *  \class MaterialRipple
 *  \internal
 */

MaterialRipple::MaterialRipple(const QPoint &center, QObject *parent)
    : QParallelAnimationGroup(parent)
    , m_overlay(0)
    , m_radiusAnimation(animate("radius"))
    , m_opacityAnimation(animate("opacity"))
    , m_radius(0)
    , m_opacity(0)
    , m_center(center)
{
    init();
}

MaterialRipple::MaterialRipple(const QPoint &center, MaterialRippleOverlay *overlay, QObject *parent)
    : QParallelAnimationGroup(parent)
    , m_overlay(overlay)
    , m_radiusAnimation(animate("radius"))
    , m_opacityAnimation(animate("opacity"))
    , m_radius(0)
    , m_opacity(0)
    , m_center(center)
{
    init();
}

MaterialRipple::~MaterialRipple()
{
}

void MaterialRipple::setRadius(qreal radius)
{
    Q_ASSERT(m_overlay);

    if (m_radius == radius) {
        return;
    }
    m_radius = radius;
    m_overlay->update();
}

void MaterialRipple::setOpacity(qreal opacity)
{
    Q_ASSERT(m_overlay);

    if (m_opacity == opacity) {
        return;
    }
    m_opacity = opacity;
    m_overlay->update();
}

void MaterialRipple::setColor(const QColor &color)
{
    if (m_brush.color() == color) {
        return;
    }
    m_brush.setColor(color);

    if (m_overlay) {
        m_overlay->update();
    }
}

void MaterialRipple::setBrush(const QBrush &brush)
{
    m_brush = brush;

    if (m_overlay) {
        m_overlay->update();
    }
}

void MaterialRipple::destroy()
{
    Q_ASSERT(m_overlay);

    m_overlay->removeRipple(this);
}

/*!
 *  \internal
 */
QPropertyAnimation *MaterialRipple::animate(const QByteArray &property, const QEasingCurve &easing, int duration)
{
    QPropertyAnimation *animation = new QPropertyAnimation;
    animation->setTargetObject(this);
    animation->setPropertyName(property);
    animation->setEasingCurve(easing);
    animation->setDuration(duration);
    addAnimation(animation);
    return animation;
}

/*!
 *  \internal
 */
void MaterialRipple::init()
{
    setOpacityStartValue(0.5);
    setOpacityEndValue(0);
    setRadiusStartValue(0);
    setRadiusEndValue(300);

    m_brush.setColor(Qt::black);
    m_brush.setStyle(Qt::SolidPattern);

    connect(this, SIGNAL(finished()), this, SLOT(destroy()));
}

QX_MATERIAL_END_NAMESPACE
