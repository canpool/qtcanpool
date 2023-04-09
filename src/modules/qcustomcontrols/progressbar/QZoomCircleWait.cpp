/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include "QZoomCircleWait.h"
#include <QPainter>
#include <QTimer>

QZoomCircleWait::QZoomCircleWait(QWidget *parent)
    : QWidget(parent)
    , m_color(Qt::black)
    , m_zoomOut(false)
    , m_zoomDelta(5)
    , m_radius(0)
    , m_minRadius(0)
{
    m_timer = new QTimer;
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer->start(100);
}

void QZoomCircleWait::setColor(QColor color)
{
    if (color != m_color) {
        m_color = color;
        update();
    }
}

void QZoomCircleWait::setZoomOut(bool zoomOut)
{
    if (zoomOut != m_zoomOut) {
        m_zoomOut = zoomOut;
        update();
    }
}

void QZoomCircleWait::setZoomDelta(int delta)
{
    if (delta != m_zoomDelta) {
        m_zoomDelta = delta;
        update();
    }
}

void QZoomCircleWait::setMinRadius(int value)
{
    if (value > 0 && value != m_minRadius) {
        m_minRadius = value;
        update();
    }
}

void QZoomCircleWait::setRadius(int value)
{
    if (value != m_radius && value > m_minRadius) {
        m_radius = value;
        update();
    }
}

void QZoomCircleWait::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.translate(width() / 2, height() / 2);
    int side = qMin(width(), height());
    p.scale(side / 100.0, side / 100.0);

    m_radius += m_zoomDelta;
    fixRaidus(50);
    drawZoomCricleWait(p);
}

void QZoomCircleWait::drawZoomCricleWait(QPainter &p)
{
    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(m_color));
    p.drawEllipse(-m_radius, -m_radius, 2 * m_radius, 2 * m_radius);
    p.restore();
}

void QZoomCircleWait::fixRaidus(int max)
{
    if (m_radius < m_minRadius) {
        m_radius = m_minRadius;
        m_zoomDelta = qAbs(m_zoomDelta);
    } else if (m_radius > max) {
        if (m_zoomOut) {
            m_radius = max;
            m_zoomDelta = -qAbs(m_zoomDelta);
        } else {
            m_radius = m_minRadius;
            m_zoomDelta = qAbs(m_zoomDelta);
        }
    }
}
