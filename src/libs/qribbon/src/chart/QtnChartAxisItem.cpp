/****************************************************************************
**
** Qtitan Library by Developer Machines (Charts and Diagrams for Qt.C++)
** 
** Copyright (c) 2009-2013 Developer Machines (http://www.devmachines.com)
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/
#include <math.h>

#include "QtnChart.h"
#include "QtnChartAxisItemPrivate.h"
#include "QtnChartAxisPrivate.h"
#include "QtnChartPainter.h"
#include "QtnChartAxis.h"
#include "QtnChartScaleTypeMap.h"
#include "QtnChartTextPainter.h"
#include "QtnChartArea2D.h"
#include "QtnChartLegendPrivate.h"
#include "QtnChartMath.h"


using namespace Qtitan;

#define  SCROLLBARSIZE 10

namespace Qtitan
{
    class ChartAxisViewTickEnumerator
    {
    public:
        ChartAxisViewTickEnumerator(ChartAxisItem* item, bool minor);
        bool getNext(qreal& mark);

    protected:
        ChartAxisItem* m_item;
        bool m_minor;
        int m_minorCount;
        qreal m_minValue;
        qreal m_maxValue;
        qreal m_gridSpacing;
        qreal m_minordGridSpacing;
        int m_curMinor;

        qreal m_mark;
        int m_curCustomLabel;
    };
}

ChartAxisViewTickEnumerator::ChartAxisViewTickEnumerator(ChartAxisItem* item, bool minor)
{
    m_item = item;
    m_minor = minor;
    m_minorCount = item->axis()->minorCount();

    m_minValue = item->rangeMinValueItem();
    m_maxValue = item->rangeMaxValueItem();
    m_gridSpacing = item->gridSpacing();

    m_minordGridSpacing = m_gridSpacing / (m_minorCount + 1);

    m_mark = ((int)(m_minValue / m_gridSpacing)) * m_gridSpacing;
    if (m_mark < m_minValue)
        m_mark += m_gridSpacing;

    m_curMinor = 0;

    if (m_minor)
    {
        m_mark -= m_gridSpacing;
        while (m_mark < m_minValue)
        {
            m_mark += m_minordGridSpacing;
            m_curMinor++;
        }
    }

    m_curCustomLabel = -1;
//  if (item->axis()->customLabelCount())
//      m_curCustomLabel = 0;

}

bool ChartAxisViewTickEnumerator::getNext(qreal& mark)
{
    if (m_minor && m_curCustomLabel != -1)
        return false;
/*
    if (m_curCustomLabel != -1)
    {
        ChartAxis* axis = m_item->axis();
        if (m_curCustomLabel >= axis->customLabelCount())
            return false;

        mark = !axis->axisCustomLabelValue(m_curCustomLabel).isEmpty() ? m_item->axis()->qtn_d().m_scaleTypeMap->valueToInternal(axis->axisCustomLabelValue(m_curCustomLabel)) :
                axis->axisCustomLabelValueInternal(m_curCustomLabel);

        m_curCustomLabel++;
        return true;
    }
*/
    if (m_mark > m_maxValue)
        return false;

    mark = m_mark;

    if (m_minor)
    {
        m_mark += m_minordGridSpacing;
        m_curMinor += 1;

        if (m_curMinor == m_minorCount + 1)
        {
            m_mark += m_minordGridSpacing;
            m_curMinor = 1;
        }
    }
    else
    {
        m_mark += m_gridSpacing;
    }
    return true;
}


/*!
    \class Qtitan::ChartAxisItem
    \inmodule QtitanChart
    \internal
*/
ChartAxisItem::ChartAxisItem(ChartAxis* axis, GraphicsItem* parentItem)
    : GraphicsItem(parentItem)
{
    m_axis = axis;
    m_gridSpacing = 1;
    m_size = 0;
}

ChartAxisItem::~ChartAxisItem()
{
}

qreal ChartAxisItem::rangeMinValue() const
{
    return m_axis->minValue();
}

qreal ChartAxisItem::rangeMaxValue() const
{
    return m_axis->maxValue();
}

qreal ChartAxisItem::rangeMinValueItem() const
{
    return qMax(m_axis->viewMinValue(), rangeMinValue());
}

