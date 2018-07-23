#include "dialplot.h"
#include "qpainter.h"
#include "qdebug.h"

DialPlot::DialPlot(QWidget *parent) :
	QWidget(parent)
{
	radius = 0;
	minValue = 0;
	maxValue = 100;
    m_upperValue = 0;
    m_lowerValue = 0;
	value = 0;
	unit = "";
    desc = "";
	centerPoint = QPointF(0, 0);

    textFont = QFont("Arial", 8, QFont::Normal);
	textColor = QColor(255, 255, 255);
	bgColor = QColor(173, 173, 173);
	firstColor = QColor(34, 139, 34);
	secondColor = QColor(238, 238, 0);
	thirdColor = QColor(205, 51, 51);
    centerColor = QColor(94, 94, 94);

//    setStyleSheet("QToolTip { background-color:rgb(200,200,200); border: 2px solid #333333; color: yellow;}");

    setMinimumSize(150,150);
}

DialPlot::~DialPlot()
{

}

void DialPlot::refresh()
{
    update();
    setToolTip(QString("<b>%1</b> %2<br>%3").arg(value).arg(unit).arg(desc));
}

void DialPlot::paintEvent(QPaintEvent *)
{
	int width = this->width();
	int height = this->height();

	//绘制准备工作,旋转坐标轴中心
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	painter.translate(width / 2, height / 2);

	//更新仪表盘最外圈半径大小
	radius = width > height ? height : width;
	radius = radius / 2;

	//绘制表盘背景
	drawBg(&painter);
	//绘制表盘
    drawDial(&painter);
	//绘制表盘刻度
    drawTicks(&painter);
	//绘制指针
    drawPointer(&painter);
	//绘制当前值
    drawValue(&painter);
}

void DialPlot::drawBg(QPainter *painter)
{
	painter->save();
	painter->setPen(Qt::NoPen);
	QRadialGradient bgGradient(centerPoint, radius, centerPoint);
//    bgGradient.setColorAt(0, bgColor.lighter(100));
//    bgGradient.setColorAt(0.6, bgColor.darker(150));
//	bgGradient.setColorAt(0.61, bgColor);
//    bgGradient.setColorAt(0.8, bgColor.darker(150));
//	bgGradient.setColorAt(1, bgColor);
    bgGradient.setColorAt(0, bgColor.lighter(100));
    bgGradient.setColorAt(0.7, bgColor.darker(150));
    bgGradient.setColorAt(0.71, bgColor);
    bgGradient.setColorAt(0.85, bgColor.darker(150));
    bgGradient.setColorAt(1, bgColor);
	painter->setBrush(bgGradient);
	painter->drawEllipse(centerPoint, radius, radius);
	painter->restore();
}

