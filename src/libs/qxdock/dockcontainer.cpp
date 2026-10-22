/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "dockcontainer_p.h"
#include "dockwindow.h"
#include "docksplitter.h"
#include "dockpanel.h"
#include "dockutils.h"
#include "dockmanager.h"
#include "dockoverlay.h"

#include <QGridLayout>
#include <QTimer>
#include <QXmlStreamWriter>
#include <QApplication>
#include <QPointer>
#include <QTextStream>

QX_DOCK_BEGIN_NAMESPACE

static unsigned int s_zOrderCounter = 0;

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

/**
 * The visible dock panel count changes, if dock panels are remove, added or
 * when its view is toggled
 */
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

/**
 * This function forces the dock container widget to update handles of splitters
 * based if a central widget exists.
 */
void DockContainerPrivate::updateSplitterHandles(QSplitter *s)
{
    if (!m_window->centralWidget() || !s) {
        return;
    }

    for (int i = 0; i < s->count(); ++i) {
        s->setStretchFactor(i, widgetResizesWithContainer(s->widget(i)) ? 1 : 0);
    }
}

/**
 * Ensures equal distribution of the sizes of a splitter if an dock widget
 * is inserted from code
 */
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

/**
 * If no central widget exists, the widgets resize with the container.
 * If a central widget exists, the widgets surrounding the central widget
 * do not resize its height or width.
 */
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

void DockContainerPrivate::onDockAreaViewToggled(bool visible)
{
    Q_Q(DockContainer);
    DockPanel *panel = qobject_cast<DockPanel *>(q->sender());
    onVisibleDockAreaCountChanged();
    Q_EMIT q->dockAreaViewToggled(panel, visible);
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

void DockContainerPrivate::appendPanels(const QList<DockPanel *> panels)
{
    for (auto *panel : panels) {
        m_panels.append(panel);
    }
    Q_Q(DockContainer);
    for (auto *panel : panels) {
        QObject::connect(panel, &DockPanel::viewToggled, q,
                         std::bind(&DockContainerPrivate::onDockAreaViewToggled, this, std::placeholders::_1));
        ;
    }
}

DockContainer::DockContainer(DockWindow *window, QWidget *parent)
    : Super(parent)
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
    QXDOCK_SET_BGCOLOR(this);
}

DockContainer::~DockContainer()
{
    Q_D(DockContainer);

    // A DockWindow passes itself as the window (see DockWindow::DockWindow), and
    // this destructor then runs as part of its base destruction - that is, after
    // ~DockWindow() released the private data, and with the dynamic type already
    // degraded to DockContainer. Calling into the derived class in that state is
    // undefined behaviour, and it would be pointless anyway, because
    // DockWindow::removeDockContainer() ignores the self case. Only the real
    // child containers unregister here.
    if (d->m_window && d->m_window != this) {
        d->m_window->removeDockContainer(this);
    }

    QX_FINI_PRIVATE();
}

/**
 * Adds dockwidget into the given area.
 * If p is not null, then the area parameter indicates the area
 * into the panel. If p is null, the Dockwidget will be dropped into the container.
 * \return Returns the dock panel that contains the new DockWidget
 */
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
        // If the container contained only one visible dock widget, then we need
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

/**
 * This function returns true if this container widgets z order index is
 * higher than the index of the container widget given in other parameter
 */
bool DockContainer::isInFrontOf(DockContainer *other) const
{
    return this->zOrderIndex() > other->zOrderIndex();
}