qreal ChartAxisItem::rangeMaxValueItem() const
{
    return qMin(m_axis->viewMaxValue(), rangeMaxValue());
}

void ChartAxisItem::drawInterlaced(QPainter* p, const QRect& rcPane)
{
    if (!m_axis->isInterlaced())
        return;

    if (m_arrTicks.size() < 1)
        return;

    bool vertical = isVertical();

    bool reversed = m_axis->isReversed();

    int left = vertical ?  (reversed ? m_boundingRect.top() : m_boundingRect.bottom()) : 
        (reversed ? m_boundingRect.right() : m_boundingRect.left());

    int i = 0;
    qreal mark;

    qreal spacing = gridSpacing();

    int count = int((m_arrTicks[0].m_value - rangeMinValue()) / spacing);

    if ((count & 1) == 1)
    {
        mark = m_arrTicks[0].m_value;
        left = (int)valueToPoint(mark);
        i = 1;
    }

    while (i <= m_arrTicks.size())
    {
        int right;

        if (i >= m_arrTicks.size())
        {
            right = vertical ?  (!reversed ? m_boundingRect.top() : m_boundingRect.bottom()) : 
                (!reversed ? m_boundingRect.right() : m_boundingRect.left());
        }
        else
        {
            mark = m_arrTicks[i].m_value;
            right = (int)valueToPoint(mark);
        }

        i++;

        QRectF rc = vertical ? QRectF(QPointF((qreal)rcPane.left(), (qreal)right), QSizeF((qreal)rcPane.width(), (qreal)left - right)) :
            QRectF(QPointF((qreal)left, (qreal)rcPane.top()+1), QSizeF((qreal)right - left, (qreal)rcPane.height()));

        if (rc.width() < 0.0) 
            rc.setX(rc.x() + rc.width()), rc.setWidth(-rc.width());

        if (rc.height() < 0.0) 
            rc.setY(rc.x() + rc.height()), rc.setHeight(-rc.height());

        m_axis->qtn_d().interlacedFillHelper()->drawGradientRect(p, rc, m_axis->qtn_d().interlacedStartColor(), m_axis->qtn_d().interlacedFinalColor());

        if (i >= m_arrTicks.size())
            break;

        mark = m_arrTicks[i].m_value;
        left = (int)valueToPoint(mark);

        i++;
    }
}

void ChartAxisItem::drawStrips(QPainter* p, const QRect& rcPane)
{
    const StreakList& strips = m_axis->qtn_d().m_streaks;
    if (strips.count() == 0)
        return;

    bool vertical = isVertical();

    for (int i = 0; i < strips.count(); i++)
    {
        ChartAxisIntervalStreak* strip = strips.at(i);
        if (!strip->isVisible())
            continue;

        qreal markLeft = !strip->getAxisMinValue().isEmpty() ? m_axis->qtn_d().m_scaleTypeMap->valueToInternal(strip->getAxisMinValue()) :
            strip->getAxisMinValueInternal();

        qreal markRight = !strip->getAxisMaxValue().isEmpty() ? m_axis->qtn_d().m_scaleTypeMap->valueToInternal(strip->getAxisMaxValue()) :
            strip->getAxisMaxValueInternal();

        int left = (int)valueToPoint(markLeft);
        int right = (int)valueToPoint(markRight);

        QRectF rc;
        if (vertical)
            rc = QRectF(QPointF((qreal)rcPane.left(), (qreal)right), QSizeF((qreal)rcPane.width(), (qreal)left - right));
        else
            rc = QRectF(QPointF((qreal)left, (qreal)rcPane.top()), QSizeF((qreal)right - left, (qreal)rcPane.height()));

        rc = rc.normalized();
        strip->fillHelper()->drawGradientRect(p, rc, strip->getActualColor(), strip->getActualColor2());
    }
}

