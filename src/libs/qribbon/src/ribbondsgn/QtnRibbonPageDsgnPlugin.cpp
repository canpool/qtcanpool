/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2010 Developer Machines (http://www.devmachines.com)
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

#include <QtDesigner/QExtensionFactory>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerPropertySheetExtension>
#include <QtDesigner/QExtensionManager>

#include "QtnRibbonPageDsgnPlugin.h"
#include "QtnRibbonBar.h"
#include "QtnRibbonPage.h"
#include "../src/ribbon/QtnRibbonTabBar.h"


using namespace Qtitan;


static void setObjectNameChildren(RibbonTabBar& widget)
{
//    QList<Qtitan::RibbonTab*> child_list = qFindChildren<Qtitan::RibbonTab*>(&widget);
    QList<Qtitan::RibbonTab*> child_list = widget.findChildren<Qtitan::RibbonTab*>();
    for(int i = 0, count = child_list.count(); count > i; i++)
    {
        if (Qtitan::RibbonTab* pWd = child_list[i])
            pWd->setObjectName("__qt__passive_RibbonTab");
    }
}


/* RibbonPageDsgnPlugin */
RibbonPageDsgnPlugin::RibbonPageDsgnPlugin(QObject *parent)
    : QObject(parent), m_core(0)
{
    initialized = false;
}

void RibbonPageDsgnPlugin::initialize(QDesignerFormEditorInterface* core)
{
    if (initialized)
        return;

    initialized = true;
    m_core = core;
}

bool RibbonPageDsgnPlugin::isInitialized() const
{
    return initialized;
}

QWidget* RibbonPageDsgnPlugin::createWidget(QWidget* parent)
{
    if (RibbonBar* ribbonBar = qobject_cast<RibbonBar*>(parent))
    {
        if (RibbonPage* ribbonPage = ribbonBar->addPage(tr("")))
        {
            connect(ribbonPage, SIGNAL(titleChanged(QString)), this, SLOT(titleChanged(QString)));

            ribbonPage->setObjectName("__qt__passive_RibbonPage");

//            if (RibbonTabBar* ribbonTabBar = qFindChild<Qtitan::RibbonTabBar*>((QWidget*)ribbonBar))
            if (RibbonTabBar* ribbonTabBar = ribbonBar->findChild<Qtitan::RibbonTabBar*>())
                setObjectNameChildren(*ribbonTabBar);
            return ribbonPage;
        }
    }
    return new RibbonPage;
}

QString RibbonPageDsgnPlugin::name() const
{
    return "Qtitan::RibbonPage";
}

QString RibbonPageDsgnPlugin::group() const
{
    return "Developer Machines: Components";
}

QIcon RibbonPageDsgnPlugin::icon() const
{
    return QIcon(":res/ribbon_icon.png");
}

QString RibbonPageDsgnPlugin::toolTip() const
{
    return "Advanced RibbonPage for Nokia-Qt";
}

QString RibbonPageDsgnPlugin::whatsThis() const
{
    return "Advanced RibbonPage for Nokia-Qt";
}

bool RibbonPageDsgnPlugin::isContainer() const
{
    return false;
}

QString RibbonPageDsgnPlugin::domXml() const
{
    return "";
}

QString RibbonPageDsgnPlugin::includeFile() const
{
    return "QtnRibbonPage.h";
}

void RibbonPageDsgnPlugin::titleChanged(const QString& title)
{
    Q_UNUSED(title);
    if (RibbonPage* ribbonPage = qobject_cast<RibbonPage*>(sender())) 
    {
        if (QDesignerFormWindowInterface* form = QDesignerFormWindowInterface::findFormWindow(ribbonPage))
        {
            QDesignerFormEditorInterface* editor = form->core();
            QExtensionManager* manager = editor->extensionManager();
            QDesignerPropertySheetExtension *sheet = qt_extension<QDesignerPropertySheetExtension*>(manager, ribbonPage);
            const int propertyIndex = sheet->indexOf(QLatin1String("windowTitle"));
            sheet->setChanged(propertyIndex, true);
        }
    }
}




