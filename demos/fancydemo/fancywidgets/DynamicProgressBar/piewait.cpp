#include "piewait.h"
#include <QPainter>
#include <QTimer>

PieWait::PieWait(QWidget *parent) : QWidget(parent),
    m_frontColor(Qt::black),
    m_backColor(Qt::white),
    m_clockwiseRotate(true),
    m_rotateDelta(36),
    m_rotateAngle(0)
{
    m_timer = new QTimer;
    connect(m_timer,SIGNAL(timeout()),this,SLOT(update()));
    m_timer->start(100);
}

void PieWait::setFrontColor(QColor color)
{
    if(color != m_frontColor)
    {
        m_frontColor = color;
        update();
    }
}

void PieWait::setBackColor(QColor color)
{
    if(color != m_backColor)
    {
        m_backColor = color;
        update();
    }
}

void PieWait::setClockwiseRotate(bool clockwise)
{
    if(clockwise != m_clockwiseRotate)
    {
        m_clockwiseRotate = clockwise;
        update();
    }
}

void PieWait::setRotateDelta(int delta)
{
    if(delta != m_rotateDelta)
    {
        m_rotateDelta = delta;
        update();
    }
}

void PieWait::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.translate(width()/2, height()/2);
    int side = qMin(width(), height());
    p.scale(side/100.0, side/100.0);

    p.rotate(m_rotateAngle);
    drawPieWait(p);
    m_rotateAngle += m_clockwiseRotate?m_rotateDelta:-m_rotateDelta;
    m_rotateAngle %= 360;
}

void PieWait::drawPieWait(QPainter &p)
{
    //根据100*100的大小进行画,在paintEvent中进行过缩放
    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(m_backColor));
    p.drawEllipse(-50, -50, 100, 100);
    p.setBrush(QBrush(m_frontColor));
    p.drawPie(-50,-50,100,100,0*16,60*16);
    p.restore();
}
