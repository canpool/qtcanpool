/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockwidget.h"
#include "dockwindow.h"
#include "dockpanel.h"
#include "docktab.h"

#include <QPointer>
#include <QBoxLayout>

QX_DOCK_BEGIN_NAMESPACE

class DockWidgetPrivate
{
public:
    QX_DECLARE_PUBLIC(DockWidget)
public:
    DockWidgetPrivate();
    void init();
public:
    DockWindow *m_window = nullptr;
    QPointer<DockPanel> m_panel = nullptr;
    QBoxLayout *m_layout = nullptr;
    DockTab *m_tab = nullptr;
    DockWidget::DockWidgetFeatures m_features = DockWidget::DefaultDockWidgetFeatures;
};

DockWidgetPrivate::DockWidgetPrivate()
{
}

void DockWidgetPrivate::init()
{
    Q_Q(DockWidget);

    m_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    q->setLayout(m_layout);

    m_tab = new DockTab(q);
}

DockWidget::DockWidget(const QString &title, QWidget *parent)
    : QWidget{parent}
{
    QX_INIT_PRIVATE(DockWidget)

    // must before d->init(), create tab need title
    setWindowTitle(title);
    setObjectName(title);

    Q_D(DockWidget);
    d->init();
}

DockWidget::~DockWidget()
{
    QX_FINI_PRIVATE()
}

DockTab *DockWidget::tab() const
{
    Q_D(const DockWidget);
    return d->m_tab;
}

DockWidget::DockWidgetFeatures DockWidget::features() const
{
    Q_D(const DockWidget);
    return d->m_features;
}

void DockWidget::setDockWindow(DockWindow *window)
{
    Q_D(DockWidget);
    d->m_window = window;
}

void DockWidget::setDockPanel(DockPanel *panel)
{
    Q_D(DockWidget);
    d->m_panel = panel;
    setParent(panel);
}

QX_DOCK_END_NAMESPACE
