#include "colorprogressbar.h"
#include <QPainter>
#include <QDebug>

ColorProgressBar::ColorProgressBar(QWidget *parent)
    : QWidget(parent),
      m_min(0),
      m_max(100),
      m_value(0),
      m_barBackGroundColor(Qt::white),
      m_barColor(QColor(255, 107, 107)),
      m_decimal(0),
      m_xRadius(5),
      m_yRadius(5)
{
}

ColorProgressBar::~ColorProgressBar()
{

}

void ColorProgressBar::setBarBackGroundColor(QColor color)
{
    if(color != m_barBackGroundColor)
    {
        m_barBackGroundColor = color;
        update();
    }
}

void ColorProgressBar::setBarSplitLineColor(QColor color)
{
    if(color != m_barSplitLineColor)
    {
        m_barSplitLineColor = color;
        update();
    }
}

void ColorProgressBar::setBarColor(QColor color)
{
    if(color != m_barColor)
    {
        m_barColor = color;
        update();
    }
}

void ColorProgressBar::setDecimal(int decimal)
{
    if(decimal != m_decimal)
    {
        m_decimal = decimal;

        update();
    }
}

void ColorProgressBar::setBarSplitLineDelta(int delta)
{
    if(delta != m_barSplitLineDelta)
    {
        m_barSplitLineDelta = delta;
        update();
    }
}

void ColorProgressBar::setShowBarSplitLine(bool show)
{
    if(show != m_showBarSplitLine)
    {
        m_showBarSplitLine = show;
        update();
    }
}

void ColorProgressBar::setValue(double value)
{
    if(value != m_value)
    {
        m_value = value;
        update();
    }
}

void ColorProgressBar::setValue(int value)
{
    setValue((double)value);
}

void ColorProgressBar::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int space = 10;
    QRectF barRect(0, 0, 0.9*width()-space, height());
    QRectF textRect(0.9*width()+space, 0, 0.1*width()-space, height());

    drawBackGround(p, barRect, textRect);
    drawData(p, barRect);
    if(m_showBarSplitLine)
        drarBarSplitLine(p, barRect);
    drawText(p, textRect);
}

void ColorProgressBar::drawBackGround(QPainter &p, const QRectF &barRect, const QRectF &textRect)
{
    p.save();
    //进度条背景
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(m_barBackGroundColor));
    p.drawRoundedRect(barRect, m_xRadius, m_yRadius);
    //值背景
    p.drawRoundedRect(textRect, m_xRadius, m_yRadius);
    p.restore();
}

void ColorProgressBar::drawData(QPainter &p, const QRectF &barRect)
{
    p.save();
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(m_barColor));
    double dx = (1 - (m_value - m_min) / (m_max - m_min)) * barRect.width();
    p.drawRoundedRect(barRect.adjusted(0, 0, -dx, 0), m_xRadius, m_yRadius);
    p.restore();
}

void ColorProgressBar::drarBarSplitLine(QPainter &p, const QRectF &barRect)
{
    if(!m_showBarSplitLine)
        return;
    p.save();
    p.setPen(m_barSplitLineColor);
    for(int i=0; i<barRect.right(); i+=m_barSplitLineDelta+p.pen().width())
    {
        p.drawLine(i, 0, i, height());
    }
    p.restore();
}

void ColorProgressBar::drawText(QPainter &p, const QRectF &textRect)
{
    p.save();
    //画值
    p.setPen(palette().text().color());
    double percent = (m_value - m_min) / (m_max - m_min) * 100.0;
    QString valueStr = QString("%1%").arg(QString::number(percent, 'f', m_decimal));
    p.drawText(textRect, Qt::AlignCenter, valueStr);
    p.restore();
}
