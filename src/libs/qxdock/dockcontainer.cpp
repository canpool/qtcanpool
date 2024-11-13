/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockcontainer.h"
#include "dockwindow.h"
#include "docksplitter.h"
#include "dockpanel.h"
#include "dockutils.h"
#include "dockmanager.h"
#include "dockfloatingcontainer.h"
#include "dockoverlay.h"
#include "dockautohidecontainer.h"
#include "docksidebar.h"
#include "docksidetab.h"

#include <QGridLayout>
#include <QTimer>

QX_DOCK_BEGIN_NAMESPACE

static void insertWidgetIntoSplitter(QSplitter *s, QWidget *w, bool append)
{
    if (append) {
        s->addWidget(w);
    } else {
        s->insertWidget(0, w);
    }
}

static int areaIdToIndex(Qx::DockWidgetArea area)
{
    switch (area) {
    case Qx::LeftDockWidgetArea:
        return 0;
    case Qx::RightDockWidgetArea:
        return 1;
    case Qx::TopDockWidgetArea:
        return 2;
    case Qx::BottomDockWidgetArea:
        return 3;
    case Qx::CenterDockWidgetArea:
        return 4;
    default:
        return 4;
    }
}

class DockContainerPrivate
{
public:
    QX_DECLARE_PUBLIC(DockContainer)
public:
    DockContainerPrivate();

    void init();
    void createRootSplitter();

    void onVisibleDockAreaCountChanged();
    void emitDockAreasRemoved();
    void emitDockAreasAdded();

    DockSplitter *newSplitter(Qt::Orientation orientation, QWidget *parent = nullptr);
    void updateSplitterHandles(QSplitter *s);
    void adjustSplitterSizesOnInsertion(QSplitter *s, qreal lastRatio = 1.0);

    bool widgetResizesWithContainer(QWidget *w);

    DockPanel *addDockWidgetToContainer(Qx::DockWidgetArea area, DockWidget *w);
    DockPanel *addDockWidgetToPanel(Qx::DockWidgetArea area, DockWidget *w, DockPanel *targetPanel, int index = -1);

    void addDockPanel(DockPanel *np, Qx::DockWidgetArea area = Qx::CenterDockWidgetArea);
    void addDockPanelsToList(const QList<DockPanel *> ps);
    void appendPanels(const QList<DockPanel *> ps);

    void dropIntoContainer(DockFloatingContainer *floatingWidget, Qx::DockWidgetArea area);
    void dropIntoCenterOfPanel(DockFloatingContainer *floatingWidget, DockPanel *targetPanel, int tabIndex = 0);
    void dropIntoPanel(DockFloatingContainer *floatingWidget, DockPanel *targetPanel, Qx::DockWidgetArea area,
                       int tabIndex = 0);

    void moveToContainer(QWidget *widget, Qx::DockWidgetArea area);
    void moveIntoCenterOfSection(QWidget *widget, DockPanel *targetPanel, int tabIndex = 0);
    void moveToNewSection(QWidget *widget, DockPanel *targetPanel, Qx::DockWidgetArea area, int tabIndex = 0);

    void dropIntoAutoHideSideBar(DockFloatingContainer *floatingWidget, Qx::DockWidgetArea area);
public:
    DockWindow *m_window = nullptr;
    QGridLayout *m_layout = nullptr;
    DockSplitter *m_rootSplitter = nullptr;
    QList<QPointer<DockPanel>> m_panels;
    DockPanel *m_lastAddedPanelCache[5];
    DockPanel *m_topLevelPanel = nullptr;
    bool m_isFloating = false;
    unsigned int m_zOrderIndex = 0;
    QMap<Qx::DockSideBarArea, DockSideBar *> m_sideTabBarWidgets;
    QList<DockAutoHideContainer *> m_autoHideWidgets;
    DockSideTab *m_delayedAutoHideTab;
    QTimer m_delayedAutoHideTimer;
    bool m_delayedAutoHideShow = false;
};

DockContainerPrivate::DockContainerPrivate()
{
    std::fill(std::begin(m_lastAddedPanelCache), std::end(m_lastAddedPanelCache), nullptr);
    m_delayedAutoHideTimer.setSingleShot(true);
    m_delayedAutoHideTimer.setInterval(500);
    QObject::connect(&m_delayedAutoHideTimer, &QTimer::timeout, [this]() {
        auto GlobalPos = m_delayedAutoHideTab->mapToGlobal(QPoint(0, 0));
        qApp->sendEvent(m_delayedAutoHideTab, new QMouseEvent(QEvent::MouseButtonPress, QPoint(0, 0), GlobalPos,
                                                              Qt::LeftButton, {Qt::LeftButton}, Qt::NoModifier));
    });
}

void DockContainerPrivate::init()
{
    Q_Q(DockContainer);

    m_isFloating = q->floatingWidget() != nullptr;

    m_layout = new QGridLayout();
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->setColumnStretch(1, 1);
    m_layout->setRowStretch(1, 1);
    q->setLayout(m_layout);
}

void DockContainerPrivate::createRootSplitter()
{
    if (m_rootSplitter) {
        return;
    }
    m_rootSplitter = newSplitter(Qt::Horizontal);
    // Add it to the center - the 0 and 2 indexes are used for the SideTabBar widgets
    m_layout->addWidget(m_rootSplitter, 1, 1);
}

