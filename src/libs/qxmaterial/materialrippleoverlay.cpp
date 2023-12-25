/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#include "materialrippleoverlay.h"
#include "materialripple.h"
#include <QPainter>

QX_MATERIAL_BEGIN_NAMESPACE

/*!
 *  \class MaterialRippleOverlay
 *  \internal
 */

MaterialRippleOverlay::MaterialRippleOverlay(QWidget *parent)
    : MaterialOverlayWidget(parent)
    , m_useClip(false)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
}

MaterialRippleOverlay::~MaterialRippleOverlay()
{
}

void MaterialRippleOverlay::addRipple(MaterialRipple *ripple)
{
    ripple->setOverlay(this);
    m_ripples.push_back(ripple);
    ripple->start();

    connect(this, SIGNAL(destroyed(QObject *)), ripple, SLOT(stop()));
    connect(this, SIGNAL(destroyed(QObject *)), ripple, SLOT(deleteLater()));
}

void MaterialRippleOverlay::addRipple(const QPoint &position, qreal radius)
{
    MaterialRipple *ripple = new MaterialRipple(position);
    ripple->setRadiusEndValue(radius);
    addRipple(ripple);
}

void MaterialRippleOverlay::removeRipple(MaterialRipple *ripple)
{
    if (m_ripples.removeOne(ripple)) {
        delete ripple;
        update();
    }
}

/*!
 *  \reimp
 */
void MaterialRippleOverlay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    if (m_useClip) {
        painter.setClipPath(m_clipPath);
    }

    QList<MaterialRipple *>::const_iterator i;
    for (i = m_ripples.begin(); i != m_ripples.end(); ++i) {
        paintRipple(&painter, *i);
    }
}

void MaterialRippleOverlay::paintRipple(QPainter *painter, MaterialRipple *ripple)
{
    const qreal radius = ripple->radius();
    const QPointF center = ripple->center();
    painter->setOpacity(ripple->opacity());
    painter->setBrush(ripple->brush());
    painter->drawEllipse(center, radius, radius);
}

QX_MATERIAL_END_NAMESPACE
