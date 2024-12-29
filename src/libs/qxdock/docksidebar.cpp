/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "docksidebar.h"
#include "dockcontainer.h"
#include "dockautohidecontainer.h"
#include "dockwindow.h"
#include "dockfocuscontroller.h"
#include "docksidetab.h"

#include <QBoxLayout>
#include <QXmlStreamWriter>
#include <QApplication>

QX_DOCK_BEGIN_NAMESPACE

class SideTabsWidget;

class DockSideBarPrivate
{
public:
    QX_DECLARE_PUBLIC(DockSideBar)
public:
    DockSideBarPrivate();
    void init();
    inline bool isHorizontal() const;
    void handleViewportEvent(QEvent *e);
public:
    DockContainer *m_container;
    QBoxLayout *m_tabsLayout;
    Qx::DockSideBarArea m_sideTabArea = Qx::DockSideBarLeft;
    Qt::Orientation m_orientation;
    SideTabsWidget *m_tabsContainerWidget;
};

class SideTabsWidget : public QWidget
{
public:
    using QWidget::QWidget;
    using Super = QWidget;
    DockSideBarPrivate *eventHandler;

    virtual QSize minimumSizeHint() const override
    {
        return Super::sizeHint();
    }

    virtual bool event(QEvent *e) override
    {
        eventHandler->handleViewportEvent(e);
        return Super::event(e);
    }
};

DockSideBarPrivate::DockSideBarPrivate()
{
}

void DockSideBarPrivate::init()
{
    Q_Q(DockSideBar);

    m_tabsContainerWidget = new SideTabsWidget();
    m_tabsContainerWidget->eventHandler = this;
    m_tabsContainerWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_tabsContainerWidget->setObjectName("sideTabsContainerWidget");

    m_tabsLayout = new QBoxLayout(m_orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight);
    m_tabsLayout->setContentsMargins(0, 0, 0, 0);
    m_tabsLayout->setSpacing(12);
    m_tabsLayout->addStretch(1);
    m_tabsContainerWidget->setLayout(m_tabsLayout);
    q->setWidget(m_tabsContainerWidget);
}

bool DockSideBarPrivate::isHorizontal() const
{
    return Qt::Horizontal == m_orientation;
}

void DockSideBarPrivate::handleViewportEvent(QEvent *e)
{
    Q_Q(DockSideBar);
    switch (e->type()) {
    case QEvent::ChildRemoved:
        if (m_tabsLayout->isEmpty()) {
            q->hide();
        }
        break;

    default:
        break;
    }
}

DockSideBar::DockSideBar(DockContainer *parent, Qx::DockSideBarArea area)
    : QScrollArea(parent)
{
    QX_INIT_PRIVATE(DockSideBar);
    Q_D(DockSideBar);
    d->m_container = parent;
    d->m_sideTabArea = area;
    d->m_orientation = (area == Qx::DockSideBarBottom || area == Qx::DockSideBarTop) ? Qt::Horizontal : Qt::Vertical;

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setFrameStyle(QFrame::NoFrame);
    setWidgetResizable(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    d->init();

    setFocusPolicy(Qt::NoFocus);
    if (d->isHorizontal()) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    } else {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }

    hide();
}

DockSideBar::~DockSideBar()
{
    auto tabs = findChildren<DockSideTab *>(QString(), Qt::FindDirectChildrenOnly);
    for (auto tab : tabs) {
        tab->setParent(nullptr);
    }
    QX_FINI_PRIVATE();
}

void DockSideBar::removeTab(DockSideTab *sideTab)
{
    Q_D(DockSideBar);
    sideTab->removeEventFilter(this);
    d->m_tabsLayout->removeWidget(sideTab);
    if (d->m_tabsLayout->isEmpty()) {
        hide();
    }
}

/**
 * Insert dock widget into the side bar.
 * The function creates the auto hide dock container, inserts the
 * auto hide tab
 */
DockAutoHideContainer *DockSideBar::insertDockWidget(int index, DockWidget *w)
{
    Q_D(DockSideBar);
    auto autoHideContainer = new DockAutoHideContainer(w, d->m_sideTabArea, d->m_container);
    w->dockWindow()->dockFocusController()->clearDockWidgetFocus(w);
    auto tab = autoHideContainer->autoHideTab();
    w->setSideTab(tab);
    insertTab(index, tab);
    return autoHideContainer;
}


/**
 * Adds the given autoHideWidget to this sidebar.
 * If the autoHideWidget is in another sidebar, then it will be removed
 * from this sidebar.
 */
void DockSideBar::addAutoHideWidget(DockAutoHideContainer *autoHideWidget, int index)
{
    Q_D(DockSideBar);
    auto sideBar = autoHideWidget->autoHideTab()->sideBar();
    if (sideBar == this) {
        // If we move to the same tab index or if we insert before the next
        // tab index, then we will end at the same tab position and can leave
        if (autoHideWidget->tabIndex() == index || (autoHideWidget->tabIndex() + 1) == index) {
            return;
        }

        // We remove this auto hide widget from the sidebar in the code below
        // and therefore need to correct the TabIndex here
        if (autoHideWidget->tabIndex() < index) {
            --index;
        }
    }

    if (sideBar) {
        sideBar->removeAutoHideWidget(autoHideWidget);
    }
    autoHideWidget->setParent(d->m_container);
    autoHideWidget->setSideBarArea(d->m_sideTabArea);
    d->m_container->registerAutoHideWidget(autoHideWidget);
    insertTab(index, autoHideWidget->autoHideTab());
}

