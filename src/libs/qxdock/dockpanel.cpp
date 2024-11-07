/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockpanel.h"
#include "dockcontainer.h"
#include "dockwindow.h"
#include "docktitlebar.h"
#include "dockutils.h"
#include "docksplitter.h"
#include "docktabbar.h"
#include "docktab.h"

#include <QBoxLayout>

QX_DOCK_BEGIN_NAMESPACE

/**
 * Internal dock area layout mimics stack layout but only inserts the current
 * widget into the internal QLayout object.
 * \warning Only the current widget has a parent. All other widgets
 * do not have a parent. That means, a widget that is in this layout may
 * return nullptr for its parent() function if it is not the current widget.
 */
class DockAreaLayout
{
private:
    QBoxLayout *m_parentLayout;
    QList<QPointer<QWidget>> m_widgets;
    int m_currentIndex = -1;
    QWidget *m_currentWidget = nullptr;
public:
    DockAreaLayout(QBoxLayout *parentLayout) : m_parentLayout(parentLayout)
    {
    }

    int count() const
    {
        return m_widgets.count();
    }

    void insertWidget(int index, QWidget *w)
    {
        w->setParent(nullptr);
        if (index < 0) {
            index = m_widgets.count();
        }
        m_widgets.insert(index, w);
        if (m_currentIndex < 0) {
            setCurrentIndex(index);
        } else {
            if (index <= m_currentIndex) {
                ++m_currentIndex;
            }
        }
    }

    void removeWidget(QWidget *w)
    {
        if (currentWidget() == w) {
            auto layoutItem = m_parentLayout->takeAt(1);
            if (layoutItem) {
                layoutItem->widget()->setParent(nullptr);
            }
            delete layoutItem;
            m_currentWidget = nullptr;
            m_currentIndex = -1;
        } else if (indexOf(w) < m_currentIndex) {
            --m_currentIndex;
        }
        m_widgets.removeOne(w);
    }

    /**
     * Returns the current selected widget
     */
    QWidget *currentWidget() const
    {
        return m_currentWidget;
    }

    void setCurrentIndex(int index)
    {
        QWidget *prev = currentWidget();
        QWidget *next = widget(index);
        if (!next || (next == prev && !m_currentWidget)) {
            return;
        }

        bool reenableUpdates = false;
        QWidget *parent = m_parentLayout->parentWidget();

        if (parent && parent->updatesEnabled()) {
            reenableUpdates = true;
            parent->setUpdatesEnabled(false);
        }

        auto layoutItem = m_parentLayout->takeAt(1);
        if (layoutItem) {
            layoutItem->widget()->setParent(nullptr);
        }
        delete layoutItem;

        m_parentLayout->addWidget(next);
        if (prev) {
            prev->hide();
        }
        m_currentIndex = index;
        m_currentWidget = next;

        if (reenableUpdates) {
            parent->setUpdatesEnabled(true);
        }
    }

    int currentIndex() const
    {
        return m_currentIndex;
    }

    bool isEmpty() const
    {
        return m_widgets.empty();
    }

    int indexOf(QWidget *w) const
    {
        return m_widgets.indexOf(w);
    }

    QWidget *widget(int index) const
    {
        return (index < m_widgets.size()) ? m_widgets.at(index) : nullptr;
    }

    QRect geometry() const
    {
        return m_widgets.empty() ? QRect() : currentWidget()->geometry();
    }
};

class DockPanelPrivate
{
public:
    QX_DECLARE_PUBLIC(DockPanel)
public:
    DockPanelPrivate();
    ~DockPanelPrivate();
    void init();

    DockTabBar *tabBar() const;

public:
    DockWindow *m_window = nullptr;
    QBoxLayout *m_layout = nullptr;
    DockAreaLayout *m_contentsLayout = nullptr;
    DockTitleBar *m_titleBar = nullptr;
};

DockPanelPrivate::DockPanelPrivate()
{
}

DockPanelPrivate::~DockPanelPrivate()
{
    delete m_contentsLayout;
}

void DockPanelPrivate::init()
{
    Q_Q(DockPanel);

    m_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    q->setLayout(m_layout);

    m_titleBar = new DockTitleBar(q);
    QObject::connect(m_titleBar, &DockTitleBar::tabBarClicked, q, &DockPanel::setCurrentIndex);
    m_layout->addWidget(m_titleBar);

    m_contentsLayout = new DockAreaLayout(m_layout);
}

DockTabBar *DockPanelPrivate::tabBar() const
{
    return m_titleBar->tabBar();
}

DockPanel::DockPanel(DockWindow *window, DockContainer *parent)
    : QWidget{parent}
{
    QX_INIT_PRIVATE(DockPanel)

    Q_D(DockPanel);
    d->m_window = window;
    d->init();
}

DockPanel::~DockPanel()
{
    QX_FINI_PRIVATE()
}

DockWindow *DockPanel::dockWindow() const
{
    Q_D(const DockPanel);
    return d->m_window;
}

DockContainer *DockPanel::dockContainer() const
{
    // need include header of DockContainer
    return internal::findParent<DockContainer *>(this);
}

DockSplitter *DockPanel::parentSplitter() const
{
    // need include header of DockSplitter
    return internal::findParent<DockSplitter *>(this);
}

int DockPanel::dockWidgetsCount() const
{
    Q_D(const DockPanel);
    return d->m_contentsLayout->count();
}

QList<DockWidget *> DockPanel::dockWidgets() const
{
    Q_D(const DockPanel);
    QList<DockWidget *> dwList;
    for (int i = 0; i < d->m_contentsLayout->count(); ++i) {
        dwList.append(dockWidget(i));
    }
    return dwList;
}

