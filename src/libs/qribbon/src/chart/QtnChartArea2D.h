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
#ifndef QTN_CHARTAREA2D_H
#define QTN_CHARTAREA2D_H

#include "QtnChartArea.h"

namespace Qtitan
{
    class ChartAxis;
    class ChartAreaItem;
    class ChartAreaForm;
    class ChartAreas2DTheme;
    class ChartArea2DPrivate;

    /* ChartArea2D */
    class QTITAN_EXPORT ChartArea2D : public ChartArea
    {
        Q_OBJECT
    public:
        ChartArea2D(QObject* parent = Q_NULL);
        virtual ~ChartArea2D();

    public:
        bool isRotated() const;
        void setRotated(bool rotated);

        bool isZoomAllowed() const;
        void setZoomAllowed(bool allowed);

        bool isScrollAllowed() const;
        void setScrollAllowed(bool allowed);

    public:
        ChartAxis* axisX() const;
        ChartAxis* axisY() const;

        ChartAxis* secondaryAxisX() const;
        ChartAxis* secondaryAxisY() const;

        ChartAxis* axisByCustomId(int axisID) const;
        ChartAxis* axis(int axisID) const;

        QRect boundingRect() const;

    protected:
        void updateLayout(ChartAreaItem* item, QRect boundingRect);

    private:
        friend class ChartArea2DItem;

        Q_DISABLE_COPY(ChartArea2D)
        QTN_DECLARE_EX_PRIVATE(ChartArea2D)
    };

}; //namespace Qtitan


#endif // QTN_CHARTAREA2D_H