void ChartAxisItem::drawConstantLines(QPainter* p, const QRect& rcPane, bool behind)
{
    const ConstantLinesList& constantLines = m_axis->qtn_d().m_constantLines;
    if (constantLines.count() == 0)
        return;

    bool vertical = isVertical();

    for (int i = 0; i < constantLines.count(); i++)
    {
        ChartAxisConstantLine* constantLine = constantLines.at(i);
        if (!constantLine->isVisible())
            continue;

        if (constantLine->isShowBehind() != behind)
            continue;

        qreal mark = !constantLine->axisValue().isEmpty() ? m_axis->qtn_d().m_scaleTypeMap->valueToInternal(constantLine->axisValue()) :
            constantLine->axisValueInternal();

        int left = (int)valueToPoint(mark);

        if (vertical)
            constantLine->lineHelper()->draw(p, QPointF(rcPane.left(), left), QPointF(rcPane.right(), left), constantLine->getActualColor());
        else
            constantLine->lineHelper()->draw(p, QPointF(left, rcPane.top()), QPointF(left, rcPane.bottom()), constantLine->getActualColor());

        QString strText = constantLine->text();

        if (vertical)
        {
            ChartTextPainter painter(strText, constantLine);
            int x = constantLine->alignment() == Qt::AlignLeft ? rcPane.left() + 2 :
                constantLine->alignment() == Qt::AlignRight ? rcPane.right() - 2 - (int)painter.size().width() :
                (rcPane.left() + rcPane.right() - (int)painter.size().width()) / 2;

            painter.setLocation(QPoint(x, constantLine->isTextBelow() ? left + 2 : left - (int)painter.size().height()));
            painter.drawBoundedText(p, constantLine->getActualTextColor());
        }
        else
        {
            ChartRotatedTextPainterNearLine painter(strText, constantLine, QPoint(0, 0), true ? TextNearRight : TextNearTop, 90);

            int y = constantLine->alignment() == Qt::AlignLeft ? rcPane.bottom() - (int)painter.size().width() / 2 :
                constantLine->alignment() == Qt::AlignRight ? rcPane.top() + (int)painter.size().width() / 2 : (rcPane.top() + rcPane.bottom()) / 2;

            painter.setBasePoint(QPoint(constantLine->isTextBelow() ? left + 2 - (int)painter.size().height() : left + 2, y));

            painter.drawBoundedText(p, constantLine->getActualTextColor());

        }
    }
}

void ChartAxisItem::drawGridLines(QPainter* p, const QRect& rcPane)
{
    if (!m_axis->isGridLinesVisible())
        return;

    qreal mark;
    QColor clrGridLines = m_axis->gridLinesColor();
    QColor clrMinorGridLines = m_axis->minorGridLinesColor();
    bool minorVisible = m_axis->isMinorGridLinesVisible();

    bool vertical = isVertical();

    for (int i = 0; i < m_arrTicks.size(); i++)
    {
        int left = (int)valueToPoint(m_arrTicks[i].m_value);

        if (vertical)
            m_axis->qtn_d().m_gridLines->lineHelper()->draw(p, QPointF(rcPane.left(), left), QPointF(rcPane.right(), left), clrGridLines);
        else
            m_axis->qtn_d().m_gridLines->lineHelper()->draw(p, QPointF(left, rcPane.top()), QPointF(left, rcPane.bottom()), clrGridLines);
    }

    if (minorVisible)
    {
        ChartAxisViewTickEnumerator* emumerator = createTickMarksEnumerator(true);

        while (emumerator->getNext(mark))
        {
            int left = (int)valueToPoint(mark);

            if (vertical)
                m_axis->qtn_d().m_gridLines->minorLineHelper()->draw(p, QPointF(rcPane.left(), left), QPointF(rcPane.right(), left), clrMinorGridLines);
            else
                m_axis->qtn_d().m_gridLines->minorLineHelper()->draw(p, QPointF(left, rcPane.top()), QPointF(left, rcPane.bottom()), clrMinorGridLines);
        }
        delete emumerator;
    }
}

qreal ChartAxisItem::scale() const
{
    qreal minValue = rangeMinValueItem();
    qreal maxValue = rangeMaxValueItem();

    if (isVertical())
        return (m_boundingRect.height()) / (maxValue - minValue);
    else
        return (m_boundingRect.width()) / (maxValue - minValue);
}

static int qtnRound(qreal value)
{
    int x = (int)floor(value);
    return fabs(x - value) < fabs(x + 1 - value) ? x : x + 1;
}

