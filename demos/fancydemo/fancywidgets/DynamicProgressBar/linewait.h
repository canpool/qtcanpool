#ifndef LINEWAIT_H
#define LINEWAIT_H

#include <QWidget>

class LineWait : public QWidget
{
    Q_OBJECT
public:
    explicit LineWait(QWidget *parent = 0);

    void setColor(QColor color);
    void setClockwiseRotate(bool clockwise);
    void setRotateDelta(int delta);

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *);
    void drawLineWait(QPainter& p);//圆形等待

private:
    QColor m_color;
    bool m_clockwiseRotate;//顺时针旋转
    int m_rotateDelta;//旋转步进角度
    int m_rotateAngle;//旋转角度

    QTimer* m_timer;
};

#endif // LINEWAIT_H