void DockContainerPrivate::onVisibleDockAreaCountChanged()
{
    Q_Q(DockContainer);
    auto panel = q->topLevelDockPanel();

    if (panel) {
        this->m_topLevelPanel = panel;
        panel->updateTitleBarButtonVisibility(true);
    } else if (this->m_topLevelPanel) {
        this->m_topLevelPanel->updateTitleBarButtonVisibility(false);
        this->m_topLevelPanel = nullptr;
    }
}

void DockContainerPrivate::emitDockAreasRemoved()
{
    Q_Q(DockContainer);
    onVisibleDockAreaCountChanged();
    Q_EMIT q->dockAreasRemoved();
}

void DockContainerPrivate::emitDockAreasAdded()
{
    Q_Q(DockContainer);
    onVisibleDockAreaCountChanged();
    Q_EMIT q->dockAreasAdded();
}

DockSplitter *DockContainerPrivate::newSplitter(Qt::Orientation orientation, QWidget *parent)
{
    DockSplitter *s = new DockSplitter(orientation, parent);
    s->setOpaqueResize(DockManager::testConfigFlag(DockManager::OpaqueSplitterResize));
    s->setChildrenCollapsible(false);
    return s;
}

void DockContainerPrivate::updateSplitterHandles(QSplitter *s)
{
    if (!m_window->centralWidget() || !s) {
        return;
    }

    for (int i = 0; i < s->count(); ++i) {
        s->setStretchFactor(i, widgetResizesWithContainer(s->widget(i)) ? 1 : 0);
    }
}

void DockContainerPrivate::adjustSplitterSizesOnInsertion(QSplitter *s, qreal lastRatio)
{
    int areaSize = (s->orientation() == Qt::Horizontal) ? s->width() : s->height();
    auto splitterSizes = s->sizes();

    qreal totRatio = splitterSizes.size() - 1.0 + lastRatio;
    for (int i = 0; i < splitterSizes.size() - 1; i++) {
        splitterSizes[i] = areaSize / totRatio;
    }
    splitterSizes.back() = areaSize * lastRatio / totRatio;
    s->setSizes(splitterSizes);
}

bool DockContainerPrivate::widgetResizesWithContainer(QWidget *w)
{
    if (!m_window->centralWidget()) {
        return true;
    }

    auto panel = qobject_cast<DockPanel *>(w);
    if (panel) {
        return panel->isCentralWidgetArea();
    }

    auto innerSplitter = qobject_cast<DockSplitter *>(w);
    if (innerSplitter) {
        return innerSplitter->isResizingWithContainer();
    }

    return false;
}

DockPanel *DockContainerPrivate::addDockWidgetToContainer(Qx::DockWidgetArea area, DockWidget *w)
{
    Q_Q(DockContainer);
    DockPanel *panel = new DockPanel(m_window, q);
    panel->addDockWidget(w);
    addDockPanel(panel, area);
    panel->updateTitleBarVisibility();
    m_lastAddedPanelCache[areaIdToIndex(area)] = panel;
    return panel;
}

DockPanel *DockContainerPrivate::addDockWidgetToPanel(Qx::DockWidgetArea area, DockWidget *w, DockPanel *targetPanel,
                                                      int index)
{
    Q_Q(DockContainer);
    if (area == Qx::CenterDockWidgetArea) {
        targetPanel->insertDockWidget(index, w);
        targetPanel->updateTitleBarVisibility();
        return targetPanel;
    }
    DockPanel *np = new DockPanel(m_window, q);
    np->addDockWidget(w);

    auto param = internal::dockAreaInsertParameters(area);
    auto targetSplitter = targetPanel->parentSplitter();
    // use target index instead of parameter index
    index = targetSplitter->indexOf(targetPanel);
    if (targetSplitter->orientation() == param.orientation()) {
        targetSplitter->insertWidget(index + param.insertOffset(), np);
        updateSplitterHandles(targetSplitter);
        if (DockManager::testConfigFlag(DockManager::EqualSplitOnInsertion)) {
            adjustSplitterSizesOnInsertion(targetSplitter);
        }
    } else {
        auto sizes = targetSplitter->sizes();
        auto ns = newSplitter(param.orientation());
        ns->addWidget(targetPanel);
        insertWidgetIntoSplitter(ns, np, param.append());
        updateSplitterHandles(ns);
        targetSplitter->insertWidget(index, ns);
        updateSplitterHandles(targetSplitter);
        if (DockManager::testConfigFlag(DockManager::EqualSplitOnInsertion)) {
            targetSplitter->setSizes(sizes);
            adjustSplitterSizesOnInsertion(ns);
        }
    }
    addDockPanelsToList({np});
    return np;
}

