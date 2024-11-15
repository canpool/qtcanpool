/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "docktab.h"
#include "dockwidget.h"
#include "dockpanel.h"
#include "docklabel.h"
#include "dockmanager.h"
#include "dockutils.h"
#include "dockfloatingcontainer.h"
#include "dockfloatingpreview.h"
#include "dockcontainer.h"
#include "dockwindow.h"
#include "dockoverlay.h"
#include "dockfocuscontroller.h"

#include <QBoxLayout>
#include <QToolButton>
#include <QPushButton>
#include <QMouseEvent>
#include <QApplication>
#include <QLabel>
#include <QMenu>
#include <QAction>

QX_DOCK_BEGIN_NAMESPACE

class DockTabPrivate
{
public:
    QX_DECLARE_PUBLIC(DockTab)
public:
    DockTabPrivate();
    void init();

    bool testConfigFlag(DockManager::ConfigFlag flag) const;
    QAbstractButton *createCloseButton() const;
    void updateCloseButtonSizePolicy();
    void updateCloseButtonVisibility(bool active);

    void saveDragStartMousePosition(const QPoint &globalPos);
    bool isDraggingState(Qx::DockDragState dragState) const;
    void moveTab(QMouseEvent *e);
    bool startFloating(Qx::DockDragState draggingState = Qx::DockDraggingFloatingWidget);

    template <typename T> DockFloatingWidget *createFloatingWidget(T *widget, bool needCreateContainer);
    DockFocusController *focusController() const;

    void updateIcon();
    QAction *createAutoHideToAction(const QString &title, Qx::DockSideBarArea area, QMenu *menu);
public:
    DockWidget *m_dockWidget = nullptr;
    DockPanel *m_panel = nullptr;
    DockLabel *m_label = nullptr;
    QLabel *m_iconLabel = nullptr;
    QAbstractButton *m_closeButton = nullptr;
    QIcon m_icon;
    QSize m_iconSize;
    bool m_isActive = false;

    QPoint m_globalDragStartMousePosition;
    QPoint m_dragStartMousePosition;
    QPoint m_tabDragStartPosition;
    Qx::DockDragState m_dragState = Qx::DockDraggingInactive;
    DockFloatingWidget *m_floatingWidget = nullptr;
};

DockTabPrivate::DockTabPrivate()
{
}

void DockTabPrivate::init()
{
    Q_Q(DockTab);

    m_label = new DockLabel();
    if (DockManager::testConfigFlag(DockManager::DisableTabTextEliding)) {
        m_label->setElideMode(Qt::ElideNone);
    } else {
        m_label->setElideMode(Qt::ElideRight);
    }
    m_label->setText(m_dockWidget->windowTitle());
    m_label->setObjectName("dockTabLabel");
    m_label->setAlignment(Qt::AlignCenter);
    q->connect(m_label, SIGNAL(elidedChanged(bool)), SIGNAL(elidedChanged(bool)));

    m_closeButton = createCloseButton();
    m_closeButton->setObjectName("dockTabCloseButton");
    internal::setButtonIcon(m_closeButton, QStyle::SP_TitleBarCloseButton, Qx::DockTabCloseIcon);
    m_closeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_closeButton->setFocusPolicy(Qt::NoFocus);
    updateCloseButtonSizePolicy();
    internal::setToolTip(m_closeButton, QObject::tr("Close Tab"));
    // DockTab and DockWidget are partners. When a DockWidget is created, a DockTab is created.
    // The associated DockTab can be obtained through the tab() interface of the DockWidget.
    //  closeButton -> clicked()
    //  DockTab -> closeRequested()
    //  DockTabBar -> onTabCloseRequested() -> tabCloseRequested(index)
    //  DockPanel -> onTabCloseRequested(index)
    //  DockWidget -> requestCloseDockWidget()
    q->connect(m_closeButton, SIGNAL(clicked()), SIGNAL(closeRequested()));

    QFontMetrics fm(m_label->font());
    int spacing = qRound(fm.height() / 4.0);

    // Fill the layout
    QBoxLayout *layout = new QBoxLayout(QBoxLayout::LeftToRight);
    layout->setContentsMargins(2 * spacing, 0, 0, 0);
    layout->setSpacing(0);
    q->setLayout(layout);

    layout->addWidget(m_label, 1);
    layout->addSpacing(spacing);
    layout->addWidget(m_closeButton);
    layout->addSpacing(qRound(spacing * 4.0 / 3.0));
    layout->setAlignment(Qt::AlignCenter);

    m_label->setVisible(true);
}

