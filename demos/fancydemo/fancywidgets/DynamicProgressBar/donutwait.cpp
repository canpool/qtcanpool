#include "donutwait.h"
#include <QPainter>
#include <QTimer>

DonutWait::DonutWait(QWidget *parent) : QWidget(parent),
    m_frontColor(Qt::yellow),
    m_backColor(Qt::black),
    m_clockwiseRotate(true),
    m_rotateDelta(36),
    m_rotateAngle(0)
{
    m_timer = new QTimer;
    connect(m_timer,SIGNAL(timeout()),this,SLOT(update()));
    m_timer->start(100);
}

void DonutWait::setFrontColor(QColor color)
{
    if(color != m_frontColor)
    {
        m_frontColor = color;
        update();
    }
}

void DonutWait::setBackColor(QColor color)
{
    if(color != m_backColor)
    {
        m_backColor = color;
        update();
    }
}

void DonutWait::setClockwiseRotate(bool clockwise)
{
    if(clockwise != m_clockwiseRotate)
    {
        m_clockwiseRotate = clockwise;
        update();
    }
}

void DonutWait::setRotateDelta(int delta)
{
    if(delta != m_rotateDelta)
    {
        m_rotateDelta = delta;
        update();
    }
}

void DonutWait::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.translate(width()/2, height()/2);
    int side = qMin(width(), height());
    p.scale(side/100.0, side/100.0);

    m_rotateAngle += m_clockwiseRotate?m_rotateDelta:-m_rotateDelta;
    p.rotate(m_rotateAngle);
    drawDonutWait(p);
    m_rotateAngle %= 360;
}

void DonutWait::drawDonutWait(QPainter &p)
{
    //根据100*100的大小进行画,在paintEvent中进行过缩放
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
