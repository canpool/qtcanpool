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
#include <QPainter>

#include "QtnChartLegend.h"
#include "QtnChart.h"
#include "QtnChartPainter.h"
#include "QtnChartThemePrivate.h"
#include "QtnChartTextPainter.h"
#include "QtnChartLegendPrivate.h"

using namespace Qtitan;

/*!
    \class Qtitan::ChartLegendInterface
    \inmodule QtitanChart
    \internal
*/

/*!
    \class Qtitan::ChartLegendItem
    \inmodule QtitanChart
    \internal
*/
ChartLegendItem::ChartLegendItem(ChartLegend* legend, GraphicsItem* parentItem)
    : GraphicsItem(parentItem)
{
    m_legend = legend;
}

ChartLegendItem::~ChartLegendItem()
{
}

void ChartLegendItem::paint(QPainter* p)
{
    int count = (int)m_arrItems.count();
    if (count == 0)
        return;

    ChartBorder* border = m_legend->border();
    Q_ASSERT(border != Q_NULL);

    int borderSize = 0;
    if (border->isVisible())
        borderSize = border->thickness();
    borderSize += 2;

    int xround, yround;
    xround = yround = m_legend->borderRounded();

    m_legend->qtn_d().fillHelper()->drawGradientRect(p, m_boundingRect, xround, yround);
//    m_legend->fillHelper()->drawGradientRect(p, m_boundingRect, m_legend->backgroundColor(), xround, yround);

    if (border->isVisible())
        border->drawInnerBorder(p, m_boundingRect, m_legend->borderColor(), xround, yround);

    bool vertical = m_legend->direction() == ChartLegend::LegendTopToBottom;
    int columnCount = m_legend->columnCount();

    if (columnCount == 0)
        columnCount = vertical ? 1 : count;

    if (columnCount > count)
        columnCount = count;

    int rowCount = (count + columnCount - 1) / columnCount;

    int left = m_boundingRect.left() + borderSize;

    for (int column = 0; column < columnCount; column++)
    {
        int columnWidth = 0;
        int top = m_boundingRect.top() + borderSize;

        for (int nRow = 0; nRow < rowCount; nRow++)
        {
            int i = vertical ? column * rowCount + nRow : nRow * columnCount + column;
            if (i >= count)
                continue;

            ChartLegendInterface* item = m_arrItems[i];

            QString strName = item->legendName();

            if (strName.length() == 0)
                continue;

            ChartTextPainter painter(strName, m_legend);

            QSizeF szText = painter.size();
            QSizeF sz = szText;

            if (m_legend->isMarkerVisible())
            {
                if (sz.height() < m_legend->markerSize().height())
                    sz.setHeight((float)m_legend->markerSize().height());
            }

            int textLeft = left;

            if (m_legend->isMarkerVisible())
            {
                QSize szMarker = m_legend->markerSize();

//                QSize size = sz.toSize();
                QRect rcMarker(QPoint(left, top  + sz.toSize().height() * 0.5 - szMarker.height() * 0.5), szMarker);

                item->drawLegendComponent(p, rcMarker);

                textLeft += szMarker.width() + 2;
                sz.setWidth(sz.width() + szMarker.height() + 2);
            }


            painter.setLocation(QPointF((float)textLeft, (float)(top + sz.height() / 2 - szText.height() / 2)));
            painter.drawBoundedText(p, m_legend->textColor());

            columnWidth = qMax(columnWidth, (int)sz.width() + 2);

            top += (int)sz.height() + 2;
        }
        left += columnWidth;
    }
}

bool ChartLegendItem::hitTest(const QPoint& pt, ChartHitInfo* hitInfo, unsigned long hitInfoFlags)
{
    if (m_legend->isVisible() && m_boundingRect.contains(pt) && (hitInfoFlags & ChartHitInfo::HitLegend) != 0)
    {
        hitInfo->m_hitInfo = ChartHitInfo::HitLegend;
        return true;
    }
    return false;
}

QSize ChartLegendItem::size()
{
    int nHeight = 0;
    int width = 0;

    bool vertical = m_legend->direction() == ChartLegend::LegendTopToBottom;
    int columnCount = m_legend->columnCount();
    int borderSize = m_legend->border()->isVisible() ? m_legend->border()->thickness() : 0;
    borderSize += 2;

    int count = (int)m_arrItems.count();
    if (count == 0)
        return QSize(0, 0);

    if (columnCount == 0)
        columnCount = vertical ? 1 : count;

    if (columnCount > count)
        columnCount = count;

    int rowCount = (count + columnCount - 1) / columnCount;

    for (int column = 0; column < columnCount; column++)
    {
        int columnWidth = 0;
        int nColumnHeight = 0;

        for (int nRow = 0; nRow < rowCount; nRow++)
        {
            int i = vertical ? column * rowCount + nRow : nRow * columnCount + column;
            if (i >= count)
                continue;

            ChartLegendInterface* item = m_arrItems[i];

            QString strName = item->legendName();

            if (strName.length() == 0)
                continue;

            ChartTextPainter painter(strName, m_legend);
            QSizeF sz = painter.size();

            if (m_legend->isMarkerVisible())
            {
                if (sz.height() < m_legend->markerSize().height())
                    sz.setHeight((float)m_legend->markerSize().height());

                sz.setWidth(sz.width() + m_legend->markerSize().width() + 2);
            }

            columnWidth = qMax(columnWidth, (int)sz.width() + 2);

            if (nRow) nColumnHeight += 2;
            nColumnHeight += (int)sz.height();
        }

        width += columnWidth;
        nHeight = qMax(nHeight, nColumnHeight);
    }

    return QSize(width + 2 * borderSize, nHeight + 2 * borderSize);
}

