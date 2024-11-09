/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockfloatingcontainer.h"
#include "dockwindow.h"
#include "dockpanel.h"
#include "dockcontainer.h"
#include "dockutils.h"
#include "dockoverlay.h"

#include <QBoxLayout>
#include <QEvent>
#include <QCloseEvent>
#include <QApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#ifdef _MSC_VER
#pragma comment(lib, "User32.lib")
#endif
#endif

QX_DOCK_BEGIN_NAMESPACE

class DockFloatingContainerPrivate
{
public:
    QX_DECLARE_PUBLIC(DockFloatingContainer)
public:
    DockFloatingContainerPrivate();

    void titleMouseReleaseEvent();
    void updateDropOverlays(const QPoint &globalPos);

    bool isState(Qx::DockDragState stateId) const;
    void setState(Qx::DockDragState stateId);

    void handleEscapeKey();

public:
    QPointer<DockWindow> m_window;
    DockContainer *m_dockContainer;
    DockContainer *m_dropContainer = nullptr;
    QPoint m_dragStartMousePosition;
    Qx::DockDragState m_draggingState = Qx::DockDraggingInactive;
    QPoint m_dragStartPos;
};

DockFloatingContainerPrivate::DockFloatingContainerPrivate()
{

}

void DockFloatingContainerPrivate::titleMouseReleaseEvent()
{
    Q_Q(DockFloatingContainer);

    setState(Qx::DockDraggingInactive);

    if (!m_dropContainer) {
        return;
    }

    if (m_window->panelOverlay()->dropAreaUnderCursor() != Qx::InvalidDockWidgetArea ||
        m_window->containerOverlay()->dropAreaUnderCursor() != Qx::InvalidDockWidgetArea) {
        DockOverlay *overlay = m_window->containerOverlay();
        if (!overlay->dropOverlayRect().isValid()) {
            overlay = m_window->panelOverlay();
        }

        QRect rect = overlay->dropOverlayRect();
        int frameWidth = (q->frameSize().width() - q->rect().width()) / 2;
        int titleBarHeight = q->frameSize().height() - q->rect().height() - frameWidth;
        if (rect.isValid()) {
            QPoint topLeft = overlay->mapToGlobal(rect.topLeft());
            topLeft.ry() += titleBarHeight;
            q->setGeometry(QRect(topLeft, QSize(rect.width(), rect.height() - titleBarHeight)));
            QApplication::processEvents();
        }
        m_dropContainer->dropFloatingWidget(q, QCursor::pos());
    }

    m_window->containerOverlay()->hideOverlay();
    m_window->panelOverlay()->hideOverlay();
}

