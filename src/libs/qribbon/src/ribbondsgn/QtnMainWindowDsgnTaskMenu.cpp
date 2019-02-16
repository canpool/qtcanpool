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


using namespace Qtitan;

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

namespace Qtitan
{
    class MainWindowDsgnTaskMenuPrivate : public QObject
    {
    public:
        QTN_DECLARE_PUBLIC(MainWindowDsgnTaskMenu)
    public:
        explicit MainWindowDsgnTaskMenuPrivate();
        void init();
    public:
        QPointer<QWidget> m_widget;
        QAction* m_addRibbonBar;
    };

}; // namespace Qtitan

MainWindowDsgnTaskMenuPrivate::MainWindowDsgnTaskMenuPrivate()
    : m_addRibbonBar(Q_NULL)
{
}

void MainWindowDsgnTaskMenuPrivate::init()
{
    QTN_P(MainWindowDsgnTaskMenu);
    m_addRibbonBar = new QAction(MainWindowDsgnTaskMenu::tr("Create Ribbon Bar"), &p);
}



/* MainWindowDsgnTaskMenu */
MainWindowDsgnTaskMenu::MainWindowDsgnTaskMenu(QWidget* pWidget, QObject* parent)
    : QObject(parent)
{
    QTN_INIT_PRIVATE(MainWindowDsgnTaskMenu);
    QTN_D(MainWindowDsgnTaskMenu);
    d.init();
    Q_ASSERT(qobject_cast<QDesignerFormWindowInterface*>(pWidget) == 0);

    d.m_widget = pWidget;
    connect(d.m_addRibbonBar, SIGNAL(triggered()), this, SLOT(createRibbonBar()));
}

QWidget* MainWindowDsgnTaskMenu::widget() const
{
    QTN_D(const MainWindowDsgnTaskMenu);
    return d.m_widget;
}

void MainWindowDsgnTaskMenu::createRibbonBar()
{
    if (QDesignerFormWindowInterface* fw = formWindow()) 
    {
        QMainWindow *mw = qobject_cast<QMainWindow*>(fw->mainContainer());
        if (!mw) 
        {
            // ### warning message
            return;
        }

        CreateRibbonBarCommand* cmd = new CreateRibbonBarCommand(fw);
        cmd->init(mw);
        cmd->redo();
//        fw->commandHistory()->push(cmd);
    }
}

QDesignerFormWindowInterface* MainWindowDsgnTaskMenu::formWindow() const
{
    QDesignerFormWindowInterface* result = QDesignerFormWindowInterface::findFormWindow(widget());
    Q_ASSERT(result != 0);
    return result;
}

QList<QAction*> MainWindowDsgnTaskMenu::taskActions() const
{
    QTN_D(const MainWindowDsgnTaskMenu);
    QList<QAction *> list;

    const bool isMainContainer = formWindow()->mainContainer() == widget();

    if (const QMainWindow *mw = qobject_cast<const QMainWindow*>(formWindow()->mainContainer()))  
    {
        if (isMainContainer || mw->centralWidget() == widget()) {
            if (!findWidgetBar<RibbonBar>(mw) && !findWidgetBar<QMenuBar>(mw)) 
                list.append(d.m_addRibbonBar);
        }
    }
    return list;
}

/* MainWindowMenuExFactory */
MainWindowMenuExFactory::MainWindowMenuExFactory(QExtensionManager* parent)
    : QExtensionFactory(parent)
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






