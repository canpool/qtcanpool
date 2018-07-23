#include "zoomcirclewait.h"
#include <QPainter>
#include <QTimer>
#include <QDebug>

ZoomCircleWait::ZoomCircleWait(QWidget *parent) : QWidget(parent),
    m_color(Qt::black),
    m_zoomOut(false),
    m_zoomDelta(5),
    m_radious(0),
    m_minRadious(0)
{
    m_timer = new QTimer;
    connect(m_timer,SIGNAL(timeout()),this,SLOT(update()));
    m_timer->start(100);
}

void ZoomCircleWait::setColor(QColor color)
{
    if(color != m_color)
    {
        m_color = color;
        update();
    }
}

void ZoomCircleWait::setZoomOut(bool zoomOut)
{
    if(zoomOut != m_zoomOut)
    {
        m_zoomOut = zoomOut;
        update();
    }
}

void ZoomCircleWait::setZoomDelta(int delta)
{
    if(delta != m_zoomDelta)
    {
        m_zoomDelta = delta;
        update();
    }
}

void ZoomCircleWait::setMinRadious(int value)
{
    if(value > 0 && value != m_minRadious)
    {
        m_minRadious = value;
        update();
    }
}

void ZoomCircleWait::setRadious(int value)
{
    if(value != m_radious && value > m_minRadious)
    {
        m_radious = value;
        update();
    }
}

void ZoomCircleWait::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.translate(width()/2, height()/2);
    int side = qMin(width(), height());
    p.scale(side/100.0, side/100.0);

    m_radious += m_zoomDelta;
    fixRaidous(50);
    drawZoomCricleWait(p);
}

void ZoomCircleWait::drawZoomCricleWait(QPainter &p)
{
    //根据100*100的大小进行画,在paintEvent中进行过缩放
    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(m_color));
    p.drawEllipse(-m_radious, -m_radious, 2*m_radious, 2*m_radious);
    p.restore();
}

void ZoomCircleWait::fixRaidous(int max)
{
    if(m_radious < m_minRadious)
    {
        m_radious = m_minRadious;
        m_zoomDelta = qAbs(m_zoomDelta);
    }
    else if(m_radious > max)
    {

        //半径开始减小
        if(m_zoomOut)
        {
            m_radious = max;
            m_zoomDelta = -qAbs(m_zoomDelta);
        }
        else
        {
            m_radious = m_minRadious;
            m_zoomDelta = qAbs(m_zoomDelta);
        }
    }
}
