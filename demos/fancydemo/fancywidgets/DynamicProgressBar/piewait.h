#ifndef PIEWAIT_H
#define PIEWAIT_H

#include <QWidget>

class PieWait : public QWidget
{
    Q_OBJECT
public:
    explicit PieWait(QWidget *parent = 0);

    void setFrontColor(QColor color);
    void setBackColor(QColor color);
    void setClockwiseRotate(bool clockwise);
    void setRotateDelta(int delta);

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *);
    void drawPieWait(QPainter& p);//饼状等待图

private:
    QColor m_frontColor;//前景色
    QColor m_backColor;//背景色
    bool m_clockwiseRotate;//顺时针旋转
    int m_rotateDelta;//旋转步进角度
    int m_rotateAngle;//旋转角度

    QTimer* m_timer;
};

#endif // PIEWAIT_H