void DockContainerPrivate::addDockPanel(DockPanel *np, Qx::DockWidgetArea area)
{
    auto param = internal::dockAreaInsertParameters(area);
    if (m_panels.count() <= 1) {
        m_rootSplitter->setOrientation(param.orientation());
    }
    QSplitter *s = m_rootSplitter;
    if (s->orientation() == param.orientation()) {
        insertWidgetIntoSplitter(s, np, param.append());
        updateSplitterHandles(s);
        if (s->isHidden()) {
            s->show();
        }
    } else {
        auto ns = newSplitter(param.orientation());
        if (param.append()) {
            QLayoutItem *li = m_layout->replaceWidget(s, ns);
            ns->addWidget(s);
            ns->addWidget(np);
            updateSplitterHandles(ns);
            delete li;
        } else {
            ns->addWidget(np);
            QLayoutItem *li = m_layout->replaceWidget(s, ns);
            ns->addWidget(s);
            updateSplitterHandles(ns);
            delete li;
        }
        m_rootSplitter = ns;
    }
    addDockPanelsToList({np});
}

void DockContainerPrivate::addDockPanelsToList(const QList<DockPanel *> ps)
{
    int beforeCount = m_panels.count();
    int newCount = ps.count();
    appendPanels(ps);
    for (auto p : ps) {
        p->titleBarButton(Qx::TitleBarButtonClose)->setVisible(true);
        p->titleBarButton(Qx::TitleBarButtonAutoHide)->setVisible(true);
    }
    if (beforeCount == 1) {
        m_panels[0]->updateTitleBarVisibility();
    }
    if (newCount == 1) {
        m_panels[0]->updateTitleBarVisibility();
    }
    emitDockAreasAdded();
}

void DockContainerPrivate::appendPanels(const QList<DockPanel *> ps)
{
    for (auto *p : ps) {
        m_panels.append(p);
    }
}

void DockContainerPrivate::dropIntoContainer(DockFloatingContainer *floatingWidget, Qx::DockWidgetArea area)
{
    auto param = internal::dockAreaInsertParameters(area);
    DockContainer *floatingDockContainer = floatingWidget->dockContainer();
    auto newPanels = floatingDockContainer->findChildren<DockPanel *>(QString(), Qt::FindChildrenRecursively);
    auto splitter = m_rootSplitter;

    if (m_panels.count() <= 1) {
        splitter->setOrientation(param.orientation());
    } else if (splitter->orientation() != param.orientation()) {
        auto newSplitter = this->newSplitter(param.orientation());
        QLayoutItem *li = m_layout->replaceWidget(splitter, newSplitter);
        newSplitter->addWidget(splitter);
        updateSplitterHandles(newSplitter);
        splitter = newSplitter;
        delete li;
    }

    // Now we can insert the floating widget content into this container
    auto floatingSplitter = floatingDockContainer->rootSplitter();
    if (floatingSplitter->count() == 1) {
        insertWidgetIntoSplitter(splitter, floatingSplitter->widget(0), param.append());
        updateSplitterHandles(splitter);
    } else if (floatingSplitter->orientation() == param.orientation()) {
        int insertIndex = param.append() ? splitter->count() : 0;
        while (floatingSplitter->count()) {
            splitter->insertWidget(insertIndex++, floatingSplitter->widget(0));
            updateSplitterHandles(splitter);
        }
    } else {
        insertWidgetIntoSplitter(splitter, floatingSplitter, param.append());
    }

    m_rootSplitter = splitter;
    addDockPanelsToList(newPanels);

    // If we dropped the floating widget into the main dock container that does
    // not contain any dock widgets, then splitter is invisible and we need to
    // show it to display the docked widgets
    if (!splitter->isVisible()) {
        splitter->show();
    }
}

void DockContainerPrivate::dropIntoCenterOfPanel(DockFloatingContainer *floatingWidget, DockPanel *targetPanel,
                                                 int tabIndex)
{
    DockContainer *floatingContainer = floatingWidget->dockContainer();
    auto newDockWidgets = floatingContainer->dockWidgets();
    auto topLevelPanel = floatingContainer->topLevelDockPanel();
    int newCurrentIndex = -1;
    tabIndex = qMax(0, tabIndex);

    // If the floating widget contains only one single dock are, then the
    // current dock widget of the dock area will also be the future current
    // dock widget in the drop area.
    if (topLevelPanel) {
        newCurrentIndex = topLevelPanel->currentIndex();
    }

    for (int i = 0; i < newDockWidgets.count(); ++i) {
        DockWidget *dockWidget = newDockWidgets[i];
        targetPanel->insertDockWidget(tabIndex + i, dockWidget, false);
        // If the floating widget contains multiple visible dock areas, then we
        // simply pick the first visible open dock widget and make it
        // the current one.
        if (newCurrentIndex < 0 && !dockWidget->isClosed()) {
            newCurrentIndex = i;
        }
    }
    targetPanel->setCurrentIndex(newCurrentIndex + tabIndex);
    targetPanel->updateTitleBarVisibility();
    return;
}

