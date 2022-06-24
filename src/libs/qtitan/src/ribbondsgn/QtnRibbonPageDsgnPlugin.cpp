/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2015 Developer Machines (http://www.devmachines.com)
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
#include <QStyle>
#include <QContextMenuEvent>
#include <QtDesigner/QExtensionFactory>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerPropertySheetExtension>
#include <QtDesigner/QExtensionManager>

#include "QtnRibbonPageDsgnPlugin.h"
#include "QtnRibbonBar.h"
#include "../src/ribbon/QtnRibbonTabBar.h"


QTITAN_USE_NAMESPACE

/* DsgnRibbonPagePlugin */
DsgnRibbonPagePlugin::DsgnRibbonPagePlugin(QObject *parent)
    : QObject(parent)
    , m_initialized(false)
    , m_core(Q_NULL)
{
}

DsgnRibbonPagePlugin::~DsgnRibbonPagePlugin()
{
}

void DsgnRibbonPagePlugin::initialize(QDesignerFormEditorInterface* core)
{
    if (m_initialized)
        return;

    m_initialized = true;
    m_core = core;
}

bool DsgnRibbonPagePlugin::isInitialized() const
{
    return m_initialized;
}

QWidget* DsgnRibbonPagePlugin::createWidget(QWidget* parent)
{
    RibbonPage* result = new RibbonPage(parent);
    connect(result, SIGNAL(titleChanged(QString)), this, SLOT(titleChanged(QString)));
    result->setObjectName("__qt__passive_RibbonPage");
    result->setAcceptDrops(true);
    return result;
}

QString DsgnRibbonPagePlugin::name() const
{
    return create_qtitan_className("RibbonPage");
}

QString DsgnRibbonPagePlugin::group() const
{
    return "Developer Machines: Components";
}

QIcon DsgnRibbonPagePlugin::icon() const
{
    return QIcon(":res/ribbon_icon.png");
}

QString DsgnRibbonPagePlugin::toolTip() const
{
    return tr("Advanced RibbonPage for Qt.C++");
}

QString DsgnRibbonPagePlugin::whatsThis() const
{
    return tr("Advanced RibbonPage for Qt.C++");
}

bool DsgnRibbonPagePlugin::isContainer() const
{
    return false;
}

QString DsgnRibbonPagePlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
        " <widget class=\"" + create_qtitan_className("RibbonPage") + "\" name=\"ribbonPage\"/>\n"
        " <customwidgets>\n"
        "   <customwidget>\n"
        "       <class>" + create_qtitan_className("RibbonPage") + "</class>\n"
        "       <extends>QWidget</extends>\n"
        "       <addpagemethod>addGroup</addpagemethod>\n"
        "   </customwidget>\n"
        " </customwidgets>\n"
        "</ui>\n";
}

QString DsgnRibbonPagePlugin::includeFile() const
{
    return "QtnRibbonPage.h";
}

void DsgnRibbonPagePlugin::titleChanged(const QString& title)
{
    Q_UNUSED(title);
    if (RibbonPage* ribbonPage = qobject_cast<RibbonPage*>(sender())) 
    {
        if (QDesignerFormWindowInterface* form = QDesignerFormWindowInterface::findFormWindow(ribbonPage))
        {
            QDesignerFormEditorInterface* editor = form->core();
            QExtensionManager* manager = editor->extensionManager();
            QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(manager, ribbonPage);
            const int propertyIndex = sheet->indexOf(QLatin1String("title"));
            sheet->setChanged(propertyIndex, true);
        }
    }
}