qreal ChartAxisItem::valueToPoint(qreal x) const
{
    qreal minValue = rangeMinValueItem();
    qreal maxValue = rangeMaxValueItem();

    bool revered = m_axis->isReversed();

    qreal value;

    if (revered)
    {
        if (isVertical())
            value = m_boundingRect.top() + (x - minValue) / (maxValue - minValue) * (m_boundingRect.height());
        else
            value = m_boundingRect.right() - (x - minValue) / (maxValue - minValue) * (m_boundingRect.width());
    }
    else
    {
        if (isVertical())
            value = m_boundingRect.bottom() - (x - minValue) / (maxValue - minValue) * (m_boundingRect.height());
        else
            value = m_boundingRect.left() + (x - minValue) / (maxValue - minValue) * (m_boundingRect.width());
    }

    return qtnRound(value);
}

double ChartAxisItem::distanceToPoint(double x) const
{
    double minValue = rangeMinValue();
    double maxValue = rangeMaxValue();

    if (isVertical())
        return x / (maxValue - minValue) * (m_boundingRect.height());
    else
        return x / (maxValue - minValue) * (m_boundingRect.width());
}

ChartAxisViewTickEnumerator* ChartAxisItem::createTickMarksEnumerator(bool minor)
{
    return new ChartAxisViewTickEnumerator(this, minor);
}

void ChartAxisItem::drawTickMarks(QPainter* p)
{
    ChartAxisTickMarks* tickMarks = m_axis->tickMarks();

    if (!tickMarks->isVisible())
        return;

    int scrollBarSize = isScollBarVisible() ? SCROLLBARSIZE : 0;

    QColor clrAxis = m_axis->color();
    int length = tickMarks->length();
//  int nMinorCount = m_axis->GetMinorCount();
    int minorThickness = tickMarks->minorThickness();
    int thickness = tickMarks->thickness();
    bool minorVisible = tickMarks->isMinorVisible();
    bool vertical = isVertical();
    bool cross = tickMarks->isCrossAxis();
    int axisThickness = m_axis->thickness() - 1 + scrollBarSize;
    int extraLength = cross ? length + axisThickness : 0;

    qreal mark;

    for (int i = 0; i < m_arrTicks.size(); i++)
    {
        int left = (int)valueToPoint(m_arrTicks[i].m_value);

        if (vertical)
        {
            if (m_axis->alignment() == ChartAxis::AxisNear)
                ChartPainter::painter()->drawFillLine(p, QPointF(m_boundingRect.right() - axisThickness - length, left), QPointF(m_boundingRect.right() - axisThickness + extraLength, left), clrAxis, thickness);
            else
                ChartPainter::painter()->drawFillLine(p, QPointF(m_boundingRect.left() + axisThickness - extraLength, left), QPointF(m_boundingRect.left() + axisThickness + length, left), clrAxis, thickness);
        }
        else
        {
            if (m_axis->alignment() == ChartAxis::AxisNear)
                ChartPainter::painter()->drawFillLine(p, QPointF(left, m_boundingRect.top() + axisThickness - extraLength), QPointF(left, m_boundingRect.top() + axisThickness + length), clrAxis, thickness);
            else
                ChartPainter::painter()->drawFillLine(p, QPointF(left, m_boundingRect.bottom() - axisThickness - length), QPointF(left, m_boundingRect.bottom() - axisThickness + extraLength), clrAxis, thickness);
        }
    }

    if (minorVisible)
    {
        length = tickMarks->minorLength();
        int extraLength = cross ? length + axisThickness : 0;

        ChartAxisViewTickEnumerator* emumerator = createTickMarksEnumerator(true);

        while (emumerator->getNext(mark))
        {
            int left = (int)valueToPoint(mark);

            if (vertical)
            {
                if (m_axis->alignment() == ChartAxis::AxisNear)
                    ChartPainter::painter()->drawFillLine(p, QPointF(m_boundingRect.right() - axisThickness  - length, left), QPointF(m_boundingRect.right() - axisThickness + extraLength, left), clrAxis, minorThickness);
                else
                    ChartPainter::painter()->drawFillLine(p, QPointF(m_boundingRect.left() + axisThickness  - extraLength, left), QPointF(m_boundingRect.left() + axisThickness + length, left), clrAxis, minorThickness);
            }
            else
            {
                if (m_axis->alignment() == ChartAxis::AxisNear)
                    ChartPainter::painter()->drawFillLine(p, QPointF(left, m_boundingRect.top() + axisThickness  - extraLength), QPointF(left, m_boundingRect.top() + axisThickness + length), clrAxis, minorThickness);
                else
                    ChartPainter::painter()->drawFillLine(p, QPointF(left, m_boundingRect.bottom() - axisThickness - length), QPointF(left, m_boundingRect.bottom() - axisThickness + extraLength), clrAxis, minorThickness);
            }
        }

        delete emumerator;
    }
}

