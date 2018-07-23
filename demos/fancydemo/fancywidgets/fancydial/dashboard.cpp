#include "dashboard.h"
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
#include <QtGui>
#else
#include <QtWidgets>
#endif

Dashboard::Dashboard(QWidget *parent)
    :QWidget(parent)
{
    m_bAnimating = false;
    m_valueSuffix = QString("");

    initVariables();
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

//    setFixedSize(180,180);
}

void Dashboard::setRange(qreal min, qreal max)
{
    if(min >= max) return;
    m_minValue = min;
    m_maxValue = max;
    refresh();
}

void Dashboard::setMinimumValue(qreal value)
{
    if(value >= m_maxValue) return;
    m_minValue = value;
    refresh();
}

void Dashboard::setMaximumValue(qreal value)
{
    if(value <= m_minValue) return;
    m_maxValue = value;
    refresh();
}

void Dashboard::setUpperLimit(qreal value)
{
    if(value <= m_lowerLimitValue || value >= m_maxValue)
        return;
    m_upperLimitValue = value;
    update();
}

void Dashboard::setLowerLimit(qreal value)
{
    if(value >= m_upperLimitValue || value <= m_minValue)
        return;
    m_lowerLimitValue = value;
    update();
}

void Dashboard::initVariables()
{
    resetVariables();

    m_minValue = 0;
    m_maxValue = 100;
    m_value = m_minValue;
    m_currentValue = m_minValue;
    m_range = m_maxValue - m_minValue;
    m_upperLimitValue = 0.7 * m_range;
    m_lowerLimitValue = 0.2 * m_range;
    m_desc = QString(tr("Description"));

    m_normalColor = QColor(0,218,0);
    m_warnColor = QColor(218,218,0);
    m_riskColor = QColor(240,50,50);
    m_valueColor = QColor(240,240,240);

    updateTimer = new QTimer(this);
    updateTimer->setInterval(10);
    connect(updateTimer,SIGNAL(timeout()),this,SLOT(UpdateGraph()));
    singleTimer = new QTimer(this);
    singleTimer->setInterval(10);
    connect(singleTimer,SIGNAL(timeout()),this,SLOT(update()));
    singleTimer->start();
}

void Dashboard::refresh()
{
    m_value = m_minValue;
    m_currentValue = m_minValue;
    m_range = m_maxValue - m_minValue;
    m_upperLimitValue = 0.7 * m_range;
    m_lowerLimitValue = 0.2 * m_range;

    update();
}

void Dashboard::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    resetVariables(&painter);
    drawOuterCircle(&painter);
    drawInnerCircle(&painter);
    drawColorPie(&painter);
    drawCoverCircle(&painter); // cover pie center
    drawMark(&painter);
    drawIndicator(&painter);
    drawCoverBall(&painter);
    drawTextRect(&painter);
    painter.end();
}

void Dashboard::drawOuterCircle(QPainter *painter)
{
    painter->save();

    QRadialGradient outerGradient(m_center, m_outerRadius, m_center);
    outerGradient.setColorAt(0.0,QColor(200,200,200));
    outerGradient.setColorAt(0.9,QColor(80,80,80));
    outerGradient.setColorAt(0.95,QColor(180,180,180));
    outerGradient.setColorAt(1.0,QColor(80,80,80));

    painter->setPen(Qt::NoPen);
    painter->setBrush(outerGradient);
    painter->drawEllipse(m_center, m_outerRadius, m_outerRadius);
    painter->restore();
}

void Dashboard::drawInnerCircle(QPainter *painter)
{
    painter->save();
    QRadialGradient innerGradient(m_center ,m_innerRadius, m_center);
    innerGradient.setColorAt(0.0,QColor(130,130,130));
    innerGradient.setColorAt(0.7,QColor(130,130,130));
    innerGradient.setColorAt(1.0,QColor(80,80,80));
    painter->setPen(Qt::NoPen);
    painter->setBrush(innerGradient);
    painter->drawEllipse(m_center, m_innerRadius, m_innerRadius);

    painter->restore();
}

