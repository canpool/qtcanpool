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
#ifndef QTN_CHARTCOMPONENT_H
#define QTN_CHARTCOMPONENT_H

#include <QObject>
#include <QVector>
#include "QtitanDef.h"

class QFont;

namespace Qtitan
{
    class Chart;

    /* ChartComponent */
    class QTITAN_EXPORT ChartComponent : public QObject
    {
        Q_OBJECT
    public:
        enum UpdateOptions
        {
            UpdateView   = 1,
            UpdateLayout = 2,
            UpdateRedraw = 4,
        };
    public:
        explicit ChartComponent(QObject* parent = Q_NULL);
        virtual ~ChartComponent();

    public:
        virtual void chartUpdate(UpdateOptions opt = UpdateView);
        Chart* getChartObject() const;

    private:
        friend class ChartDataTablePrivate;
        Q_DISABLE_COPY(ChartComponent);
    };

    /* ChartComponent */
    class QTITAN_EXPORT ChartTextComponent : public ChartComponent
    {
        Q_OBJECT
    public:
        explicit ChartTextComponent(QObject* parent = Q_NULL);
        virtual ~ChartTextComponent();

    public:
        virtual const QFont& font() const = 0;
        virtual bool antialiasing() const = 0;

    private:
        Q_DISABLE_COPY(ChartTextComponent);
    };
}; //namespace Qtitan


#endif // QTN_CHARTCOMPONENT_H