void ChartAxisItem::drawLabels(QPainter* p)
{
    if (!m_axis->label()->isVisible())
        return;

//    QColor clrAxis = m_axis->color();

    bool vertical = isVertical();
    int angle = m_axis->label()->angle();
    bool near = m_axis->alignment() == ChartAxis::AxisNear;

    int offset = m_axis->thickness() + (m_axis->tickMarks()->isVisible() ? m_axis->tickMarks()->length() : 0)
        + (isScollBarVisible() ? SCROLLBARSIZE : 0);

    for (int i = 0; i < m_arrTicks.size(); i++)
    {
        int left = (int)valueToPoint(m_arrTicks[i].m_value);
        QString s = m_arrTicks[i].m_strLabel;

        if (vertical)
        {
            ChartRotatedTextPainterNearLine painter(s, m_axis->label(), QPoint(near ? m_boundingRect.right() - offset : m_boundingRect.left() + offset, left), 
                near ? TextNearLeft : TextNearRight, (qreal)angle);
            painter.drawBoundedText(p, m_axis->label()->textColor());

        }
        else
        {
            ChartRotatedTextPainterNearLine painter(s, m_axis->label(), QPoint(left, near ? m_boundingRect.top() + offset : m_boundingRect.bottom() - offset), 
                near ? TextNearBottom : TextNearTop, (qreal)angle);
            painter.drawBoundedText(p, m_axis->label()->textColor());
        }
    }
}

void ChartAxisItem::drawTitle(QPainter* p)
{
    ChartAxisTitle* title = axis()->title();
    if (!title->isVisible())
        return;

    QString strText = title->text();
    if (isVertical())
    {
        if (m_axis->alignment() == ChartAxis::AxisNear)
        {
            ChartRotatedTextPainterNearLine painter(title->text(), title,
                QPoint(m_boundingRect.left(), m_boundingRect.center().y()), TextNearRight, 270);
            painter.drawBoundedText(p, title->textColor());
        }
        else
        {
            ChartRotatedTextPainterNearLine painter(title->text(), title,
                QPoint(m_boundingRect.right(), m_boundingRect.center().y()), TextNearLeft, 90);
            painter.drawBoundedText(p, title->textColor());

        }
    }
    else
    {
        if (m_axis->alignment() == ChartAxis::AxisNear)
        {
            if (title->alignment() == Qt::AlignCenter)
            {
                ChartRotatedTextPainterNearLine painter(title->text(), title,
                    QPoint(m_boundingRect.center().x(), m_boundingRect.bottom() + 3), TextNearTop, 0);
                painter.drawBoundedText(p, title->textColor());
            }
            else
            {
                ChartTextPainter painter(title->text(), title);
                painter.setLocation(QPointF(title->alignment() == Qt::AlignLeft ? m_boundingRect.left() :
                    m_boundingRect.right() - painter.size().width(), m_boundingRect.bottom() + 3 - painter.size().height()));
                painter.drawBoundedText(p, title->textColor());
            }
        }
        else
        {
            ChartRotatedTextPainterNearLine painter(title->text(), title, QPoint(m_boundingRect.center().x(), m_boundingRect.top()), 
                TextNearBottom, 0);
            painter.drawBoundedText(p, title->textColor());
        }
    }
}

bool ChartAxisItem::isScollBarVisible() const
{
    if (m_axis->isViewAutoRange())
        return false;

    if (fabs(rangeMinValueItem() - rangeMinValue()) < 1e-6 &&
        fabs(rangeMaxValueItem() - rangeMaxValue()) < 1e-6)
        return false;

    if (!m_axis->area()->isScrollAllowed())
        return false;

    return true;
}

