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
#include <qevent.h>

#include <QtDesigner/QExtensionManager>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerMetaDataBaseInterface>
#include <QtDesigner/QDesignerWidgetFactoryInterface>

#include "QtnRibbonDsgnPlugins.h"
#include "QtnRibbonDsgnContainer.h"
#include "QtnRibbonBar.h"
#include "QtnRibbonPage.h"
#include "../src/ribbon/QtnRibbonTabBar.h"

QTITAN_USE_NAMESPACE

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
        RibbonTabBar* pRibbonTabBar = pWidget->findChild<RibbonTabBar*>();
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
            RibbonTabBar* pRibbonTabBar = pWidget->findChild<RibbonTabBar*>();
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
DsgnRibbonBarContainer::DsgnRibbonBarContainer(RibbonBar* widget, QObject* parent)
    : QObject(parent)
{
    m_pRibbon = widget;
}

static void setObjectNameChildren(RibbonTabBar& widget)
{
    QList<RibbonTab*> child_list = widget.findChildren<RibbonTab*>();
    for(int i = 0, count = child_list.count(); count > i; i++)
    {
        if (RibbonTab* pWd = child_list[i])
            pWd->setObjectName("__qt__passive_RibbonTab");
    }
}

void DsgnRibbonBarContainer::addWidget(QWidget* widget)
{
    if (RibbonPage* ribbonPage = qobject_cast<RibbonPage*>(widget))
    {
        if (ribbonPage->title().isEmpty())
        {
            QString title = tr("Page %1").arg(count());
            ribbonPage->setTitle(title);
        }

        m_pRibbon->addPage(ribbonPage);
        ribbonPage->setAutoFillBackground(false);

        if (RibbonTabBar* ribbonTabBar = m_pRibbon->findChild<RibbonTabBar*>())
            setObjectNameChildren(*ribbonTabBar);

        if (QDesignerFormWindowInterface* formWindowInterface =  QDesignerFormWindowInterface::findFormWindow(m_pRibbon))
            formWindowInterface->manageWidget(ribbonPage);

        // set current page
        m_pRibbon->setCurrentPageIndex(count() - 1);
    }
}

int DsgnRibbonBarContainer::count() const
{
    return m_pRibbon->getPageCount();
}

int DsgnRibbonBarContainer::currentIndex() const
{
    return m_pRibbon->currentPageIndex();
}

void DsgnRibbonBarContainer::insertWidget(int index, QWidget* widget)
{
    if (RibbonPage* ribbonPage = qobject_cast<RibbonPage*>(widget))
    {
        if (ribbonPage->title().isEmpty())
        {
            QString title = tr("Page %1").arg(count());
            ribbonPage->setTitle(title);
        }

        m_pRibbon->insertPage(index, ribbonPage);
        ribbonPage->setAutoFillBackground(false);

        if (RibbonTabBar* ribbonTabBar = m_pRibbon->findChild<RibbonTabBar*>())
            setObjectNameChildren(*ribbonTabBar);

        if (QDesignerFormWindowInterface* formWindowInterface = QDesignerFormWindowInterface::findFormWindow(m_pRibbon))
            formWindowInterface->manageWidget(ribbonPage);

        // set current page
        m_pRibbon->setCurrentPageIndex(index);
    }
}

void DsgnRibbonBarContainer::remove(int index)
{
    m_pRibbon->detachPage(index);
}

void DsgnRibbonBarContainer::setCurrentIndex(int index)
{
    m_pRibbon->setCurrentPageIndex(index);
}

QWidget* DsgnRibbonBarContainer::widget(int index) const
{
    return m_pRibbon->getPage(index);
}

/* DsgnRibbonBarContainerFactory */
DsgnRibbonBarContainerFactory::DsgnRibbonBarContainerFactory(QExtensionManager* parent)
    : QExtensionFactory(parent)
{
}

QObject* DsgnRibbonBarContainerFactory::createExtension(QObject* object, const QString& iid, QObject* parent) const
{
    RibbonBar* widget = qobject_cast<RibbonBar*>(object);

    if (widget && (iid == Q_TYPEID(QDesignerContainerExtension))) 
        return new DsgnRibbonBarContainer(widget, parent);
    else
        return Q_NULL;
}
