#ifndef CIRCLEWAIT_H
#define CIRCLEWAIT_H

#include <QWidget>

class CircleWait : public QWidget
{
    Q_OBJECT
public:
    explicit CircleWait(QWidget *parent = 0);

    void setColor(QColor color);
    void setClockwiseRotate(bool clockwise);
    void setRotateDelta(int delta);

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *);
    void drawCircleWait(QPainter& p);//圆形等待

private:
    QColor m_color;
    bool m_clockwiseRotate;//顺时针旋转
    int m_rotateDelta;//旋转步进角度
    int m_rotateAngle;//旋转角度

    QTimer* m_timer;
};

#endif // CIRCLEWAIT_H