bool DockTabPrivate::testConfigFlag(DockManager::ConfigFlag flag) const
{
    return DockManager::testConfigFlag(flag);
}

QAbstractButton *DockTabPrivate::createCloseButton() const
{
    if (testConfigFlag(DockManager::TabCloseButtonIsToolButton)) {
        auto b = new QToolButton();
        b->setAutoRaise(true);
        return b;
    } else {
        return new QPushButton();
    }
}

void DockTabPrivate::updateCloseButtonSizePolicy()
{
    auto features = m_dockWidget->features();
    auto sizePolicy = m_closeButton->sizePolicy();
    sizePolicy.setRetainSizeWhenHidden(features.testFlag(DockWidget::DockWidgetClosable) &&
                                       testConfigFlag(DockManager::RetainTabSizeWhenCloseButtonHidden));
    m_closeButton->setSizePolicy(sizePolicy);
}

void DockTabPrivate::updateCloseButtonVisibility(bool active)
{
    bool dockWidgetClosable = m_dockWidget->features().testFlag(DockWidget::DockWidgetClosable);
    bool activeTabHasCloseButton = testConfigFlag(DockManager::ActiveTabHasCloseButton);
    bool allTabsHaveCloseButton = testConfigFlag(DockManager::AllTabsHaveCloseButton);
    bool tabHasCloseButton = (activeTabHasCloseButton && active) | allTabsHaveCloseButton;
    m_closeButton->setVisible(dockWidgetClosable && tabHasCloseButton);
}

void DockTabPrivate::saveDragStartMousePosition(const QPoint &globalPos)
{
    Q_Q(DockTab);
    m_globalDragStartMousePosition = globalPos;
    m_dragStartMousePosition = q->mapFromGlobal(globalPos);
}

bool DockTabPrivate::isDraggingState(Qx::DockDragState dragState) const
{
    return m_dragState == dragState;
}

void DockTabPrivate::moveTab(QMouseEvent *e)
{
    Q_Q(DockTab);
    e->accept();
    QPoint distance = internal::globalPositionOf(e) - m_globalDragStartMousePosition;
    distance.setY(0);
    auto targetPos = distance + m_tabDragStartPosition;
    targetPos.rx() = qMax(targetPos.x(), 0);
    targetPos.rx() = qMin(q->parentWidget()->rect().right() - q->width() + 1, targetPos.rx());
    q->move(targetPos);
    q->raise();
}

bool DockTabPrivate::startFloating(Qx::DockDragState draggingState)
{
    Q_Q(DockTab);
    auto container = m_dockWidget->dockContainer();
    // if this is the last dock widget inside of this floating widget,
    // then it does not make any sense, to make it floating because
    // it is already floating
    if (container->isFloating() && (container->visibleDockPanelCount() == 1) &&
        (m_dockWidget->dockPanel()->dockWidgetsCount() == 1)) {
        return false;
    }

    m_dragState = draggingState;
    DockFloatingWidget *floatingWidget = nullptr;
    bool needCreateContainer = (Qx::DockDraggingFloatingWidget != draggingState);

    // If section widget has multiple tabs, we take only one tab
    // If it has only one single tab, we can move the complete
    // dock area into floating widget
    QSize size;
    if (m_panel->dockWidgetsCount() > 1) {
        floatingWidget = createFloatingWidget(m_dockWidget, needCreateContainer);
        size = m_dockWidget->size();
    } else {
        floatingWidget = createFloatingWidget(m_panel, needCreateContainer);
        size = m_panel->size();
    }

    if (Qx::DockDraggingFloatingWidget == draggingState) {
        floatingWidget->startFloating(m_dragStartMousePosition, size, Qx::DockDraggingFloatingWidget, q);
        auto window = m_dockWidget->dockWindow();
        auto overlay = window->containerOverlay();
        overlay->setAllowedAreas(Qx::OuterDockAreas);
        this->m_floatingWidget = floatingWidget;
        qApp->postEvent(m_dockWidget, new QEvent((QEvent::Type)internal::DockedWidgetDragStartEvent));
    } else {
        floatingWidget->startFloating(m_dragStartMousePosition, size, Qx::DockDraggingInactive, nullptr);
    }

    return true;
}

DockFocusController *DockTabPrivate::focusController() const
{
    return m_dockWidget->dockWindow()->dockFocusController();
}

