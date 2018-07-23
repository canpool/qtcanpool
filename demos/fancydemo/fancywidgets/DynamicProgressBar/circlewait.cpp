#include "circlewait.h"
#include <QPainter>
#include <QTimer>

CircleWait::CircleWait(QWidget *parent) : QWidget(parent),
    m_color(Qt::black),
    m_clockwiseRotate(true),
    m_rotateDelta(36),
    m_rotateAngle(0)
{
    m_timer = new QTimer;
    connect(m_timer,SIGNAL(timeout()),this,SLOT(update()));
    m_timer->start(100);
}

void CircleWait::setColor(QColor color)
{
    if(color != m_color)
    {
        m_color = color;
        update();
    }
}

void CircleWait::setClockwiseRotate(bool clockwise)
{
    if(clockwise != m_clockwiseRotate)
    {
        m_clockwiseRotate = clockwise;
        update();
    }
}

void CircleWait::setRotateDelta(int delta)
{
    if(delta != m_rotateDelta)
    {
        m_rotateDelta = delta;
        update();
    }
}

void CircleWait::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.translate(width()/2, height()/2);
    int side = qMin(width(), height());
    p.scale(side/100.0, side/100.0);

    p.rotate(m_rotateAngle);
    drawCircleWait(p);
    m_rotateAngle += m_clockwiseRotate?m_rotateDelta:-m_rotateDelta;
    m_rotateAngle %= 360;
}

void CircleWait::drawCircleWait(QPainter &p)
{
    //根据100*100的大小进行画,在paintEvent中进行过缩放
    p.save();
    p.setPen(Qt::NoPen);
    QColor color = m_color;
    for(int i = 0; i<=10; ++i)
    {
      color.setAlphaF(1.0*i/10);
      p.setBrush(QBrush(color));
      p.drawEllipse(30,-10, 20, 20);
      p.rotate(36);
    }
    p.restore();
}