void DockContainerPrivate::dropIntoPanel(DockFloatingContainer *floatingWidget, DockPanel *targetPanel,
                                         Qx::DockWidgetArea area, int tabIndex)
{
    // Dropping into center means all dock widgets in the dropped floating
    // widget will become tabs of the drop area
    if (Qx::CenterDockWidgetArea == area) {
        dropIntoCenterOfPanel(floatingWidget, targetPanel, tabIndex);
        return;
    }

    DockContainer *floatingContainer = floatingWidget->dockContainer();
    auto param = internal::dockAreaInsertParameters(area);
    auto newDockpanels = floatingContainer->findChildren<DockPanel *>(QString(), Qt::FindChildrenRecursively);
    auto targetSplitter = targetPanel->parentSplitter();
    int areaIndex = targetSplitter->indexOf(targetPanel);
    auto floatingSplitter = floatingContainer->rootSplitter();
    if (targetSplitter->orientation() == param.orientation()) {
        auto sizes = targetSplitter->sizes();
        int targetAreaSize = (param.orientation() == Qt::Horizontal) ? targetPanel->width() : targetPanel->height();
        bool adjustSplitterSizes = true;
        if ((floatingSplitter->orientation() != param.orientation()) && floatingSplitter->count() > 1) {
            targetSplitter->insertWidget(areaIndex + param.insertOffset(), floatingSplitter);
            updateSplitterHandles(targetSplitter);
        } else {
            adjustSplitterSizes = (floatingSplitter->count() == 1);
            int insertIndex = areaIndex + param.insertOffset();
            while (floatingSplitter->count()) {
                targetSplitter->insertWidget(insertIndex++, floatingSplitter->widget(0));
                updateSplitterHandles(targetSplitter);
            }
        }

        if (adjustSplitterSizes) {
            int size = (targetAreaSize - targetSplitter->handleWidth()) / 2;
            sizes[areaIndex] = size;
            sizes.insert(areaIndex, size);
            targetSplitter->setSizes(sizes);
        }
    } else {
        QSplitter *newSplitter = this->newSplitter(param.orientation());
        int targetAreaSize = (param.orientation() == Qt::Horizontal) ? targetPanel->width() : targetPanel->height();
        bool adjustSplitterSizes = true;
        if ((floatingSplitter->orientation() != param.orientation()) && floatingSplitter->count() > 1) {
            newSplitter->addWidget(floatingSplitter);
            updateSplitterHandles(newSplitter);
        } else {
            adjustSplitterSizes = (floatingSplitter->count() == 1);
            while (floatingSplitter->count()) {
                newSplitter->addWidget(floatingSplitter->widget(0));
                updateSplitterHandles(newSplitter);
            }
        }

        // Save the sizes before insertion and restore it later to prevent
        // shrinking of existing area
        auto sizes = targetSplitter->sizes();
        insertWidgetIntoSplitter(newSplitter, targetPanel, !param.append());
        updateSplitterHandles(newSplitter);
        if (adjustSplitterSizes) {
            int size = targetAreaSize / 2;
            newSplitter->setSizes({size, size});
        }
        targetSplitter->insertWidget(areaIndex, newSplitter);
        targetSplitter->setSizes(sizes);
        updateSplitterHandles(targetSplitter);
    }

    addDockPanelsToList(newDockpanels);
}

void DockContainerPrivate::moveToContainer(QWidget *widget, Qx::DockWidgetArea area)
{
    Q_Q(DockContainer);
    DockWidget *droppedDockWidget = qobject_cast<DockWidget *>(widget);
    DockPanel *droppedPanel = qobject_cast<DockPanel *>(widget);
    DockPanel *newPanel;

    if (droppedDockWidget) {
        newPanel = new DockPanel(m_window, q);
        DockPanel *oldPanel = droppedDockWidget->dockPanel();
        if (oldPanel) {
            oldPanel->removeDockWidget(droppedDockWidget);
        }
        newPanel->addDockWidget(droppedDockWidget);
    } else {
        // We check, if we insert the dropped widget into the same place that
        // it already has and do nothing, if it is the same place. It would
        // also work without this check, but it looks nicer with the check
        // because there will be no layout updates
        auto splitter = droppedPanel->parentSplitter();
        auto insertParam = internal::dockAreaInsertParameters(area);
        if (splitter == m_rootSplitter && insertParam.orientation() == splitter->orientation()) {
            if (insertParam.append() && splitter->lastWidget() == droppedPanel) {
                return;
            } else if (!insertParam.append() && splitter->firstWidget() == droppedPanel) {
                return;
            }
        }
        droppedPanel->dockContainer()->removeDockPanel(droppedPanel);
        newPanel = droppedPanel;
    }

    addDockPanel(newPanel, area);
    m_lastAddedPanelCache[areaIdToIndex(area)] = newPanel;
}

void DockContainerPrivate::moveIntoCenterOfSection(QWidget *widget, DockPanel *targetPanel, int tabIndex)
{
    auto droppedDockWidget = qobject_cast<DockWidget *>(widget);
    auto droppedPanel = qobject_cast<DockPanel *>(widget);

    tabIndex = qMax(0, tabIndex);
    if (droppedDockWidget) {
        DockPanel *oldPanel = droppedDockWidget->dockPanel();
        if (oldPanel == targetPanel) {
            return;
        }

        if (oldPanel) {
            oldPanel->removeDockWidget(droppedDockWidget);
        }
        targetPanel->insertDockWidget(tabIndex, droppedDockWidget, true);
    } else {
        QList<DockWidget *> newDockWidgets = droppedPanel->dockWidgets();
        int newCurrentIndex = droppedPanel->currentIndex();
        for (int i = 0; i < newDockWidgets.count(); ++i) {
            DockWidget *w = newDockWidgets[i];
            targetPanel->insertDockWidget(tabIndex + i, w, false);
        }
        targetPanel->setCurrentIndex(tabIndex + newCurrentIndex);
        droppedPanel->dockContainer()->removeDockPanel(droppedPanel);
        droppedPanel->deleteLater();
    }

    targetPanel->updateTitleBarVisibility();
    return;
}