void DockTabPrivate::updateIcon()
{
    Q_Q(DockTab);
    if (!m_iconLabel || m_icon.isNull()) {
        return;
    }

    if (m_iconSize.isValid()) {
        m_iconLabel->setPixmap(m_icon.pixmap(m_iconSize));
    } else {
        m_iconLabel->setPixmap(m_icon.pixmap(q->style()->pixelMetric(QStyle::PM_SmallIconSize, nullptr, q)));
    }
    m_iconLabel->setVisible(true);
}

QAction *DockTabPrivate::createAutoHideToAction(const QString &title, Qx::DockSideBarArea area, QMenu *menu)
{
    Q_Q(DockTab);
    auto action = menu->addAction(title);
    action->setProperty(internal::LocationProperty, area);
    QObject::connect(action, &QAction::triggered, q, &DockTab::onAutoHideToActionClicked);
    return action;
}

template <typename T> DockFloatingWidget *DockTabPrivate::createFloatingWidget(T *widget, bool needCreateContainer)
{
    Q_Q(DockTab);
    if (needCreateContainer) {
        return new DockFloatingContainer(widget);
    } else {
        auto w = new DockFloatingPreview(widget);
        q->connect(w, &DockFloatingPreview::draggingCanceled, [=]() {
            m_dragState = Qx::DockDraggingInactive;
        });
        return w;
    }
}

/* DockTab */
DockTab::DockTab(DockWidget *w, QWidget *parent)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(DockTab);

    Q_D(DockTab);
    d->m_dockWidget = w;
    d->init();

    setAttribute(Qt::WA_NoMousePropagation, true);
    setFocusPolicy(Qt::NoFocus);
}

DockTab::~DockTab()
{
    QX_FINI_PRIVATE();
}

bool DockTab::isActive() const
{
    Q_D(const DockTab);
    return d->m_isActive;
}

void DockTab::setActive(bool active)
{
    Q_D(DockTab);
    d->updateCloseButtonVisibility(active);
    if (DockManager::testConfigFlag(DockManager::ShowTabTextOnlyForActiveTab) && !d->m_icon.isNull()) {
        if (active) {
            d->m_label->setVisible(true);
        } else {
            d->m_label->setVisible(false);
        }
    }
    // Focus related stuff
    if (DockManager::testConfigFlag(DockManager::FocusHighlighting)) {
        bool updateFocusStyle = false;
        if (active && !hasFocus()) {
            // setFocus(Qt::OtherFocusReason);
            d->focusController()->setDockWidgetTabFocused(this);
            updateFocusStyle = true;
        }

        if (d->m_isActive == active) {
            if (updateFocusStyle) {
                updateStyle();
            }
            return;
        }
    } else if (d->m_isActive == active) {
        return;
    }

    d->m_isActive = active;
    updateStyle();
    update();
    updateGeometry();
    Q_EMIT activeTabChanged();
}

void DockTab::setDockPanel(DockPanel *panel)
{
    Q_D(DockTab);
    d->m_panel = panel;
}

DockPanel *DockTab::dockPanel() const
{
    Q_D(const DockTab);
    return d->m_panel;
}

DockWidget *DockTab::dockWidget() const
{
    Q_D(const DockTab);
    return d->m_dockWidget;
}

const QIcon &DockTab::icon() const
{
    Q_D(const DockTab);
    return d->m_icon;
}

void DockTab::setIcon(const QIcon &icon)
{
    Q_D(DockTab);
    QBoxLayout *lay = qobject_cast<QBoxLayout *>(layout());
    if (!d->m_iconLabel && icon.isNull()) {
        return;
    }

    if (!d->m_iconLabel) {
        d->m_iconLabel = new QLabel();
        d->m_iconLabel->setAlignment(Qt::AlignVCenter);
        d->m_iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        internal::setToolTip(d->m_iconLabel, d->m_label->toolTip());
        lay->insertWidget(0, d->m_iconLabel, Qt::AlignVCenter);
        lay->insertSpacing(1, qRound(1.5 * lay->contentsMargins().left() / 2.0));
    } else if (icon.isNull()) {
        // Remove icon label and spacer item
        lay->removeWidget(d->m_iconLabel);
        lay->removeItem(lay->itemAt(0));
        delete d->m_iconLabel;
        d->m_iconLabel = nullptr;
    }

    d->m_icon = icon;
    d->updateIcon();
}

QSize DockTab::iconSize() const
{
    Q_D(const DockTab);
    return d->m_iconSize;
}

void DockTab::setIconSize(const QSize &size)
{
    Q_D(DockTab);
    d->m_iconSize = size;
    d->updateIcon();
}

QString DockTab::text() const
{
    Q_D(const DockTab);
    return d->m_label->text();
}