void DialPlot::drawDial(QPainter *painter)
{
	painter->save();
	painter->setPen(Qt::NoPen);

    QPointF pieRectTopLeftPot(- radius, - radius);
    QPointF pieRectBottomRightPot(radius, radius);
	QRectF pieRect = QRectF(pieRectTopLeftPot, pieRectBottomRightPot);

	//将表盘分区间三种不同颜色显示
//	QRadialGradient firstGradient(centerPoint, radius, centerPoint);
//	firstGradient.setColorAt(0, Qt::transparent);
//	firstGradient.setColorAt(0.6, Qt::transparent);
//	firstGradient.setColorAt(0.61, firstColor.darker(150));
//    firstGradient.setColorAt(0.8, firstColor.lighter(100));
//	firstGradient.setColorAt(1.0, firstColor.darker(150));
//	painter->setBrush(firstGradient);
//    painter->drawPie(pieRect, 130 * 16, 80 * 16);

//	QRadialGradient secondGradient(centerPoint, radius, centerPoint);
//	secondGradient.setColorAt(0, Qt::transparent);
//	secondGradient.setColorAt(0.6, Qt::transparent);
//	secondGradient.setColorAt(0.61, secondColor.darker(150));
//    secondGradient.setColorAt(0.8, secondColor.lighter(100));
//	secondGradient.setColorAt(1.0, secondColor.darker(150));
//	painter->setBrush(secondGradient);
//	painter->drawPie(pieRect, 50 * 16, 80 * 16);

//	QRadialGradient thirdGradient(centerPoint, radius, centerPoint);
//	thirdGradient.setColorAt(0, Qt::transparent);
//	thirdGradient.setColorAt(0.6, Qt::transparent);
//	thirdGradient.setColorAt(0.61, thirdColor.darker(150));
//    thirdGradient.setColorAt(0.8, thirdColor.lighter(100));
//	thirdGradient.setColorAt(1.0, thirdColor.darker(150));
//	painter->setBrush(thirdGradient);
//	painter->drawPie(pieRect, 330 * 16, 80 * 16);

    int startAngle = -30*16;
    int spanAngle = 240*16*(maxValue-m_upperValue)/(maxValue-minValue);
    QRadialGradient thirdGradient(centerPoint, radius, centerPoint);
    thirdGradient.setColorAt(0, Qt::transparent);
    thirdGradient.setColorAt(0.7, Qt::transparent);
    thirdGradient.setColorAt(0.71, thirdColor.darker(150));
    thirdGradient.setColorAt(0.85, thirdColor.lighter(100));
    thirdGradient.setColorAt(1.0, thirdColor.darker(150));
    painter->setBrush(thirdGradient);
    painter->drawPie(pieRect, startAngle, spanAngle);

    startAngle = startAngle + spanAngle;
    spanAngle = 240*16*(m_upperValue-m_lowerValue)/(maxValue-minValue);
    QRadialGradient secondGradient(centerPoint, radius, centerPoint);
    secondGradient.setColorAt(0, Qt::transparent);
    secondGradient.setColorAt(0.7, Qt::transparent);
    secondGradient.setColorAt(0.71, secondColor.darker(150));
    secondGradient.setColorAt(0.85, secondColor.lighter(100));
    secondGradient.setColorAt(1.0, secondColor.darker(150));
    painter->setBrush(secondGradient);
    painter->drawPie(pieRect, startAngle, spanAngle);

    startAngle = startAngle + spanAngle;
    spanAngle = 240*16*(m_lowerValue-minValue)/(maxValue-minValue);
    QRadialGradient firstGradient(centerPoint, radius, centerPoint);
    firstGradient.setColorAt(0, Qt::transparent);
    firstGradient.setColorAt(0.7, Qt::transparent);
    firstGradient.setColorAt(0.71, firstColor.darker(150));
    firstGradient.setColorAt(0.85, firstColor.lighter(100));
    firstGradient.setColorAt(1.0, firstColor.darker(150));
    painter->setBrush(firstGradient);
    painter->drawPie(pieRect, startAngle, spanAngle);

	painter->restore();
}

void DialPlot::drawTicks(QPainter *painter)
{
	painter->save();
	painter->setPen(textColor);

	//刻度长度占圆盘半径比例
	double tickRatio = 0.05;

	//旋转坐标轴到刻度起始位置
	painter->rotate(60);

	for (int scale = 0; scale <= 100; ++scale) {
		//逢十的倍数,刻度变长
		tickRatio = scale % 10 ? 0.05 : 0.1;

		//绘制刻度线
//        painter->drawLine(QPointF(0, radius * (0.6 - tickRatio)), QPointF(0, radius * 0.6));

//        if (scale == 0) {
//            //绘制表盘上最小值
//            painter->drawText(-10, radius * (0.38), 20, 10, Qt::AlignCenter, QString::number(minValue));
//        } else if (scale == 100) {
//            //绘制表盘上最大值
//            painter->drawText(-10, radius * (0.38), 20, 10, Qt::AlignCenter, QString::number(maxValue));
//        }
        painter->drawLine(QPointF(0, radius * (0.7 - tickRatio)), QPointF(0, radius * 0.7));
        painter->setFont(textFont);
		if (scale == 0) {
            //绘制表盘上最小值
            painter->drawText(-15, radius * (0.42), 30, 10, Qt::AlignCenter, QString::number(minValue));
		} else if (scale == 100) {
            //绘制表盘上最大值
            painter->drawText(-15, radius * (0.42), 30, 10, Qt::AlignCenter, QString::number(maxValue));
		}

		painter->rotate(2.4);
	}

	painter->restore();
}