void DockContainerPrivate::moveToNewSection(QWidget *widget, DockPanel *targetPanel, Qx::DockWidgetArea area,
                                            int tabIndex)
{
    Q_Q(DockContainer);
    // Dropping into center means all dock widgets in the dropped floating
    // widget will become tabs of the drop area
    if (Qx::CenterDockWidgetArea == area) {
        moveIntoCenterOfSection(widget, targetPanel, tabIndex);
        return;
    }

    DockWidget *droppedDockWidget = qobject_cast<DockWidget *>(widget);
    DockPanel *droppedPanel = qobject_cast<DockPanel *>(widget);
    DockPanel *newPanel;
    if (droppedDockWidget) {
        newPanel = new DockPanel(m_window, q);
        DockPanel *oldPanel = droppedDockWidget->dockPanel();
        if (oldPanel) {
            oldPanel->removeDockWidget(droppedDockWidget);
        }
        newPanel->addDockWidget(droppedDockWidget);
    } else {
        droppedPanel->dockContainer()->removeDockPanel(droppedPanel);
        newPanel = droppedPanel;
    }

    auto insertParam = internal::dockAreaInsertParameters(area);
    auto targetSplitter = targetPanel->parentSplitter();
    int panelIndex = targetSplitter->indexOf(targetPanel);
    auto sizes = targetSplitter->sizes();
    if (targetSplitter->orientation() == insertParam.orientation()) {
        int targetPanelSize =
            (insertParam.orientation() == Qt::Horizontal) ? targetPanel->width() : targetPanel->height();
        targetSplitter->insertWidget(panelIndex + insertParam.insertOffset(), newPanel);
        updateSplitterHandles(targetSplitter);
        int size = (targetPanelSize - targetSplitter->handleWidth()) / 2;
        sizes[panelIndex] = size;
        sizes.insert(panelIndex, size);
    } else {
        int targetPanelSize =
            (insertParam.orientation() == Qt::Horizontal) ? targetPanel->width() : targetPanel->height();
        QSplitter *newSplitter = this->newSplitter(insertParam.orientation());
        newSplitter->addWidget(targetPanel);
        insertWidgetIntoSplitter(newSplitter, newPanel, insertParam.append());
        updateSplitterHandles(newSplitter);
        int size = targetPanelSize / 2;
        newSplitter->setSizes({size, size});
        targetSplitter->insertWidget(panelIndex, newSplitter);
        updateSplitterHandles(targetSplitter);
    }
    targetSplitter->setSizes(sizes);

    addDockPanelsToList({newPanel});
}

void DockContainerPrivate::dropIntoAutoHideSideBar(DockFloatingContainer *floatingWidget, Qx::DockWidgetArea area)
{
    Q_Q(DockContainer);
    auto sideBarArea = internal::toSideBarArea(area);
    auto newPanels = floatingWidget->findChildren<DockPanel *>(QString(), Qt::FindChildrenRecursively);
    int tabIndex = m_window->containerOverlay()->tabIndexUnderCursor();
    for (auto panel : newPanels) {
        auto widgets = panel->dockWidgets();
        for (auto w : widgets) {
            q->createAndSetupAutoHideContainer(sideBarArea, w, tabIndex++);
        }
    }
}

/* DockContainer */
DockContainer::DockContainer(DockWindow *window, QWidget *parent)
    : QWidget(parent)
{
    Q_ASSERT(window);
    QX_INIT_PRIVATE(DockContainer);

    Q_D(DockContainer);
    d->m_window = window;
    d->init();

    if (window != this) {
        d->m_window->registerDockContainer(this);
        createRootSplitter();
        createSideTabBarWidgets();
    }
}

DockContainer::~DockContainer()
{
    Q_D(DockContainer);
    if (d->m_window) {
        d->m_window->removeDockContainer(this);
    }
    QX_FINI_PRIVATE();
}

DockPanel *DockContainer::addDockWidget(Qx::DockWidgetArea area, DockWidget *w, DockPanel *p, int index)
{
    Q_D(DockContainer);
    auto topLevelWidget = topLevelDockWidget();
    DockPanel *oldPanel = w->dockPanel();
    if (oldPanel) {
        oldPanel->removeDockWidget(w);
    }
    w->setDockWindow(d->m_window);

    DockPanel *panel = nullptr;
    if (p) {
        panel = d->addDockWidgetToPanel(area, w, p, index);
    } else {
        panel = d->addDockWidgetToContainer(area, w);
    }
    if (topLevelWidget) {
        auto newTopLevelWidget = topLevelDockWidget();
        // If the container contained only one visible dock widget, the we need
        // to emit a top level event for this widget because it is not the one and
        // only visible docked widget anymore
        if (!newTopLevelWidget) {
            DockWidget::emitTopLevelEventForWidget(topLevelWidget, false);
        }
    }
    return panel;
}

void DockContainer::removeDockWidget(DockWidget *w)
{
    DockPanel *panel = w->dockPanel();
    if (panel) {
        panel->removeDockWidget(w);
    }
}

