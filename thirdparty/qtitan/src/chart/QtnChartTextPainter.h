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
#ifndef QTN_CHARTTEXTPAINTER_H
#define QTN_CHARTTEXTPAINTER_H

#include <QRect>
#include <QColor>

#include "QtitanDef.h"

namespace Qtitan
{
    class ChartTextComponent;

    /* ChartTextPainterBase */
    class ChartTextPainterBase
    {
    protected:
        ChartTextPainterBase(const QString& text, ChartTextComponent* textProvider);

    public:
        virtual ~ChartTextPainterBase();

    public:
        QRect roundedBounds();
        QSizeF size() const;

    protected:
        virtual QRectF calculateBounds() = 0;

    protected:
        QString m_strText;
        QSizeF m_szTextSize;
        ChartTextComponent* m_textProvider;
        int m_height;
        int m_width;

        QRect m_rcRoundedBounds;
        QRectF m_boundingRect;
    };

    /* ChartTextPainter */
    class ChartTextPainter : public ChartTextPainterBase
    {
    public:
        ChartTextPainter(const QString& text, ChartTextComponent* textProvider);
        void setLocation(const QPointF& location);

    public:
        void drawBoundedText(QPainter* p, const QColor& color);
        QRectF calculateBounds();

    protected:
        QPointF m_ptLocation;
    };

    /* ChartRotatedTextPainterBase */
    class ChartRotatedTextPainterBase : public ChartTextPainterBase
    {
    public:
        enum TextRotation
        {
            TextLeftTop,
            TextCenterTop,
            TextRightTop,
            TextLeftCenter,
            TextCenterCenter,
            TextRightCenter,
            TextLeftBottom,
            TextCenterBottom,
            TextRightBottom
        };

    protected:
        ChartRotatedTextPainterBase(const QString& text, ChartTextComponent* textProvider, QPoint ptBase = QPoint(0, 0));

    public:
        void drawBoundedText(QPainter* p, const QColor& color);

    public:
        void setBasePoint(QPoint pt);

    protected:
        QRectF calculateBounds();
        QRect getInitialTextRect();

        virtual QPoint calculateLeftTopPoint() = 0;
        virtual TextRotation calculateRotation() = 0;

        void calculatePoints(TextRotation rotation, QRect rect, float angle, float points[4][2]);

    protected:
        QPoint m_ptBasePoint;
        float m_angle;
    };

    /* ChartRotatedTextPainterNearLine */ 
    class ChartRotatedTextPainterNearLine : public ChartRotatedTextPainterBase
    {
    public:
        ChartRotatedTextPainterNearLine(const QString& text, ChartTextComponent* textProvider, QPoint ptBase, NearTextPosition position, float angle);

    protected:
        QPoint calculateLeftTopPoint();
        TextRotation calculateRotation();

    protected:
        NearTextPosition m_nearPosition;
    };

    /* ChartRotatedTextPainterOnCircleRadial */
    class ChartRotatedTextPainterOnCircleRadial : public ChartRotatedTextPainterBase
    {
    public:
        ChartRotatedTextPainterOnCircleRadial(const QString& text, ChartTextComponent* textProvider, QPoint ptBase, float angle);

    protected:
        QPoint calculateLeftTopPoint();
        TextRotation calculateRotation();
    };


}; //namespace Qtitan


#endif // QTN_CHARTTEXTPAINTER_H
