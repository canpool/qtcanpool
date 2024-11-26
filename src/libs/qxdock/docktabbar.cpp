/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "docktabbar.h"
#include "dockpanel.h"
#include "docktab.h"
#include "dockutils.h"

#include <QBoxLayout>
#include <QWheelEvent>
#include <QScrollBar>
#include <QTimer>

QX_DOCK_BEGIN_NAMESPACE

class DockTabBarPrivate
{
public:
    QX_DECLARE_PUBLIC(DockTabBar)
public:
    DockTabBarPrivate();
    void init();
    void updateTabs();
public:
    DockPanel *m_panel = nullptr;
    QWidget *m_tabsContainerWidget;
    QBoxLayout *m_tabsLayout;
    int m_currentIndex = -1;
};

DockTabBarPrivate::DockTabBarPrivate()
{
}

void DockTabBarPrivate::init()
{
    Q_Q(DockTabBar);

    m_tabsContainerWidget = new QWidget();
    m_tabsContainerWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_tabsContainerWidget->setObjectName("tabsContainerWidget");
    m_tabsLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_tabsLayout->setContentsMargins(0, 0, 0, 0);
    m_tabsLayout->setSpacing(0);
    m_tabsLayout->addStretch(1);
    m_tabsContainerWidget->setLayout(m_tabsLayout);
    q->setWidget(m_tabsContainerWidget);
}

void DockTabBarPrivate::updateTabs()
{
    Q_Q(DockTabBar);
    for (int i = 0; i < q->count(); ++i) {
        auto tab = q->tab(i);
        if (!tab) {
            continue;
        }
        if (i == m_currentIndex) {
            tab->show();
            tab->setActive(true);
            // Sometimes the synchronous calculation of the rectangular area fails
            // Therefore we use QTimer::singleShot here to execute the call
            // within the event loop - see #520
            // FIXME: Execute the following code to finish the program unexpectedly
            /*
            QTimer::singleShot(0, q, [&, tab] {
                q->ensureWidgetVisible(tab);
            });
            */
        } else {
            tab->setActive(false);
        }
    }
}