unsigned int DockContainer::zOrderIndex() const
{
    Q_D(const DockContainer);
    return d->m_zOrderIndex;
}

bool DockContainer::isInFrontOf(DockContainer *other) const
{
    return this->zOrderIndex() > other->zOrderIndex();
}

DockPanel *DockContainer::dockPanelAt(const QPoint &globalPos) const
{
    Q_D(const DockContainer);

    for (const auto &panel : d->m_panels) {
        if (panel && panel->isVisible() && panel->rect().contains(panel->mapFromGlobal(globalPos))) {
            return panel;
        }
    }

    return nullptr;
}

DockPanel *DockContainer::dockPanel(int index) const
{
    Q_D(const DockContainer);
    return (index < dockPanelCount()) ? d->m_panels[index] : nullptr;
}

QList<DockPanel *> DockContainer::openedDockPanels() const
{
    Q_D(const DockContainer);
    QList<DockPanel *> result;
    for (const auto p : d->m_panels) {
        if (p && !p->isHidden()) {
            result.append(p);
        }
    }
    return result;
}

QList<DockWidget *> DockContainer::openedDockWidgets() const
{
    Q_D(const DockContainer);
    QList<DockWidget *> result;
    for (const auto p : d->m_panels) {
        if (p && !p->isHidden()) {
            result.append(p->openedDockWidgets());
        }
    }
    return result;
}

bool DockContainer::hasTopLevelDockWidget() const
{
    auto panels = openedDockPanels();
    if (panels.count() != 1) {
        return false;
    }

    return panels[0]->openDockWidgetsCount() == 1;
}

int DockContainer::dockPanelCount() const
{
    Q_D(const DockContainer);
    return d->m_panels.count();
}

int DockContainer::visibleDockPanelCount() const
{
    Q_D(const DockContainer);
    int result = 0;
    for (const auto &panel : d->m_panels) {
        result += (!panel || panel->isHidden()) ? 0 : 1;
    }

    return result;
}

DockWidget::DockWidgetFeatures DockContainer::features() const
{
    Q_D(const DockContainer);
    DockWidget::DockWidgetFeatures f(DockWidget::AllDockWidgetFeatures);
    for (const auto &panel : d->m_panels) {
        if (!panel) {
            continue;
        }
        f &= panel->features();
    }

    return f;
}

bool DockContainer::isFloating() const
{
    Q_D(const DockContainer);
    return d->m_isFloating;
}

DockFloatingContainer *DockContainer::floatingWidget() const
{
    return internal::findParent<DockFloatingContainer *>(this);
}

DockSideBar *DockContainer::autoHideSideBar(Qx::DockSideBarArea area) const
{
    Q_D(const DockContainer);
    return d->m_sideTabBarWidgets[area];
}

QList<DockAutoHideContainer *> DockContainer::autoHideWidgets() const
{
    Q_D(const DockContainer);
    return d->m_autoHideWidgets;
}

QRect DockContainer::contentRect() const
{
    Q_D(const DockContainer);
    if (!d->m_rootSplitter) {
        return QRect();
    }

    if (d->m_rootSplitter->hasVisibleContent()) {
        return d->m_rootSplitter->geometry();
    } else {
        auto contentRect = this->rect();
        contentRect.adjust(autoHideSideBar(Qx::DockSideBarLeft)->sizeHint().width(),
                           autoHideSideBar(Qx::DockSideBarTop)->sizeHint().height(),
                           -autoHideSideBar(Qx::DockSideBarRight)->sizeHint().width(),
                           -autoHideSideBar(Qx::DockSideBarBottom)->sizeHint().height());

        return contentRect;
    }
}

DockSplitter *DockContainer::rootSplitter() const
{
    Q_D(const DockContainer);
    return d->m_rootSplitter;
}

void DockContainer::createRootSplitter()
{
    Q_D(DockContainer);
    d->createRootSplitter();
}

void DockContainer::addDockPanel(DockPanel *panel, Qx::DockWidgetArea area)
{
    Q_D(DockContainer);
    DockContainer *container = panel->dockContainer();
    if (container && container != this) {
        container->removeDockPanel(panel);
    }
    d->addDockPanel(panel, area);
}