void ChartLegendItem::calculateItem(QRect& rcChart)
{
    QSize sz = size();
    QPoint pt;

    int horizontalAlignment = m_legend->horizontalAlignment();
    int verticalAlignment = m_legend->verticalAlignment();

    bool inside = horizontalAlignment != ChartLegend::LegendNearOutside && horizontalAlignment != ChartLegend::LegendFarOutside &&
        verticalAlignment != ChartLegend::LegendNearOutside && verticalAlignment != ChartLegend::LegendFarOutside;

    if (horizontalAlignment == ChartLegend::LegendNear || horizontalAlignment == ChartLegend::LegendNearOutside)
    {
        pt.setX(rcChart.left() + (inside ? 10 : 0));

        if (horizontalAlignment == ChartLegend::LegendNearOutside)
            rcChart.setLeft(rcChart.left() + sz.width() + 10);

    }
    else if (horizontalAlignment == ChartLegend::LegendCenter)
    {
        pt.setX((rcChart.left() + rcChart.right() - sz.width()) / 2);
    }
    else
    {
        pt.setX(rcChart.right() - sz.width() -  (inside ? 10 : 0));

        if (horizontalAlignment == ChartLegend::LegendFarOutside)
            rcChart.setRight(rcChart.right() - (sz.width() + 10));
    }


    if (verticalAlignment == ChartLegend::LegendNear || verticalAlignment == ChartLegend::LegendNearOutside)
    {
        pt.setY(rcChart.top() + (inside ? 10 : 0));

        if (verticalAlignment== ChartLegend::LegendNearOutside)
            rcChart.setTop(rcChart.top() + (sz.height() + 10));

    }
    else if (verticalAlignment == ChartLegend::LegendCenter)
    {
        pt.setY((rcChart.top() + rcChart.bottom() - sz.height()) / 2);
    }
    else
    {
        pt.setY(rcChart.bottom() - sz.height() - (inside ? 10 : 0));
        if (verticalAlignment == ChartLegend::LegendFarOutside)
            rcChart.setBottom(rcChart.bottom() - sz.height() + 10);
    }
    m_boundingRect = QRect(pt, sz);
}

void ChartLegendItem::addItem(ChartLegendInterface* item)
{
    m_arrItems.append(item);
}


/*!
    \class Qtitan::ChartBorder
    \inmodule QtitanChart
    \internal
*/
ChartBorder::ChartBorder(QObject* parent)
    : QObject(parent)
{
    m_visible = true;
    m_thickness = 1;
    m_round = -1;
}

ChartBorder::~ChartBorder()
{
}

void ChartBorder::drawBorder(QPainter* p, const QRectF& boundingRect, const QColor& color)
{
    if (m_visible && m_thickness > 0)
        ChartPainter::painter()->drawBoundedRect(p, boundingRect, color, m_thickness);
}

void ChartBorder::drawInnerBorder(QPainter* p, const QRectF& boundingRect, const QColor& color, int xround, int yround)
{
    if (m_visible && m_thickness > 0)
        ChartPainter::painter()->drawInnerBorder(p, boundingRect, color, m_thickness, xround, yround);
}

bool ChartBorder::isVisible() const 
{
    return m_visible;
}

void ChartBorder::setVisible(bool visible) 
{
    m_visible = visible;
}

const QColor& ChartBorder::color() const 
{
    return m_color;
}

void ChartBorder::setColor(const QColor& clr) 
{
    m_color = clr;
}

int ChartBorder::thickness() const 
{
    return m_thickness;
}

void ChartBorder::setThickness(int thickness) 
{
    m_thickness = thickness;
}

int ChartBorder::borderRounded() const
{
    return m_round;
}

void ChartBorder::setBorderRounded(int round)
{
    m_round = round;
}


/*!
    \class Qtitan::ChartLegendPrivate
    \inmodule QtitanChart
    \internal
*/
ChartLegendPrivate::ChartLegendPrivate()
{
    m_clrBackground = Q_NULL;
}

ChartLegendPrivate::~ChartLegendPrivate()
{
    delete m_clrBackground;
}

void ChartLegendPrivate::init()
{
    QTN_P(ChartLegend);

    m_font = QFont(QObject::tr("Tahoma"), 8);
    m_font.setWeight(QFont::Normal);

    m_visible = false;
    m_antialiasing = false;

    m_border = new ChartBorder(&p);

    m_fillHelper = new ChartFillDrawHelper(&p);
//    m_fillHelper->setFillStyle(Qt::SolidPattern);

    m_markerSize = QSize(20, 16);
    m_markerVisible = true;

    m_direction           = ChartLegend::LegendTopToBottom;
    m_horizontalAlignment = ChartLegend::LegendFarOutside;
    m_verticalAlignment   = ChartLegend::LegendNear;

    m_columnCount = 0;
}

