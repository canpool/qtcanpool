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
#include "QtnCommandDsgn.h"

#include <QApplication>
#include <QAction>

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerWidgetFactoryInterface>
#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QDesignerMetaDataBaseInterface>

#include "QtnRibbonMainWindow.h"
#include "QtnRibbonBar.h"
#include "QtnRibbonPage.h"


using namespace Qtitan;

QDesignerFormEditorInterface* FormDsgnCommand::core() const
{
    if (QDesignerFormWindowInterface *fw = formWindow())
        return fw->core();
    return 0;
}


/* CreateRibbonBarCommand */
CreateRibbonBarCommand::CreateRibbonBarCommand(QDesignerFormWindowInterface *formWindow)
    : FormDsgnCommand(QApplication::translate("Command", "Create Ribbon Bar"), formWindow)
{
}

void CreateRibbonBarCommand::init(QMainWindow *mainWindow)
{
    m_mainWindow = mainWindow;
    QDesignerFormEditorInterface *core = formWindow()->core();
    m_ribbonBar = qobject_cast<QWidget*>(core->widgetFactory()->createWidget(QLatin1String("Qtitan::RibbonBar"), m_mainWindow));
    core->widgetFactory()->initialize((QObject*)m_ribbonBar.data());
}

void CreateRibbonBarCommand::redo()
{
    QDesignerFormEditorInterface* core = formWindow()->core();
    QDesignerContainerExtension* c = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), m_mainWindow);
    c->addWidget((QWidget*)m_ribbonBar.data());

    ((QWidget*)m_ribbonBar)->setObjectName(QLatin1String("ribbonBar"));
    formWindow()->ensureUniqueObjectName((QObject*)m_ribbonBar.data());
    core->metaDataBase()->add((QObject*)m_ribbonBar.data());
    formWindow()->emitSelectionChanged();
    ((QWidget*)m_ribbonBar.data())->setFocus();
}

void CreateRibbonBarCommand::undo()
{
    QDesignerFormEditorInterface* core = formWindow()->core();
    QDesignerContainerExtension* c = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), m_mainWindow);
    for (int i = 0; i < c->count(); ++i) 
    {
        if (c->widget(i) == ((QWidget*)m_ribbonBar)) 
        {
            c->remove(i);
            break;
        }
    }
    core->metaDataBase()->remove((QObject*)m_ribbonBar.data());
    formWindow()->emitSelectionChanged();
}


// ---- DeleteRibbonBarCommand ----
DeleteRibbonBarCommand::DeleteRibbonBarCommand(QDesignerFormWindowInterface* formWindow)
    : FormDsgnCommand(QApplication::translate("Command", "Delete Ribbon Bar"), formWindow)
{
}

void DeleteRibbonBarCommand::init(RibbonBar* ribbonBar)
{
    m_ribbonBar = (QWidget*)ribbonBar;
    m_mainWindow = qobject_cast<QMainWindow*>(m_ribbonBar->parentWidget());
}

void DeleteRibbonBarCommand::redo()
{
    if (m_mainWindow) 
    {
        QDesignerContainerExtension* c = qt_extension<QDesignerContainerExtension*>(core()->extensionManager(), m_mainWindow);
        Q_ASSERT(c != Q_NULL);
        for (int i=0; i<c->count(); ++i) 
        {
            if (c->widget(i) == m_ribbonBar) 
            {
                c->remove(i);
                break;
            }
        }
    }

    core()->metaDataBase()->remove(m_ribbonBar);
    m_ribbonBar->hide();
    m_ribbonBar->setParent(formWindow());
    formWindow()->emitSelectionChanged();
}

void DeleteRibbonBarCommand::undo()
{
    if (m_mainWindow) 
    {
        m_ribbonBar->setParent(m_mainWindow);
        QDesignerContainerExtension* c = qt_extension<QDesignerContainerExtension*>(core()->extensionManager(), m_mainWindow);

        c->addWidget(m_ribbonBar);

        core()->metaDataBase()->add(m_ribbonBar);
        m_ribbonBar->show();
        formWindow()->emitSelectionChanged();
    }
}

/* RibbonBarCommand */
RibbonBarCommand::RibbonBarCommand(const QString& description, QDesignerFormWindowInterface* formWindow)
    : FormDsgnCommand(description, formWindow)
{
}


