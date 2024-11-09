/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockfloatingcontainer.h"
#include "dockwindow.h"
#include "dockpanel.h"
#include "dockcontainer.h"
#include "dockutils.h"

#include <QBoxLayout>
#include <QEvent>
#include <QCloseEvent>

QX_DOCK_BEGIN_NAMESPACE

class DockFloatingContainerPrivate
{
public:
    QX_DECLARE_PUBLIC(DockFloatingContainer)
public:
    DockFloatingContainerPrivate();

    void setState(Qx::DockDragState stateId);
public:
    QPointer<DockWindow> m_window;
    DockContainer *m_dockContainer;
    QPoint m_dragStartMousePosition;
    Qx::DockDragState m_draggingState = Qx::DockDraggingInactive;
};

DockFloatingContainerPrivate::DockFloatingContainerPrivate()
{

}

void DockFloatingContainerPrivate::setState(Qx::DockDragState stateId)
{
    Q_Q(DockFloatingContainer);
    if (m_draggingState == stateId) {
        return;
    }
    m_draggingState = stateId;
    if (Qx::DockDraggingFloatingWidget == m_draggingState) {
        qApp->postEvent(q, new QEvent((QEvent::Type)internal::FloatingWidgetDragStartEvent));
    }
}

DockFloatingContainer::DockFloatingContainer(DockWindow *window)
    : DockFloatingContainerBase(window)
{
    QX_INIT_PRIVATE(DockFloatingContainer)

    Q_D(DockFloatingContainer);
    d->m_window = window;
    d->m_dockContainer = new DockContainer(window, this);
    connect(d->m_dockContainer, SIGNAL(dockAreasAdded()), this,
            SLOT(onDockAreasAddedOrRemoved()));
    connect(d->m_dockContainer, SIGNAL(dockAreasRemoved()), this,
        SLOT(onDockAreasAddedOrRemoved()));

    setWindowFlags(Qt::Window | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    QBoxLayout *l = new QBoxLayout(QBoxLayout::TopToBottom);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    setLayout(l);
    l->addWidget(d->m_dockContainer);

    window->registerFloatingWidget(this);
}

DockFloatingContainer::DockFloatingContainer(DockPanel *panel)
    : DockFloatingContainer(panel->dockWindow())
{
    Q_D(DockFloatingContainer);
    d->m_dockContainer->addDockPanel(panel);
    d->m_window->notifyDockAreaRelocation(panel);
}

DockFloatingContainer::~DockFloatingContainer()
{
    Q_D(DockFloatingContainer);
    if (d->m_window) {
        d->m_window->removeFloatingWidget(this);
    }
    QX_FINI_PRIVATE()
}

bool DockFloatingContainer::isClosable() const
{
    Q_D(const DockFloatingContainer);
    return d->m_dockContainer->features().testFlag(DockWidget::DockWidgetClosable);
}

DockContainer *DockFloatingContainer::dockContainer() const
{
    Q_D(const DockFloatingContainer);
    return d->m_dockContainer;
}

DockWidget *DockFloatingContainer::topLevelDockWidget() const
{
    Q_D(const DockFloatingContainer);
    return d->m_dockContainer->topLevelDockWidget();
}

QList<DockWidget *> DockFloatingContainer::dockWidgets() const
{
    Q_D(const DockFloatingContainer);
    return d->m_dockContainer->dockWidgets();
}

void DockFloatingContainer::onDockAreasAddedOrRemoved()
{

}

void QxDock::DockFloatingContainer::startFloating(const QPoint &dragStartMousePos, const QSize &size,
                                                  Qx::DockDragState dragState, QWidget *mouseEventHandler)
{
    Q_D(DockFloatingContainer);
    resize(size);
    d->m_dragStartMousePosition = dragStartMousePos;
    d->setState(dragState);
    moveFloating();
    show();
}

void DockFloatingContainer::finishDragging()
{

}

void DockFloatingContainer::moveFloating()
{
    Q_D(DockFloatingContainer);
    const QPoint moveToPos = QCursor::pos() - d->m_dragStartMousePosition;
    move(moveToPos);
}

void DockFloatingContainer::deleteContent()
{
    std::vector<QPointer<DockPanel> > panels;
    for (int i = 0; i < dockContainer()->dockPanelCount(); ++i) {
        panels.push_back(dockContainer()->dockPanel(i));
    }
    for (auto p : panels) {
        if (!p) {
            continue;
        }
        std::vector<QPointer<QWidget> > deleteWidgets;
        for (auto w : p->dockWidgets()) {
            deleteWidgets.push_back(w);
        }
        for (auto w : deleteWidgets) {
            delete w;
        }
    }
}

void DockFloatingContainer::updateWindowTitle()
{

}

void DockFloatingContainer::closeEvent(QCloseEvent *event)
{
    Q_D(DockFloatingContainer);
    d->setState(Qx::DockDraggingInactive);
    event->ignore();
    if (!isClosable()) {
        return;
    }

    bool hasOpenDockWidgets = false;
    for (auto w : d->m_dockContainer->openedDockWidgets()) {
        if (w->features().testFlag(DockWidget::DockWidgetDeleteOnClose) ||
            w->features().testFlag(DockWidget::CustomCloseHandling)) {
            bool closed = w->closeDockWidgetInternal();
            if (!closed) {
                hasOpenDockWidgets = true;
            }
        } else {
            w->toggleView(false);
        }
    }

    if (hasOpenDockWidgets) {
        return;
    }

    // In Qt version after 5.9.2 there seems to be a bug that causes the
    // QWidget::event() function to not receive any NonClientArea mouse
    // events anymore after a close/show cycle. The bug is reported here:
    // https://bugreports.qt.io/browse/QTBUG-73295
    // The following code is a workaround for Qt versions > 5.9.2 that seems
    // to work
    // Starting from Qt version 5.12.2 this seems to work again. But
    // now the QEvent::NonClientAreaMouseButtonPress function returns always
    // Qt::RightButton even if the left button was pressed
    this->hide();
}

void DockFloatingContainer::hideEvent(QHideEvent *event)
{
    Super::hideEvent(event);
    if (event->spontaneous()) {
        return;
    }
}

void DockFloatingContainer::showEvent(QShowEvent *event)
{
    Super::showEvent(event);
}

QX_DOCK_END_NAMESPACE