void DockTab::setText(const QString &title)
{
    Q_D(DockTab);
    d->m_label->setText(title);
}

bool DockTab::isTitleElided() const
{
    Q_D(const DockTab);
    return d->m_label->isElided();
}

bool DockTab::isClosable() const
{
    Q_D(const DockTab);
    return d->m_dockWidget && d->m_dockWidget->features().testFlag(DockWidget::DockWidgetClosable);
}

void DockTab::updateStyle()
{
    internal::repolishStyle(this, internal::RepolishDirectChildren);
}

void DockTab::detachDockWidget()
{
    Q_D(DockTab);
    if (!d->m_dockWidget->features().testFlag(DockWidget::DockWidgetFloatable)) {
        return;
    }

    d->saveDragStartMousePosition(QCursor::pos());
    d->startFloating(Qx::DockDraggingInactive);
}

void DockTab::autoHideDockWidget()
{
    Q_D(DockTab);
    d->m_dockWidget->setAutoHide(true);
}

void DockTab::onAutoHideToActionClicked()
{
    Q_D(DockTab);
    int location = sender()->property(internal::LocationProperty).toInt();
    d->m_dockWidget->toggleAutoHide((Qx::DockSideBarArea)location);
}

void DockTab::onDockWidgetFeaturesChanged()
{
    Q_D(DockTab);
    d->updateCloseButtonSizePolicy();
    d->updateCloseButtonVisibility(isActive());
}

void DockTab::mousePressEvent(QMouseEvent *e)
{
    Q_D(DockTab);
    if (e->button() == Qt::LeftButton) {
        e->accept();
        d->saveDragStartMousePosition(internal::globalPositionOf(e));
        d->m_dragState = Qx::DockDraggingMousePressed;
        if (DockManager::testConfigFlag(DockManager::FocusHighlighting)) {
            d->focusController()->setDockWidgetTabPressed(true);
            d->focusController()->setDockWidgetTabFocused(this);
        }
        Q_EMIT clicked();
        return;
    }
    Super::mousePressEvent(e);
}

void DockTab::mouseReleaseEvent(QMouseEvent *e)
{
    Q_D(DockTab);
    if (e->button() == Qt::LeftButton) {
        auto currentDragState = d->m_dragState;
        d->m_globalDragStartMousePosition = QPoint();
        d->m_dragStartMousePosition = QPoint();
        d->m_dragState = Qx::DockDraggingInactive;

        switch (currentDragState) {
        case Qx::DockDraggingTab:
            // End of tab moving, emit signal
            if (d->m_panel) {
                e->accept();
                Q_EMIT moved(internal::globalPositionOf(e));
            }
            break;

        case Qx::DockDraggingFloatingWidget:
            e->accept();
            d->m_floatingWidget->finishDragging();
            break;

        default:
            break;
        }
        if (DockManager::testConfigFlag(DockManager::FocusHighlighting)) {
            d->focusController()->setDockWidgetTabPressed(false);
        }
    } else if (e->button() == Qt::MiddleButton) {
        if (DockManager::testConfigFlag(DockManager::MiddleMouseButtonClosesTab) &&
            d->m_dockWidget->features().testFlag(DockWidget::DockWidgetClosable)) {
            // Only attempt to close if the mouse is still
            // on top of the widget, to allow the user to cancel.
            if (rect().contains(mapFromGlobal(QCursor::pos()))) {
                e->accept();
                Q_EMIT closeRequested();
            }
        }
    }
    Super::mouseReleaseEvent(e);
}