void Dashboard::drawMark(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::white);
    painter->translate(m_center);

    qreal dAngle = (qreal)270/100;
    qreal startAngle = 45;
    int value = (int)m_minValue;
    QString strValue;
    for(int i = 0; i <= 100; i++)
    {
        painter->save();
        painter->rotate(startAngle);

        if(i%10 == 0)
        {
            strValue = tr("%1").arg(value);
            qreal textWidth = fontMetrics().width(strValue);
            qreal textHeight = fontMetrics().height();
            QPointF bottomPot(0, m_colorCircleRadius+SPACE1);
            QPointF topPot(0, m_colorCircleRadius+SPACE1+LONG1);
            painter->drawLine(bottomPot,topPot);
            value += (int)(m_range/10);

            painter->save();
            QPointF textPot(0-textWidth/2, m_colorCircleRadius+SPACE1+LONG1+textHeight);
            painter->translate(textPot);
            painter->rotate(180);
            painter->drawText(QPointF(-textWidth,textHeight/2), strValue);
            painter->restore();
        }
        else if(i%5 == 0)
        {
            QPointF bottomPot(0, m_colorCircleRadius+SPACE1);
            QPointF topPot(0, m_colorCircleRadius+SPACE1+OK1);
            painter->drawLine(bottomPot,topPot);

        }
        else
        {
            QPointF bottomPot(0, m_colorCircleRadius+SPACE1);
            QPointF topPot(0, m_colorCircleRadius+SPACE1+SHORT1);
            painter->drawLine(bottomPot,topPot);
        }
        painter->restore();
        startAngle += dAngle;
    }
    painter->restore();
}

void Dashboard::drawCoverBall(QPainter *painter)
{
    painter->save();

    QRadialGradient ballGradient(m_center, m_coverBallRadius, m_center);
    ballGradient.setColorAt(0.0,QColor(140,140,140));
    ballGradient.setColorAt(0.7,QColor(140,140,140));
    ballGradient.setColorAt(1.0,QColor(60,60,60));
    painter->setBrush(ballGradient);
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(m_center, m_coverBallRadius, m_coverBallRadius);

    painter->restore();
}

void Dashboard::drawTextRect(QPainter *painter)
{
    painter->save();
    qreal rectWidth = m_coverCircleRadius/5;

    QPointF topLeftPot(m_center.x()-2*rectWidth, m_center.y()+rectWidth*2.8);
    QPointF bottomRightPot(topLeftPot.x()+4*rectWidth, topLeftPot.y()+rectWidth*2);
    QRectF textRect(topLeftPot,bottomRightPot);

    painter->setPen(Qt::NoPen);
//    painter->setBrush(QColor(0,170,255));
    painter->setBrush(QColor(100,100,100));
    painter->setOpacity(0.6);
    painter->drawRoundRect(textRect,ANGLE1,ANGLE1);

    qreal fontSize = textRect.height()/2;
    QFont font;
    font.setBold(true);
    font.setPointSize(fontSize);
    painter->setFont(font);

    painter->setOpacity(1.0);
    painter->setPen(m_valueColor);
    QString strValue;
    strValue = tr("%1%2").arg(m_value).arg(m_valueSuffix);
    painter->drawText(textRect, Qt::AlignHCenter|Qt::AlignVCenter, strValue);
    painter->restore();

    painter->save();
    topLeftPot.setX(m_center.x()-4*rectWidth);
    topLeftPot.setY(m_center.y()+rectWidth*4);
    bottomRightPot.setX(topLeftPot.x()+8*rectWidth);
    bottomRightPot.setY(topLeftPot.y()+rectWidth*5);
    QRectF titleRect(topLeftPot,bottomRightPot);
    painter->setOpacity(1.0);
    painter->setPen(Qt::white);
    strValue = tr("%1").arg(m_desc);
    painter->drawText(titleRect, Qt::AlignHCenter|Qt::AlignVCenter, strValue);
    painter->restore();
}

void Dashboard::drawCoverCircle(QPainter *painter)
{
    painter->save();
    painter->setBrush(QColor(130,130,130));
    painter->setPen(Qt::NoPen);
    painter->drawEllipse(m_center,m_coverCircleRadius,m_coverCircleRadius);
    painter->restore();
}