void DockContainer::removeDockPanel(DockPanel *panel)
{
    Q_D(DockContainer);
    // If it is an auto hide area, then there is nothing much to do
    if (panel->isAutoHide()) {
        panel->setAutoHideContainer(nullptr);
        return;
    }

    panel->disconnect(this);
    d->m_panels.removeAll(panel);
    auto splitter = panel->parentSplitter();

    // Remove are from parent splitter and recursively hide tree of parent
    // splitters if it has no visible content
    panel->setParent(nullptr);
    internal::hideEmptyParentSplitters(splitter);

    // Remove this panel from cached panels
    auto p = std::find(std::begin(d->m_lastAddedPanelCache), std::end(d->m_lastAddedPanelCache), panel);
    if (p != std::end(d->m_lastAddedPanelCache)) {
        *p = nullptr;
    }

    // If splitter has more than 1 widgets, we are finished and can leave
    if (splitter->count() > 1) {
        goto emitAndExit;
    }

    // If this is the RootSplitter we need to remove empty splitters to
    // avoid too many empty splitters
    if (splitter == d->m_rootSplitter) {
        // If splitter is empty, we are finished
        if (!splitter->count()) {
            splitter->hide();
            goto emitAndExit;
        }

        QWidget *widget = splitter->widget(0);
        auto childSplitter = qobject_cast<DockSplitter *>(widget);
        // If the one and only content widget of the splitter is not a splitter
        // then we are finished
        if (!childSplitter) {
            goto emitAndExit;
        }

        // We replace the superfluous RootSplitter with the childSplitter
        childSplitter->setParent(nullptr);
        QLayoutItem *li = d->m_layout->replaceWidget(splitter, childSplitter);
        d->m_rootSplitter = childSplitter;
        delete li;
    } else if (splitter->count() == 1) {
        QSplitter *parentSplitter = internal::findParent<QSplitter *>(splitter);
        auto Sizes = parentSplitter->sizes();
        QWidget *widget = splitter->widget(0);
        widget->setParent(this);
        internal::replaceSplitterWidget(parentSplitter, splitter, widget);
        parentSplitter->setSizes(Sizes);
    }

    delete splitter;
    splitter = nullptr;

emitAndExit:
    updateSplitterHandles(splitter);
    DockWidget *topLevelWidget = topLevelDockWidget();

    // Updated the title bar visibility of the dock widget if there is only
    // one single visible dock widget
    DockWidget::emitTopLevelEventForWidget(topLevelWidget, true);
    d->emitDockAreasRemoved();
}

QList<QPointer<DockPanel>> DockContainer::removeAllDockPanels()
{
    Q_D(DockContainer);
    auto result = d->m_panels;
    d->m_panels.clear();
    return result;
}

DockWidget *DockContainer::topLevelDockWidget() const
{
    auto panel = topLevelDockPanel();
    if (!panel) {
        return nullptr;
    }
    auto widgets = panel->openedDockWidgets();
    if (widgets.count() != 1) {
        return nullptr;
    }
    return widgets[0];
}

DockPanel *DockContainer::topLevelDockPanel() const
{
    auto panels = openedDockPanels();
    if (panels.count() != 1) {
        return nullptr;
    }
    return panels[0];
}

DockPanel *DockContainer::lastAddedDockPanel(Qx::DockWidgetArea area) const
{
    Q_D(const DockContainer);
    return d->m_lastAddedPanelCache[areaIdToIndex(area)];
}

QList<DockWidget *> DockContainer::dockWidgets() const
{
    Q_D(const DockContainer);
    QList<DockWidget *> result;
    for (const auto &panel : d->m_panels) {
        if (!panel) {
            continue;
        }
        result.append(panel->dockWidgets());
    }

    return result;
}

void DockContainer::updateSplitterHandles(QSplitter *splitter)
{
    Q_D(DockContainer);
    d->updateSplitterHandles(splitter);
}

void DockContainer::dropFloatingWidget(DockFloatingContainer *floatingWidget, const QPoint &targetPos)
{
    Q_D(DockContainer);
    DockWidget *singleDroppedDockWidget = floatingWidget->topLevelDockWidget();
    DockWidget *singleDockWidget = topLevelDockWidget();
    auto dropArea = Qx::InvalidDockWidgetArea;
    auto containerDropArea = d->m_window->containerOverlay()->dropAreaUnderCursor();
    bool dropped = false;

    DockPanel *panel = dockPanelAt(targetPos);
    // mouse is over dock area
    if (panel) {
        auto dropOverlay = d->m_window->panelOverlay();
        dropOverlay->setAllowedAreas(panel->allowedAreas());
        dropArea = dropOverlay->showOverlay(panel);
        if (containerDropArea != Qx::InvalidDockWidgetArea && containerDropArea != dropArea) {
            dropArea = Qx::InvalidDockWidgetArea;
        }

        if (dropArea != Qx::InvalidDockWidgetArea) {
            int tabIndex = d->m_window->panelOverlay()->tabIndexUnderCursor();
            d->dropIntoPanel(floatingWidget, panel, dropArea, tabIndex);
            dropped = true;
        }
    }

    // mouse is over container or auto hide side bar
    if (Qx::InvalidDockWidgetArea == dropArea && Qx::InvalidDockWidgetArea != containerDropArea) {
        if (internal::isSideBarArea(containerDropArea)) {
            d->dropIntoAutoHideSideBar(floatingWidget, containerDropArea);
        } else {
            d->dropIntoContainer(floatingWidget, containerDropArea);
        }
        dropped = true;
    }
    // Remove the auto hide widgets from the floatingWidget and insert
    // them into this widget
    for (auto autoHideWidget : floatingWidget->dockContainer()->autoHideWidgets()) {
        auto sideBar = autoHideSideBar(autoHideWidget->sideBarArea());
        sideBar->addAutoHideWidget(autoHideWidget);
    }

    if (dropped) {
        floatingWidget->finishDropOperation();

        // If we dropped a floating widget with only one single dock widget, then we
        // drop a top level widget that changes from floating to docked now
        DockWidget::emitTopLevelEventForWidget(singleDroppedDockWidget, false);

        // If there was a top level widget before the drop, then it is not top
        // level widget anymore
        DockWidget::emitTopLevelEventForWidget(singleDockWidget, false);
    }

    window()->activateWindow();
    if (singleDroppedDockWidget) {
        d->m_window->notifyDockAreaRelocation(singleDroppedDockWidget);
    }
    d->m_window->notifyFloatingWidgetDrop(floatingWidget);
}

