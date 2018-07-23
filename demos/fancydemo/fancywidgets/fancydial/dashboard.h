#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>

#define LONG1 7
#define OK1 5
#define SHORT1 3
#define SPACE1 3
#define ANGLE1 10

#include <QtGui>

class Dashboard : public QWidget
{
    Q_OBJECT
public:
    explicit Dashboard(QWidget *parent = 0);

    void setDesc(const QString &text) { m_desc = text;}
    void setValueSuffix(const QString &text) { m_valueSuffix = text;}

    void setRange(qreal min, qreal max);
    void setMinimumValue(qreal value);
    void setMaximumValue(qreal value);
    void setUpperLimit(qreal value);
    void setLowerLimit(qreal value);

public slots:
    void setValue(qreal value);
    void setValue(int value);
    void setAnimating(bool enable)
    {
        m_bAnimating=enable;
        update();
    }

public:
    bool isAnimating() const
    {
        return m_bAnimating;
    }

protected:
    void paintEvent(QPaintEvent *);

    QSize sizeHint() const
    {
        return QSize(300,300);
    }

    QSize minimumSizeHint() const
    {
        return QSize(200,200);
    }

private:
    void drawOuterCircle(QPainter* painter);
    void drawInnerCircle(QPainter* painter);
    void drawMark(QPainter* painter);
    void drawCoverCircle(QPainter* painter);
    void drawColorPie(QPainter* painter);
    void drawIndicator(QPainter* painter);
    void drawCoverBall(QPainter* painter);
    void resetVariables(QPainter* painter = 0);
    void drawTextRect(QPainter* painter);

private:
    void initVariables();
    void refresh();

private slots:
    void UpdateGraph();

private:
    qreal m_outerRadius;
    qreal m_innerRadius;
    qreal m_coverCircleRadius;
    qreal m_colorCircleRadius;
    qreal m_coverBallRadius;
    QPointF m_center;
    QRectF m_colorCircleRect;
    qreal m_value;
    QTimer* updateTimer;
    bool m_bReverse;
    qreal m_currentValue;
    QTimer* singleTimer;
    bool m_bAnimating;

    qreal m_minValue;
    qreal m_maxValue;
    qreal m_upperLimitValue;
    qreal m_lowerLimitValue;
    qreal m_range;

    QString m_desc;
    QString m_valueSuffix;

    QColor m_normalColor;
    QColor m_warnColor;
    QColor m_riskColor;

    QColor m_valueColor;
};

#endif // DASHBOARD_H
