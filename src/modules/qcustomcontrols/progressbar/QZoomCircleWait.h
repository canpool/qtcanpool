/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#ifndef QZOOMCIRCLEWAIT_H
#define QZOOMCIRCLEWAIT_H

#include <QWidget>

class QZoomCircleWait : public QWidget
{
    Q_OBJECT
public:
    explicit QZoomCircleWait(QWidget *parent = nullptr);

    void setColor(QColor color);
    void setZoomOut(bool zoomOut);
    void setZoomDelta(int delta);
    void setMinRadius(int value);
    void setRadius(int value);
protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void drawZoomCricleWait(QPainter &p);
private:
    void fixRaidus(int max);
private:
    QColor m_color;
    bool m_zoomOut;
    int m_zoomDelta;
    int m_radius;
    int m_minRadius;

    QTimer *m_timer;
};

#endif   // QTZOOMCIRCLEWAIT_H