void DockContainer::dropWidget(QWidget *widget, Qx::DockWidgetArea dropArea, DockPanel *targetPanel, int tabIndex)
{
    Q_D(DockContainer);
    DockWidget *singleDockWidget = topLevelDockWidget();
    if (targetPanel) {
        d->moveToNewSection(widget, targetPanel, dropArea, tabIndex);
    } else {
        d->moveToContainer(widget, dropArea);
    }

    // If there was a top level widget before the drop, then it is not top
    // level widget anymore
    DockWidget::emitTopLevelEventForWidget(singleDockWidget, false);

    window()->activateWindow();
    d->m_window->notifyDockAreaRelocation(widget);
}

void DockContainer::createSideTabBarWidgets()
{
    Q_D(DockContainer);
    if (!DockManager::testAutoHideConfigFlag(DockManager::AutoHideFeatureEnabled)) {
        return;
    }

    {
        auto area = Qx::DockSideBarLeft;
        d->m_sideTabBarWidgets[area] = new DockSideBar(this, area);
        d->m_layout->addWidget(d->m_sideTabBarWidgets[area], 1, 0);
    }

    {
        auto area = Qx::DockSideBarRight;
        d->m_sideTabBarWidgets[area] = new DockSideBar(this, area);
        d->m_layout->addWidget(d->m_sideTabBarWidgets[area], 1, 2);
    }

    {
        auto area = Qx::DockSideBarBottom;
        d->m_sideTabBarWidgets[area] = new DockSideBar(this, area);
        d->m_layout->addWidget(d->m_sideTabBarWidgets[area], 2, 1);
    }

    {
        auto area = Qx::DockSideBarTop;
        d->m_sideTabBarWidgets[area] = new DockSideBar(this, area);
        d->m_layout->addWidget(d->m_sideTabBarWidgets[area], 0, 1);
    }
}

DockAutoHideContainer *DockContainer::createAndSetupAutoHideContainer(Qx::DockSideBarArea area, DockWidget *w,
                                                                      int tabIndex)
{
    Q_D(DockContainer);
    if (!DockManager::testAutoHideConfigFlag(DockManager::AutoHideFeatureEnabled)) {
        Q_ASSERT_X(false, "DockContainer::createAndSetupAutoHideContainer", "Requested area does not exist in config");
        return nullptr;
    }
    if (d->m_window != w->dockWindow()) {
        w->setDockWindow(d->m_window);   // Auto hide Dock Container needs a valid dock window
    }

    return autoHideSideBar(area)->insertDockWidget(tabIndex, w);
}

void DockContainer::registerAutoHideWidget(DockAutoHideContainer *autoHideWidget)
{
    Q_D(DockContainer);
    d->m_autoHideWidgets.append(autoHideWidget);
    Q_EMIT autoHideWidgetCreated(autoHideWidget);
}

void DockContainer::removeAutoHideWidget(DockAutoHideContainer *autoHideWidget)
{
    Q_D(DockContainer);
    d->m_autoHideWidgets.removeAll(autoHideWidget);
}

void DockContainer::handleAutoHideWidgetEvent(QEvent *e, QWidget *w)
{
    Q_D(DockContainer);
    if (!DockManager::testAutoHideConfigFlag(DockManager::AutoHideShowOnMouseOver)) {
        return;
    }

    auto autoHideTab = qobject_cast<DockSideTab *>(w);
    if (autoHideTab) {
        switch (e->type()) {
        case QEvent::Enter:
            if (!autoHideTab->dockWidget()->isVisible()) {
                d->m_delayedAutoHideTab = autoHideTab;
                d->m_delayedAutoHideShow = true;
                d->m_delayedAutoHideTimer.start();
            } else {
                d->m_delayedAutoHideTimer.stop();
            }
            break;

        case QEvent::MouseButtonPress:
            d->m_delayedAutoHideTimer.stop();
            break;

        case QEvent::Leave:
            if (autoHideTab->dockWidget()->isVisible()) {
                d->m_delayedAutoHideTab = autoHideTab;
                d->m_delayedAutoHideShow = false;
                d->m_delayedAutoHideTimer.start();
            } else {
                d->m_delayedAutoHideTimer.stop();
            }
            break;

        default:
            break;
        }
        return;
    }

    auto autoHideContainer = qobject_cast<DockAutoHideContainer *>(w);
    if (autoHideContainer) {
        switch (e->type()) {
        case QEvent::Enter:
        case QEvent::Hide:
            d->m_delayedAutoHideTimer.stop();
            break;

        case QEvent::Leave:
            if (autoHideContainer->isVisible()) {
                d->m_delayedAutoHideTab = autoHideContainer->autoHideTab();
                d->m_delayedAutoHideShow = false;
                d->m_delayedAutoHideTimer.start();
            }
            break;

        default:
            break;
        }
        return;
    }
}

QX_DOCK_END_NAMESPACE