void DockTab::mouseMoveEvent(QMouseEvent *e)
{
    Q_D(DockTab);
    if (!(e->buttons() & Qt::LeftButton) || d->isDraggingState(Qx::DockDraggingInactive)) {
        d->m_dragState = Qx::DockDraggingInactive;
        Super::mouseMoveEvent(e);
        return;
    }

    // move floating window
    if (d->isDraggingState(Qx::DockDraggingFloatingWidget)) {
        d->m_floatingWidget->moveFloating();
        Super::mouseMoveEvent(e);
        return;
    }

    // move tab
    if (d->isDraggingState(Qx::DockDraggingTab)) {
        // Moving the tab is always allowed because it does not mean moving the
        // dock widget around
        d->moveTab(e);
    }

    auto mappedPos = mapToParent(e->pos());
    bool mouseOutsideBar = (mappedPos.x() < 0) || (mappedPos.x() > parentWidget()->rect().right());
    // Maybe a fixed drag distance is better here ?
    int dragDistanceY = qAbs(d->m_globalDragStartMousePosition.y() - internal::globalPositionOf(e).y());
    if (dragDistanceY >= DockManager::startDragDistance() || mouseOutsideBar) {
        // If this is the last dock area in a dock container with only
        // one single dock widget it does not make  sense to move it to a new
        // floating widget and leave this one empty
        if (d->m_panel->dockContainer()->isFloating() && d->m_panel->openDockWidgetsCount() == 1 &&
            d->m_panel->dockContainer()->visibleDockPanelCount() == 1) {
            return;
        }

        // Floating is only allowed for widgets that are floatable
        // We can create the drag preview if the widget is movable.
        auto features = d->m_dockWidget->features();
        if (features.testFlag(DockWidget::DockWidgetFloatable) || (features.testFlag(DockWidget::DockWidgetMovable))) {
            // If we undock, we need to restore the initial position of this
            // tab because it looks strange if it remains on its dragged position
            if (d->isDraggingState(Qx::DockDraggingTab)) {
                parentWidget()->layout()->update();
            }
            d->startFloating();
        }
        return;
    } else if (d->m_panel->openDockWidgetsCount() > 1 &&
               (internal::globalPositionOf(e) - d->m_globalDragStartMousePosition).manhattanLength() >=
                   QApplication::startDragDistance())   // Wait a few pixels before start moving
    {
        // If we start dragging the tab, we save its initial position to
        // restore it later
        if (Qx::DockDraggingTab != d->m_dragState) {
            d->m_tabDragStartPosition = this->pos();
        }
        d->m_dragState = Qx::DockDraggingTab;
        return;
    }

    Super::mouseMoveEvent(e);
}

/**
 * Double clicking the tab widget makes the assigned dock widget floating
 */
void DockTab::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_D(DockTab);
    if (e->button() == Qt::LeftButton) {
        // If this is the last dock panel in a dock container it does not make
        // sense to move it to a new floating widget and leave this one empty
        if ((!d->m_panel->dockContainer()->isFloating() || d->m_panel->dockWidgetsCount() > 1) &&
            d->m_dockWidget->features().testFlag(DockWidget::DockWidgetFloatable)) {
            e->accept();
            d->saveDragStartMousePosition(internal::globalPositionOf(e));
            d->startFloating(Qx::DockDraggingInactive);
        }
    }

    Super::mouseDoubleClickEvent(e);
}

void DockTab::contextMenuEvent(QContextMenuEvent *e)
{
    Q_D(DockTab);
    e->accept();
    if (d->isDraggingState(Qx::DockDraggingFloatingWidget)) {
        return;
    }

    d->saveDragStartMousePosition(e->globalPos());

    const bool isFloatable = d->m_dockWidget->features().testFlag(DockWidget::DockWidgetFloatable);
    const bool isNotOnlyTabInContainer = !d->m_panel->dockContainer()->hasTopLevelDockWidget();
    const bool isTopLevelArea = d->m_panel->isTopLevelArea();
    const bool isDetachable = isFloatable && isNotOnlyTabInContainer;
    QAction *action;
    QMenu menu(this);

    if (!isTopLevelArea) {
        action = menu.addAction(tr("Detach"), this, SLOT(detachDockWidget()));
        action->setEnabled(isDetachable);
        if (DockManager::testAutoHideConfigFlag(DockManager::AutoHideFeatureEnabled)) {
            action = menu.addAction(tr("Pin"), this, SLOT(autoHideDockWidget()));
            auto isPinnable = d->m_dockWidget->features().testFlag(DockWidget::DockWidgetPinnable);
            action->setEnabled(isPinnable);

            auto pinMenu = menu.addMenu(tr("Pin To..."));
            pinMenu->setEnabled(isPinnable);
            d->createAutoHideToAction(tr("Top"), Qx::DockSideBarTop, pinMenu);
            d->createAutoHideToAction(tr("Left"), Qx::DockSideBarLeft, pinMenu);
            d->createAutoHideToAction(tr("Right"), Qx::DockSideBarRight, pinMenu);
            d->createAutoHideToAction(tr("Bottom"), Qx::DockSideBarBottom, pinMenu);
        }
    }

    menu.addSeparator();
    action = menu.addAction(tr("Close"), this, SIGNAL(closeRequested()));
    action->setEnabled(isClosable());
    if (d->m_panel->openDockWidgetsCount() > 1) {
        action = menu.addAction(tr("Close Others"), this, SIGNAL(closeOtherTabsRequested()));
    }
    menu.exec(e->globalPos());
}

QX_DOCK_END_NAMESPACE
