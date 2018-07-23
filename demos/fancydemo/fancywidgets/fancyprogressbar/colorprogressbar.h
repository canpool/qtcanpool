#ifndef COLORPROGRESSBAR_H
#define COLORPROGRESSBAR_H

#include <QWidget>

class ColorProgressBar : public QWidget
{
    Q_OBJECT

public:
    ColorProgressBar(QWidget *parent = 0);
    ~ColorProgressBar();

    void setBarBackGroundColor(QColor color);
    void setBarSplitLineColor(QColor color);
    void setBarColor(QColor color);
    void setDecimal(int decimal);
    void setBarSplitLineDelta(int delta);
    void setShowBarSplitLine(bool show);

public slots:
    void setValue(double value);
    void setValue(int value);

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void drawBackGround(QPainter& p, const QRectF& barRect, const QRectF& textRect);
    virtual void drawData(QPainter& p, const QRectF& barRect);
    virtual void drarBarSplitLine(QPainter& p, const QRectF& barRect);
    virtual void drawText(QPainter& p, const QRectF& textRect);

private:
    double m_min,m_max,m_value;
    QColor m_barBackGroundColor;//背景颜色
    QColor m_barSplitLineColor;//背景分割线颜色
    int m_barSplitLineDelta;//背景分割线间距
    bool m_showBarSplitLine;//是否显示背景分割线
    QColor m_barColor;
    int m_decimal;

    int m_valueWidth;//数值宽度
    int m_xRadius;
    int m_yRadius;
};

#endif // COLORPROGRESSBAR_H
