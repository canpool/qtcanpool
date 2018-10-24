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
#include <QContextMenuEvent>
#include <QMessageBox>
#include <QtDesigner/QExtensionFactory>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerWidgetFactoryInterface>
#include <QtDesigner/QDesignerMetaDataBaseInterface>
#include <QtDesigner/QDesignerFormWindowManagerInterface>
#include <QtDesigner/QDesignerPropertySheetExtension>

#include "QtnRibbonPlugins.h"
#include "QtnRibbonDsgnContainer.h"
#include "QtnRibbonDsgnContainer.h"

#include "QtnRibbonDsgnTaskMenu.h"
#include "QtnRibbonMainWindow.h"
#include "QtnCommandDsgn.h"
#include "QtnRibbonDsgnPlugin.h"
#include "QtnRibbonPage.h"
#include "QtnRibbonBar.h"
#include "../src/ribbon/QtnRibbonTabBar.h"


using namespace Qtitan;


/* RibbonEventFilter */
RibbonEventFilter* RibbonEventFilter::rf = Q_NULL; 
RibbonEventFilter::RibbonEventFilter(QWidget* pWidget)
    : QObject(pWidget)
    , m_pRibbonBar(pWidget)
{
    m_removeRibbonBar = new QAction(tr("Remove Ribbon Bar"), this);
    connect(m_removeRibbonBar, SIGNAL(triggered()), this, SLOT(slotRemoveRibbonBar()));

    m_addRibbonPage = new QAction(tr("Insert page"), this);
    connect(m_addRibbonPage, SIGNAL(triggered()), this, SLOT(slotAddNewPage()));

    m_deleteRibbonPage = new QAction(tr("Delete current page"), this);
    connect(m_deleteRibbonPage, SIGNAL(triggered()), this, SLOT(slotRemoveCurrentPage()));
}

RibbonEventFilter::~RibbonEventFilter()
{
    uninstall(m_pRibbonBar, false);
}

void RibbonEventFilter::install(QWidget* pWidget)
{
    if (!RibbonEventFilter::rf)
        RibbonEventFilter::rf = new RibbonEventFilter(pWidget);

    pWidget->installEventFilter(RibbonEventFilter::rf);
    pWidget->setMouseTracking(true);

    RibbonTabBarFilter::install(pWidget);
}

void RibbonEventFilter::uninstall(QWidget* pWidget, bool deleteThis)
{
    if (deleteThis)
        RibbonTabBarFilter::uninstall(pWidget, deleteThis);

    if (RibbonEventFilter::rf)
    {
        pWidget->removeEventFilter(RibbonEventFilter::rf);
        if (deleteThis)
            delete RibbonEventFilter::rf;
        RibbonEventFilter::rf = Q_NULL;
    }
//    RibbonTabBarFilter::uninstall(pWidget);
}

bool RibbonEventFilter::eventFilter(QObject* watched, QEvent* event)
{
    if (watched != m_pRibbonBar)
        return QObject::eventFilter(watched, event);

    switch (event->type()) 
    {
        case QEvent::ContextMenu:
            return handleContextMenuEvent(static_cast<QContextMenuEvent*>(event));
        default:
            break;
    }

    return QObject::eventFilter(watched, event);
}

bool RibbonEventFilter::handleContextMenuEvent(QContextMenuEvent* event)
{
    event->accept();
    m_pRibbonBar->update();
    QMenu menu;
    menu.addAction(m_removeRibbonBar);
    menu.addSeparator();
    menu.addAction(m_addRibbonPage);
    if (((RibbonBar*)m_pRibbonBar)->getPageCount() > 0)
        menu.addAction(m_deleteRibbonPage);
    menu.exec(event->globalPos());
    return true;
}

void RibbonEventFilter::slotRemoveRibbonBar()
{
    Q_ASSERT(formWindow() != 0);

    QDesignerFormWindowInterface* fw = formWindow();

    DeleteRibbonBarCommand* cmd = new DeleteRibbonBarCommand(fw);
    cmd->init(qobject_cast<RibbonBar*>(m_pRibbonBar));
    cmd->redo();
//    fw->commandHistory()->push(cmd);
}

