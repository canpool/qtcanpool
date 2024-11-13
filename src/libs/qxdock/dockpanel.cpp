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
#include "dockmanager.h"
#include "dockautohidecontainer.h"
#include "docktitlebar_p.h"
#include "docklabel.h"
#include "dockfloatingcontainer.h"

#include <QBoxLayout>

QX_DOCK_BEGIN_NAMESPACE

static const char *const INDEX_PROPERTY = "index";

static bool isAutoHideFeatureEnabled()
{
    return DockManager::testAutoHideConfigFlag(DockManager::AutoHideFeatureEnabled);
}

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
    DockAreaLayout(QBoxLayout *parentLayout)
        : m_parentLayout(parentLayout)
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

static const Qx::DockWidgetAreas s_defaultAllowedAreas = Qx::AllDockAreas;

class DockPanelPrivate
{
public:
    QX_DECLARE_PUBLIC(DockPanel)
public:
    DockPanelPrivate();
    ~DockPanelPrivate();
    void init();

    DockTabBar *tabBar() const;
    void updateTitleBarButtonStates();
    void updateTitleBarButtonVisibility(bool isTopLevel);
public:
    DockWindow *m_window = nullptr;
    QBoxLayout *m_layout = nullptr;
    DockAreaLayout *m_contentsLayout = nullptr;
    DockTitleBar *m_titleBar = nullptr;
    DockPanel::DockAreaFlags m_flags{DockPanel::DefaultFlags};
    bool m_updateTitleBarButtons = false;
    Qx::DockWidgetAreas m_allowedAreas = s_defaultAllowedAreas;
    DockAutoHideContainer *m_autoHideContainer = nullptr;
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
    QObject::connect(tabBar(), &DockTabBar::tabCloseRequested, q, &DockPanel::onTabCloseRequested);
    QObject::connect(tabBar(), &DockTabBar::tabMoved, q, &DockPanel::reorderDockWidget);
    m_layout->addWidget(m_titleBar);

    m_contentsLayout = new DockAreaLayout(m_layout);
}

DockTabBar *DockPanelPrivate::tabBar() const
{
    return m_titleBar->tabBar();
}

void DockPanelPrivate::updateTitleBarButtonStates()
{
    Q_Q(DockPanel);
    if (q->isHidden()) {
        m_updateTitleBarButtons = true;
        return;
    }

    m_titleBar->button(Qx::TitleBarButtonUndock)->setEnabled(q->features().testFlag(DockWidget::DockWidgetFloatable));
    m_titleBar->button(Qx::TitleBarButtonClose)->setEnabled(q->features().testFlag(DockWidget::DockWidgetClosable));
    m_titleBar->updateDockWidgetActionsButtons();
    m_updateTitleBarButtons = false;
}

void DockPanelPrivate::updateTitleBarButtonVisibility(bool isTopLevel)
{
    Q_Q(DockPanel);
    auto container = q->dockContainer();
    if (!container) {
        return;
    }
    if (isTopLevel) {
        m_titleBar->button(Qx::TitleBarButtonClose)->setVisible(!container->isFloating());
        // Undock and tabs should never show when auto hidden
        m_titleBar->button(Qx::TitleBarButtonUndock)->setVisible(!container->isFloating());
        m_titleBar->button(Qx::TitleBarButtonTabsMenu)->setVisible(true);
    } else {
        m_titleBar->button(Qx::TitleBarButtonClose)->setVisible(true);
        m_titleBar->button(Qx::TitleBarButtonUndock)->setVisible(true);
        m_titleBar->button(Qx::TitleBarButtonTabsMenu)->setVisible(true);
    }
}

DockPanel::DockPanel(DockWindow *window, DockContainer *parent)
    : QWidget{parent}
{
    QX_INIT_PRIVATE(DockPanel);

    Q_D(DockPanel);
    d->m_window = window;
    d->init();
}

