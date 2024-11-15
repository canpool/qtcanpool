/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockfloatingpreview.h"
#include "dockpanel.h"
#include "dockwindow.h"
#include "dockwidget.h"
#include "dockmanager.h"
#include "dockoverlay.h"
#include "dockcontainer.h"
#include "dockfloatingcontainer.h"
#include "dockautohidecontainer.h"
#include "dockutils.h"

#include <QApplication>
#include <QKeyEvent>
#include <QPainter>

QX_DOCK_BEGIN_NAMESPACE

class DockFloatingPreviewPrivate
{
public:
    QX_DECLARE_PUBLIC(DockFloatingPreview)
public:
    DockFloatingPreviewPrivate();

    void updateDropOverlays(const QPoint &globalPos);
    void setHidden(bool Value);

    void cancelDragging();
    void createFloatingWidget();
    bool isContentFloatable() const;
    bool isContentPinnable() const;

    DockWidget::DockWidgetFeatures contentFeatures() const;
public:
    QWidget *m_content = nullptr;
    DockPanel *m_contentSourcePanel = nullptr;
    QPoint m_dragStartMousePosition;
    DockWindow *m_window = nullptr;
    DockContainer *m_dropContainer = nullptr;
    DockWidget::DockWidgetFeatures m_contentFeatures;
    QPixmap m_contentPreviewPixmap;
    bool m_canceled = false;
    bool m_hidden = false;
};

DockFloatingPreviewPrivate::DockFloatingPreviewPrivate()
{
}

void DockFloatingPreviewPrivate::updateDropOverlays(const QPoint &globalPos)
{
    Q_Q(DockFloatingPreview);
    if (!q->isVisible() || !m_window) {
        return;
    }

    auto containers = m_window->dockContainers();
    DockContainer *topContainer = nullptr;
    for (auto container : containers) {
        if (!container->isVisible()) {
            continue;
        }

        QPoint mappedPos = container->mapFromGlobal(globalPos);
        if (container->rect().contains(mappedPos)) {
            if (!topContainer || container->isInFrontOf(topContainer)) {
                topContainer = container;
            }
        }
    }

    m_dropContainer = topContainer;
    auto containerOverlay = m_window->containerOverlay();
    auto panelOverlay = m_window->panelOverlay();

    if (!topContainer) {
        containerOverlay->hideOverlay();
        panelOverlay->hideOverlay();
        if (DockManager::testConfigFlag(DockManager::DragPreviewIsDynamic)) {
            setHidden(false);
        }
        return;
    }

    auto panelDropArea = panelOverlay->dropAreaUnderCursor();
    auto containerDropArea = containerOverlay->dropAreaUnderCursor();

    int visibleDockpanels = topContainer->visibleDockPanelCount();

    // Include the overlay widget we're dragging as a visible widget
    auto panel = qobject_cast<DockPanel *>(m_content);
    if (panel && panel->isAutoHide()) {
        visibleDockpanels++;
    }

    Qx::DockWidgetAreas allowedContainerAreas = (visibleDockpanels > 1) ? Qx::OuterDockAreas : Qx::AllDockAreas;
    auto topPanel = topContainer->dockPanelAt(globalPos);
    // If the dock container contains only one single panel, then we need
    // to respect the allowed areas - only the center area is relevant here because
    // all other allowed areas are from the container
    if (visibleDockpanels == 1 && topPanel) {
        allowedContainerAreas.setFlag(Qx::CenterDockWidgetArea,
                                      topPanel->allowedAreas().testFlag(Qx::CenterDockWidgetArea));
    }

    if (isContentPinnable()) {
        allowedContainerAreas |= Qx::AutoHideDockAreas;
    }

    containerOverlay->setAllowedAreas(allowedContainerAreas);
    containerOverlay->enableDropPreview(containerDropArea != Qx::InvalidDockWidgetArea);
    if (topPanel && topPanel->isVisible() && visibleDockpanels >= 0 && topPanel != m_contentSourcePanel) {
        panelOverlay->enableDropPreview(true);
        panelOverlay->setAllowedAreas((visibleDockpanels == 1) ? Qx::NoDockWidgetArea : topPanel->allowedAreas());
        Qx::DockWidgetArea area = panelOverlay->showOverlay(topPanel);

        // A CenterDockWidgetArea for the panelOverlay() indicates that
        // the mouse is in the title bar. If the ContainerArea is valid
        // then we ignore the dock area of the panelOverlay() and disable
        // the drop preview
        if ((area == Qx::CenterDockWidgetArea) && (containerDropArea != Qx::InvalidDockWidgetArea)) {
            panelOverlay->enableDropPreview(false);
            containerOverlay->enableDropPreview(true);
        } else {
            containerOverlay->enableDropPreview(Qx::InvalidDockWidgetArea == area);
        }
        containerOverlay->showOverlay(topContainer);
    } else {
        panelOverlay->hideOverlay();
        // If there is only one single visible dock area in a container, then
        // it does not make sense to show a dock overlay because the dock area
        // would be removed and inserted at the same position. Only auto hide
        // area is allowed
        if (visibleDockpanels == 1) {
            containerOverlay->setAllowedAreas(Qx::AutoHideDockAreas);
        }
        containerOverlay->showOverlay(topContainer);

        if (topPanel == m_contentSourcePanel && Qx::InvalidDockWidgetArea == containerDropArea) {
            m_dropContainer = nullptr;
        }
    }

    if (DockManager::testConfigFlag(DockManager::DragPreviewIsDynamic)) {
        setHidden(panelDropArea != Qx::InvalidDockWidgetArea || containerDropArea != Qx::InvalidDockWidgetArea);
    }
}

