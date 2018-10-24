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
#ifndef QTN_CHARTTITLEPRIVATE_H
#define QTN_CHARTTITLEPRIVATE_H

#include "QtitanDef.h"
#include "QtnGraphicsItem.h"

namespace Qtitan
{
    /* ChartTitlePrivate */   
    class ChartTitlePrivate
    {
    public:
        QTN_DECLARE_PUBLIC(ChartTitle)
    public:
        explicit ChartTitlePrivate();
        virtual ~ChartTitlePrivate();

    public:
        virtual GraphicsItem* createItem(GraphicsItem* parentItem);
        QPoint calculateGeometryPoint(const QRect& bounds, const QRect& startBounds, const QRect& rect);
        NearTextPosition nearTextPosition();
    public:
        QString m_text;
        QColor m_textColor;
        bool m_visible;
        QFont m_font;
        bool m_antialiasing;
        ChartDockPosition m_dockPosition;
        int m_indent;
        int m_innerIndent;
        Qt::Alignment m_alignment;
    };

    /* ChartTitleItem */
    class ChartTitleItem : public GraphicsItem
    {
    public:
        ChartTitleItem(ChartTitle* title, QString& strText, GraphicsItem* parentItem);
        ~ChartTitleItem();

    public:
        virtual void paint(QPainter* p);
        virtual bool hitTest(const QPoint& pt, ChartHitInfo* hitInfo, unsigned long hitInfoFlags);

        void calculateItem(const QRect& rectChart, QRect& boundingRect);

    public:
        QString m_text;
        ChartTitle* m_title;
        QPoint m_origin;

    private:
        Q_DISABLE_COPY(ChartTitleItem)
    };
}; //namespace Qtitan


#endif // QTN_CHARTTITLEPRIVATE_H
