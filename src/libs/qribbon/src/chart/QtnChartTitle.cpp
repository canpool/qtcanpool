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
#include "QtnChartTitle.h"
#include "QtnChartTitlePrivate.h"
#include "QtnChart.h"
#include "QtnChartThemePrivate.h"
#include "QtnChartTextPainter.h"
#include "QtnGraphicsItem.h"

using namespace Qtitan;


/*!
    \class Qtitan::ChartTitlePrivate
    \inmodule QtitanChart
    \internal
*/
ChartTitlePrivate::ChartTitlePrivate()
{
    m_visible = true;
    m_antialiasing = true;

    m_dockPosition = TopDock;
    m_alignment = Qt::AlignCenter;

    m_indent = 0;
    m_innerIndent = 5;

    m_font = QFont(QObject::tr("Tahoma"), 14);
}

ChartTitlePrivate::~ChartTitlePrivate()
{
}

GraphicsItem* ChartTitlePrivate::createItem(GraphicsItem* parentItem)
{
    QTN_P(ChartTitle);
    return  m_visible ? new ChartTitleItem(&p, m_text, parentItem) : Q_NULL;
}

QPoint ChartTitlePrivate::calculateGeometryPoint(const QRect& boundingRect, const QRect& startBoundingRect, const QRect& rect)
{
    switch (m_dockPosition)
    {
        case LeftDock :
            {
                int x = boundingRect.left() + m_innerIndent;

                if (m_alignment == Qt::AlignCenter)
                    return QPoint(x, startBoundingRect.top() + startBoundingRect.height() * 0.5);

                if (m_alignment == Qt::AlignLeft)
                    return QPoint(x, boundingRect.bottom() - rect.height() * 0.5 - m_innerIndent);

                return QPoint(x, boundingRect.top() + rect.height() * 0.5 + m_innerIndent);
            }
        case RightDock :
            {
                int x = boundingRect.right() - m_innerIndent;

                if (m_alignment == Qt::AlignCenter)
                    return QPoint(x, startBoundingRect.top() + startBoundingRect.height() * 0.5);

                if (m_alignment == Qt::AlignLeft)
                    return QPoint(x, boundingRect.top() + rect.height() * 0.5 + m_innerIndent);

                return QPoint(x, boundingRect.bottom() - rect.height() * 0.5 - m_innerIndent);
            }
        default:
            {
                int y = m_dockPosition == TopDock ? boundingRect.top() + m_innerIndent : boundingRect.bottom() - m_innerIndent;

                if (m_alignment == Qt::AlignCenter)
                    return QPoint(startBoundingRect.left() + startBoundingRect.width() * 0.5, y);

                if (m_alignment == Qt::AlignLeft)
                    return QPoint(boundingRect.left() + rect.width() * 0.5 + m_innerIndent, y);

                return QPoint(boundingRect.right() - rect.width() * 0.5 - m_innerIndent, y);
            }
    }
}

NearTextPosition ChartTitlePrivate::nearTextPosition()
{
    if (m_dockPosition == LeftDock) return TextNearRight;
    else if (m_dockPosition == RightDock) return TextNearLeft;
    else if (m_dockPosition == TopDock) return TextNearBottom;
    return TextNearTop;
}


/* ChartTitle */
ChartTitle::ChartTitle(QObject* parent)
    : ChartTextComponent(parent)
{
    QTN_INIT_PRIVATE(ChartTitle);
}

ChartTitle::~ChartTitle()
{
    QTN_FINI_PRIVATE();
}

void ChartTitle::setText(const QString& title)
{
    QTN_D(ChartTitle);
    d.m_text = title;
    chartUpdate();
}

void ChartTitle::setFont(const QFont& font)
{
    QTN_D(ChartTitle);
    d.m_font = font;
    chartUpdate();
}

void ChartTitle::setTextColor(const QColor& color)
{
    QTN_D(ChartTitle);
    d.m_textColor = color;
    chartUpdate();
}

void ChartTitle::setDockPosition(ChartDockPosition dockPosition)
{
    QTN_D(ChartTitle);
    d.m_dockPosition = dockPosition;
    chartUpdate();
}

void ChartTitle::setIndent(int indent)
{
    QTN_D(ChartTitle);
    d.m_indent = indent;
    chartUpdate();
}

void ChartTitle::setAlignment(Qt::Alignment alignment)
{
    QTN_D(ChartTitle);
    d.m_alignment = alignment;
    chartUpdate();

}