void Dashboard::drawColorPie(QPainter *painter)
{
    painter->save();

    QPointF topLeftPot(m_center.x()-m_colorCircleRadius, m_center.y()-m_colorCircleRadius);
    QPointF bottomRightPot(m_center.x()+m_colorCircleRadius, m_center.y()+m_colorCircleRadius);
    m_colorCircleRect = QRectF(topLeftPot,bottomRightPot);
    painter->setPen(Qt::NoPen);

//    QConicalGradient greenGradient(m_center, m_innerRadius);
//    greenGradient.setColorAt(0.0,QColor(0,30,0));
//    greenGradient.setColorAt(0.25,QColor(0,230,0));
//    greenGradient.setColorAt(1.0,QColor(0,230,0));
//    painter->setBrush(m_normalColor);
//    painter->drawPie(m_colorCircleRect,45*16,180*16);

//    painter->setBrush(m_warnColor);
//    painter->drawPie(m_colorCircleRect,0*16,45*16);

//    painter->setBrush(m_riskColor);
//    painter->drawPie(m_colorCircleRect,0,-45*16);

    int startAngle = -45*16;
    int spanAngle = 270*16*(m_maxValue-m_upperLimitValue)/m_range;
    painter->setBrush(m_riskColor);
    painter->drawPie(m_colorCircleRect, startAngle, spanAngle);

    startAngle = startAngle + spanAngle;
    spanAngle = 270*16*(m_upperLimitValue-m_lowerLimitValue)/m_range;
    painter->setBrush(m_normalColor);
    painter->drawPie(m_colorCircleRect, startAngle, spanAngle);

    startAngle = startAngle + spanAngle;
    spanAngle = 270*16*(m_lowerLimitValue-m_minValue)/m_range;
    painter->setBrush(m_riskColor);
    painter->drawPie(m_colorCircleRect, startAngle, spanAngle);

    painter->restore();
}

void Dashboard::drawIndicator(QPainter *painter)
{
    painter->save();

    painter->translate(m_center);
//    qreal increment = (qreal)270/100;
    qreal changedAngle = 45 + 270 * (m_currentValue - m_minValue)/m_range;
    painter->rotate(changedAngle);

    QPointF topPot(0, m_colorCircleRadius+LONG1);
    QPointF bottomLeftPot(-m_coverBallRadius/3, 0);
    QPointF bottomRightPot(m_coverBallRadius/3, 0);
    painter->setPen(Qt::NoPen);

    QLinearGradient indicatorGradient(topPot,bottomLeftPot);
    indicatorGradient.setColorAt(0.0,QColor(236,187,62));
    indicatorGradient.setColorAt(0.5,QColor(220,147,0));
    indicatorGradient.setColorAt(1.0,QColor(236,187,62));

    painter->setBrush(indicatorGradient);
    QVector<QPointF> potVec;
    potVec.push_back(topPot);
    potVec.push_back(bottomLeftPot);
    potVec.push_back(bottomRightPot);

    painter->drawPolygon(potVec);
    painter->restore();
}

void Dashboard::resetVariables(QPainter *painter)
{
    Q_UNUSED(painter);
    m_outerRadius = width()>height() ? height()/2: width()/2;
    m_innerRadius = m_outerRadius/6*5;
    m_coverCircleRadius = m_outerRadius/6*3;
    m_colorCircleRadius = m_outerRadius/6*3.5;
    m_coverBallRadius = m_outerRadius/7;

    m_center = rect().center();
}

void Dashboard::setValue(qreal value)
{
    if(value < m_minValue || value > m_maxValue)
        return;
    if(value < m_lowerLimitValue) m_valueColor = m_riskColor;
    else if(value < m_upperLimitValue) m_valueColor = m_normalColor;
    else m_valueColor = m_riskColor;

    if(value > m_value)
    {
        m_bReverse = false;
        m_value = value;

        if(!m_bAnimating)
        {
            m_currentValue = m_value;
        }
    }
    else if(value < m_value)
    {
        m_bReverse = true;
        m_value = value;
        if(!m_bAnimating)
        {
            m_currentValue = m_value;
        }
    }
    else
    {
        return ;
    }
    updateTimer->start();
}

void Dashboard::setValue(int value)
{
    setValue((qreal)value);
}

void Dashboard::UpdateGraph()
{
    if(!m_bReverse)
    {
        m_currentValue += 0.5 * m_range / 100;

        if(m_currentValue >= m_value)
        {
            m_currentValue = m_value;
            updateTimer->stop();
        }
    }
    else
    {
        m_currentValue -= 0.5 * m_range / 100;
        if(m_currentValue <= m_value)
        {
            m_currentValue = m_value;
            updateTimer->stop();
        }
    }
    update();
}
