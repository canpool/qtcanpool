#include "taichiwait.h"
#include <QPainter>
#include <QTimer>

TaiChiWait::TaiChiWait(QWidget *parent) : QWidget(parent),
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

void TaiChiWait::setFrontColor(QColor color)
{
    if(color != m_frontColor)
    {
        m_frontColor = color;
        update();
    }
}

void TaiChiWait::setBackColor(QColor color)
{
    if(color != m_backColor)
    {
        m_backColor = color;
        update();
    }
}

void TaiChiWait::setClockwiseRotate(bool clockwise)
{
    if(clockwise != m_clockwiseRotate)
    {
        m_clockwiseRotate = clockwise;
        update();
    }
}

void TaiChiWait::setRotateDelta(int delta)
{
    if(delta != m_rotateDelta)
    {
        m_rotateDelta = delta;
        update();
    }
}

void TaiChiWait::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.translate(width()/2, height()/2);
    int side = qMin(width(), height());
    p.scale(side/100.0, side/100.0);

    p.rotate(m_rotateAngle);
    drawTaiChiWait(p);
    m_rotateAngle += m_clockwiseRotate?m_rotateDelta:-m_rotateDelta;
    m_rotateAngle %= 360;
}

void TaiChiWait::drawTaiChiWait(QPainter &p)
{
    //根据100*100的大小进行画,在paintEvent中进行过缩放
    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(m_frontColor));
    p.drawPie(-50,-50,100,100,90*16,180*16);
    p.setBrush(QBrush(m_backColor));
    p.drawPie(-50,-50,100,100,270*16,180*16);
    p.drawEllipse(-25,-50,50,50);
    p.setBrush(QBrush(m_frontColor));
    p.drawEllipse(-25,0,50,50);
    p.drawEllipse(-50/8,-50*5/8,50/4,50/4);
    p.setBrush(QBrush(m_backColor));
    p.drawEllipse(-50/8,50*3/8,50/4,50/4);
    p.restore();
}
