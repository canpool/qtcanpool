/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
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
#include <QAction>

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerMetaDataBaseInterface>

#include "QtnRibbonDsgnTaskMenu.h"
#include "QtnCommandDsgn.h"
#include "QtnRibbonBar.h"
#include "QtnRibbonStyleDsgnPlugin.h"


using namespace Qtitan;

/* RibbonDsgnTaskMenu */
RibbonDsgnTaskMenu::RibbonDsgnTaskMenu(RibbonBar* pRibbon, QObject *parent)
    : QObject(parent)
{
    m_pRibbonBar = pRibbon;

    m_separator = new QAction(this);
    m_separator->setSeparator(true);

    m_removeRebbonBar = new QAction(tr("Remove Ribbon Bar"), this);
    connect(m_removeRebbonBar, SIGNAL(triggered()), this, SLOT(slotRemoveRibbonBar()));

    m_addRibbonPage = new QAction(tr("Insert page"), this);
    connect(m_addRibbonPage, SIGNAL(triggered()), this, SLOT(slotAddNewPage()));

    m_deleteRibbonPage = new QAction(tr("Delete current page"), this);
    connect(m_deleteRibbonPage, SIGNAL(triggered()), this, SLOT(slotRemoveCurrentPage()));
}

RibbonDsgnTaskMenu::~RibbonDsgnTaskMenu()
{
}

void RibbonDsgnTaskMenu::slotRemoveRibbonBar()
{
    if (QDesignerFormWindowInterface* fw = QDesignerFormWindowInterface::findFormWindow((QObject*)m_pRibbonBar)) 
    {
        DeleteRibbonBarCommand* cmd = new DeleteRibbonBarCommand(fw);
        cmd->init(m_pRibbonBar);
        fw->commandHistory()->push(cmd);
    }
}

void RibbonDsgnTaskMenu::slotAddNewPage()
{
    if (QDesignerFormWindowInterface* fw = QDesignerFormWindowInterface::findFormWindow((QObject*)m_pRibbonBar)) 
    {
        AddRibbonPageCommand* cmd = new AddRibbonPageCommand(fw);
        cmd->init(qobject_cast<RibbonBar*>(m_pRibbonBar));
        cmd->redo();
        //    fw->commandHistory()->push(cmd);
    }
}

void RibbonDsgnTaskMenu::slotRemoveCurrentPage()
{
    if (QDesignerFormWindowInterface* fw = QDesignerFormWindowInterface::findFormWindow((QObject*)m_pRibbonBar)) 
    {
        DeleteRibbonPageCommand* cmd = new DeleteRibbonPageCommand(fw);
        cmd->init(qobject_cast<RibbonBar*>(m_pRibbonBar));
        cmd->redo();
        //    fw->commandHistory()->push(cmd);
    }
}

QAction *RibbonDsgnTaskMenu::preferredEditAction() const
{
    return m_removeRebbonBar;
}

QList<QAction*> RibbonDsgnTaskMenu::taskActions() const
{
    QList<QAction *> list;
    list.append(m_removeRebbonBar);
    list.append(m_separator);
    list.append(m_addRibbonPage);
    if (m_pRibbonBar->getPageCount() > 0)
        list.append(m_deleteRibbonPage);
    return list;
}

/* RibbonDsgnTaskMenu for Styles*/
StyleDsgnTaskMenu::StyleDsgnTaskMenu(QWidget* parentStyle, QObject* parent)
    : QObject(parent)
{
    m_parentStyle = parentStyle;

    m_removeStyle = new QAction(tr("Remove Style"), this);
    connect(m_removeStyle, SIGNAL(triggered()), this, SLOT(slotRemoveStyle()));
}

StyleDsgnTaskMenu::~StyleDsgnTaskMenu()
{
}

QAction* StyleDsgnTaskMenu::preferredEditAction() const
{
    return m_removeStyle;
}

QList<QAction*> StyleDsgnTaskMenu::taskActions() const
{
    QList<QAction*> list;
    list.append(m_removeStyle);
    return list;
}

void StyleDsgnTaskMenu::slotRemoveStyle()
{
    QDesignerFormWindowInterface* formWindow = QDesignerFormWindowInterface::findFormWindow(m_parentStyle);
    Q_ASSERT(formWindow != Q_NULL);
    QDesignerFormEditorInterface* core = formWindow->core();
    QDesignerContainerExtension* container = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), formWindow->mainContainer());

    QWidget* widget = container->widget(container->count()-1);
    Q_ASSERT(widget != Q_NULL);

    const QString description = tr("Delete '%1'").arg(widget->objectName());
    formWindow->commandHistory()->beginMacro(description);

    DeleteStyleCommand* cmd = new DeleteStyleCommand(formWindow);
    cmd->init(widget);
    formWindow->commandHistory()->push(cmd);
    formWindow->commandHistory()->endMacro();
}


/* RibbonDsgnTaskMenuExFactory */
RibbonDsgnTaskMenuExFactory::RibbonDsgnTaskMenuExFactory(QExtensionManager* parent)
    : QExtensionFactory(parent)
{
}

QObject* RibbonDsgnTaskMenuExFactory::createExtension(QObject* object, const QString& iid, QObject* parent) const
{
    if (iid == QLatin1String("QDesignerInternalTaskMenuExtension"))
    {
        if (RibbonBar* widget = qobject_cast<RibbonBar*>(object))
            return new RibbonDsgnTaskMenu(widget, parent);
        else if (DesRibbonStyle* style = dynamic_cast<DesRibbonStyle*>(object))
            return new StyleDsgnTaskMenu(style->parentStyle(), parent);
        else if (DesOfficeStyle* style = dynamic_cast<DesOfficeStyle*>(object))
            return new StyleDsgnTaskMenu(style->parentStyle(), parent);
    }
    return Q_NULL;
}

