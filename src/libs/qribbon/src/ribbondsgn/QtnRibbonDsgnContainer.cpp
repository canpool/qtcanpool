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
#include <qevent.h>

#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerMetaDataBaseInterface>
#include <QtDesigner/QDesignerWidgetFactoryInterface>

#include "QtnRibbonPlugins.h"
#include "QtnRibbonDsgnContainer.h"
#include "QtnRibbonBar.h"
#include "QtnRibbonPage.h"
#include "../src/ribbon/QtnRibbonTabBar.h"

using namespace Qtitan;

/* RibbonDsgnContainer */
RibbonTabBarFilter* RibbonTabBarFilter::tf = Q_NULL;
RibbonTabBarFilter::RibbonTabBarFilter(RibbonTabBar* pWidget)
    : QObject(pWidget)
{
    m_ribbonTabBar = pWidget;
    m_ribbonTabBar->setObjectName("__qt__passive_RibbonTabBar");
}

RibbonTabBarFilter::~RibbonTabBarFilter()
{
    uninstall(m_ribbonTabBar, false);
}

void RibbonTabBarFilter::install(QWidget* pWidget)
{
    if (!tf)
    {
//        RibbonTabBar* pRibbonTabBar = qFindChild<Qtitan::RibbonTabBar*>(pWidget);
        RibbonTabBar* pRibbonTabBar = pWidget->findChild<Qtitan::RibbonTabBar*>();
        Q_ASSERT(pRibbonTabBar != Q_NULL);

        tf = new RibbonTabBarFilter(pRibbonTabBar);

        pRibbonTabBar->installEventFilter(tf);
        pRibbonTabBar->setMouseTracking(true);
    }
}

void RibbonTabBarFilter::uninstall(QWidget* pWidget, bool deleteThis)
{
    if (RibbonTabBarFilter::tf)
    {
        if (deleteThis)
        {
//            RibbonTabBar* pRibbonTabBar = qFindChild<Qtitan::RibbonTabBar*>(pWidget);
            RibbonTabBar* pRibbonTabBar = pWidget->findChild<Qtitan::RibbonTabBar*>();
            Q_ASSERT(pRibbonTabBar != Q_NULL);
            pRibbonTabBar->removeEventFilter(RibbonTabBarFilter::tf);
        }
        else if (RibbonTabBarFilter::tf->m_ribbonTabBar)
            RibbonTabBarFilter::tf->m_ribbonTabBar->removeEventFilter(RibbonTabBarFilter::tf);

        if (deleteThis)
            delete RibbonTabBarFilter::tf;
        RibbonTabBarFilter::tf = Q_NULL;
    }
}

bool RibbonTabBarFilter::eventFilter(QObject* watched, QEvent* event)
{
    if (watched != m_ribbonTabBar)
        return QObject::eventFilter(watched, event);

    switch (event->type()) 
    {
        case QEvent::ContextMenu:
            return m_ribbonTabBar->parentWidget()->eventFilter(watched, event);
        default:
            break;
    }
    return QObject::eventFilter(watched, event);
}


/* RibbonDsgnContainer */
DesignerRibbonBarContainer::DesignerRibbonBarContainer(RibbonBar* widget, QObject* parent)
    : QObject(parent)
{
    m_pRibbon = widget;
}

void setStyleChildren(QWidget& widget, QStyle* style)
{
    widget.setStyle(style);
//    QList<QWidget*> child_list = qFindChildren<QWidget*>(&widget);
    QList<QWidget*> child_list = widget.findChildren<QWidget*>();
    for(int i = 0, count = child_list.count(); count > i; i++)
    {
        if (QWidget* pWd = child_list[i])
            setStyleChildren(*pWd, style);
    }
}

void DesignerRibbonBarContainer::addWidget(QWidget* widget)
{
    setStyleChildren((QWidget&)*m_pRibbon, m_pRibbon->style());
    if (RibbonPage* ribbonPage = qobject_cast<RibbonPage*>(widget))
    {
        if (ribbonPage->title().isEmpty())
        {
            QString title = tr("Page %1").arg(count());
            ribbonPage->setTitle(title);
        }
        // set current page
        m_pRibbon->setCurrentPage(count()-1);
    }
}

int DesignerRibbonBarContainer::count() const
{
    return m_pRibbon->getPageCount();
}

int DesignerRibbonBarContainer::currentIndex() const
{
    return m_pRibbon->currentIndexPage();
}

void DesignerRibbonBarContainer::insertWidget(int index, QWidget* widget)
{
    Q_UNUSED(index);
    Q_UNUSED(widget);
}

void DesignerRibbonBarContainer::remove(int index)
{
    m_pRibbon->removePage(index);
}

void DesignerRibbonBarContainer::setCurrentIndex(int index)
{
    m_pRibbon->setCurrentPage(index);
}

QWidget* DesignerRibbonBarContainer::widget(int index) const
{
    return m_pRibbon->getPage(index);
}

/* DesignerRibbonBarContainerFactory */
DesignerRibbonBarContainerFactory::DesignerRibbonBarContainerFactory(QExtensionManager* parent)
    : QExtensionFactory(parent)
{
}

QObject* DesignerRibbonBarContainerFactory::createExtension(QObject* object, const QString& iid, QObject* parent) const
{
    RibbonBar* widget = qobject_cast<RibbonBar*>(object);

    if (widget && (iid == Q_TYPEID(QDesignerContainerExtension))) 
        return new DesignerRibbonBarContainer(widget, parent);
    else
        return Q_NULL;
}
