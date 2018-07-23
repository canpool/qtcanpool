#ifndef STATEPROGRESSBAR_H
#define STATEPROGRESSBAR_H

#include <QWidget>

#define LEFT_MARGIN     40
#define SPACE           80

class StateProgressBar : public QWidget
{
    Q_OBJECT
public:
    enum Type{ NACK, ACK };
    explicit StateProgressBar(QWidget *parent = 0);

    void setTip(QString tip);

    void setStates(QStringList states);
    void setType(Type type);

    void setBarColor(QColor color);
    void setBarBackgroundColor(QColor color);
    void setStateColor(QColor color);
    void setTipColor(QColor color);
    void setTipBackgroundColor(QColor color);

private:
    void drawBackground(QPainter* painter);      //绘制整个背景颜色
    void drawBarBackground(QPainter* painter);   //绘制灰色的导航条：包括灰色的圆，灰色的长条矩形和灰色的text
    void drawBar(QPainter* painter);         //绘制蓝色信息：圆、长条和本文
    void drawBarNumber(QPainter* painter);       //绘制数字
    void adjustWidth();

signals:

public slots:
    void setValue(int value);

protected:
    void paintEvent(QPaintEvent *event);

private:
    int m_value;
    int m_bigRadius;
    int m_smallRadius;

    int m_lineHeight;
    int m_lineWidth;
    int m_innerLineHeight;

    QStringList m_states;

    QString m_tip;

    QColor m_barColor;
    QColor m_barBackgroundColor;
    QColor m_stateColor;
    QColor m_tipColor;
    QColor m_tipBackgroundColor;

    Type m_type;

};

#endif // STATEPROGRESSBAR_H
