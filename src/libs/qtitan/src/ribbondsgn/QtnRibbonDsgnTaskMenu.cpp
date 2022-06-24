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
#include <QAction>
#include <QMenu>

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerMetaDataBaseInterface>
#include <QtDesigner/QDesignerWidgetFactoryInterface>

#include "QtnRibbonDsgnTaskMenu.h"
#include "QtnCommandDsgn.h"
#include "QtnRibbonBar.h"
#include "QtnRibbonStyleDsgnPlugin.h"


QTITAN_USE_NAMESPACE

/* DsgnRibbonTaskMenu */
DsgnRibbonTaskMenu::DsgnRibbonTaskMenu(RibbonBar* pRibbon, QObject *parent)
    : QObject(parent)
{
    m_pRibbonBar = pRibbon;

    m_separator = new QAction(this);
    m_separator->setSeparator(true);

    m_removeRebbonBar = new QAction(tr("Remove Ribbon Bar"), this);
    connect(m_removeRebbonBar, SIGNAL(triggered()), this, SLOT(slot_removeRibbonBar()));

    m_addRibbonPage = new QAction(tr("Insert Ribbon Page"), this);
    connect(m_addRibbonPage, SIGNAL(triggered()), this, SLOT(slot_addNewPage()));

    m_deleteRibbonPage = new QAction(tr("Delete current Ribbon Page"), this);
    connect(m_deleteRibbonPage, SIGNAL(triggered()), this, SLOT(slot_removeCurrentPage()));
}

DsgnRibbonTaskMenu::~DsgnRibbonTaskMenu()
{
}

void DsgnRibbonTaskMenu::slot_removeRibbonBar()
{
    if (QDesignerFormWindowInterface* fw = QDesignerFormWindowInterface::findFormWindow((QObject*)m_pRibbonBar)) 
    {
        DeleteRibbonBarCommand* cmd = new DeleteRibbonBarCommand(fw);
        cmd->init(m_pRibbonBar);
        fw->commandHistory()->push(cmd);
    }
}

void DsgnRibbonTaskMenu::slot_addNewPage()
{
    if (QDesignerFormWindowInterface* fw = QDesignerFormWindowInterface::findFormWindow((QObject*)m_pRibbonBar)) 
    {
        AddRibbonPageCommand* cmd = new AddRibbonPageCommand(fw);
        cmd->init(qobject_cast<RibbonBar*>(m_pRibbonBar));
        fw->commandHistory()->push(cmd);
    }
}

void DsgnRibbonTaskMenu::slot_removeCurrentPage()
{
    if (QDesignerFormWindowInterface* fw = QDesignerFormWindowInterface::findFormWindow((QObject*)m_pRibbonBar)) 
    {
        DeleteRibbonPageCommand* cmd = new DeleteRibbonPageCommand(fw);
        cmd->init(qobject_cast<RibbonBar*>(m_pRibbonBar));
        fw->commandHistory()->push(cmd);
    }
}

QAction *DsgnRibbonTaskMenu::preferredEditAction() const
{
    return m_removeRebbonBar;
}

QList<QAction*> DsgnRibbonTaskMenu::taskActions() const
{
    QList<QAction *> list;
    list.append(m_removeRebbonBar);
    list.append(m_separator);
    list.append(m_addRibbonPage);
    if (m_pRibbonBar->getPageCount() > 0)
        list.append(m_deleteRibbonPage);
    return list;
}

/* DsgnRibbonTaskMenu for Styles*/
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

/* DsgnRibbonPageTaskMenu */
DsgnRibbonPageTaskMenu::DsgnRibbonPageTaskMenu(RibbonPage* page, QObject* parent)
    : QObject(parent)
{
    m_ribbonPage = page;
    m_separatorAction = new QAction(this);
    m_separatorAction->setSeparator(true);
    m_addGroupAction = new QAction(tr("Add Ribbon Group"), this);
    m_removePageAction = new QAction(tr("Remove Ribbon Page"), this);

    connect(m_addGroupAction, SIGNAL(triggered()), this, SLOT(slotAddGroup()));
    connect(m_removePageAction, SIGNAL(triggered()), this, SLOT(slotRemovePage()));

    QDesignerFormWindowInterface* form = QDesignerFormWindowInterface::findFormWindow((QWidget*)m_ribbonPage);
    connect(form, SIGNAL(contextMenuRequested(QMenu*, QWidget*)), this, SLOT(contextMenuRequested(QMenu*, QWidget*)));
}

DsgnRibbonPageTaskMenu::~DsgnRibbonPageTaskMenu()
{
}

QAction* DsgnRibbonPageTaskMenu::preferredEditAction() const
{
    return m_addGroupAction;
}

QList<QAction*> DsgnRibbonPageTaskMenu::taskActions() const
{
    QList<QAction *> list;
    list.append(m_addGroupAction);
    list.append(m_separatorAction);
    list.append(m_removePageAction);
    return list;
}

void DsgnRibbonPageTaskMenu::contextMenuRequested(QMenu* menu, QWidget* widget)
{
    Q_ASSERT(menu != Q_NULL);
    if (widget == m_ribbonPage)
    {
        menu->clear();
        menu->addActions(taskActions());
    }
}