QList<DockWidget *> DockPanel::openedDockWidgets() const
{
    Q_D(const DockPanel);
    QList<DockWidget *> dwList;
    for (int i = 0; i < d->m_contentsLayout->count(); ++i) {
        DockWidget *w = dockWidget(i);
        if (w && !w->isClosed()) {
            dwList.append(dockWidget(i));
        }
    }
    return dwList;
}

DockWidget *DockPanel::dockWidget(int index) const
{
    Q_D(const DockPanel);
    return qobject_cast<DockWidget *>(d->m_contentsLayout->widget(index));
}

int DockPanel::currentIndex() const
{
    Q_D(const DockPanel);
    return d->m_contentsLayout->currentIndex();
}

DockWidget *DockPanel::currentDockWidget() const
{
    int index = currentIndex();
    if (index < 0) {
        return nullptr;
    }
    return dockWidget(index);
}

void DockPanel::setCurrentDockWidget(DockWidget *w)
{
    internalSetCurrentDockWidget(w);
}

QAbstractButton *DockPanel::titleBarButton(Qx::DockTitleBarButton which) const
{
    Q_D(const DockPanel);
    return d->m_titleBar->button(which);
}

void DockPanel::setVisible(bool visible)
{
    QWidget::setVisible(visible);
}

bool DockPanel::isCentralWidgetArea() const
{
    if (dockWidgetsCount() != 1) {
        return false;
    }

    return dockWindow()->centralWidget() == dockWidgets().constFirst();
}

void DockPanel::setCurrentIndex(int index)
{
    Q_D(DockPanel);
    auto tabBar = d->tabBar();
    if (index < 0 || index > (tabBar->count() - 1)) {
        qWarning() << Q_FUNC_INFO << "Invalid index" << index;
        return;
    }

    auto cw = d->m_contentsLayout->currentWidget();
    auto nw = d->m_contentsLayout->widget(index);
    if (cw == nw && !nw->isHidden()) {
        return;
    }

    Q_EMIT currentChanging(index);
    tabBar->setCurrentIndex(index);
    d->m_contentsLayout->setCurrentIndex(index);
    d->m_contentsLayout->currentWidget()->show();
    Q_EMIT currentChanged(index);
}

void DockPanel::toggleView(bool open)
{
    setVisible(open);
    Q_EMIT viewToggled(open);
}

void DockPanel::addDockWidget(DockWidget *w)
{
    Q_D(DockPanel);
    insertDockWidget(d->m_contentsLayout->count(), w);
}

void DockPanel::insertDockWidget(int index, DockWidget *w, bool activate)
{
    Q_D(DockPanel);
    if (index < 0 || index > d->m_contentsLayout->count()) {
        index = d->m_contentsLayout->count();
    }
    d->m_contentsLayout->insertWidget(index, w);
    w->setDockPanel(this);
    auto tab = w->tab();
    tab->setDockPanel(this);
    d->tabBar()->blockSignals(true);
    d->tabBar()->insertTab(index, tab);
    d->tabBar()->blockSignals(false);
    tab->setVisible(!w->isClosed());
    if (activate) {
        setCurrentIndex(index);
        // Set current index can show the widget without changing the close state,
        // added to keep the close state consistent
        w->setClosedState(false);
    }
    // If this dock area is hidden, then we need to make it visible again
    // by calling DockWidget->toggleViewInternal(true);
    if (!this->isVisible() && d->m_contentsLayout->count() > 1) {
        w->toggleViewInternal(true);
    }
}

DockWidget *DockPanel::nextOpenDockWidget(DockWidget *w) const
{
    auto openWidgets = openedDockWidgets();

    if (openWidgets.count() > 1 || (openWidgets.count() == 1 && openWidgets[0] != w)) {
        if (openWidgets.last() == w) {
            DockWidget *nextDockWidget = openWidgets[openWidgets.count() - 2];
            // search backwards for widget with tab
            for (int i = openWidgets.count() - 2; i >= 0; --i) {
                auto dw = openWidgets[i];
                if (!dw->features().testFlag(DockWidget::NoTab)) {
                    return dw;
                }
            }
            // return widget without tab
            return nextDockWidget;
        } else {
            int indexOfDockWidget = openWidgets.indexOf(w);
            DockWidget *nextDockWidget = openWidgets[indexOfDockWidget + 1];
            // search forwards for widget with tab
            for (int i = indexOfDockWidget + 1; i < openWidgets.count(); ++i) {
                auto dw = openWidgets[i];
                if (!dw->features().testFlag(DockWidget::NoTab)) {
                    return dw;
                }
            }

            // search backwards for widget with tab
            for (int i = indexOfDockWidget - 1; i >= 0; --i) {
                auto dw = openWidgets[i];
                if (!dw->features().testFlag(DockWidget::NoTab)) {
                    return dw;
                }
            }

            // return widget without tab
            return nextDockWidget;
        }
    } else {
        return nullptr;
    }
}

void DockPanel::toggleDockWidgetView(DockWidget *w, bool open)
{

}

int DockPanel::indexOf(DockWidget *w) const
{
    Q_D(const DockPanel);
    return d->m_contentsLayout->indexOf(w);
}

void DockPanel::hideAreaWithNoVisibleContent()
{
    this->toggleView(false);

    // Hide empty parent splitters
    auto splitter = parentSplitter();
    internal::hideEmptyParentSplitters(splitter);
}

void DockPanel::internalSetCurrentDockWidget(DockWidget *w)
{
    int index = indexOf(w);
    if (index < 0) {
        return;
    }
    setCurrentIndex(index);
    // Set current index can show the widget without changing the close state, added to keep the close state consistent
    w->setClosedState(false);
}

QX_DOCK_END_NAMESPACE
