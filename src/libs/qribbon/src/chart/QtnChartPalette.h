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
#ifndef QTN_CHARTPALETTE_H
#define QTN_CHARTPALETTE_H

#include <QColor>
#include <QPalette>

#include "QtnChartComponent.h"

class QDomElement;

namespace Qtitan
{
    /* ChartPalette */
    class ChartPalette : public ChartComponent
    {
    public:
        explicit ChartPalette(QObject* parent = Q_NULL);
        virtual ~ChartPalette();

    public:
        QColor lightColor(int index) const;
        QColor darkColor(int index) const;

        int getCount() const;

        QVector<QPalette>& entries();

    public:
        void extractXML(const QDomElement& el);

    protected:
        QVector<QPalette> m_arrEntries;
    };
};

#endif // QTN_CHARTPALETTE_H