void RibbonEventFilter::slotAddNewPage()
{
    Q_ASSERT(formWindow() != 0);

    QDesignerFormWindowInterface *fw = formWindow();

    AddRibbonPageCommand* cmd = new AddRibbonPageCommand(fw);
    cmd->init(qobject_cast<RibbonBar*>(m_pRibbonBar));
    cmd->redo();
//    fw->commandHistory()->push(cmd);
}

void RibbonEventFilter::slotRemoveCurrentPage()
{
    Q_ASSERT(formWindow() != 0);

    QDesignerFormWindowInterface* fw = formWindow();

    DeleteRibbonPageCommand* cmd = new DeleteRibbonPageCommand(fw);
    cmd->init(qobject_cast<RibbonBar*>(m_pRibbonBar));
    cmd->redo();
//    fw->commandHistory()->push(cmd);
}

QDesignerFormWindowInterface* RibbonEventFilter::formWindow() const
{
    return QDesignerFormWindowInterface::findFormWindow(m_pRibbonBar);
}


/* RibbonDsgnPlugin */
RibbonBarDsgnPlugin::RibbonBarDsgnPlugin(QObject *parent)
    : QObject(parent)
    , m_core(0)
{
    initialized = false;
}

void RibbonBarDsgnPlugin::initialize(QDesignerFormEditorInterface* core)
{
    if (initialized)
        return;

    QExtensionManager *manager = core->extensionManager();
    Q_ASSERT(manager != Q_NULL);

    manager->registerExtensions(new RibbonDsgnTaskMenuExFactory(manager), QLatin1String("QDesignerInternalTaskMenuExtension"));
    manager->registerExtensions(new DesignerRibbonBarContainerFactory(manager), Q_TYPEID(QDesignerContainerExtension));

    initialized = true;
    m_core = core;
    connect(core->formWindowManager(), SIGNAL(formWindowAdded(QDesignerFormWindowInterface*)), 
        this, SLOT(formWindowAdded(QDesignerFormWindowInterface*)));
}

bool RibbonBarDsgnPlugin::isInitialized() const
{
    return initialized;
}

namespace Qtitan
{
    class DesignerRibbonBar : public RibbonBar
    {
    public:
        DesignerRibbonBar(QWidget* parent);
        virtual ~DesignerRibbonBar();
    protected:
        virtual void resizeEvent(QResizeEvent* event);
    };
};

/* DesignerRibbonBar */
DesignerRibbonBar::DesignerRibbonBar(QWidget* parent) 
    : RibbonBar(parent) 
{ 
    installEventFilter(new RibbonEventFilter(this));
//    RibbonTabBar* ribbonTabBar = qFindChild<Qtitan::RibbonTabBar*>(this);
    RibbonTabBar* ribbonTabBar = this->findChild<Qtitan::RibbonTabBar*>();
    Q_ASSERT(ribbonTabBar != Q_NULL);

    installEventFilter(new RibbonTabBarFilter(ribbonTabBar));
}

DesignerRibbonBar::~DesignerRibbonBar()
{
}

void DesignerRibbonBar::resizeEvent(QResizeEvent* event)
{
    RibbonBar::resizeEvent(event);

    QDesignerFormWindowInterface* form = QDesignerFormWindowInterface::findFormWindow(this);
    if (form == Q_NULL)
        return;
    if (QDesignerPropertySheetExtension *sheet = 
        qt_extension<QDesignerPropertySheetExtension*>(form->core()->extensionManager(), this))
    {
        sheet->setChanged(sheet->indexOf(QLatin1String("geometry")), false);
    } 
}

QWidget* RibbonBarDsgnPlugin::createWidget(QWidget* parent)
{
    RibbonBar* pRibbonBar = new DesignerRibbonBar(parent);
    pRibbonBar->setTitleBarVisible(false);
    connect((QWidget*)pRibbonBar, SIGNAL(currentPageChanged(int)), this, SLOT(currentPageChanged(int)));
    return (QWidget*)pRibbonBar;
}