DockTabBar::DockTabBar(DockPanel *parent)
    : QScrollArea(parent)
{
    QX_INIT_PRIVATE(DockTabBar);
    Q_D(DockTabBar);
    d->m_panel = parent;
    d->init();

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setFrameStyle(QFrame::NoFrame);
    setWidgetResizable(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFocusPolicy(Qt::NoFocus);
}

DockTabBar::~DockTabBar()
{
    QX_FINI_PRIVATE();
}

void DockTabBar::insertTab(int index, DockTab *tab)
{
    Q_D(DockTabBar);
    d->m_tabsLayout->insertWidget(index, tab);
    connect(tab, SIGNAL(clicked()), this, SLOT(onTabClicked()));
    connect(tab, SIGNAL(closeRequested()), this, SLOT(onTabCloseRequested()));
    connect(tab, SIGNAL(closeOtherTabsRequested()), this, SLOT(onCloseOtherTabsRequested()));
    connect(tab, SIGNAL(moved(QPoint)), this, SLOT(onTabWidgetMoved(QPoint)));
    connect(tab, SIGNAL(elidedChanged(bool)), this, SIGNAL(elidedChanged(bool)));
    tab->installEventFilter(this);
    Q_EMIT tabInserted(index);
    if (index <= d->m_currentIndex) {
        setCurrentIndex(d->m_currentIndex + 1);
    } else if (d->m_currentIndex == -1) {
        setCurrentIndex(index);
    }
    updateGeometry();
}

void DockTabBar::removeTab(DockTab *tab)
{
    if (!count()) {
        return;
    }
    Q_D(DockTabBar);
    int newCurrentIndex = currentIndex();
    int removeIndex = d->m_tabsLayout->indexOf(tab);
    if (count() == 1) {
        newCurrentIndex = -1;
    }
    if (newCurrentIndex > removeIndex) {
        newCurrentIndex--;
    } else if (newCurrentIndex == removeIndex) {
        newCurrentIndex = -1;
        // First we walk to the right to search for the next visible tab
        for (int i = (removeIndex + 1); i < count(); ++i) {
            if (this->tab(i)->isVisibleTo(this)) {
                newCurrentIndex = i - 1;
                break;
            }
        }

        // If there is no visible tab right to this tab then we walk to
        // the left to find a visible tab
        if (newCurrentIndex < 0) {
            for (int i = (removeIndex - 1); i >= 0; --i) {
                if (this->tab(i)->isVisibleTo(this)) {
                    newCurrentIndex = i;
                    break;
                }
            }
        }
    }

    Q_EMIT removingTab(removeIndex);
    d->m_tabsLayout->removeWidget(tab);
    tab->disconnect(this);
    tab->removeEventFilter(this);
    if (newCurrentIndex != d->m_currentIndex) {
        setCurrentIndex(newCurrentIndex);
    } else {
        d->updateTabs();
    }

    updateGeometry();
}

int DockTabBar::count() const
{
    Q_D(const DockTabBar);
    // The tab bar contains a stretch item as last item
    return d->m_tabsLayout->count() - 1;
}

int DockTabBar::currentIndex() const
{
    Q_D(const DockTabBar);
    return d->m_currentIndex;
}

DockTab *DockTabBar::currentTab() const
{
    Q_D(const DockTabBar);
    if (d->m_currentIndex < 0 || d->m_currentIndex >= d->m_tabsLayout->count()) {
        return nullptr;
    } else {
        return qobject_cast<DockTab *>(d->m_tabsLayout->itemAt(d->m_currentIndex)->widget());
    }
}

DockTab *DockTabBar::tab(int index) const
{
    if (index < 0 || index >= count()) {
        return nullptr;
    }
    Q_D(const DockTabBar);
    return qobject_cast<DockTab *>(d->m_tabsLayout->itemAt(index)->widget());
}

int DockTabBar::tabAt(const QPoint &pos) const
{
    if (!isVisible()) {
        return Qx::DockTabInvalidIndex;
    }

    if (pos.x() < tab(0)->geometry().x()) {
        return -1;
    }

    int cnt = count();
    for (int i = 0; i < cnt; ++i) {
        if (tab(i)->geometry().contains(pos)) {
            return i;
        }
    }

    return cnt;
}

int DockTabBar::tabInsertIndexAt(const QPoint &pos) const
{
    int index = tabAt(pos);
    if (index == Qx::DockTabInvalidIndex) {
        return Qx::DockTabDefaultInsertIndex;
    } else {
        return (index < 0) ? 0 : index;
    }
}

bool DockTabBar::isTabOpen(int index) const
{
    if (index < 0 || index >= count()) {
        return false;
    }

    return !tab(index)->isHidden();
}

QSize DockTabBar::minimumSizeHint() const
{
    QSize Size = sizeHint();
    Size.setWidth(10);
    return Size;
}

QSize DockTabBar::sizeHint() const
{
    Q_D(const DockTabBar);
    // Limit the size of the entire QScrollArea based on the size of the widgets in the container,
    // otherwise the size of the QScrollArea will be large.
    // Notes: setting SizePolicy alone will not make the size smaller.
    return d->m_tabsContainerWidget->sizeHint();
}

bool DockTabBar::areTabsOverflowing() const
{
    Q_D(const DockTabBar);
    return d->m_tabsContainerWidget->width() > width();
}

void DockTabBar::setCurrentIndex(int index)
{
    Q_D(DockTabBar);
    if (index == d->m_currentIndex) {
        return;
    }
    if (index < -1 || index > (count() - 1)) {
        return;
    }
    Q_EMIT currentChanging(index);
    d->m_currentIndex = index;
    d->updateTabs();
    updateGeometry();
    Q_EMIT currentChanged(index);
}

void DockTabBar::closeTab(int index)
{
    if (index < 0 || index >= count()) {
        return;
    }

    auto tab = this->tab(index);
    if (tab->isHidden()) {
        return;
    }
    Q_EMIT tabCloseRequested(index);
}

void DockTabBar::onTabClicked()
{
    Q_D(DockTabBar);
    DockTab *tab = qobject_cast<DockTab *>(sender());
    if (!tab) {
        return;
    }
    int index = d->m_tabsLayout->indexOf(tab);
    if (index < 0) {
        return;
    }
    setCurrentIndex(index);
    Q_EMIT tabBarClicked(index);
}

void DockTabBar::onTabCloseRequested()
{
    Q_D(DockTabBar);
    DockTab *tab = qobject_cast<DockTab *>(sender());
    int index = d->m_tabsLayout->indexOf(tab);
    closeTab(index);
}

void DockTabBar::onCloseOtherTabsRequested()
{
    auto senderTab = qobject_cast<DockTab *>(sender());
    for (int i = 0; i < count(); ++i) {
        auto otherTab = this->tab(i);
        if (otherTab->isClosable() && !otherTab->isHidden() && otherTab != senderTab) {
            // If the dock widget is deleted with the closeTab() call, its tab
            // it will no longer be in the layout, and thus the index needs to
            // be updated to not skip any tabs
            int offset = otherTab->dockWidget()->features().testFlag(DockWidget::DockWidgetDeleteOnClose) ? 1 : 0;
            closeTab(i);

            // If the the dock widget blocks closing, i.e. if the flag
            // CustomCloseHandling is set, and the dock widget is still open,
            // then we do not need to correct the index
            if (otherTab->dockWidget()->isClosed()) {
                i -= offset;
            }
        }
    }
}

void DockTabBar::onTabWidgetMoved(const QPoint &globalPos)
{
    Q_D(DockTabBar);
    DockTab *movingTab = qobject_cast<DockTab *>(sender());
    if (!movingTab) {
        return;
    }

    int fromIndex = d->m_tabsLayout->indexOf(movingTab);
    auto mousePos = mapFromGlobal(globalPos);
    mousePos.rx() = qMax(0, mousePos.x());
    mousePos.rx() = qMin(width(), mousePos.x());
    int toIndex = -1;
    // Find tab under mouse
    for (int i = 0; i < count(); ++i) {
        DockTab *dropTab = this->tab(i);
        auto tabGeometry = dropTab->geometry();
        tabGeometry.setTopLeft(d->m_tabsContainerWidget->mapToParent(tabGeometry.topLeft()));
        tabGeometry.setBottomRight(d->m_tabsContainerWidget->mapToParent(tabGeometry.bottomRight()));
        if (dropTab == movingTab || !dropTab->isVisibleTo(this) || !tabGeometry.contains(mousePos)) {
            continue;
        }

        toIndex = d->m_tabsLayout->indexOf(dropTab);
        if (toIndex == fromIndex) {
            toIndex = -1;
        }
        break;
    }

    if (toIndex > -1) {
        d->m_tabsLayout->removeWidget(movingTab);
        d->m_tabsLayout->insertWidget(toIndex, movingTab);
        Q_EMIT tabMoved(fromIndex, toIndex);
        setCurrentIndex(toIndex);
    } else {
        // Ensure that the moved tab is reset to its start position
        d->m_tabsLayout->update();
    }
}

bool DockTabBar::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(DockTabBar);
    bool result = Super::eventFilter(watched, event);
    DockTab *tab = qobject_cast<DockTab *>(watched);
    if (!tab) {
        return result;
    }

    switch (event->type()) {
    case QEvent::Hide:
        Q_EMIT tabClosed(d->m_tabsLayout->indexOf(tab));
        updateGeometry();
        break;

    case QEvent::Show:
        Q_EMIT tabOpened(d->m_tabsLayout->indexOf(tab));
        updateGeometry();
        break;

    // Setting the text of a tab will cause a LayoutRequest event
    case QEvent::LayoutRequest:
        updateGeometry();
        break;

    // Manage wheel event
    case QEvent::Wheel:
        // Ignore wheel events if tab is currently dragged
        if (tab->dragState() == Qx::DockDraggingInactive) {
            wheelEvent((QWheelEvent *)event);
        }
        break;

    default:
        break;
    }

    return result;
}

void DockTabBar::wheelEvent(QWheelEvent *event)
{
    event->accept();
    const int direction = event->angleDelta().y();
    if (direction < 0) {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() + 20);
    } else {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - 20);
    }
}

QX_DOCK_END_NAMESPACE