void DockFloatingContainerPrivate::updateDropOverlays(const QPoint &globalPos)
{
    Q_Q(DockFloatingContainer);
    if (!q->isVisible() || !m_window) {
        return;
    }
    auto containers = m_window->dockContainers();
    DockContainer *topContainer = nullptr;
    for (const auto c : containers) {
        if (!c->isVisible()) {
            continue;
        }
        if (m_dockContainer == c) {
            continue;
        }
        QPoint mappedPos = c->mapFromGlobal(globalPos);
        if (c->rect().contains(mappedPos)) {
            if (!topContainer || c->isInFrontOf(topContainer)) {
                topContainer = c;
            }
        }
    }
    m_dropContainer = topContainer;

    auto containerOverlay = m_window->containerOverlay();
    auto panelOverlay = m_window->panelOverlay();
    if (!topContainer) {
        containerOverlay->hideOverlay();
        panelOverlay->hideOverlay();
        return;
    }

    int visiblePanels = topContainer->visibleDockPanelCount();
    Qx::DockWidgetAreas allowdContainerAreas = (visiblePanels > 1) ? Qx::OuterDockAreas : Qx::AllDockAreas;
    auto panel = topContainer->dockPanelAt(globalPos);
    // If the dock container contains only one single panel, then we need
    // to respect the allowed areas - only the center area is relevant here because
    // all other allowed areas are from the container
    if (visiblePanels == 1 && panel) {
        allowdContainerAreas.setFlag(Qx::CenterDockWidgetArea,
                                     panel->allowedAreas().testFlag(Qx::CenterDockWidgetArea));
    }
    containerOverlay->setAllowedAreas(allowdContainerAreas);

    Qx::DockWidgetArea containerArea = containerOverlay->showOverlay(topContainer);
    containerOverlay->enableDropPreview(containerArea != Qx::InvalidDockWidgetArea);
    if (panel && panel->isVisible() && visiblePanels > 0) {
        panelOverlay->enableDropPreview(true);
        panelOverlay->setAllowedAreas((visiblePanels == 1) ? Qx::NoDockWidgetArea : panel->allowedAreas());
        Qx::DockWidgetArea area = panelOverlay->showOverlay(panel);

        // A CenterDockWidgetArea for the dockAreaOverlay() indicates that
        // the mouse is in the title bar. If the ContainerArea is valid
        // then we ignore the dock area of the dockAreaOverlay() and disable
        // the drop preview
        if ((area == Qx::CenterDockWidgetArea) && (containerArea != Qx::InvalidDockWidgetArea)) {
            panelOverlay->enableDropPreview(false);
            containerOverlay->enableDropPreview(true);
        } else {
            containerOverlay->enableDropPreview(Qx::InvalidDockWidgetArea == area);
        }
    } else {
        panelOverlay->hideOverlay();
    }
}

bool DockFloatingContainerPrivate::isState(Qx::DockDragState stateId) const
{
    return stateId == m_draggingState;
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

void DockFloatingContainerPrivate::handleEscapeKey()
{
    // TODO
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

void DockFloatingContainer::finishDropOperation()
{
    // TODO
}

void DockFloatingContainer::onDockAreasAddedOrRemoved()
{
    // TODO
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
    Q_D(DockFloatingContainer);
    d->titleMouseReleaseEvent();
}

void DockFloatingContainer::moveFloating()
{
    Q_D(DockFloatingContainer);
    const QPoint moveToPos = QCursor::pos() - d->m_dragStartMousePosition;
    move(moveToPos);

    switch (d->m_draggingState)
    {
    case Qx::DockDraggingMousePressed:
        d->setState(Qx::DockDraggingFloatingWidget);
        d->updateDropOverlays(QCursor::pos());
        break;

    case Qx::DockDraggingFloatingWidget:
        d->updateDropOverlays(QCursor::pos());
        break;
    default:
        break;
    }
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
    // TODO
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


#ifdef Q_OS_WIN
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
bool DockFloatingContainer::nativeEvent(const QByteArray &eventType, void *message, long *result)
#else
bool DockFloatingContainer::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
#endif
{
    QWidget::nativeEvent(eventType, message, result);

    Q_D(DockFloatingContainer);
    MSG *msg = static_cast<MSG *>(message);
    switch (msg->message) {
    case WM_MOVING: {
        if (d->isState(Qx::DockDraggingFloatingWidget)) {
            d->updateDropOverlays(QCursor::pos());
        }
    } break;

    case WM_NCLBUTTONDOWN:
        if (msg->wParam == HTCAPTION && d->isState(Qx::DockDraggingInactive)) {
            d->m_dragStartPos = pos();
            d->setState(Qx::DockDraggingMousePressed);
        }
        break;

    case WM_NCLBUTTONDBLCLK:
        d->setState(Qx::DockDraggingInactive);
        break;

    case WM_ENTERSIZEMOVE:
        if (d->isState(Qx::DockDraggingMousePressed)) {
            d->setState(Qx::DockDraggingFloatingWidget);
            d->updateDropOverlays(QCursor::pos());
        }
        break;

    case WM_EXITSIZEMOVE:
        if (d->isState(Qx::DockDraggingFloatingWidget)) {
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                d->handleEscapeKey();
            } else {
                d->titleMouseReleaseEvent();
            }
        }
        break;
    }
    return false;
}
#endif


QX_DOCK_END_NAMESPACE