ChartFillDrawHelper* ChartLegendPrivate::fillHelper() const 
{
    QTN_P(const ChartLegend);
    if (m_fillHelper->fillStyle() != Qt::NoBrush)
        return m_fillHelper;
    return ChartTheme::themeTopParent(&p)->frameTheme()->getLegendTheme()->m_fillHelper;
}

ChartLegendItem* ChartLegendPrivate::createItem(GraphicsItem* parentItem)
{
    QTN_P(ChartLegend);
    return m_visible ? new ChartLegendItem(&p, parentItem) : Q_NULL;
}

/* ChartLegend */
ChartLegend::ChartLegend(QObject* parent)
    : ChartTextComponent(parent)
{
    QTN_INIT_PRIVATE(ChartLegend);
    QTN_D(ChartLegend);
    d.init();
}

ChartLegend::~ChartLegend()
{
    QTN_FINI_PRIVATE();
}

bool ChartLegend::isVisible() const 
{
    QTN_D(const ChartLegend);
    return d.m_visible;
}

void ChartLegend::setVisible(bool visible) 
{
    QTN_D(ChartLegend);
    d.m_visible = visible;
    chartUpdate();
}

const QFont& ChartLegend::font() const 
{
    QTN_D(const ChartLegend);
    return d.m_font;
}

const QSize& ChartLegend::markerSize() const 
{
    QTN_D(const ChartLegend);
    return d.m_markerSize;
}

void ChartLegend::setMarkerSize(const QSize& sz) 
{
    QTN_D(ChartLegend);
    d.m_markerSize = sz;
    chartUpdate();
}

bool ChartLegend::isMarkerVisible() const 
{
    QTN_D(const ChartLegend);
    return d.m_markerVisible;
}

void ChartLegend::setMarkerVisible(bool visible) 
{
    QTN_D(ChartLegend);
    d.m_markerVisible = visible;
    chartUpdate();
}

void ChartLegend::setAntialiasing(bool antialiasing) 
{
    QTN_D(ChartLegend);
    d.m_antialiasing = antialiasing;
    chartUpdate();
}

bool ChartLegend::antialiasing() const 
{
    QTN_D(const ChartLegend);
    return d.m_antialiasing;
}

ChartBorder* ChartLegend::border() const 
{
    QTN_D(const ChartLegend);
    return d.m_border;
}

void ChartLegend::setHorizontalAlignment(LegendAlignment alignment) 
{
    QTN_D(ChartLegend);
    d.m_horizontalAlignment = alignment;
    chartUpdate();
}

ChartLegend::LegendAlignment ChartLegend::horizontalAlignment() const 
{
    QTN_D(const ChartLegend);
    return d.m_horizontalAlignment;
}

void ChartLegend::setVerticalAlignment(LegendAlignment alignment) 
{
    QTN_D(ChartLegend);
    d.m_verticalAlignment = alignment;
    chartUpdate();
}

ChartLegend::LegendAlignment ChartLegend::verticalAlignment() const 
{
    QTN_D(const ChartLegend);
    return d.m_verticalAlignment;
}

void ChartLegend::setDirection(LegendDirection direction) 
{
    QTN_D(ChartLegend);
    d.m_direction = direction;
    chartUpdate();
}

ChartLegend::LegendDirection ChartLegend::direction() const 
{
    QTN_D(const ChartLegend);
    return d.m_direction;
}

void ChartLegend::setColumnCount(int columnCount) 
{
    QTN_D(ChartLegend);
    d.m_columnCount = columnCount;
    chartUpdate();
}

int ChartLegend::columnCount() const 
{
    QTN_D(const ChartLegend);
    return d.m_columnCount;
}

void ChartLegend::setFont(const QFont& font)
{
    QTN_D(ChartLegend);
    d.m_font = font;
    chartUpdate();
}

void ChartLegend::setTextColor(const QColor& clrText)
{
    QTN_D(ChartLegend);
    d.m_colorText = clrText;
    chartUpdate();
}

const QColor& ChartLegend::textColor() const
{
    QTN_D(const ChartLegend);
    if (d.m_colorText.isValid())
        return d.m_colorText;

    return ChartTheme::themeTopParent(this)->frameTheme()->getLegendTheme()->m_textColor;
}

const QColor& ChartLegend::borderColor() const
{
    QTN_D(const ChartLegend);
    if (d.m_border->color().isValid())
        return d.m_border->color();

    return ChartTheme::themeTopParent(this)->frameTheme()->getLegendTheme()->m_borderColor;
}

int ChartLegend::borderRounded() const
{
    QTN_D(const ChartLegend);
    int round = d.m_border->borderRounded();
    if (round != -1)
        return round;
    return ChartTheme::themeTopParent(this)->frameTheme()->getLegendTheme()->m_round;
}






