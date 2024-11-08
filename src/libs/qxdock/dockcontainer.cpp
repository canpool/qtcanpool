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
};

DockContainerPrivate::DockContainerPrivate()
{

}

void DockContainerPrivate::init()
{
    Q_Q(DockContainer);

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
    addDockPanel(panel, area);
    panel->addDockWidget(w);
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
    appendPanels(ps);
    for (auto p : ps) {
        p->titleBarButton(Qx::TitleBarButtonClose)->setVisible(true);
    }
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
    w->setDockWindow(d->m_window);

    DockPanel *panel = nullptr;
    if (p) {
        panel = d->addDockWidgetToPanel(area, w, p, index);
    } else {
        panel = d->addDockWidgetToContainer(area, w);
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

int DockContainer::dockPanelCount() const
{
    Q_D(const DockContainer);
    return d->m_panels.count();
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

QX_DOCK_END_NAMESPACE