void DialPlot::drawPointer(QPainter *painter)
{
	//指针宽度
	int pointerWidth = 6;

	//指针路径点
	const QPoint pointers[3] = {
		QPoint(centerPoint.x() - pointerWidth / 3, centerPoint.y()),
		QPoint(centerPoint.x() + pointerWidth / 3, centerPoint.y()),
		QPoint(centerPoint.x(), centerPoint.y() - radius)
	};

	painter->save();
	painter->rotate(240);
	painter->setPen(Qt::NoPen);

	//将当前实际值转换为刻度值
//	double first = (double)100 / 3;
//	double second = first * 2;
//	double currentValue = 100 * (this->value - this->minValue) / (this->maxValue - this->minValue);
    double first = (double)(m_lowerValue-minValue);
    double second = (double)(m_upperValue-minValue);
    double currentValue = (value - minValue);
//	QColor pointerColor = firstColor;
    pointerColor = firstColor;

    if (currentValue <= first) {
		pointerColor = firstColor;
    } else if (currentValue <= second) {
		pointerColor = secondColor;
	} else {
		pointerColor = thirdColor;
	}

	//画刷颜色设定
	painter->setBrush(pointerColor);

    if (currentValue <= (maxValue-minValue)) {
        painter->rotate(240 * currentValue/(maxValue-minValue));
	} else {
		painter->rotate(360);
	}

	painter->drawConvexPolygon(pointers, 3);
	painter->restore();

	//绘制指针中心圆
	painter->save();
	painter->setPen(Qt::NoPen);
	painter->setBrush(centerColor);
	painter->drawEllipse(QPoint(0, 0), pointerWidth, pointerWidth);
	painter->restore();
}

void DialPlot::drawValue(QPainter *painter)
{
	painter->save();
	painter->setPen(textColor);
    painter->setFont(textFont);
    painter->drawText(QRectF(-30, radius * 0.6 * 0.4, 60, 30), Qt::AlignCenter, QString::number(value) + unit);
    painter->drawText(QRectF(-30, radius * 0.7 * 0.5 + textFont.pointSize()/2, 60, 30), Qt::AlignCenter, desc);
	painter->restore();
}

void DialPlot::setRange(double minValue, double maxValue)
{
	//如果最小值大于或者等于最大值则不设置
	if (minValue >= maxValue) {
		return;
	}

	this->minValue = minValue;
	this->maxValue = maxValue;

    m_upperValue = 0;
    m_lowerValue = 0;

	//如果目标值不在范围值内,则重新设置目标值
	if (value < minValue || value > maxValue) {
		setValue(value);
	}

    refresh();
}

void DialPlot::setRange(int minValue, int maxValue)
{
	setRange((double)minValue, (double)maxValue);
}

void DialPlot::setValue(double value)
{
	//如果目标值比最小值小则取最小值,如果目标值比最大值大则取最大值
	if (value < minValue) {
		this->value = minValue;
	} else if (value > maxValue) {
		this->value = maxValue;
	} else {
		this->value = value;
	}

    refresh();
	emit valueChanged(value);
}

void DialPlot::setValue(int value)
{
    setValue((double)value);
}

void DialPlot::setUpper(double value)
{
    if(value <= m_lowerValue || value > maxValue)
        return;
    m_upperValue = value;
    refresh();
}

void DialPlot::setLower(double value)
{
    if(value >= m_upperValue || value < minValue)
        return;
    m_lowerValue = value;
    refresh();
}

void DialPlot::setUnit(QString unit)
{
	this->unit = unit;
    refresh();
}

void DialPlot::setDesc(QString desc)
{
    this->desc = desc;
    refresh();
}

void DialPlot::setTextFont(QFont textFont)
{
	this->textFont = textFont;
    refresh();
}

void DialPlot::setTextColor(QColor textColor)
{
	this->textColor = textColor;
    refresh();
}

void DialPlot::setBgColor(QColor bgColor)
{
	this->bgColor = bgColor;
    refresh();
}

void DialPlot::setFirstColor(QColor firstColor)
{
	this->firstColor = firstColor;
    refresh();
}

void DialPlot::setSecondColor(QColor secondColor)
{
	this->secondColor = secondColor;
    refresh();
}

void DialPlot::setThirdColor(QColor thirdColor)
{
	this->thirdColor = thirdColor;
    refresh();
}

void DialPlot::setCenterColor(QColor centerColor)
{
	this->centerColor = centerColor;
    refresh();
}