DockWindow *DockContainer::dockWindow() const
{
    Q_D(const DockContainer);
    return d->m_window;
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

/**
 * Returns the list of dock panels that are not closed
 * If all dock widgets in a dock panel are closed, the dock panel will be closed
 */
QList<DockPanel *> DockContainer::openedDockPanels() const
{
    Q_D(const DockContainer);
    QList<DockPanel *> result;
    for (const auto &p : d->m_panels) {
        if (p && !p->isHidden()) {
            result.append(p);
        }
    }
    return result;
}

/**
 * Returns a list for all open dock widgets in all open dock panels
 */
QList<DockWidget *> DockContainer::openedDockWidgets() const
{
    Q_D(const DockContainer);
    QList<DockWidget *> result;
    for (const auto &p : d->m_panels) {
        if (p && !p->isHidden()) {
            result.append(p->openedDockWidgets());
        }
    }
    return result;
}

/**
 * This function returns true, if the container has open dock panels.
 * This functions is a little bit faster than calling openedDockPanels().isEmpty()
 * because it returns as soon as it finds an open dock panel
 */
bool DockContainer::hasOpenDockPanels() const
{
    Q_D(const DockContainer);
    for (auto panel : d->m_panels) {
        if (panel && !panel->isHidden()) {
            return true;
        }
    }

    return false;
}

/**
 * This function returns true if this dock panel has only one single
 * visible dock widget.
 * A top level widget is a real floating widget. Only the isFloating()
 * function of top level widgets may returns true.
 */
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

/**
 * This functions returns the dock widget features of all dock widget in
 * this container.
 * A bitwise and is used to combine the flags of all dock widgets. That
 * means, if only dock widget does not support a certain flag, the whole
 * dock are does not support the flag.
 */
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

/**
 * If this dock container is in a floating widget, this function returns
 * the floating widget.
 * Else, it returns a nullptr.
 */
DockFloatingContainer *DockContainer::floatingWidget() const
{
    return internal::findParent<DockFloatingContainer *>(this);
}

void DockContainer::closeOtherAreas(DockPanel *keepOpenPanel)
{
    Q_D(DockContainer);
    for (const auto &panel : d->m_panels) {
        if (!panel || panel == keepOpenPanel) {
            continue;
        }

        if (!panel->features(Qx::DockBitwiseAnd).testFlag(DockWidget::DockWidgetClosable)) {
            continue;
        }

        // We do not close areas with widgets with custom close handling
        if (panel->features(Qx::DockBitwiseOr).testFlag(DockWidget::CustomCloseHandling)) {
            continue;
        }

        panel->closeArea();
    }
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

/**
 * Returns the content rectangle mapped to global space.
 * The content rectangle is the rectangle of the dock container minus
 * the auto hide side bars. So that means, it is the geometry of the
 * internal root splitter mapped to global space.
 */
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

/**
 * Access function for the internal root splitter
 */
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

/**
 * Remove all dock panles and returns the list of removed dock panles
 */
QList<QPointer<DockPanel>> DockContainer::removeAllDockPanels()
{
    Q_D(DockContainer);
    auto result = d->m_panels;
    d->m_panels.clear();
    return result;
}

/**
 * If hasTopLevelDockWidget() returns true, this function returns the
 * one and only visible dock widget. Otherwise it returns a nullptr.
 */
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

/**
 * This function returns the last added dock panel for the given
 * area identifier or 0 if no dock panel has been added for the given area
 */
DockPanel *DockContainer::lastAddedDockPanel(Qx::DockWidgetArea area) const
{
    Q_D(const DockContainer);
    return d->m_lastAddedPanelCache[areaIdToIndex(area)];
}

/**
 * This function returns a list of all dock widgets in this container.
 * It may be possible, depending on the implementation, that dock widgets,
 * that are not visible to the user have no parent widget. Therefore simply
 * calling findChildren() would not work here. Therefore this function
 * iterates over all dock panels and creates a list that contains all
 * dock widgets returned from all dock panels.
 */
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

/**
 * This function forces the dock container widget to update handles of splitters
 * based on resize modes of dock widgets contained in the container.
 */

bool DockContainer::event(QEvent *e)
{
    Q_D(DockContainer);
    bool result = QWidget::event(e);
    if (e->type() == QEvent::WindowActivate) {
        d->m_zOrderIndex = ++s_zOrderCounter;
    } else if (e->type() == QEvent::Show && !d->m_zOrderIndex) {
        d->m_zOrderIndex = ++s_zOrderCounter;
    }

    return result;
}

QX_DOCK_END_NAMESPACE