void DockSideBar::removeAutoHideWidget(DockAutoHideContainer *autoHideWidget)
{
    autoHideWidget->autoHideTab()->removeFromSideBar();
    auto dockContainer = autoHideWidget->dockContainer();
    if (dockContainer) {
        dockContainer->removeAutoHideWidget(autoHideWidget);
    }
    autoHideWidget->setParent(nullptr);
}

Qt::Orientation DockSideBar::orientation() const
{
    Q_D(const DockSideBar);
    return d->m_orientation;
}

DockSideTab *DockSideBar::tab(int index) const
{
    Q_D(const DockSideBar);
    return qobject_cast<DockSideTab *>(d->m_tabsLayout->itemAt(index)->widget());
}

/**
 * Returns the tab at the given position.
 * Returns -1 if the position is left of the first tab and count() if the
 * position is right of the last tab. Returns Qx::DockTabInvalidIndex (-2) to
 * indicate an invalid value.
 */
int DockSideBar::tabAt(const QPoint &pos) const
{
    if (!isVisible()) {
        return Qx::DockTabInvalidIndex;
    }

    if (orientation() == Qt::Horizontal) {
        if (pos.x() < tab(0)->geometry().x()) {
            return -1;
        }
    } else {
        if (pos.y() < tab(0)->geometry().y()) {
            return -1;
        }
    }

    for (int i = 0; i < count(); ++i) {
        if (tab(i)->geometry().contains(pos)) {
            return i;
        }
    }

    return count();
}

int DockSideBar::tabInsertIndexAt(const QPoint &pos) const
{
    int index = tabAt(pos);
    if (index == Qx::DockTabInvalidIndex) {
        return Qx::DockTabDefaultInsertIndex;
    } else {
        return (index < 0) ? 0 : index;
    }
}

int DockSideBar::indexOfTab(const DockSideTab &tab) const
{
    for (auto i = 0; i < count(); i++) {
        if (this->tab(i) == &tab) {
            return i;
        }
    }

    return -1;
}

int DockSideBar::count() const
{
    Q_D(const DockSideBar);
    return d->m_tabsLayout->count() - 1;
}

int DockSideBar::visibleTabCount() const
{
    int visibleTabCount = 0;
    auto parentWidget = this->parentWidget();
    for (auto i = 0; i < count(); i++) {
        if (tab(i)->isVisibleTo(parentWidget)) {
            visibleTabCount++;
        }
    }

    return visibleTabCount;
}

/**
 * Returns true, if the sidebar contains visible tabs to its parent widget.
 * The function returns as soon as it finds the first visible tab.
 * That means, if you just want to find out if there are visible tabs
 * then this function is quicker than visibleTabCount()
 */
bool DockSideBar::hasVisibleTabs() const
{
    auto parentWidget = this->parentWidget();
    for (auto i = 0; i < count(); i++) {
        if (tab(i)->isVisibleTo(parentWidget)) {
            return true;
        }
    }

    return false;
}

Qx::DockSideBarArea DockSideBar::sideBarArea() const
{
    Q_D(const DockSideBar);
    return d->m_sideTabArea;
}

/**
 * Overrides the minimumSizeHint() function of QScrollArea
 * The minimumSizeHint() is bigger than the sizeHint () for the scroll
 * area because even if the scrollbars are invisible, the required space
 * is reserved in the minimumSizeHint(). This override simply returns
 * sizeHint();
 */
QSize DockSideBar::minimumSizeHint() const
{
    Q_D(const DockSideBar);
    QSize size = sizeHint();
    if (d->isHorizontal()) {
        size.setWidth(0);
    } else {
        size.setHeight(0);
    }
    return size;
}

QSize DockSideBar::sizeHint() const
{
    Q_D(const DockSideBar);
    return d->m_tabsContainerWidget->sizeHint();
}

int DockSideBar::spacing() const
{
    Q_D(const DockSideBar);
    return d->m_tabsLayout->spacing();
}

void DockSideBar::setSpacing(int spacing)
{
    Q_D(DockSideBar);
    d->m_tabsLayout->setSpacing(spacing);
}

void DockSideBar::insertTab(int index, DockSideTab *sideTab)
{
    Q_D(DockSideBar);
    sideTab->setSideBar(this);
    sideTab->installEventFilter(this);
    // Default insertion is append
    if (index < 0) {
        d->m_tabsLayout->insertWidget(d->m_tabsLayout->count() - 1, sideTab);
    } else {
        d->m_tabsLayout->insertWidget(index, sideTab);
    }
    show();
}

void DockSideBar::saveState(QXmlStreamWriter &s) const
{
    if (!count()) {
        return;
    }

    s.writeStartElement("SideBar");
    s.writeAttribute("Area", QString::number(sideBarArea()));
    s.writeAttribute("Tabs", QString::number(count()));

    for (auto i = 0; i < count(); ++i) {
        auto t = tab(i);
        if (!t) {
            continue;
        }
        t->dockWidget()->autoHideContainer()->saveState(s);
    }

    s.writeEndElement();
}

bool DockSideBar::eventFilter(QObject *watched, QEvent *event)
{
    auto tab = qobject_cast<DockSideTab *>(watched);
    if (!tab) {
        return false;
    }

    switch (event->type()) {
    case QEvent::ShowToParent:
        show();
        break;

    case QEvent::HideToParent:
        if (!hasVisibleTabs()) {
            hide();
        }
        break;

    default:
        break;
    }

    return false;
}

QX_DOCK_END_NAMESPACE
