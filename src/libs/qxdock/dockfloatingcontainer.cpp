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
#include "dockmanager.h"

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

static unsigned int s_zOrderCounterFloating = 0;

class DockFloatingContainerPrivate
{
public:
    QX_DECLARE_PUBLIC(DockFloatingContainer)
public:
    DockFloatingContainerPrivate();

    void titleMouseReleaseEvent();
    void updateDropOverlays(const QPoint &globalPos);

    static bool testConfigFlag(DockManager::ConfigFlag flag);

    bool isState(Qx::DockDragState stateId) const;
    void setState(Qx::DockDragState stateId);

    void setWindowTitle(const QString &text);
    void reflectCurrentWidget(DockWidget *currentWidget);

    static QString floatingContainersTitle();

    void handleEscapeKey();
public:
    QPointer<DockWindow> m_window;
    DockContainer *m_dockContainer;
    DockContainer *m_dropContainer = nullptr;
    DockPanel *m_singlePanel = nullptr;
    QPoint m_dragStartMousePosition;
    Qx::DockDragState m_draggingState = Qx::DockDraggingInactive;
    QPoint m_dragStartPos;
    bool m_hiding = false;
    bool m_autoHideChildren = true;
    unsigned int m_zOrderIndex = ++s_zOrderCounterFloating;
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
        // Do not resize if we drop into an autohide sidebar area to preserve
        // the dock area size for the initial size of the auto hide area
        if (!internal::isSideBarArea(overlay->dropAreaUnderCursor())) {
            // Resize the floating widget to the size of the highlighted drop area rectangle
            QRect rect = overlay->dropOverlayRect();
            int frameWidth = (q->frameSize().width() - q->rect().width()) / 2;
            int titleBarHeight = q->frameSize().height() - q->rect().height() - frameWidth;
            if (rect.isValid()) {
                QPoint topLeft = overlay->mapToGlobal(rect.topLeft());
                topLeft.ry() += titleBarHeight;
                q->setGeometry(QRect(topLeft, QSize(rect.width(), rect.height() - titleBarHeight)));
                QApplication::processEvents();
            }
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
    if (m_dockContainer->features().testFlag(DockWidget::DockWidgetPinnable)) {
        allowdContainerAreas |= Qx::AutoHideDockAreas;
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

bool DockFloatingContainerPrivate::testConfigFlag(DockManager::ConfigFlag flag)
{
    return DockManager::testConfigFlag(flag);
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

void DockFloatingContainerPrivate::setWindowTitle(const QString &text)
{
    Q_Q(DockFloatingContainer);
    q->setWindowTitle(text);
}

void DockFloatingContainerPrivate::reflectCurrentWidget(DockWidget *currentWidget)
{
    Q_Q(DockFloatingContainer);
    // reflect currentWidget's title if configured to do so, otherwise display application name as window title
    if (testConfigFlag(DockManager::FloatingContainerHasWidgetTitle)) {
        setWindowTitle(currentWidget->windowTitle());
    } else {
        setWindowTitle(floatingContainersTitle());
    }

    // reflect currentWidget's icon if configured to do so, otherwise display application icon as window icon
    QIcon CurrentWidgetIcon = currentWidget->icon();
    if (testConfigFlag(DockManager::FloatingContainerHasWidgetIcon) && !CurrentWidgetIcon.isNull()) {
        q->setWindowIcon(currentWidget->icon());
    } else {
        q->setWindowIcon(QApplication::windowIcon());
    }
}

QString DockFloatingContainerPrivate::floatingContainersTitle()
{
    return DockManager::floatingContainersTitle();
}

void DockFloatingContainerPrivate::handleEscapeKey()
{
    setState(Qx::DockDraggingInactive);
    m_window->containerOverlay()->hideOverlay();
    m_window->panelOverlay()->hideOverlay();
}

DockFloatingContainer::DockFloatingContainer(DockWindow *window)
    : DockFloatingContainerBase(window)
{
    QX_INIT_PRIVATE(DockFloatingContainer);

    Q_D(DockFloatingContainer);
    d->m_window = window;
    d->m_dockContainer = new DockContainer(window, this);
    connect(d->m_dockContainer, SIGNAL(dockAreasAdded()), this, SLOT(onDockAreasAddedOrRemoved()));
    connect(d->m_dockContainer, SIGNAL(dockAreasRemoved()), this, SLOT(onDockAreasAddedOrRemoved()));

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
    auto topLevelWidget = topLevelDockWidget();
    if (topLevelWidget) {
        topLevelWidget->emitTopLevelChanged(true);
    }
    d->m_window->notifyDockAreaRelocation(panel);
}

DockFloatingContainer::DockFloatingContainer(DockWidget *widget)
    : DockFloatingContainer(widget->dockWindow())
{
    Q_D(DockFloatingContainer);
    d->m_dockContainer->addDockWidget(Qx::CenterDockWidgetArea, widget);
    auto topLevelWidget = topLevelDockWidget();
    if (topLevelWidget) {
        topLevelWidget->emitTopLevelChanged(true);
    }

    d->m_window->notifyDockAreaRelocation(widget);
}

DockFloatingContainer::~DockFloatingContainer()
{
    Q_D(DockFloatingContainer);
    if (d->m_window) {
        d->m_window->removeFloatingWidget(this);
    }
    QX_FINI_PRIVATE();
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
    Q_D(DockFloatingContainer);
    // Widget has been redocked, so it must be hidden right way.
    // but AutoHideChildren must be set to false because "this" still contains
    // dock widgets that shall not be toggled hidden.
    d->m_autoHideChildren = false;
    hide();
    // The floating widget will be deleted now. Ensure, that the destructor
    // of the floating widget does not delete any dock areas that have been
    // moved to a new container - simply remove all dock areas before deleting
    // the floating widget
    d->m_dockContainer->removeAllDockPanels();

    deleteLater();

    if (d->m_window) {
        d->m_window->removeFloatingWidget(this);
        d->m_window->removeDockContainer(this->dockContainer());
    }
}

void DockFloatingContainer::onDockAreasAddedOrRemoved()
{
    Q_D(DockFloatingContainer);
    auto topLevelPanel = d->m_dockContainer->topLevelDockPanel();
    if (topLevelPanel) {
        d->m_singlePanel = topLevelPanel;
        DockWidget *currentWidget = d->m_singlePanel->currentDockWidget();
        d->reflectCurrentWidget(currentWidget);
        connect(d->m_singlePanel, SIGNAL(currentChanged(int)), this, SLOT(onDockAreaCurrentChanged(int)));
    } else {
        if (d->m_singlePanel) {
            disconnect(d->m_singlePanel, SIGNAL(currentChanged(int)), this, SLOT(onDockAreaCurrentChanged(int)));
            d->m_singlePanel = nullptr;
        }
        d->setWindowTitle(d->floatingContainersTitle());
        setWindowIcon(QApplication::windowIcon());
    }
}

void DockFloatingContainer::onDockAreaCurrentChanged(int index)
{
    Q_UNUSED(index);
    Q_D(DockFloatingContainer);
    DockWidget *currentWidget = d->m_singlePanel->currentDockWidget();
    d->reflectCurrentWidget(currentWidget);
}

void QxDock::DockFloatingContainer::startFloating(const QPoint &dragStartMousePos, const QSize &size,
                                                  Qx::DockDragState dragState, QWidget *mouseEventHandler)
{
    Q_UNUSED(mouseEventHandler)
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
    int borderSize = (frameSize().width() - size().width()) / 2;
    const QPoint moveToPos = QCursor::pos() - d->m_dragStartMousePosition - QPoint(borderSize, 0);
    move(moveToPos);

    switch (d->m_draggingState) {
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
    Q_D(DockFloatingContainer);
    // If this floating container will be hidden, then updating the window
    // tile is not required anymore
    if (d->m_hiding) {
        return;
    }
    auto topLevelPanel = d->m_dockContainer->topLevelDockPanel();
    if (topLevelPanel) {
        DockWidget *currentWidget = topLevelPanel->currentDockWidget();
        if (currentWidget) {
            d->reflectCurrentWidget(currentWidget);
        }
    } else {
        d->setWindowTitle(d->floatingContainersTitle());
        setWindowIcon(QApplication::windowIcon());
    }
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
    Q_D(DockFloatingContainer);
    if (event->spontaneous()) {
        return;
    }
    if (d->m_autoHideChildren) {
        d->m_hiding = true;
        for (auto panel : d->m_dockContainer->openedDockPanels()) {
            for (auto w : panel->openedDockWidgets()) {
                w->toggleView(false);
            }
        }
        d->m_hiding = false;
    }
}

void DockFloatingContainer::showEvent(QShowEvent *event)
{
    Super::showEvent(event);
}

void DockFloatingContainer::changeEvent(QEvent *event)
{
    Super::changeEvent(event);
    Q_D(DockFloatingContainer);
    switch (event->type()) {
    case QEvent::ActivationChange:
        if (isActiveWindow()) {
            d->m_zOrderIndex = ++s_zOrderCounterFloating;
        }
        break;

    case QEvent::WindowStateChange:
        // If the DockWindow is restored from minimized on Windows
        // then the FloatingWidgets are not properly restored to maximized but
        // to normal state.
        // We simply check here, if the FloatingWidget was maximized before
        // and if the DockWindow is just leaving the minimized state. In this
        // case, we restore the maximized state of this floating widget
        if (d->m_window->isLeavingMinimizedState()) {
            QWindowStateChangeEvent *ev = static_cast<QWindowStateChangeEvent *>(event);
            if (ev->oldState().testFlag(Qt::WindowMaximized)) {
                this->showMaximized();
            }
        }
        break;

    default:
        break;   // do nothing
    }
}

#ifdef Q_OS_WIN
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
bool DockFloatingContainer::nativeEvent(const QByteArray &eventType, void *message, long *result)
#else
bool DockFloatingContainer::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
#endif
{
    Super::nativeEvent(eventType, message, result);

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
