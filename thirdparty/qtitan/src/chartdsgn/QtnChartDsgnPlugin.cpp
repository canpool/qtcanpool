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

#include <QWidget>
#include <QtitanChart.h>
#include <QtPlugin>
#include "QtnChartDsgnPlugin.h"

using namespace Qtitan;

ChartDsgnPlugin::ChartDsgnPlugin(QObject *parent)
    : QObject(parent)
{
    initialized = false;
}

void ChartDsgnPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (initialized)
        return;

    initialized = true;
}

bool ChartDsgnPlugin::isInitialized() const
{
    return initialized;
}

QWidget *ChartDsgnPlugin::createWidget(QWidget *parent)
{
    return new Qtitan::Chart(parent);
}

QString ChartDsgnPlugin::name() const
{
    return "Qtitan::Chart";
}

QString ChartDsgnPlugin::group() const
{
    return "Developer Machines: Components";
}

QIcon ChartDsgnPlugin::icon() const
{
    return QIcon(":res/chart_icon.png");
}

QString ChartDsgnPlugin::toolTip() const
{
    return "Chart Widget";
}

QString ChartDsgnPlugin::whatsThis() const
{
    return "Chart Widget";
}

bool ChartDsgnPlugin::isContainer() const
{
    return false;
}

QString ChartDsgnPlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
        " <widget class=\"Qtitan::Chart\" name=\"chart\">\n"
           "  <property name=\"geometry\">\n"
           "   <rect>\n"
           "    <x>0</x>\n"
           "    <y>0</y>\n"
           "    <width>300</width>\n"
           "    <height>200</height>\n"
           "   </rect>\n"
           "  </property>\n"
           "  <property name=\"toolTip\" >\n"
           "   <string>Chart Widget</string>\n"
           "  </property>\n"
           "  <property name=\"whatsThis\" >\n"
           "   <string>Charts and Diagrams for Qt.</string>\n"
           "  </property>\n"
           " </widget>\n"
           "</ui>\n";
}

QString ChartDsgnPlugin::includeFile() const
{
    return "QtitanChart.h";
}