QString RibbonBarDsgnPlugin::name() const
{
    return "Qtitan::RibbonBar";
}

QString RibbonBarDsgnPlugin::group() const
{
    return "Developer Machines: Components";
}

QIcon RibbonBarDsgnPlugin::icon() const
{
    return QIcon(":res/ribbon_icon.png");
}

QString RibbonBarDsgnPlugin::toolTip() const
{
    return tr("Menu Bar for Ribbon Interface");
}

QString RibbonBarDsgnPlugin::whatsThis() const
{
    return tr("Menu Bar for Ribbon Interface");
}

bool RibbonBarDsgnPlugin::isContainer() const
{
    return false;
}

QString RibbonBarDsgnPlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
        " <widget class=\"Qtitan::RibbonBar\" name=\"ribbonBar\"/>\n"
        " <customwidgets>\n"
        "   <customwidget>\n"
        "       <class>Qtitan::RibbonBar</class>\n"
        "       <extends>QMenuBar</extends>\n"
        "       <addpagemethod>addPage</addpagemethod>\n"
        "   </customwidget>\n"
        " </customwidgets>\n"
        "</ui>\n";
}

QString RibbonBarDsgnPlugin::includeFile() const
{
    return "QtnRibbonBar.h";
}

void RibbonBarDsgnPlugin::currentPageChanged(int index)
{
    Q_UNUSED(index);
    RibbonBar* widget = qobject_cast<RibbonBar*>(sender());
    if (widget) 
    {
        QDesignerFormWindowInterface *form = QDesignerFormWindowInterface::findFormWindow((QWidget*)widget);
        if (form)
            form->emitSelectionChanged();
    }
}

void RibbonBarDsgnPlugin::formWindowAdded(QDesignerFormWindowInterface *formWindow)
{
    connect(formWindow, SIGNAL(widgetManaged(QWidget*)), this, SLOT(widgetManaged(QWidget*)));
}

void RibbonBarDsgnPlugin::formWindowRemoved(QDesignerFormWindowInterface *formWindow)
{
    disconnect(formWindow, SIGNAL(widgetManaged(QWidget*)), this, SLOT(widgetManaged(QWidget*)));
}

void RibbonBarDsgnPlugin::widgetManaged(QWidget* widget)
{
    if (widget->metaObject()->className() == QString("Qtitan::RibbonBar"))
    {
        QDesignerFormWindowInterface* formWindow = static_cast<QDesignerFormWindowInterface *>(sender());
        QDesignerFormEditorInterface* core = formWindow->core();
        QDesignerContainerExtension* container = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), formWindow->mainContainer());
        formWindow->unmanageWidget(widget);

        QUndoStack* stack = formWindow->commandHistory();
        if (!stack->isClean())
        {
            //This code check the InsertWidget command on the stack.
            const QUndoCommand* command = stack->command(stack->index());
            if (command->childCount() == 0)
                return;
        }

        if (qobject_cast<QMainWindow *>(formWindow->mainContainer()) == 0)
        {
                QMessageBox::critical(
                    formWindow->mainContainer(),
                    tr("Can't add Ribbon Bar"), 
                    tr("You can add Ribbon Bar onto Qtitan::RibbonMainWindow or QMainWindow only."));
                widget->deleteLater();
                return;
        }

        for (int i = 0; i < container->count(); ++i)
        {
            QWidget* w = container->widget(i);
            if (w->metaObject()->className() == QString("Qtitan::RibbonBar") ||
                w->metaObject()->className() == QString("QDesignerMenuBar"))
            {
                QMessageBox::critical(
                    formWindow->mainContainer(),
                    tr("Can't add Ribbon Bar"), 
                    tr("Only one instance of the Ribbon Bar can be adding to the main form."));
                widget->deleteLater();
                return;
            }
        }

        container->addWidget(widget);
        formWindow->core()->metaDataBase()->add(widget);
    }
}