int ChartTitle::textAngle() const
{
    QTN_D(const ChartTitle);
    if (d.m_dockPosition == TopDock || d.m_dockPosition == BottomDock)
        return 0;

    if (d.m_dockPosition == LeftDock)
        return 270;

    return 90;
}

QColor ChartTitle::textColor() const
{
    QTN_D(const ChartTitle);
    if (d.m_textColor.isValid())
        return d.m_textColor;
    return ChartTheme::themeTopParent(this)->frameTheme()->getTitleTheme()->m_textColor;
}

const QString& ChartTitle::text() const 
{
    QTN_D(const ChartTitle);
    return d.m_text;
}

const QFont& ChartTitle::font() const 
{
    QTN_D(const ChartTitle);
    return d.m_font;
}

ChartDockPosition ChartTitle::dockPosition() const 
{
    QTN_D(const ChartTitle);
    return d.m_dockPosition;
}

int ChartTitle::indent() const 
{
    QTN_D(const ChartTitle);
    return d.m_indent;
}

Qt::Alignment ChartTitle::alignment() const 
{
    QTN_D(const ChartTitle);
    return d.m_alignment;
}

void ChartTitle::setAntialiasing(bool antialiasing) 
{
    QTN_D(ChartTitle);
    d.m_antialiasing = antialiasing;
    chartUpdate();
}

bool ChartTitle::antialiasing() const 
{
    QTN_D(const ChartTitle);
    return d.m_antialiasing;
}

bool ChartTitle::isVisible() const 
{
    QTN_D(const ChartTitle);
    return d.m_visible;
}

void ChartTitle::setVisible(bool visible) 
{
    QTN_D(ChartTitle);
    if (d.m_visible != visible)
    {
        d.m_visible = visible;
        emit visibleChanged(visible);
        chartUpdate();
    }
}

/*!
    \class Qtitan::ChartTitleItem
    \inmodule QtitanChart
    \internal
*/
ChartTitleItem::ChartTitleItem(ChartTitle* title, QString& strText, GraphicsItem* parentItem)
    : GraphicsItem(parentItem)
{
    m_text = strText;
    m_title = title;
    m_origin = QPoint(0, 0);
}

ChartTitleItem::~ChartTitleItem()
{
}

void ChartTitleItem::paint(QPainter* p)
{
    ChartRotatedTextPainterNearLine painter(m_text, m_title, m_origin, m_title->qtn_d().nearTextPosition(), (float)m_title->textAngle());
    painter.drawBoundedText(p, m_title->textColor());
}

bool ChartTitleItem::hitTest(const QPoint& pt, ChartHitInfo* hitInfo, unsigned long hitInfoFlags)
{
    if ((hitInfoFlags & ChartHitInfo::HitTitle) != 0)
    {
        ChartRotatedTextPainterNearLine painter(m_text, m_title, m_origin, m_title->qtn_d().nearTextPosition(), (float)m_title->textAngle());
        if (painter.roundedBounds().contains(pt))
        {
            hitInfo->m_hitInfo = ChartHitInfo::HitTitle;
            return true;
        }
    }
    return false;
}

void ChartTitleItem::calculateItem(const QRect& rectChart, QRect& boundingRect)
{
    ChartRotatedTextPainterNearLine painter(m_text, m_title, QPoint(0, 0) , m_title->qtn_d().nearTextPosition(), (float)m_title->textAngle());
//    QSizeF sizeString = painter.size();

    QRect rect = painter.roundedBounds();

    ChartDockPosition dockPosition = m_title->dockPosition();

    int size = (dockPosition == LeftDock || dockPosition == RightDock ? rect.width() : rect.height());

    QPoint origin = m_title->qtn_d().calculateGeometryPoint(boundingRect, rectChart, rect);
    rect.translate(origin);

    m_origin = origin;

    if (dockPosition == LeftDock) 
        boundingRect.setLeft(boundingRect.left() + size + m_title->indent());
    else if (dockPosition == RightDock) 
        boundingRect.setRight(boundingRect.right() - size + m_title->indent());
    else if (dockPosition == TopDock) 
        boundingRect.setTop(boundingRect.top() + size + m_title->indent());
    else if (dockPosition == BottomDock) 
        boundingRect.setBottom(boundingRect.bottom() - size + m_title->indent());
}
