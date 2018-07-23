#ifndef DIALPLOT_H
#define DIALPLOT_H

#include <QWidget>

/**
 * 作者:feiyangqingyun(QQ:517216493) 2016-10-15
 * 本控件来源于网络(原作者:lx100991(http://www.qtcn.org/bbs/read-htm-tid-62042.html))
 * 1:增加设置背景色/第一第二第三颜色
 * 2:增加设置文字字体及文字和刻度颜色
 * 3:增加设置中间圆点颜色
 * 4:修正当范围值中最小值为负数时的bug
 * 5:修正刻度值在三分之一阶段时切换不准确的bug
 * 6:增加一些完整性校验
 */

class DialPlot : public QWidget
{
	Q_OBJECT

public:
	explicit DialPlot(QWidget *parent = 0);
	~DialPlot();

private:
    void refresh();

public:
	//获取最小值
	double getMinValue()const
	{
		return minValue;
	}

	//获取最大值
	double getMaxValue()const
	{
		return maxValue;
	}

    double getUpperValue() const { return m_upperValue; }
    double getLowerValue() const { return m_lowerValue; }

	//获取目标值
	double getValue()const
	{
		return value;
	}

	//获取单位
	QString getUnit()const
	{
		return unit;
	}

signals:
    void valueChanged(double value);

public slots:
	//设置最大最小值-范围值
	void setRange(double minValue, double maxValue);
	void setRange(int minValue, int maxValue);

	//设置目标值
	void setValue(double value);
	void setValue(int value);

    void setUpper(double value);
    void setLower(double value);

	//设置文字后面的单位
	void setUnit(QString unit);
    void setDesc(QString desc);

	//设置文字字体
	void setTextFont(QFont textFont);

	//设置文字及刻度的颜色
	void setTextColor(QColor textColor);

	//设置背景色
	void setBgColor(QColor bgColor);

	//设置第一/第二/第三颜色
	void setFirstColor(QColor firstColor);
	void setSecondColor(QColor secondColor);
	void setThirdColor(QColor thirdColor);

	//设置中间圆点颜色
	void setCenterColor(QColor centerColor);

private slots:
    void drawBg(QPainter *painter);         //绘制表盘背景
    void drawDial(QPainter *painter);       //绘制表盘
    void drawTicks(QPainter *painter);      //绘制表盘刻度
    void drawPointer(QPainter *painter);    //绘制指针
    void drawValue(QPainter *painter);      //绘制显示值和单位

protected:
    void paintEvent(QPaintEvent *);

private:
    int radius;             //仪表盘最外圈半径
    QPointF centerPoint;    //中心坐标
    int minValue;           //表盘刻度最小值
    int maxValue;           //表盘刻度最大值
    double m_upperValue;
    double m_lowerValue;
    double value;           //当前值
    QString unit;           //单位
    QString desc;

    QFont textFont;         //文字字体
    QColor textColor;       //刻度及文字颜色
    QColor bgColor;         //背景色
    QColor firstColor;      //第一颜色
    QColor secondColor;     //第二颜色
    QColor thirdColor;      //第三颜色
    QColor centerColor;     //中间圆点颜色
    QColor pointerColor;
};

#endif // DIALPLOT_H
