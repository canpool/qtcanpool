#ifndef ZOOMCIRCLEWAIT_H
#define ZOOMCIRCLEWAIT_H

#include <QWidget>

class ZoomCircleWait : public QWidget
{
    Q_OBJECT
public:
    explicit ZoomCircleWait(QWidget *parent = 0);

    void setColor(QColor color);
    void setZoomOut(bool zoomOut);
    void setZoomDelta(int delta);
    void setMinRadious(int value);
    void setRadious(int value);

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *);
    void drawZoomCricleWait(QPainter& p);//缩放圆

private:
    QColor m_color;//前景色
    bool m_zoomOut;//是否缩小
    int m_zoomDelta;//缩放步进
    int m_radious;//当前半径
    int m_minRadious;//最小半径

    QTimer* m_timer;

    void fixRaidous(int max);
};

#endif // ZOOMCIRCLEWAIT_H
