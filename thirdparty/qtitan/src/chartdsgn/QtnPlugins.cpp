/****************************************************************************
**
** Qtitan Framework by Developer Machines
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

#include <QtPlugin>
#include "QtnPlugins.h"
#include "QtnChartDsgnPlugin.h"

using namespace Qtitan;

/* QtitanPlugins */
QtitanPlugins::QtitanPlugins(QObject *parent)
: QObject(parent)
{
    m_plugins << new ChartDsgnPlugin(this);
}

QList<QDesignerCustomWidgetInterface*> QtitanPlugins::customWidgets() const
{ 
    return m_plugins;
}

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
Q_EXPORT_PLUGIN2(qtnchartdsgn, QtitanPlugins)
#endif