void ChartAxisItem::paint(QPainter* p)
{
    QColor clrAxis = m_axis->color();

    int scrollBarSize = isScollBarVisible() ? SCROLLBARSIZE : 0;
    int thickness = m_axis->thickness();

    if (m_axis->isVisible())
    {
        if (isVertical())
        {
            qreal position = m_axis->alignment() == ChartAxis::AxisNear ? m_boundingRect.right() - scrollBarSize - thickness / 2.0 + 0.5 : m_boundingRect.left() + scrollBarSize +  thickness / 2.0 - 0.5;
            ChartPainter::painter()->drawFillLine(p, QPointF(position, (qreal)m_boundingRect.top()), QPointF(position, m_boundingRect.bottom() + 0.5), clrAxis, m_axis->thickness());
        }
        else
        {
            qreal position = m_axis->alignment() == ChartAxis::AxisNear ? m_boundingRect.top() + scrollBarSize + thickness / 2.0 - 0.5 : m_boundingRect.bottom() - thickness / 2.0 + 0.5 - scrollBarSize;
            ChartPainter::painter()->drawFillLine(p, QPointF(m_boundingRect.left(), position), QPointF(m_boundingRect.right() + 0.5, position), clrAxis, thickness);
        }

        drawTickMarks(p);
        drawLabels(p);
        drawTitle(p);
    }
}

bool ChartAxisItem::hitTest(const QPoint& pt, ChartHitInfo* hitInfo, unsigned long hitInfoFlags)
{
    if ((hitInfoFlags & ChartHitInfo::HitAxis) != 0 || 
        (hitInfoFlags & ChartHitInfo::HitAxisName) != 0)
    {
        if (m_axis->isVisible())
        {
            ChartAxisTitle* title = axis()->title();
            if (title && title->isVisible() && (hitInfoFlags & ChartHitInfo::HitAxisName) != 0)
            {
                QRect rectTitle;
                if (isVertical())
                {
                    if (m_axis->alignment() == ChartAxis::AxisNear)
                    {
                        ChartRotatedTextPainterNearLine painter(title->text(), title,
                            QPoint(m_boundingRect.left(), m_boundingRect.center().y()), TextNearRight, 270);
                        rectTitle = painter.roundedBounds();
                    }
                    else
                    {
                        ChartRotatedTextPainterNearLine painter(title->text(), title,
                            QPoint(m_boundingRect.right(), m_boundingRect.center().y()), TextNearLeft, 90);
                        rectTitle = painter.roundedBounds();
                    }
                }
                else
                {
                    if (m_axis->alignment() == ChartAxis::AxisNear)
                    {
                        if (title->alignment() == Qt::AlignCenter)
                        {
                            ChartRotatedTextPainterNearLine painter(title->text(), title,
                                QPoint(m_boundingRect.center().x(), m_boundingRect.bottom() + 3), TextNearTop, 0);
                            rectTitle = painter.roundedBounds();
                        }
                        else
                        {
                            ChartTextPainter painter(title->text(), title);
                            painter.setLocation(QPointF(title->alignment() == Qt::AlignLeft ? m_boundingRect.left() :
                                m_boundingRect.right() - painter.size().width(), m_boundingRect.bottom() + 3 - painter.size().height()));
                            rectTitle = painter.roundedBounds();
                        }
                    }
                    else
                    {
                        ChartRotatedTextPainterNearLine painter(title->text(), title, QPoint(m_boundingRect.center().x(), m_boundingRect.top()), 
                            TextNearBottom, 0);
                        rectTitle = painter.roundedBounds();
                    }
                }

                if (rectTitle.normalized().contains(pt))
                {
                    hitInfo->m_hitInfo = ChartHitInfo::HitAxisName;
                    hitInfo->m_index1 = axis()->axisID();
                    return true;
                }
            }

            if (m_boundingRect.normalized().contains(pt) && (hitInfoFlags & ChartHitInfo::HitArea) != 0)
            {
                hitInfo->m_hitInfo = ChartHitInfo::HitAxis;
                hitInfo->m_index1 = axis()->axisID();
                return true;
            }
        }
    }
    return false;
}

