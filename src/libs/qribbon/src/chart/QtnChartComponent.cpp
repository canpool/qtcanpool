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

#include "QtnChartComponent.h"
#include "QtnChart.h"
#include "QtnGraphicsItem.h"

using namespace Qtitan;

ChartComponent::ChartComponent(QObject* parent)
    : QObject(parent)
{
}

ChartComponent::~ChartComponent()
{
}

void ChartComponent::chartUpdate(UpdateOptions opt)
{
    if (Chart* chart = getChartObject())
        chart->chartUpdate(opt);
}

Chart* ChartComponent::getChartObject() const
{
    Chart* chart = Q_NULL;
    if (!chart)
    {
        QObject* currentObject = this->parent();
        if (currentObject)
        {
            chart = qobject_cast<Chart*>(currentObject);

            while (currentObject && chart == Q_NULL)
            {
                currentObject = currentObject->parent();
                if (currentObject)
                    chart = qobject_cast<Chart*>(currentObject);
            }
        }
    }
    return chart ? chart : Q_NULL;
}


/* ChartTextComponent */
ChartTextComponent::ChartTextComponent(QObject* parent)
    : ChartComponent(parent)
{
}

ChartTextComponent::~ChartTextComponent()
{
}
