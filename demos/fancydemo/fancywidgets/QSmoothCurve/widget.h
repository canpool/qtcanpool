#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QList>
#include <QPointF>
#include <QPainterPath>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

protected:
    void paintEvent(QPaintEvent *event);

private:
    Ui::Widget *ui;
    QList<QPointF> knots;        // 曲线上的点
    QPainterPath smoothCurve;    // 平滑曲线
    QPainterPath nonSmoothCurve; // 直接连接点的非平滑曲线
};

#endif // WIDGET_H
