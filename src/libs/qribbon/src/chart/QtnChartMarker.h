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
#ifndef QTN_CHARTMARKER_H
#define QTN_CHARTMARKER_H

#include <QColor>

#include "QtnChartDefines.h"
#include "QtnChartComponent.h"

namespace Qtitan
{
    class AbstractSeries;
    class ChartFillDrawHelper;
    /* ChartMarker */
    class ChartMarker : public ChartComponent
    {
    public:
        explicit ChartMarker(AbstractSeries* owner);
        virtual ~ChartMarker();

    public:
        int size() const;
        void setSize(int size);

        void setBorderVisible(bool borderVisible);
        bool borderVisible() const;

        MarkerType type() const;
        void setType(MarkerType type);

        ChartFillDrawHelper* fillHelper() const;

        bool isVisible() const;
        void setVisible(bool visible);

    public:
        void paint(QPainter* p, const QPointF& point, QColor color, QColor color2, QColor colorBorder);
        void paint(QPainter* p, const QPointF& point, int width, QColor color, QColor color2, QColor colorBorder);

    protected:
        bool m_borderVisible;
        QColor m_borderColor;
        QColor m_color;
        MarkerType m_type;
        int m_size;
        bool m_visible;
        ChartFillDrawHelper* m_fillHelper;
    };
}

#endif // QTN_CHARTMARKER_H
