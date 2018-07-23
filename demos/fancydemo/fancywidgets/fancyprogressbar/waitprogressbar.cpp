#include "waitprogressbar.h"
#include <QPainter>
#include <QTimer>

WaitProgressBar::WaitProgressBar(WaitType type, QWidget *parent)
    : QWidget(parent),
      m_frontColor(Qt::black),
      m_backColor(Qt::lightGray),
      m_clockwiseRotate(true),
      m_rotateDelta(36),
      m_rotateAngle(0),
      m_type(type)
{
    m_timer = new QTimer;
    connect(m_timer,SIGNAL(timeout()),this,SLOT(update()));
    m_timer->start(100);
}

void WaitProgressBar::setFrontColor(QColor color)
{
    if(color != m_frontColor)
    {
        m_frontColor = color;
        update();
    }
}

void WaitProgressBar::setBackColor(QColor color)
{
    if(color != m_backColor)
    {
        m_backColor = color;
        update();
    }
}

void WaitProgressBar::setClockwiseRotate(bool clockwise)
{
    if(clockwise != m_clockwiseRotate)
    {
        m_clockwiseRotate = clockwise;
        update();
    }
}

void WaitProgressBar::setRotateDelta(int delta)
{
    if(delta != m_rotateDelta)
    {
        m_rotateDelta = delta;
        update();
    }
}

void WaitProgressBar::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.translate(width()/2, height()/2);
    int side = qMin(width(), height());
    p.scale(side/100.0, side/100.0);

    p.rotate(m_rotateAngle);
    switch (m_type) {
    case Pie: drawPieWait(p); break;
    case Line: drawLineWait(p); break;
    case Circle: drawCircleWait(p); break;
    case Taichi: drawTaiChiWait(p); break;
    case Donut: drawDonutWait(p); break;
    }
    m_rotateAngle += m_clockwiseRotate?m_rotateDelta:-m_rotateDelta;
    m_rotateAngle %= 360;
}

void WaitProgressBar::drawLineWait(QPainter &p)
{
    //根据100*100的大小进行画,在paintEvent中进行过缩放
    p.save();
    p.setPen(Qt::NoPen);
    QColor color = m_frontColor;
    for(int i = 0; i <= 10; ++i)
    {
      color.setAlphaF(1.0*i/10);
      p.setBrush(QBrush(color));
      p.drawRoundedRect(50*2/5, -3, 50*3/5, 6, 2, 2);
      p.rotate(36);
    }
    p.restore();
}

void WaitProgressBar::drawCircleWait(QPainter &p)
{
    p.save();
    p.setPen(Qt::NoPen);
    QColor color = m_frontColor;
    for(int i = 0; i <= 10; ++i)
    {
      color.setAlphaF(1.0*i/10);
      p.setBrush(QBrush(color));
      p.drawEllipse(30,-10, 20, 20);
      p.rotate(36);
    }
    p.restore();
}

void WaitProgressBar::drawDonutWait(QPainter &p)
{
    int outRadious = 50;
    int inRadious = 30;
    QString valueStr = QString("%1%").arg(QString::number(qAbs(1.0*m_rotateAngle/360*100), 'f', 0));
    QRectF outRect(-outRadious, -outRadious, 2*outRadious, 2*outRadious);
    QRectF inRect(-inRadious, -inRadious, 2*inRadious, 2*inRadious);
    p.save();
    p.setPen(Qt::NoPen);
    //画外圆
    p.setBrush(QBrush(m_backColor));
    p.drawEllipse(outRect);
    p.setBrush(QBrush(m_frontColor));
    p.drawPie(outRect, 0, 60*16);
    //画遮罩
    p.setBrush(palette().window().color());
    p.drawEllipse(inRect);
    //画文字
    p.rotate(-m_rotateAngle);
    QFont f = p.font();
    f.setBold(true);
    p.setFont(f);
    p.setPen(m_frontColor);
    p.drawText(inRect, Qt::AlignCenter, valueStr);

    p.restore();
}

void WaitProgressBar::drawPieWait(QPainter &p)
{
    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(m_backColor));
    p.drawEllipse(-50, -50, 100, 100);
    p.setBrush(QBrush(m_frontColor));
    p.drawPie(-50,-50,100,100,0*16,60*16);
    p.restore();
}

void WaitProgressBar::drawTaiChiWait(QPainter &p)
{
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
