#ifndef DONUTWAIT_H
#define DONUTWAIT_H

#include <QWidget>

class DonutWait : public QWidget
{
    Q_OBJECT
public:
    explicit DonutWait(QWidget *parent = 0);

    void setFrontColor(QColor color);
    void setBackColor(QColor color);
    void setClockwiseRotate(bool clockwise);
    void setRotateDelta(int delta);

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *);
    void drawDonutWait(QPainter& p);//环形
    void drawText(QPainter& p);

private:
    QColor m_frontColor;//前景色
    QColor m_backColor;//背景色
    bool m_clockwiseRotate;//顺时针旋转
    int m_rotateDelta;//旋转步进角度
    int m_rotateAngle;//旋转角度

    QTimer* m_timer;
};

#endif // DONUTWAIT_H
