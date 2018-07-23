#include "linewait.h"
#include <QPainter>
#include <QTimer>

LineWait::LineWait(QWidget *parent) : QWidget(parent),
    m_color(Qt::black),
    m_clockwiseRotate(true),
    m_rotateDelta(36),
    m_rotateAngle(0)
{
    m_timer = new QTimer;
    connect(m_timer,SIGNAL(timeout()),this,SLOT(update()));
    m_timer->start(100);
}

void LineWait::setColor(QColor color)
{
    if(color != m_color)
    {
        m_color = color;
        update();
    }
}

void LineWait::setClockwiseRotate(bool clockwise)
{
    if(clockwise != m_clockwiseRotate)
    {
        m_clockwiseRotate = clockwise;
        update();
    }
}

void LineWait::setRotateDelta(int delta)
{
    if(delta != m_rotateDelta)
    {
        m_rotateDelta = delta;
        update();
    }
}

void LineWait::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.translate(width()/2, height()/2);
    int side = qMin(width(), height());
    p.scale(side/100.0, side/100.0);

    p.rotate(m_rotateAngle);
    drawLineWait(p);
    m_rotateAngle += m_clockwiseRotate?m_rotateDelta:-m_rotateDelta;
    m_rotateAngle %= 360;
}

void LineWait::drawLineWait(QPainter &p)
{
    //根据100*100的大小进行画,在paintEvent中进行过缩放
    p.save();
    p.setPen(Qt::NoPen);
    QColor color = m_color;
    for(int i = 0; i<=10; ++i)
    {
      color.setAlphaF(1.0*i/10);
      p.setBrush(QBrush(color));
      p.drawRoundedRect(50*2/5, -3, 50*3/5, 6, 2, 2);
      p.rotate(36);
    }
    p.restore();
}
