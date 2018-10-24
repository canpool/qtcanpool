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
#include "QtnMainWindowDsgnContainer.h"

#include <qevent.h>
#include <QToolBar>
#include <QDockWidget>
#include <QLayout>
#include <QDesignerFormWindowInterface>
#include <QDesignerFormEditorInterface>
#include <QDesignerMetaDataBaseInterface>
#include "QtnRibbonStyleDsgnPlugin.h"

#include "QtnRibbonMainWindow.h"
#include "QtnRibbonBar.h"
#include "QtnRibbonStatusBar.h"
#include "QtnRibbonDsgnPlugin.h"

using namespace Qtitan;

// Pair of <area,break_before>
typedef QPair<Qt::ToolBarArea,bool> ToolBarData;

static ToolBarData toolBarData(QToolBar *me) 
{
    const QMainWindow *mw = qobject_cast<const QMainWindow*>(me->parentWidget());
    if (!mw || !mw->layout() ||  mw->layout()->indexOf(me) == -1)
        return ToolBarData(Qt::TopToolBarArea,false);
    return ToolBarData(mw->toolBarArea(me), mw->toolBarBreak(me));
}

static Qt::DockWidgetArea dockWidgetArea(QDockWidget *me) 
{
    if (const QMainWindow *mw = qobject_cast<const QMainWindow*>(me->parentWidget())) {
        // Make sure that me is actually managed by mw, otherwise
        // QMainWindow::dockWidgetArea() will be VERY upset
        QList<QLayout*> candidates;
        if (mw->layout()) {
            candidates.append(mw->layout());
            candidates +=  mw->layout()->findChildren<QLayout*>();
//            candidates += qFindChildren<QLayout*>(mw->layout());
        }
        foreach (QLayout *l, candidates) {
            if (l->indexOf(me) != -1) {
                return mw->dockWidgetArea(me);
            }
        }
    }
    return Qt::LeftDockWidgetArea;
}

/* MainWindowContainer */
MainWindowContainer::MainWindowContainer(RibbonMainWindow* widget, QObject* parent)
    : QObject(parent)
    , m_mainWindow(widget)
{
    widget->setObjectName(QLatin1String("RibbonMainWindow"));
    widget->setWindowTitle(QLatin1String("RibbonMainWindow"));
}

MainWindowContainer::~MainWindowContainer()
{
}

int MainWindowContainer::count() const
{
    return m_widgets.count();
}

QWidget *MainWindowContainer::widget(int index) const
{
    if (index == -1)
        return 0;

    return m_widgets.at(index);
}

int MainWindowContainer::currentIndex() const
{
    return m_mainWindow->centralWidget() ? 0 : -1;
}

void MainWindowContainer::setCurrentIndex(int index)
{
    Q_UNUSED(index);
}

void MainWindowContainer::addWidget(QWidget* widget)
{
    // remove all the occurrences of widget
    m_widgets.removeAll(widget);

    if (QToolBar *toolBar = qobject_cast<QToolBar*>(widget)) 
    {
        m_widgets.append(widget);
        const ToolBarData data = toolBarData(toolBar);
        m_mainWindow->addToolBar(data.first, toolBar);
        if (data.second) 
            m_mainWindow->insertToolBarBreak(toolBar);
        toolBar->show();
    }
    else if (RibbonBar* ribbonBar = qobject_cast<RibbonBar*>(widget)) 
    {
        //if (ribbonBar != m_mainWindow->menuWidget())
        m_mainWindow->setRibbonBar(ribbonBar);
//        m_mainWindow->setMenuBar(ribbonBar);
        if (!m_widgets.contains(widget))
        {
            m_widgets.append(widget);
        }
        ribbonBar->show();
    } 
    else if (QMenuBar* menuBar = qobject_cast<QMenuBar*>(widget)) 
    {
        //if (menuBar != m_mainWindow->menuBar())
        m_mainWindow->setMenuBar(menuBar);
        m_widgets.append(widget);
        menuBar->show();
    }
    else if (QStatusBar* statusBar = qobject_cast<QStatusBar*>(widget)) 
    {
        //if (statusBar != m_mainWindow->statusBar())
        m_mainWindow->setStatusBar(statusBar);
        if (!m_widgets.contains(widget))
            m_widgets.append(widget);
        statusBar->show();
    } 
    else if (QDockWidget* dockWidget = qobject_cast<QDockWidget*>(widget)) 
    {
        m_widgets.append(widget);
        m_mainWindow->addDockWidget(dockWidgetArea(dockWidget), dockWidget);
        dockWidget->show();
    }
    else if (widget)
    {
        m_widgets.prepend(widget);

        if (widget != m_mainWindow->centralWidget()) 
        {
            // note that qmainwindow will delete the current central widget if you
            // call setCentralWidget(), we end up with dangeling pointers in m_widgets list
            m_widgets.removeAll(m_mainWindow->centralWidget());

            widget->setParent(m_mainWindow);
            m_mainWindow->setCentralWidget(widget);
        }
    }
}

void MainWindowContainer::insertWidget(int index, QWidget* widget)
{
    m_widgets.insert(index, widget);
}

void MainWindowContainer::remove(int index)
{
    QWidget* widget = m_widgets.at(index);
    if (QToolBar* toolBar = qobject_cast<QToolBar*>(widget)) 
    {
        m_mainWindow->removeToolBar(toolBar);
    } 
    else if (RibbonBar* ribbonBar = qobject_cast<RibbonBar*>(widget)) 
    {
        ribbonBar->hide();
        ribbonBar->setParent(Q_NULL);
        m_mainWindow->setRibbonBar(Q_NULL);
    } 
    else if (QMenuBar* menuBar = qobject_cast<QMenuBar*>(widget)) 
    {
        menuBar->hide();
        menuBar->setParent(Q_NULL);
        m_mainWindow->setMenuBar(Q_NULL);
    } 
    else if (QStatusBar* statusBar = qobject_cast<QStatusBar*>(widget)) 
    {
        statusBar->hide();
        statusBar->setParent(Q_NULL);
        m_mainWindow->setStatusBar(Q_NULL);
    } 
    //else if (widget->metaObject()->className() == QString("Qtitan::RibbonStyle"))
    //{
    //    widget->setParent(Q_NULL);
    //    widget->hide();
    //    RibbonStyleWidget* styleWidget = static_cast<RibbonStyleWidget *>(widget);
    //    styleWidget->setHostStyle(Q_NULL);
    //}
    //else if (widget->metaObject()->className() == QString("Qtitan::OfficeStyle"))
    //{
    //    widget->setParent(Q_NULL);
    //    widget->hide();
    //    RibbonStyleWidget* styleWidget = static_cast<RibbonStyleWidget *>(widget);
    //    styleWidget->setHostStyle(Q_NULL);
    //}

    m_widgets.removeAt(index);
}

/* MainWindowContainerFactory */
MainWindowContainerFactory::MainWindowContainerFactory(QExtensionManager* parent)
    : QExtensionFactory(parent)
{
}

QObject *MainWindowContainerFactory::createExtension(QObject* object, const QString& iid, QObject* parent) const
{
    if (iid != Q_TYPEID(QDesignerContainerExtension))
        return 0;

    if (RibbonMainWindow* w = qobject_cast<RibbonMainWindow*>(object))
        return new MainWindowContainer(w, parent);

    return 0;
}