qreal ChartAxisItem::calculateGridSpacing(qreal axisRangeDelta, qreal screenDelta, qreal gridSpacingFactor)
{
    if (m_axis->qtn_d().m_scaleType == ChartScaleQualitative)
        return 1.0;

    qreal multipliers[]  = { 1, 2, 3, 5 };

    if (screenDelta <= 0)
        return 1;

    qreal deltaCoef = gridSpacingFactor * axisRangeDelta / screenDelta;
    if (deltaCoef > 1.0)
    {
        for (qreal factor = 1;; factor *= 10)
        {
            for (int i = 0; i < 4; i++) {
                qreal result = multipliers[i] * factor;
                if (deltaCoef <= result)
                    return result;
            }
        }
    }
    else
    {
        qreal result = 1;
        for (qreal factor = 0.1;; factor /= 10)
        {
            for (int i = 4 - 1; i >= 0; i--) {
                qreal newResult = multipliers[i] * factor;
                if (deltaCoef > newResult)
                    return result;
                result = newResult;
            }
        }
    }
}

void ChartAxisItem::setBoundingRect(const QRect& boundingRect)
{
    m_boundingRect = boundingRect;
    if (ChartAxis* chartAxis = axis())
        chartAxis->updateAxisPos(m_boundingRect);
}

ChartAxis* ChartAxisItem::axis() const 
{
    return m_axis;
}

int ChartAxisItem::size() const 
{
    return m_size;
}

qreal ChartAxisItem::gridSpacing() const 
{
    return m_gridSpacing;
}

QRect ChartAxisItem::bounds() const 
{
    return m_boundingRect;
}

void ChartAxisItem::createTickMarks(QRect rectArea)
{
    Q_UNUSED(rectArea)

    m_arrTicks.clear();

    qreal minValue = rangeMinValueItem();
    qreal maxValue = rangeMaxValueItem();

//    double axisMinValue = rangeMinValueItem();
//    double axisMaxValue = rangeMaxValueItem();
    qreal spacing = gridSpacing();


    bool vertical = isVertical();
    bool near = m_axis->alignment() == ChartAxis::AxisNear;
    int angle = m_axis->label()->angle();
/*
    if (m_axis->customLabelCount() > 0)
    {
        for (int i = 0, count = m_axis->customLabelCount(); i < count; i++)
        {
            ChartAxisViewTick tick;
            tick.m_value = !m_axis->axisCustomLabelValue(i).isEmpty() ? m_axis->qtn_d().m_scaleTypeMap->valueToInternal(m_axis->axisCustomLabelValue(i)) :
                m_axis->axisCustomLabelValueInternal(i);

            tick.m_strLabel = m_axis->customLabeltext(i);

            ChartRotatedTextPainterNearLine painter(tick.m_strLabel, m_axis->label(), QPoint(0, 0), 
                vertical ? (near ? TextNearLeft : TextNearRight) :
                (near ? TextNearBottom : TextNearTop), (qreal)angle);

            tick.m_szLabel = painter.size();

            tick.m_szBounds = painter.roundedBounds().size();

            m_arrTicks.append(tick);
        }

    }
    else
*/
    {
        if (minValue < -ChartMath::QTN_EPSILON_1)
            minValue += -ChartMath::QTN_EPSILON_1;

        qreal mark = ((int)(minValue / spacing)) * spacing;
        if (mark < minValue)
            mark += spacing;

        while (mark < maxValue + ChartMath::QTN_EPSILON_1)
        {
            ChartAxisViewTick tick;
            tick.m_value = mark;

            tick.m_strLabel = m_axis->qtn_d().m_scaleTypeMap->internalToValue(m_axis->label()->format(), mark);

            ChartRotatedTextPainterNearLine painter(tick.m_strLabel, m_axis->label(), QPoint(0, 0), 
                vertical ? (near ? TextNearLeft : TextNearRight) :
                (near ? TextNearBottom : TextNearTop), (qreal)angle);

            tick.m_szLabel = painter.size();

            tick.m_szBounds = painter.roundedBounds().size();

            m_arrTicks.append(tick);

            mark += spacing;
        }
    }
}

