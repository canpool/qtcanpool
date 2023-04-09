/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#ifndef QWAITPROGRESSBAR_H
#define QWAITPROGRESSBAR_H

#include <QWidget>

class QWaitProgressBar : public QWidget
{
    Q_OBJECT
public:
    enum WaitType {
        Line,
        Pie,
        Donut,
        Circle,
        Taichi,
        Petal,
        Bearing,
        Bubble,
        Infinite,
    };

    QWaitProgressBar(WaitType type = Line, QWidget *parent = nullptr);

    void setForeColor(QColor color);
    void setBackColor(QColor color);
    void setClockwiseRotate(bool clockwise);
    void setRotateDelta(int delta);
protected:
    void paintEvent(QPaintEvent *event);
private:
    void drawLineWait(QPainter &p);
    void drawCircleWait(QPainter &p);
    void drawDonutWait(QPainter &p);
    void drawPieWait(QPainter &p);
    void drawTaiChiWait(QPainter &p);
    void drawPetalWait(QPainter &p);
    void drawBearingWait(QPainter &p);
    void drawBubbleWait(QPainter &p);
    void drawInfiniteWait(QPainter &p);
private:
    QColor m_foreColor;
    QColor m_backColor;
    bool m_clockwiseRotate;
    int m_rotateDelta;
    int m_rotateAngle;

    QTimer *m_timer;

    WaitType m_type;
};

#endif   // QTWAITPROGRESSBAR_H
