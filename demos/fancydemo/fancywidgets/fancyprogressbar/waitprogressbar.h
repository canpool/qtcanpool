#ifndef WAITPROGRESSBAR_H
#define WAITPROGRESSBAR_H

#include <QWidget>

class WaitProgressBar : public QWidget
{
    Q_OBJECT
public:
    enum WaitType{ Line, Pie, Donut, Circle, Taichi};
    explicit WaitProgressBar(WaitType type = Line, QWidget *parent = 0);

    void setFrontColor(QColor color);
    void setBackColor(QColor color);
    void setClockwiseRotate(bool clockwise);
    void setRotateDelta(int delta);

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *event);
    void drawLineWait(QPainter& p); //线形
    void drawCircleWait(QPainter& p);//圆形
    void drawDonutWait(QPainter& p);//环形
    void drawPieWait(QPainter& p);//饼状
    void drawTaiChiWait(QPainter& p);//太极

private:
    QColor m_frontColor;//前景色
    QColor m_backColor;//背景色
    bool m_clockwiseRotate;//顺时针旋转
    int m_rotateDelta;//旋转步进角度
    int m_rotateAngle;//旋转角度

    QTimer* m_timer;

    WaitType m_type;
};

#endif // WAITPROGRESSBAR_H