void DockFloatingPreviewPrivate::setHidden(bool Value)
{
    Q_Q(DockFloatingPreview);
    m_hidden = Value;
    q->update();
}

void DockFloatingPreviewPrivate::cancelDragging()
{
    Q_Q(DockFloatingPreview);
    m_canceled = true;
    Q_EMIT q->draggingCanceled();
    m_window->containerOverlay()->hideOverlay();
    m_window->panelOverlay()->hideOverlay();
    q->close();
}

void DockFloatingPreviewPrivate::createFloatingWidget()
{
    Q_Q(DockFloatingPreview);
    DockWidget *widget = qobject_cast<DockWidget *>(m_content);
    DockPanel *panel = qobject_cast<DockPanel *>(m_content);

    DockFloatingContainer *floatingWidget = nullptr;

    if (widget && widget->features().testFlag(DockWidget::DockWidgetFloatable)) {
        floatingWidget = new DockFloatingContainer(widget);
    } else if (panel && panel->features().testFlag(DockWidget::DockWidgetFloatable)) {
        floatingWidget = new DockFloatingContainer(panel);
    }

    if (floatingWidget) {
        floatingWidget->setGeometry(q->geometry());
        floatingWidget->show();
        if (!DockManager::testConfigFlag(DockManager::DragPreviewHasWindowFrame)) {
            QApplication::processEvents();
            int FrameHeight = floatingWidget->frameGeometry().height() - floatingWidget->geometry().height();
            QRect FixedGeometry = q->geometry();
            FixedGeometry.adjust(0, FrameHeight, 0, 0);
            floatingWidget->setGeometry(FixedGeometry);
        }
    }
}

bool DockFloatingPreviewPrivate::isContentFloatable() const
{
    return m_contentFeatures.testFlag(DockWidget::DockWidgetFloatable);
}

bool DockFloatingPreviewPrivate::isContentPinnable() const
{
    return m_contentFeatures.testFlag(DockWidget::DockWidgetPinnable);
}

DockWidget::DockWidgetFeatures DockFloatingPreviewPrivate::contentFeatures() const
{
    DockPanel *panel = qobject_cast<DockPanel *>(m_content);
    if (panel) {
        return panel->features();
    }

    return DockWidget::DockWidgetFeatures();
}

DockFloatingPreview::DockFloatingPreview(DockPanel *content)
    : DockFloatingPreview(content, content->dockWindow())
{
    Q_D(DockFloatingPreview);
    d->m_window = content->dockWindow();
    d->m_contentSourcePanel = content;
    setWindowTitle(content->currentDockWidget()->windowTitle());
}

DockFloatingPreview::DockFloatingPreview(DockWidget *content)
    : DockFloatingPreview(content, content->dockWindow())
{
    Q_D(DockFloatingPreview);
    d->m_window = content->dockWindow();
    if (content->dockPanel()->openDockWidgetsCount() == 1) {
        d->m_contentSourcePanel = content->dockPanel();
    }
    setWindowTitle(content->windowTitle());
}

DockFloatingPreview::DockFloatingPreview(QWidget *content, QWidget *parent)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(DockFloatingPreview);

    Q_D(DockFloatingPreview);
    d->m_content = content;
    d->m_contentFeatures = d->contentFeatures();

    setAttribute(Qt::WA_DeleteOnClose);
    if (DockManager::testConfigFlag(DockManager::DragPreviewHasWindowFrame)) {
        setWindowFlags(Qt::Window | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    } else {
        setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
        setAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_TranslucentBackground);
    }

    // Create a static image of the widget that should get undocked
    // This is like some kind preview image like it is uses in drag and drop
    // operations
    if (DockManager::testConfigFlag(DockManager::DragPreviewShowsContentPixmap)) {
        d->m_contentPreviewPixmap = QPixmap(content->size());
        content->render(&d->m_contentPreviewPixmap);
    }

    connect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)),
            SLOT(onApplicationStateChanged(Qt::ApplicationState)));

    // The only safe way to receive escape key presses is to install an event
    // filter for the application object
    qApp->installEventFilter(this);
}

DockFloatingPreview::~DockFloatingPreview()
{
    QX_FINI_PRIVATE();
}

void DockFloatingPreview::startFloating(const QPoint &dragStartMousePos, const QSize &size, Qx::DockDragState dragState,
                                        QWidget *mouseEventHandler)
{
    Q_UNUSED(mouseEventHandler)
    Q_UNUSED(dragState)
    Q_D(DockFloatingPreview);
    resize(size);
    d->m_dragStartMousePosition = dragStartMousePos;
    moveFloating();
    show();
}

