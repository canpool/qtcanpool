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
    return targetPanel;
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

void DockContainer::createRootSplitter()
{
    Q_D(DockContainer);
    d->createRootSplitter();
}

QX_DOCK_END_NAMESPACE