int ChartAxisItem::calcLabelSize()
{
    int labelSize = 0;

    ChartAxis* chartAxis = axis();
    bool vertical = isVertical();

    if (chartAxis->label()->isVisible())
    {
        for (int i = 0; i < m_arrTicks.size(); i++)
        {
            QSize szBounds = m_arrTicks[i].m_szBounds;
            labelSize = qMax(labelSize, vertical ? szBounds.width() : szBounds.height());
        }
    }

    if (chartAxis->title()->isVisible())
    {
        ChartAxisTitle* title = chartAxis->title();

        bool near = m_axis->alignment() == ChartAxis::AxisNear;

        ChartRotatedTextPainterNearLine painter(title->text(), title, QPoint(0, 0), 
            isVertical() ? (near ? TextNearRight : TextNearLeft) :
            (near ? TextNearTop : TextNearBottom), 
            (qreal)(vertical ? (near ? 270.0 : 90.0) : 0.0));

        QSize szTitle = painter.roundedBounds().size();

        labelSize += vertical ? szTitle.width() : szTitle.height();
    }

    return labelSize;
}

void ChartAxisItem::calcSize(const QRect& rectArea, int labelSize)
{
    ChartAxis* chartAxis = axis();

    qreal screenLength = isVertical() ? rectArea.height() : rectArea.width();

    if (screenLength < 1)
    {
        m_size = 0;
        return;
    }

    qreal rangeDelta = rangeMaxValueItem() - rangeMinValueItem();

    m_gridSpacing = chartAxis->gridSpacingAuto() ? calculateGridSpacing(rangeDelta, screenLength, isVertical() ? 30.0 : 50.0) : chartAxis->gridSpacing();
    chartAxis->qtn_d().m_gridSpacing = m_gridSpacing;

    createTickMarks(rectArea);

    if (!chartAxis->isVisible())
    {
        m_size = 0;
        return;
    }

//    bool vertical = isVertical();

    m_size = chartAxis->thickness();

    if (chartAxis->tickMarks()->isVisible())
        m_size += chartAxis->tickMarks()->length();

    if (isScollBarVisible())
        m_size += SCROLLBARSIZE;

    m_size += labelSize == -1 ? calcLabelSize() : labelSize;
/*
    if (labelSize == -1 && axis->title()->isVisible())
    {
        ChartAxisTitle* title = axis()->title();

        bool near = m_axis->alignment() == ChartAxis::AxisNear;

        ChartRotatedTextPainterNearLine painter(title->text(), title, QPoint(0, 0), 
            isVertical() ? (near ? ChartRotatedTextPainterNearLine::TextNearRight : ChartRotatedTextPainterNearLine::TextNearLeft) :
            (near ? ChartRotatedTextPainterNearLine::TextNearTop : ChartRotatedTextPainterNearLine::TextNearBottom), 
            (qreal)(isVertical() ? (near ? 270 : 90) : 0));

        QSize szTitle = painter.roundedBounds().size();

        m_size += vertical ? szTitle.width() : szTitle.height();
    }
*/
}

bool ChartAxisItem::isVertical() const
{
    return m_axis->isVertical();
}

void ChartAxisItem::createItem()
{
}

void ChartAxisItem::addLegendItems()
{
    GraphicsItem* parent = parentItem();
    while (parent->parentItem() != Q_NULL)
        parent = parent->parentItem();

    Chart2DItem* contentItem = (Chart2DItem*)parent;

    if (contentItem->legendItem())
    {
        const ConstantLinesList& constantLines = m_axis->qtn_d().m_constantLines;

        for (int i = 0; i < constantLines.count(); i++)
        {
            ChartAxisConstantLine* constantLine = constantLines.at(i);
            if (!constantLine->isVisible() || !constantLine->isLegendVisible())
                continue;
            contentItem->legendItem()->addItem(constantLine);
        }

        const StreakList& strips = m_axis->qtn_d().m_streaks;

        for (int i = 0; i < strips.count(); i++)
        {
            ChartAxisIntervalStreak* strip = strips.at(i);
            if (!strip->isVisible() || !strip->isLegendVisible())
                continue;
            contentItem->legendItem()->addItem(strip);
        }
    }
}