DockPanel::~DockPanel()
{
    QX_FINI_PRIVATE();
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

QRect DockPanel::titleBarGeometry() const
{
    Q_D(const DockPanel);
    return d->m_titleBar->geometry();
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

int DockPanel::openDockWidgetsCount() const
{
    Q_D(const DockPanel);
    int cnt = 0;
    for (int i = 0; i < d->m_contentsLayout->count(); ++i) {
        if (dockWidget(i) && !dockWidget(i)->isClosed()) {
            ++cnt;
        }
    }
    return cnt;
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

DockWidget::DockWidgetFeatures DockPanel::features(Qx::DockBitwiseOperator mode) const
{
    if (mode == Qx::DockBitwiseAnd) {
        DockWidget::DockWidgetFeatures f(DockWidget::AllDockWidgetFeatures);
        for (const auto w : dockWidgets()) {
            f &= w->features();
        }
        return f;
    } else {
        DockWidget::DockWidgetFeatures f(DockWidget::NoDockWidgetFeatures);
        for (const auto w : dockWidgets()) {
            f |= w->features();
        }
        return f;
    }
}

QAbstractButton *DockPanel::titleBarButton(Qx::DockTitleBarButton which) const
{
    Q_D(const DockPanel);
    return d->m_titleBar->button(which);
}

void DockPanel::setVisible(bool visible)
{
    QWidget::setVisible(visible);
    Q_D(DockPanel);
    if (d->m_updateTitleBarButtons) {
        d->updateTitleBarButtonStates();
    }
}

Qx::DockWidgetAreas DockPanel::allowedAreas() const
{
    Q_D(const DockPanel);
    return d->m_allowedAreas;
}

void DockPanel::setAllowedAreas(Qx::DockWidgetAreas areas)
{
    Q_D(DockPanel);
    d->m_allowedAreas = areas;
}

DockTitleBar *DockPanel::titleBar() const
{
    Q_D(const DockPanel);
    return d->m_titleBar;
}

bool DockPanel::isCentralWidgetArea() const
{
    if (dockWidgetsCount() != 1) {
        return false;
    }

    return dockWindow()->centralWidget() == dockWidgets().constFirst();
}

bool DockPanel::isTopLevelArea() const
{
    auto container = dockContainer();
    if (!container) {
        return false;
    }

    return (container->topLevelDockPanel() == this);
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

void DockPanel::closeArea()
{
    auto openWidgets = openedDockWidgets();
    if (openWidgets.count() == 1 && (openWidgets[0]->features().testFlag(DockWidget::DockWidgetDeleteOnClose) ||
                                     openWidgets[0]->features().testFlag(DockWidget::CustomCloseHandling))) {
        openWidgets[0]->closeDockWidgetInternal();
    } else {
        for (auto w : openWidgets) {
            if ((w->features().testFlag(DockWidget::DockWidgetDeleteOnClose) &&
                 w->features().testFlag(DockWidget::DockWidgetForceCloseWithArea)) ||
                w->features().testFlag(DockWidget::CustomCloseHandling)) {
                w->closeDockWidgetInternal();
            } else {
                w->toggleView(false);
            }
        }
    }
}

void DockPanel::setAutoHide(bool enable, Qx::DockSideBarArea area, int tabIndex)
{
    Q_D(DockPanel);
    if (!isAutoHideFeatureEnabled()) {
        return;
    }

    if (!enable) {
        if (isAutoHide()) {
            d->m_autoHideContainer->moveContentsToParent();
        }
        return;
    }

    // If this is already an auto hide container, then move it to new location
    if (isAutoHide()) {
        d->m_autoHideContainer->moveToNewSideBarArea(area, tabIndex);
        return;
    }

    auto location = (Qx::DockSideBarNone == area) ? calculateSideBarArea() : area;
    for (const auto w : openedDockWidgets()) {
        if (enable == isAutoHide()) {
            continue;
        }

        if (!w->features().testFlag(DockWidget::DockWidgetPinnable)) {
            continue;
        }

        dockContainer()->createAndSetupAutoHideContainer(location, w, tabIndex++);
    }
}

void DockPanel::toggleAutoHide(Qx::DockSideBarArea area)
{
    if (!isAutoHideFeatureEnabled()) {
        return;
    }

    setAutoHide(!isAutoHide(), area);
}

void DockPanel::toggleView(bool open)
{
    setVisible(open);
    Q_EMIT viewToggled(open);
}

void DockPanel::onTabCloseRequested(int index)
{
    dockWidget(index)->requestCloseDockWidget();
}

void DockPanel::reorderDockWidget(int fromIndex, int toIndex)
{
    Q_D(DockPanel);
    if (fromIndex >= d->m_contentsLayout->count() || fromIndex < 0 || toIndex >= d->m_contentsLayout->count() ||
        toIndex < 0 || fromIndex == toIndex) {
        return;
    }

    auto widget = d->m_contentsLayout->widget(fromIndex);
    d->m_contentsLayout->removeWidget(widget);
    d->m_contentsLayout->insertWidget(toIndex, widget);
    setCurrentIndex(toIndex);
}

void DockPanel::updateAutoHideButtonCheckState()
{
    auto autoHideButton = titleBarButton(Qx::TitleBarButtonAutoHide);
    autoHideButton->blockSignals(true);
    autoHideButton->setChecked(isAutoHide());
    autoHideButton->blockSignals(false);
}

void DockPanel::updateTitleBarButtonsToolTips()
{
    internal::setToolTip(titleBarButton(Qx::TitleBarButtonClose),
                         titleBar()->titleBarButtonToolTip(Qx::TitleBarButtonClose));
    internal::setToolTip(titleBarButton(Qx::TitleBarButtonAutoHide),
                         titleBar()->titleBarButtonToolTip(Qx::TitleBarButtonAutoHide));
}

enum BorderLocation {
    BorderNone = 0,
    BorderLeft = 0x01,
    BorderRight = 0x02,
    BorderTop = 0x04,
    BorderBottom = 0x08,
    BorderVertical = BorderLeft | BorderRight,
    BorderHorizontal = BorderTop | BorderBottom,
    BorderTopLeft = BorderTop | BorderLeft,
    BorderTopRight = BorderTop | BorderRight,
    BorderBottomLeft = BorderBottom | BorderLeft,
    BorderBottomRight = BorderBottom | BorderRight,
    BorderVerticalBottom = BorderVertical | BorderBottom,
    BorderVerticalTop = BorderVertical | BorderTop,
    BorderHorizontalLeft = BorderHorizontal | BorderLeft,
    BorderHorizontalRight = BorderHorizontal | BorderRight,
    BorderAll = BorderVertical | BorderHorizontal
};

Qx::DockSideBarArea DockPanel::calculateSideBarArea() const
{
    auto container = dockContainer();
    auto contentRect = container->contentRect();

    int borders = BorderNone;   // contains all borders that are touched by the dock ware
    auto dockAreaTopLeft = mapTo(container, rect().topLeft());
    auto dockAreaRect = rect();
    dockAreaRect.moveTo(dockAreaTopLeft);
    const qreal aspectRatio = dockAreaRect.width() / (qMax(1, dockAreaRect.height()) * 1.0);
    const qreal sizeRatio = (qreal)contentRect.width() / dockAreaRect.width();
    static const int minBorderDistance = 16;
    bool horizontalOrientation = (aspectRatio > 1.0) && (sizeRatio < 3.0);

    // measure border distances - a distance less than 16 px means we touch the border
    int borderDistance[4];

    int distance = qAbs(contentRect.topLeft().y() - dockAreaRect.topLeft().y());
    borderDistance[Qx::DockSideBarTop] = (distance < minBorderDistance) ? 0 : distance;
    if (!borderDistance[Qx::DockSideBarTop]) {
        borders |= BorderTop;
    }

    distance = qAbs(contentRect.bottomRight().y() - dockAreaRect.bottomRight().y());
    borderDistance[Qx::DockSideBarBottom] = (distance < minBorderDistance) ? 0 : distance;
    if (!borderDistance[Qx::DockSideBarBottom]) {
        borders |= BorderBottom;
    }

    distance = qAbs(contentRect.topLeft().x() - dockAreaRect.topLeft().x());
    borderDistance[Qx::DockSideBarLeft] = (distance < minBorderDistance) ? 0 : distance;
    if (!borderDistance[Qx::DockSideBarLeft]) {
        borders |= BorderLeft;
    }

    distance = qAbs(contentRect.bottomRight().x() - dockAreaRect.bottomRight().x());
    borderDistance[Qx::DockSideBarRight] = (distance < minBorderDistance) ? 0 : distance;
    if (!borderDistance[Qx::DockSideBarRight]) {
        borders |= BorderRight;
    }

    auto sideBarArea = Qx::DockSideBarRight;
    switch (borders) {
    // 1. It's touching all borders
    case BorderAll:
        sideBarArea = horizontalOrientation ? Qx::DockSideBarBottom : Qx::DockSideBarRight;
        break;

    // 2. It's touching 3 borders
    case BorderVerticalBottom:
        sideBarArea = Qx::DockSideBarBottom;
        break;
    case BorderVerticalTop:
        sideBarArea = Qx::DockSideBarTop;
        break;
    case BorderHorizontalLeft:
        sideBarArea = Qx::DockSideBarLeft;
        break;
    case BorderHorizontalRight:
        sideBarArea = Qx::DockSideBarRight;
        break;

    // 3. It's touching horizontal or vertical borders
    case BorderVertical:
        sideBarArea = Qx::DockSideBarBottom;
        break;
    case BorderHorizontal:
        sideBarArea = Qx::DockSideBarRight;
        break;

    // 4. It's in a corner
    case BorderTopLeft:
        sideBarArea = horizontalOrientation ? Qx::DockSideBarTop : Qx::DockSideBarLeft;
        break;
    case BorderTopRight:
        sideBarArea = horizontalOrientation ? Qx::DockSideBarTop : Qx::DockSideBarRight;
        break;
    case BorderBottomLeft:
        sideBarArea = horizontalOrientation ? Qx::DockSideBarBottom : Qx::DockSideBarLeft;
        break;
    case BorderBottomRight:
        sideBarArea = horizontalOrientation ? Qx::DockSideBarBottom : Qx::DockSideBarRight;
        break;

    // 5. It's touching only one border
    case BorderLeft:
        sideBarArea = Qx::DockSideBarLeft;
        break;
    case BorderRight:
        sideBarArea = Qx::DockSideBarRight;
        break;
    case BorderTop:
        sideBarArea = Qx::DockSideBarTop;
        break;
    case BorderBottom:
        sideBarArea = Qx::DockSideBarBottom;
        break;
    }

    return sideBarArea;
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
    d->m_titleBar->autoHideTitleLabel()->setText(w->windowTitle());
    w->setProperty(INDEX_PROPERTY, index);
    if (activate) {
        setCurrentIndex(index);
        // Set current index can show the widget without changing the close state,
        // added to keep the close state consistent
        w->setClosedState(false);
    }
    // If this dock area is hidden, then we need to make it visible again
    // by calling DockWidget->toggleViewInternal(true);
    if (!this->isVisible() && d->m_contentsLayout->count() > 0) {
        // Notes: If d->m_contentsLayout->count() > 1 is required, it will appear
        // that the toggleViewAction is not checked when there is only one dock widget
        w->toggleViewInternal(true);
    }
    d->updateTitleBarButtonStates();
    updateTitleBarVisibility();
}

void DockPanel::removeDockWidget(DockWidget *w)
{
    if (!w) {
        return;
    }
    // If this dock panel is in a auto hide container, then we can delete
    // the auto hide container now
    if (isAutoHide()) {
        autoHideContainer()->cleanupAndDelete();
        return;
    }

    auto currentWidget = currentDockWidget();
    auto nextOpenWidget = (w == currentWidget) ? nextOpenDockWidget(w) : nullptr;

    Q_D(DockPanel);
    d->m_contentsLayout->removeWidget(w);
    auto tab = w->tab();
    tab->hide();
    d->tabBar()->removeTab(tab);
    tab->setParent(w);
    w->setDockPanel(nullptr);

    DockContainer *container = dockContainer();
    if (nextOpenWidget) {
        setCurrentDockWidget(nextOpenWidget);
    } else if (d->m_contentsLayout->isEmpty() && container->dockPanelCount() >= 1) {
        container->removeDockPanel(this);
        this->deleteLater();
        if (container->dockPanelCount() == 0) {
            if (DockFloatingContainer *floatingContainer = container->floatingWidget()) {
                floatingContainer->hide();
                floatingContainer->deleteLater();
            }
        }
    } else if (w == currentWidget) {
        hideAreaWithNoVisibleContent();
    }
    d->updateTitleBarButtonStates();
    updateTitleBarVisibility();
    auto topLevelWidget = container->topLevelDockWidget();
    if (topLevelWidget) {
        topLevelWidget->emitTopLevelChanged(true);
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
    Q_UNUSED(w);
    Q_UNUSED(open);
    updateTitleBarVisibility();
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

    // Hide empty floating widget
    DockContainer *container = this->dockContainer();
    if (!container->isFloating() && !DockManager::testConfigFlag(DockManager::HideSingleCentralWidgetTitleBar)) {
        return;
    }

    updateTitleBarVisibility();
    auto topLevelWidget = container->topLevelDockWidget();
    auto floatingWidget = container->floatingWidget();
    if (topLevelWidget) {
        if (floatingWidget) {
            floatingWidget->updateWindowTitle();
        }
        DockWidget::emitTopLevelEventForWidget(topLevelWidget, true);
    } else if (container->openedDockPanels().isEmpty() && floatingWidget) {
        floatingWidget->hide();
    }
    if (isAutoHide()) {
        autoHideContainer()->hide();
    }
}

void DockPanel::updateTitleBarVisibility()
{
    Q_D(DockPanel);
    DockContainer *container = dockContainer();
    if (!container) {
        return;
    }
    if (!d->m_titleBar) {
        return;
    }
    if (!DockManager::testConfigFlag(DockManager::AlwaysShowTabs)) {
        bool hidden = container->hasTopLevelDockWidget() && container->isFloating();
        hidden |= (d->m_flags.testFlag(HideSingleWidgetTitleBar) && openDockWidgetsCount() == 1);
        d->m_titleBar->setVisible(!hidden);
    }
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

void DockPanel::updateTitleBarButtonVisibility(bool isTopLevel)
{
    Q_D(DockPanel);
    d->updateTitleBarButtonVisibility(isTopLevel);
}

DockAutoHideContainer *DockPanel::autoHideContainer() const
{
    Q_D(const DockPanel);
    return d->m_autoHideContainer;
}

void DockPanel::setAutoHideContainer(DockAutoHideContainer *container)
{
    Q_D(DockPanel);
    d->m_autoHideContainer = container;
    updateAutoHideButtonCheckState();
    updateTitleBarButtonsToolTips();

    auto button = d->m_titleBar->button(Qx::TitleBarButtonAutoHide);
    TitleBarButton *autoHideButton = qobject_cast<TitleBarButton *>(button);
    autoHideButton->setShowInTitleBar(true);
}

bool DockPanel::isAutoHide() const
{
    Q_D(const DockPanel);
    return d->m_autoHideContainer != nullptr;
}

QX_DOCK_END_NAMESPACE
