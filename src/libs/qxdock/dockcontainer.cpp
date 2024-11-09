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

#include <QGridLayout>

QX_DOCK_BEGIN_NAMESPACE

static void insertWidgetIntoSplitter(QSplitter *s, QWidget *w, bool append)
{
    if (append) {
        s->addWidget(w);
    } else {
        s->insertWidget(0, w);
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

public:
    DockWindow *m_window = nullptr;
    QGridLayout *m_layout = nullptr;
    DockSplitter *m_rootSplitter = nullptr;
    QList<QPointer<DockPanel>> m_panels;
    DockPanel *m_topLevelPanel = nullptr;
    bool m_isFloating = false;
};

DockContainerPrivate::DockContainerPrivate()
{

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
    for (int i = 0; i < splitterSizes.size() -1; i++) {
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
    return panel;
}

DockPanel *DockContainerPrivate::addDockWidgetToPanel(Qx::DockWidgetArea area, DockWidget *w,
                                                      DockPanel *targetPanel, int index)
{
    Q_Q(DockContainer);
    if (area == Qx::CenterDockWidgetArea) {
        targetPanel->insertDockWidget(index, w);
        return targetPanel;
    }
    DockPanel *np = new DockPanel(m_window, q);
    np->addDockWidget(w);

    auto param = internal::dockAreaInsertParameters(area);
    auto targetSplitter = targetPanel->parentSplitter();
    // use target index instead of parameter index
    index = targetSplitter ->indexOf(targetPanel);
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

DockContainer::DockContainer(DockWindow *window, QWidget *parent)
    : QWidget(parent)
{
    Q_ASSERT(window);
    QX_INIT_PRIVATE(DockContainer)

    Q_D(DockContainer);
    d->m_window = window;
    d->init();

    if (window != this) {
        d->m_window->registerDockContainer(this);
        createRootSplitter();
    }
}

DockContainer::~DockContainer()
{
    Q_D(DockContainer);
    if (d->m_window) {
        d->m_window->removeDockContainer(this);
    }
    QX_FINI_PRIVATE()
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
    panel->disconnect(this);
    d->m_panels.removeAll(panel);
    auto splitter = panel->parentSplitter();

    // Remove are from parent splitter and recursively hide tree of parent
    // splitters if it has no visible content
    panel->setParent(nullptr);
    internal::hideEmptyParentSplitters(splitter);

    // If splitter has more than 1 widgets, we are finished and can leave
    if (splitter->count() >  1) {
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

QX_DOCK_END_NAMESPACE
