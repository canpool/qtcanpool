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
#include "QtnMainWindowDsgnTaskMenu.h"

#include <QApplication>
#include <QUndoCommand>
#include <QAction>

#include <QtPlugin>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QExtensionManager>

#include "QtnCommandDsgn.h"
#include "QtnRibbonMainWindow.h"
#include "QtnRibbonBar.h"


QTITAN_USE_NAMESPACE

template<class T>
static T* findWidgetBar(const QWidget *widget)
{
    const QList<QObject*> children = widget->children();
    foreach (QObject *obj, widget->children()) 
    {
        if (T* mb = qobject_cast<T*>(obj))
            return mb;
    }
    return Q_NULL;
}


/* MainWindowDsgnTaskMenu */
MainWindowDsgnTaskMenu::MainWindowDsgnTaskMenu(QWidget* widget, QObject* parent)
    : QObject(parent)
{
    m_widget = widget;
    Q_ASSERT(qobject_cast<QDesignerFormWindowInterface*>(m_widget) == 0);
    m_addRibbonBar = new QAction(MainWindowDsgnTaskMenu::tr("Create Ribbon Bar"), this);
    QObject::connect(m_addRibbonBar, SIGNAL(triggered()), this, SLOT(slotCreateRibbonBar()));
}

MainWindowDsgnTaskMenu::~MainWindowDsgnTaskMenu()
{
}

QWidget* MainWindowDsgnTaskMenu::widget() const
{
    return m_widget;
}

void MainWindowDsgnTaskMenu::slotCreateRibbonBar()
{
    QDesignerFormWindowInterface* fw = formWindow();
    Q_ASSERT(fw != Q_NULL);

    QMainWindow *mw = qobject_cast<QMainWindow*>(fw->mainContainer());
    if (!mw) 
    {
        // ### warning message
        return;
    }

    CreateRibbonBarCommand* cmd = new CreateRibbonBarCommand(fw);
    cmd->init(mw);
    cmd->redo();
    fw->commandHistory()->push(cmd);
}

QDesignerFormWindowInterface* MainWindowDsgnTaskMenu::formWindow() const
{
    QDesignerFormWindowInterface* result = QDesignerFormWindowInterface::findFormWindow(widget());
    Q_ASSERT(result != 0);
    return result;
}

QList<QAction*> MainWindowDsgnTaskMenu::taskActions() const
{
    QList<QAction *> list;
    const bool isMainContainer = formWindow()->mainContainer() == widget();
    if (const QMainWindow* mw = qobject_cast<const QMainWindow*>(formWindow()->mainContainer()))  
    {
        if (isMainContainer || mw->centralWidget() == widget()) {
            if (!findWidgetBar<RibbonBar>(mw) && !findWidgetBar<QMenuBar>(mw)) 
                list.append(m_addRibbonBar);
        }
    }
    return list;
}

/* MainWindowMenuExFactory */
MainWindowMenuExFactory::MainWindowMenuExFactory(QExtensionManager* parent)
    : QExtensionFactory(parent)
{
}

MainWindowMenuExFactory::~MainWindowMenuExFactory()
{
}

QObject* MainWindowMenuExFactory::createExtension(QObject* object, const QString& iid, QObject* parent) const
{
    RibbonMainWindow* widget = qobject_cast<RibbonMainWindow*>(object);

    if (widget && (iid == Q_TYPEID(QDesignerTaskMenuExtension))) 
        return new MainWindowDsgnTaskMenu(widget, parent);
    else
        return 0;
}