/* AddRibbonPageCommand */
AddRibbonPageCommand::AddRibbonPageCommand(QDesignerFormWindowInterface *formWindow)
    : RibbonBarCommand(QApplication::translate("Command", "Insert page"), formWindow)
{
}

void AddRibbonPageCommand::init(RibbonBar* ribbonBar)
{
    m_ribbonBar = (QWidget*)ribbonBar;

    QDesignerFormEditorInterface *core = formWindow()->core();
    m_ribbonPage = qobject_cast<QWidget*>(core->widgetFactory()->createWidget(QLatin1String("Qtitan::RibbonPage"), m_ribbonBar.data()));
    core->widgetFactory()->initialize(m_ribbonPage);
}

void AddRibbonPageCommand::redo()
{
    QDesignerFormEditorInterface* core = formWindow()->core();
    QDesignerContainerExtension* c = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), m_ribbonBar);
    c->addWidget(m_ribbonPage);

    m_ribbonPage->setObjectName(QLatin1String("ribbonPage"));
    formWindow()->ensureUniqueObjectName(m_ribbonPage);
    core->metaDataBase()->add(m_ribbonPage);
    formWindow()->emitSelectionChanged();
    m_ribbonPage->setFocus();
}

void AddRibbonPageCommand::undo()
{
    QDesignerFormEditorInterface* core = formWindow()->core();
    QDesignerContainerExtension* c = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), m_ribbonBar);

    core->metaDataBase()->remove(m_ribbonPage);

    for (int i = 0; i < c->count(); ++i) 
    {
        if (c->widget(i) == m_ribbonPage) 
        {
            c->remove(i);
            break;
        }
    }
//    core->metaDataBase()->remove(m_ribbonPage);
    formWindow()->emitSelectionChanged();
}


/* DeleteRibbonPageCommand */
DeleteRibbonPageCommand::DeleteRibbonPageCommand(QDesignerFormWindowInterface* formWindow)
    : RibbonBarCommand(QApplication::translate("Command", "Delete page"), formWindow)
{
}

DeleteRibbonPageCommand::~DeleteRibbonPageCommand()
{
}

void DeleteRibbonPageCommand::init(RibbonBar* ribbonBar)
{
    m_ribbonBar = (QWidget*)ribbonBar;
}

void DeleteRibbonPageCommand::redo()
{
    RibbonBar* pRibbonBar = mainWidget();
    m_ribbonPage = pRibbonBar->getPage(pRibbonBar->currentIndexPage());

    QDesignerFormEditorInterface* core = formWindow()->core();
    core->metaDataBase()->remove(m_ribbonPage);
    QDesignerContainerExtension* c = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), m_ribbonBar);
    for (int i = 0; i < c->count(); ++i) 
    {
        if (c->widget(i) == m_ribbonPage) 
        {
            c->remove(i);
            break;
        }
    }
    formWindow()->emitSelectionChanged();
}

void DeleteRibbonPageCommand::undo()
{
}

/* DeleteStyleCommand */
DeleteStyleCommand::DeleteStyleCommand(QDesignerFormWindowInterface* formWindow)
    : m_formWindow(formWindow)
{
}

void DeleteStyleCommand::init(QWidget* widget)
{
    m_widget = widget;
    m_parentWidget = widget->parentWidget();
    setText(QApplication::translate("Command", "Delete '%1'").arg(widget->objectName()));
}

void DeleteStyleCommand::redo()
{
    m_formWindow->clearSelection();
    QDesignerFormEditorInterface *core = m_formWindow->core();

    if (QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), m_parentWidget)) {
        const int count = c->count();
        for (int i=0; i<count; ++i) 
        {
            if (c->widget(i) == m_widget) 
            {
                c->remove(i);
                m_widget->hide();
                return;
            }
        }
    }
}

void DeleteStyleCommand::undo()
{
    QDesignerFormEditorInterface *core = m_formWindow->core();
    m_formWindow->clearSelection();

    m_widget->setParent(m_parentWidget);

    if (QDesignerContainerExtension *c = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), m_parentWidget)) 
    {
        c->addWidget(m_widget);
        return;
    }
    m_widget->show();
}
