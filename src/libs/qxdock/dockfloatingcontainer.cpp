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
#include "dockstatereader.h"

#include <QBoxLayout>
#include <QEvent>
#include <QCloseEvent>
#include <QApplication>
#include <QPointer>

#ifdef Q_OS_WIN
#include <windows.h>
#ifdef _MSC_VER
#pragma comment(lib, "User32.lib")
#endif
#elif defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
#include "linux/dockfloatingtitlebar.h"
#include <xcb/xcb.h>
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
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    QWidget *m_mouseEventHandler = nullptr;
    DockFloatingTitleBar *m_titleBar = nullptr;
    bool m_isResizing = false;
    bool m_mousePressed = false;
#endif
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
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    // Prevent display of drop overlays and docking as long as a model dialog
    // is active
    if (qApp->activeModalWidget()) {
        return;
    }
#endif
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
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    if (m_titleBar) {
        m_titleBar->setTitle(text);
    }
#endif
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

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    QDockWidget::setWidget(d->m_dockContainer);
    QDockWidget::setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable |
                             QDockWidget::DockWidgetFloatable);

    bool native_window = true;

    // FloatingContainerForce*TitleBar is overwritten by the "QXDOCK_UseNativeTitle" environment variable if set.
    auto env = qgetenv("QXDOCK_UseNativeTitle").toUpper();
    if (env == "1") {
        native_window = true;
    } else if (env == "0") {
        native_window = false;
    } else if (DockManager::testConfigFlag(DockManager::FloatingContainerForceNativeTitleBar)) {
        native_window = true;
    } else if (DockManager::testConfigFlag(DockManager::FloatingContainerForceQWidgetTitleBar)) {
        native_window = false;
    } else {
        // KDE doesn't seem to fire MoveEvents while moving windows, so for now no native titlebar for everything using
        // KWin.
        QString window_manager = internal::windowManager().toUpper().split(" ")[0];
        native_window = window_manager != "KWIN";
    }

    if (native_window) {
        // Native windows do not work if wayland is used. Ubuntu 22.04 uses wayland by default. To use
        // native windows, switch to Xorg
        QString XdgSessionType = qgetenv("XDG_SESSION_TYPE").toLower();
        if ("wayland" == XdgSessionType) {
            native_window = false;
        }
    }

    if (native_window) {
        setTitleBarWidget(new QWidget());
        setWindowFlags(Qt::Window | Qt::WindowMaximizeButtonHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    } else {
        d->m_titleBar = new DockFloatingTitleBar(this);
        setTitleBarWidget(d->m_titleBar);
        setWindowFlags(Qt::Window | Qt::WindowMinMaxButtonsHint | Qt::FramelessWindowHint);
        d->m_titleBar->enableCloseButton(isClosable());
        connect(d->m_titleBar, SIGNAL(closeRequested()), SLOT(close()));
        connect(d->m_titleBar, &DockFloatingTitleBar::maximizeRequested, this,
                &DockFloatingContainer::onMaximizeRequest);
    }
#else
    setWindowFlags(Qt::Window | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    QBoxLayout *l = new QBoxLayout(QBoxLayout::TopToBottom);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    setLayout(l);
    l->addWidget(d->m_dockContainer);
#endif

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

bool DockFloatingContainer::hasTopLevelDockWidget() const
{
    Q_D(const DockFloatingContainer);
    return d->m_dockContainer->hasTopLevelDockWidget();
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

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)

void DockFloatingContainer::onMaximizeRequest()
{
    if (windowState() == Qt::WindowMaximized) {
        showNormal();
    } else {
        showMaximized();
    }
}

void DockFloatingContainer::showNormal(bool fixGeometry)
{
    Q_D(DockFloatingContainer);
    if ((windowState() & Qt::WindowMaximized) != 0 || (windowState() & Qt::WindowFullScreen) != 0) {
        QRect oldNormal = normalGeometry();
        Super::showNormal();
        if (fixGeometry) {
            setGeometry(oldNormal);
        }
    }
    if (d->m_titleBar) {
        d->m_titleBar->setMaximizedIcon(false);
    }
}

void DockFloatingContainer::showMaximized()
{
    Q_D(DockFloatingContainer);
    Super::showMaximized();
    if (d->m_titleBar) {
        d->m_titleBar->setMaximizedIcon(true);
    }
}

bool DockFloatingContainer::isMaximized() const
{
    return windowState() == Qt::WindowMaximized;
}

void DockFloatingContainer::show()
{
    // Prevent this window from showing in the taskbar and pager (alt+tab)
    internal::xcb_add_prop(true, winId(), "_NET_WM_STATE", "_NET_WM_STATE_SKIP_TASKBAR");
    internal::xcb_add_prop(true, winId(), "_NET_WM_STATE", "_NET_WM_STATE_SKIP_PAGER");
    Super::show();
}

bool DockFloatingContainer::hasNativeTitleBar()
{
    Q_D(DockFloatingContainer);
    return d->m_titleBar == nullptr;
}

#endif   // defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)

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
    Q_D(DockFloatingContainer);
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    if (!isMaximized()) {
        resize(size);
        d->m_dragStartMousePosition = dragStartMousePos;
    }
    d->setState(dragState);
    if (Qx::DockDraggingFloatingWidget == dragState) {
        d->m_mouseEventHandler = mouseEventHandler;
        if (d->m_mouseEventHandler) {
            d->m_mouseEventHandler->grabMouse();
        }
    }

    if (!isMaximized()) {
        moveFloating();
    }
    show();
#else
    Q_UNUSED(mouseEventHandler)
    resize(size);
    d->m_dragStartMousePosition = dragStartMousePos;
    d->setState(dragState);
    moveFloating();
    show();
#endif
}

void DockFloatingContainer::finishDragging()
{
    Q_D(DockFloatingContainer);
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    setWindowOpacity(1);
    activateWindow();
    if (d->m_mouseEventHandler) {
        d->m_mouseEventHandler->releaseMouse();
        d->m_mouseEventHandler = nullptr;
    }
#endif
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
#ifdef Q_OS_MACOS
        // In OSX when hiding the PanelOverlay the application would set
        // the main window as the active window for some reason. This fixes
        // that by resetting the active window to the floating widget after
        // updating the overlays.
        activateWindow();
#endif
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

bool DockFloatingContainer::restoreState(DockStateReader &stream, bool testing)
{
    Q_D(DockFloatingContainer);
    if (!d->m_dockContainer->restoreState(stream, testing)) {
        return false;
    }
    onDockAreasAddedOrRemoved();
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    if (d->m_titleBar) {
        d->m_titleBar->setMaximizedIcon(windowState() == Qt::WindowMaximized);
    }
#endif
    return true;
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
    // Prevent toogleView() events during restore state
    if (d->m_window->isRestoringState()) {
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
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    if (DockManager::testConfigFlag(DockManager::FocusHighlighting)) {
        this->window()->activateWindow();
    }
#endif
}

void DockFloatingContainer::changeEvent(QEvent *event)
{
    Super::changeEvent(event);
    Q_D(DockFloatingContainer);
    switch (event->type()) {
    case QEvent::ActivationChange:
        if (isActiveWindow()) {
            d->m_zOrderIndex = ++s_zOrderCounterFloating;
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
            if (d->m_draggingState == Qx::DockDraggingFloatingWidget) {
                d->titleMouseReleaseEvent();
                d->m_draggingState = Qx::DockDraggingInactive;
            }
#endif
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

#ifdef Q_OS_MACOS
//============================================================================
bool DockFloatingContainer::event(QEvent *e)
{
    Q_D(DockFloatingContainer);
    switch (d->m_draggingState) {
    case Qx::DockDraggingInactive: {
        // Normally we would check here, if the left mouse button is pressed.
        // But from QT version 5.12.2 on the mouse events from
        // QEvent::NonClientAreaMouseButtonPress return the wrong mouse button
        // The event always returns Qt::RightButton even if the left button
        // is clicked.
        // It is really great to work around the whole NonClientMouseArea
        // bugs
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 2))
        if (e->type() ==
            QEvent::NonClientAreaMouseButtonPress /*&& QGuiApplication::mouseButtons().testFlag(Qt::LeftButton)*/)
#else
        if (e->type() == QEvent::NonClientAreaMouseButtonPress &&
            QGuiApplication::mouseButtons().testFlag(Qt::LeftButton))
#endif
        {
            QXDOCK_PRINT("FloatingWidget::event Event::NonClientAreaMouseButtonPress" << e->type());
            d->m_dragStartPos = pos();
            d->setState(Qx::DockDraggingMousePressed);
        }
    } break;

    case Qx::DockDraggingMousePressed:
        switch (e->type()) {
        case QEvent::NonClientAreaMouseButtonDblClick:
            QXDOCK_PRINT("FloatingWidget::event QEvent::NonClientAreaMouseButtonDblClick");
            d->setState(Qx::DockDraggingInactive);
            break;

        case QEvent::Resize:
            // If the first event after the mouse press is a resize event, then
            // the user resizes the window instead of dragging it around.
            // But there is one exception. If the window is maximized,
            // then dragging the window via title bar will cause the widget to
            // leave the maximized state. This in turn will trigger a resize event.
            // To know, if the resize event was triggered by user via moving a
            // corner of the window frame or if it was caused by a windows state
            // change, we check, if we are not in maximized state.
            if (!isMaximized()) {
                d->setState(Qx::DockDraggingInactive);
            }
            break;

        default:
            break;
        }
        break;

    case Qx::DockDraggingFloatingWidget:
        if (e->type() == QEvent::NonClientAreaMouseButtonRelease) {
            QXDOCK_PRINT("FloatingWidget::event QEvent::NonClientAreaMouseButtonRelease");
            d->titleMouseReleaseEvent();
        }
        break;

    default:
        break;
    }

#if (QXDOCK_DEBUG_LEVEL > 0)
    qDebug() << QTime::currentTime() << "DockFloatingContainer::event " << e->type();
#endif
    return QWidget::event(e);
}

//============================================================================
void DockFloatingContainer::moveEvent(QMoveEvent *e)
{
    Q_D(DockFloatingContainer);
    QWidget::moveEvent(e);
    switch (d->m_draggingState) {
    case Qx::DockDraggingMousePressed:
        d->setState(Qx::DockDraggingFloatingWidget);
        d->updateDropOverlays(QCursor::pos());
        break;

    case Qx::DockDraggingFloatingWidget:
        d->updateDropOverlays(QCursor::pos());
        // In OSX when hiding the DockAreaOverlay the application would set
        // the main window as the active window for some reason. This fixes
        // that by resetting the active window to the floating widget after
        // updating the overlays.
        activateWindow();
        break;
    default:
        break;
    }
}
#endif

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
void DockFloatingContainer::moveEvent(QMoveEvent *e)
{
    Q_D(DockFloatingContainer);
    Super::moveEvent(e);
    if (!d->m_isResizing && e->spontaneous() && d->m_mousePressed) {
        d->setState(Qx::DockDraggingFloatingWidget);
        d->updateDropOverlays(QCursor::pos());
    }
    d->m_isResizing = false;
}

void DockFloatingContainer::resizeEvent(QResizeEvent *e)
{
    Q_D(DockFloatingContainer);
    d->m_isResizing = true;
    Super::resizeEvent(e);
}

bool DockFloatingContainer::event(QEvent *e)
{
    Q_D(DockFloatingContainer);
    bool result = Super::event(e);
    switch (e->type()) {
    case QEvent::WindowActivate:
        d->m_mousePressed = false;
        break;
    case QEvent::WindowDeactivate:
        d->m_mousePressed = true;
        break;
    default:
        break;
    }
    return result;
}

#endif   // defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)

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