void DockFloatingPreview::finishDragging()
{
    Q_D(DockFloatingPreview);
    auto panelDropArea = d->m_window->panelOverlay()->visibleDropAreaUnderCursor();
    auto containerDropArea = d->m_window->containerOverlay()->visibleDropAreaUnderCursor();

    bool validDropArea =
        (panelDropArea != Qx::InvalidDockWidgetArea) || (containerDropArea != Qx::InvalidDockWidgetArea);

    // Non floatable auto hide widgets should stay in its current auto hide
    // state if they are dragged into a floating window
    if (validDropArea || d->isContentFloatable()) {
        cleanupAutoHideContainerWidget(containerDropArea);
    }

    if (!d->m_dropContainer) {
        d->createFloatingWidget();
    } else if (panelDropArea != Qx::InvalidDockWidgetArea) {
        d->m_dropContainer->dropWidget(d->m_content, panelDropArea, d->m_dropContainer->dockPanelAt(QCursor::pos()),
                                       d->m_window->panelOverlay()->tabIndexUnderCursor());
    } else if (containerDropArea != Qx::InvalidDockWidgetArea) {
        DockPanel *dropPanel = nullptr;
        // If there is only one single dock area, and we drop into the center
        // then we tabify the dropped widget into the only visible dock area
        if (d->m_dropContainer->visibleDockPanelCount() <= 1 && Qx::CenterDockWidgetArea == containerDropArea) {
            dropPanel = d->m_dropContainer->dockPanelAt(QCursor::pos());
        }

        d->m_dropContainer->dropWidget(d->m_content, containerDropArea, dropPanel,
                                       d->m_window->containerOverlay()->tabIndexUnderCursor());
    } else {
        d->createFloatingWidget();
    }

    this->close();
    d->m_window->containerOverlay()->hideOverlay();
    d->m_window->panelOverlay()->hideOverlay();
}

void DockFloatingPreview::moveFloating()
{
    Q_D(DockFloatingPreview);
    int borderSize = (frameSize().width() - size().width()) / 2;
    const QPoint moveToPos = QCursor::pos() - d->m_dragStartMousePosition - QPoint(borderSize, 0);
    move(moveToPos);
    d->updateDropOverlays(QCursor::pos());
}

void DockFloatingPreview::cleanupAutoHideContainerWidget(Qx::DockWidgetArea containerDropArea)
{
    Q_D(DockFloatingPreview);
    auto droppedDockWidget = qobject_cast<DockWidget *>(d->m_content);
    auto droppedPanel = qobject_cast<DockPanel *>(d->m_content);
    auto autoHideContainer =
        droppedDockWidget ? droppedDockWidget->autoHideContainer() : droppedPanel->autoHideContainer();

    if (!autoHideContainer) {
        return;
    }

    // If the dropped widget is already an auto hide widget and if it is moved
    // to a new side bar location in the same container, then we do not need
    // to cleanup
    if (internal::isSideBarArea(containerDropArea) && (d->m_dropContainer == autoHideContainer->dockContainer())) {
        return;
    }

    autoHideContainer->cleanupAndDelete();
}

void DockFloatingPreview::onApplicationStateChanged(Qt::ApplicationState state)
{
    Q_D(DockFloatingPreview);
    if (state != Qt::ApplicationActive) {
        disconnect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)), this,
                   SLOT(onApplicationStateChanged(Qt::ApplicationState)));
        d->cancelDragging();
    }
}

bool DockFloatingPreview::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(DockFloatingPreview);
    Q_UNUSED(watched);
    if (!d->m_canceled && event->type() == QEvent::KeyPress) {
        QKeyEvent *e = static_cast<QKeyEvent *>(event);
        if (e->key() == Qt::Key_Escape) {
            watched->removeEventFilter(this);
            d->cancelDragging();
        }
    }

    return false;
}

void DockFloatingPreview::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    Q_D(DockFloatingPreview);
    if (d->m_hidden) {
        return;
    }

    QPainter painter(this);
    painter.setOpacity(0.6);
    if (DockManager::testConfigFlag(DockManager::DragPreviewShowsContentPixmap)) {
        painter.drawPixmap(QPoint(0, 0), d->m_contentPreviewPixmap);
    }

    // If we do not have a window frame then we paint a QRubberBand like
    // frameless window
    if (!DockManager::testConfigFlag(DockManager::DragPreviewHasWindowFrame)) {
        QColor color = palette().color(QPalette::Active, QPalette::Highlight);
        QPen pen = painter.pen();
        pen.setColor(color.darker(120));
        pen.setStyle(Qt::SolidLine);
        pen.setWidth(1);
        pen.setCosmetic(true);
        painter.setPen(pen);
        color = color.lighter(130);
        color.setAlpha(64);
        painter.setBrush(color);
        painter.drawRect(rect().adjusted(0, 0, -1, -1));
    }
}

QX_DOCK_END_NAMESPACE