void DsgnRibbonPageTaskMenu::slotAddGroup()
{
    QDesignerFormWindowInterface* fw = QDesignerFormWindowInterface::findFormWindow(m_ribbonPage);
    Q_ASSERT(fw != 0);

    RibbonGroup* ribbonGroup = qobject_cast<RibbonGroup*>(fw->core()->widgetFactory()->createWidget(create_qtitan_className("RibbonGroup"), m_ribbonPage));
    Q_ASSERT(ribbonGroup != 0);

    AddRibbonGroupCommand* cmd = new AddRibbonGroupCommand(fw);
    cmd->init(m_ribbonPage, ribbonGroup);
    fw->commandHistory()->push(cmd);
}

void DsgnRibbonPageTaskMenu::slotRemovePage()
{
    RibbonBar* ribbonBar = qobject_cast<RibbonBar*>(m_ribbonPage->parentWidget());
    Q_ASSERT(ribbonBar != Q_NULL);
    QDesignerFormWindowInterface* fw = QDesignerFormWindowInterface::findFormWindow(ribbonBar);
    Q_ASSERT(fw != Q_NULL);

    DeleteRibbonPageCommand* cmd = new DeleteRibbonPageCommand(fw);
    cmd->init(ribbonBar);
    fw->commandHistory()->push(cmd);
}


/* DsgnRibbonGroupTaskMenu */
DsgnRibbonGroupTaskMenu::DsgnRibbonGroupTaskMenu(RibbonGroup* group, QObject* parent)
    : QObject(parent)
{
    m_ribbonGroup = group;
    m_separatorAction = new QAction(this);
    m_separatorAction->setSeparator(true);

    m_removeGroupAction = new QAction(tr("Remove Ribbon Group"), this);
    connect(m_removeGroupAction, SIGNAL(triggered()), this, SLOT(slotRemoveRibbonGroup()));
    m_removePageAction = new QAction(tr("Remove Ribbon Page"), this);
    connect(m_removePageAction, SIGNAL(triggered()), this, SLOT(slotRemoveRibbonPage()));

    QDesignerFormWindowInterface* form = QDesignerFormWindowInterface::findFormWindow((QWidget*)m_ribbonGroup);
    connect(form, SIGNAL(contextMenuRequested(QMenu*, QWidget*)), this, SLOT(contextMenuRequested(QMenu*, QWidget*)));
}


QAction* DsgnRibbonGroupTaskMenu::preferredEditAction() const
{
    return m_removeGroupAction;
}

QList<QAction*> DsgnRibbonGroupTaskMenu::taskActions() const
{
    QList<QAction *> list;
    list.append(m_removeGroupAction);
    list.append(m_separatorAction);
    list.append(m_removePageAction);
    return list;
}

void DsgnRibbonGroupTaskMenu::contextMenuRequested(QMenu* menu, QWidget* widget)
{
    Q_ASSERT(menu != Q_NULL);
    if (widget == m_ribbonGroup)
    {
        menu->clear();
        menu->addActions(taskActions());
    }
}

void DsgnRibbonGroupTaskMenu::slotRemoveRibbonGroup()
{
    RibbonPage* ribbonPage = qobject_cast<RibbonPage*>(m_ribbonGroup->parentWidget());
    Q_ASSERT(ribbonPage != Q_NULL);
    QDesignerFormWindowInterface* fw = QDesignerFormWindowInterface::findFormWindow(ribbonPage);
    Q_ASSERT(fw != Q_NULL);

    DeleteRibbonGroupCommand* cmd = new DeleteRibbonGroupCommand(fw);
    cmd->init(ribbonPage, m_ribbonGroup);
    fw->commandHistory()->push(cmd);
}

void DsgnRibbonGroupTaskMenu::slotRemoveRibbonPage()
{
    RibbonBar* ribbonBar = qobject_cast<RibbonBar*>(m_ribbonGroup->ribbonBar());
    Q_ASSERT(ribbonBar != Q_NULL);
    QDesignerFormWindowInterface* fw = QDesignerFormWindowInterface::findFormWindow(ribbonBar);
    Q_ASSERT(fw != Q_NULL);

    DeleteRibbonPageCommand* cmd = new DeleteRibbonPageCommand(fw);
    cmd->init(ribbonBar);
    fw->commandHistory()->push(cmd);
}

/* DsgnRibbonTaskMenuFactory */
DsgnRibbonTaskMenuFactory::DsgnRibbonTaskMenuFactory(QExtensionManager* parent)
    : QExtensionFactory(parent)
{
}

QObject* DsgnRibbonTaskMenuFactory::createExtension(QObject* object, const QString& iid, QObject* parent) const
{
    if (iid == QLatin1String("QDesignerInternalTaskMenuExtension"))
    {
        if (RibbonBar* widget = qobject_cast<RibbonBar*>(object))
            return new DsgnRibbonTaskMenu(widget, parent);
        else if (DsgnRibbonStyle* style = dynamic_cast<DsgnRibbonStyle*>(object))
            return new StyleDsgnTaskMenu(style->parentStyle(), parent);
        else if (DsgnOfficeStyle* style = dynamic_cast<DsgnOfficeStyle*>(object))
            return new StyleDsgnTaskMenu(style->parentStyle(), parent);
        else if (RibbonPage* widget = qobject_cast<RibbonPage *>(object))
            return new DsgnRibbonPageTaskMenu(widget, parent);
        else if (RibbonGroup* widget = qobject_cast<RibbonGroup *>(object))
            return new DsgnRibbonGroupTaskMenu(widget, parent);
    }
    return Q_NULL;
}

