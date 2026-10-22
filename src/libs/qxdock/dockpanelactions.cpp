/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "dockpanel_p.h"

QX_DOCK_BEGIN_NAMESPACE

void DockPanel::closeArea()
{
    auto openWidgets = openedDockWidgets();
    if (openWidgets.count() == 1 &&
        (openWidgets[0]->features().testFlag(DockWidget::DockWidgetDeleteOnClose) ||
         openWidgets[0]->features().testFlag(DockWidget::CustomCloseHandling)) &&
        !isAutoHide()) {
        openWidgets[0]->closeDockWidgetInternal();
    } else {
        for (auto w : openWidgets) {
            if ((w->features().testFlag(DockWidget::DockWidgetDeleteOnClose) &&
                 w->features().testFlag(DockWidget::DockWidgetForceCloseWithArea)) ||
                w->features().testFlag(DockWidget::CustomCloseHandling)) {
                w->closeDockWidgetInternal();
            } else if (w->features().testFlag(DockWidget::DockWidgetDeleteOnClose) && isAutoHide()) {
                w->closeDockWidgetInternal();
            } else {
                w->toggleView(false);
            }
        }
    }
}

void DockPanel::closeOtherAreas()
{
    dockContainer()->closeOtherAreas(this);
}

/**
 * Sets the dock panel into auto hide mode or into normal mode.
 * If the dock panel is switched to auto hide mode, then all dock widgets
 * that are pinable will be added to the sidebar
 */
void DockPanel::setAutoHide(bool enable, Qx::DockSideBarArea area, int tabIndex)
{
    Q_D(DockPanel);
    if (!isAutoHideFeatureEnabled()) {
        return;
    }

    if (!enable) {
        if (isAutoHide()) {
            d->m_autoHideContainer->moveContentsToParent();
        }
        return;
    }

    // If this is already an auto hide container, then move it to new location
    if (isAutoHide()) {
        // Same DockSideBarNone handling as DockWidget::setAutoHide(): a call
        // that names no side leaves the panel on the side it is already on,
        // instead of asking the container for a side bar it never built.
        auto targetArea = (Qx::DockSideBarNone == area) ? d->m_autoHideContainer->sideBarArea() : area;
        d->m_autoHideContainer->moveToNewSideBarArea(targetArea, tabIndex);
        return;
    }

    auto location = (Qx::DockSideBarNone == area) ? calculateSideBarArea() : area;
    for (const auto w : openedDockWidgets()) {
        if (enable == isAutoHide()) {
            continue;
        }

        if (!w->features().testFlag(DockWidget::DockWidgetPinnable)) {
            continue;
        }

        dockContainer()->createAndSetupAutoHideContainer(location, w, tabIndex++);
    }
}

/**
 * Switches the dock panel to auto hide mode or vice versa depending on its
 * current state.
 */
void DockPanel::toggleAutoHide(Qx::DockSideBarArea area)
{
    if (!isAutoHideFeatureEnabled()) {
        return;
    }

    setAutoHide(!isAutoHide(), area);
}

void DockPanel::setFloating()
{
    Q_D(DockPanel);
    d->m_titleBar->setAreaFloating();
}

void DockPanel::toggleView(bool open)
{
    setVisible(open);
    Q_EMIT viewToggled(open);
}

void DockPanel::onTabCloseRequested(int index)
{
    dockWidget(index)->requestCloseDockWidget();
}

/**
 * Reorder the index position of DockWidget at fromIndx to toIndex
 * if a tab in the tabbar is dragged from one index to another one
 */
void DockPanel::reorderDockWidget(int fromIndex, int toIndex)
{
    Q_D(DockPanel);
    if (fromIndex >= d->m_contentsLayout->count() || fromIndex < 0 || toIndex >= d->m_contentsLayout->count() ||
        toIndex < 0 || fromIndex == toIndex) {
        return;
    }

    auto widget = d->m_contentsLayout->widget(fromIndex);
    d->m_contentsLayout->removeWidget(widget);
    d->m_contentsLayout->insertWidget(toIndex, widget);
    setCurrentIndex(toIndex);
}

enum BorderLocation {
    BorderNone = 0,
    BorderLeft = 0x01,
    BorderRight = 0x02,
    BorderTop = 0x04,
    BorderBottom = 0x08,
    BorderVertical = BorderLeft | BorderRight,
    BorderHorizontal = BorderTop | BorderBottom,
    BorderTopLeft = BorderTop | BorderLeft,
    BorderTopRight = BorderTop | BorderRight,
    BorderBottomLeft = BorderBottom | BorderLeft,
    BorderBottomRight = BorderBottom | BorderRight,
    BorderVerticalBottom = BorderVertical | BorderBottom,
    BorderVerticalTop = BorderVertical | BorderTop,
    BorderHorizontalLeft = BorderHorizontal | BorderLeft,
    BorderHorizontalRight = BorderHorizontal | BorderRight,
    BorderAll = BorderVertical | BorderHorizontal
};

/**
 * Calculate the auto hide side bar are depending on the dock area
 * widget position in the container
 */
Qx::DockSideBarArea DockPanel::calculateSideBarArea() const
{
    auto container = dockContainer();
    auto contentRect = container->contentRect();

    int borders = BorderNone;   // contains all borders that are touched by the dock ware
    auto dockAreaTopLeft = mapTo(container, rect().topLeft());
    auto dockAreaRect = rect();
    dockAreaRect.moveTo(dockAreaTopLeft);
    const qreal aspectRatio = dockAreaRect.width() / (qMax(1, dockAreaRect.height()) * 1.0);
    const qreal sizeRatio = (qreal)contentRect.width() / dockAreaRect.width();
    static const int minBorderDistance = 16;
    bool horizontalOrientation = (aspectRatio > 1.0) && (sizeRatio < 3.0);

    // measure border distances - a distance less than 16 px means we touch the border
    int borderDistance[4];

    int distance = qAbs(contentRect.topLeft().y() - dockAreaRect.topLeft().y());
    borderDistance[Qx::DockSideBarTop] = (distance < minBorderDistance) ? 0 : distance;
    if (!borderDistance[Qx::DockSideBarTop]) {
        borders |= BorderTop;
    }

    distance = qAbs(contentRect.bottomRight().y() - dockAreaRect.bottomRight().y());
    borderDistance[Qx::DockSideBarBottom] = (distance < minBorderDistance) ? 0 : distance;
    if (!borderDistance[Qx::DockSideBarBottom]) {
        borders |= BorderBottom;
    }

    distance = qAbs(contentRect.topLeft().x() - dockAreaRect.topLeft().x());
    borderDistance[Qx::DockSideBarLeft] = (distance < minBorderDistance) ? 0 : distance;
    if (!borderDistance[Qx::DockSideBarLeft]) {
        borders |= BorderLeft;
    }

    distance = qAbs(contentRect.bottomRight().x() - dockAreaRect.bottomRight().x());
    borderDistance[Qx::DockSideBarRight] = (distance < minBorderDistance) ? 0 : distance;
    if (!borderDistance[Qx::DockSideBarRight]) {
        borders |= BorderRight;
    }

    auto sideBarArea = Qx::DockSideBarRight;
    switch (borders) {
    // 1. It's touching all borders
    case BorderAll:
        sideBarArea = horizontalOrientation ? Qx::DockSideBarBottom : Qx::DockSideBarRight;
        break;

    // 2. It's touching 3 borders
    case BorderVerticalBottom:
        sideBarArea = Qx::DockSideBarBottom;
        break;
    case BorderVerticalTop:
        sideBarArea = Qx::DockSideBarTop;
        break;
    case BorderHorizontalLeft:
        sideBarArea = Qx::DockSideBarLeft;
        break;
    case BorderHorizontalRight:
        sideBarArea = Qx::DockSideBarRight;
        break;

    // 3. It's touching horizontal or vertical borders
    case BorderVertical:
        sideBarArea = Qx::DockSideBarBottom;
        break;
    case BorderHorizontal:
        sideBarArea = Qx::DockSideBarRight;
        break;

    // 4. It's in a corner
    case BorderTopLeft:
        sideBarArea = horizontalOrientation ? Qx::DockSideBarTop : Qx::DockSideBarLeft;
        break;
    case BorderTopRight:
        sideBarArea = horizontalOrientation ? Qx::DockSideBarTop : Qx::DockSideBarRight;
        break;
    case BorderBottomLeft:
        sideBarArea = horizontalOrientation ? Qx::DockSideBarBottom : Qx::DockSideBarLeft;
        break;
    case BorderBottomRight:
        sideBarArea = horizontalOrientation ? Qx::DockSideBarBottom : Qx::DockSideBarRight;
        break;

    // 5. It's touching only one border
    case BorderLeft:
        sideBarArea = Qx::DockSideBarLeft;
        break;
    case BorderRight:
        sideBarArea = Qx::DockSideBarRight;
        break;
    case BorderTop:
        sideBarArea = Qx::DockSideBarTop;
        break;
    case BorderBottom:
        sideBarArea = Qx::DockSideBarBottom;
        break;
    }

    return sideBarArea;
}

void DockPanel::onDockWidgetFeaturesChanged()
{
    Q_D(DockPanel);
    if (d->m_titleBar) {
        d->updateTitleBarButtonStates();
    }
}

DockAutoHideContainer *DockPanel::autoHideContainer() const
{
    Q_D(const DockPanel);
    return d->m_autoHideContainer;
}

void DockPanel::setAutoHideContainer(DockAutoHideContainer *container)
{
    Q_D(DockPanel);
    d->m_autoHideContainer = container;
    updateAutoHideButtonCheckState();
    updateTitleBarButtonsToolTips();

    auto button = d->m_titleBar->button(Qx::TitleBarButtonAutoHide);
    TitleBarButton *autoHideButton = qobject_cast<TitleBarButton *>(button);
    autoHideButton->setShowInTitleBar(true);
}

bool DockPanel::isAutoHide() const
{
    Q_D(const DockPanel);
    return d->m_autoHideContainer != nullptr;
}

/**
 * Returns the index of the first open dock widgets in the list of
 * dock widgets.
 * This function is here for performance reasons. Normally it would
 * be possible to take the first dock widget from the list returned by
 * openedDockWidgets() function. But that function enumerates all
 * dock widgets while this functions stops after the first open dock widget.
 * If there are no open dock widgets, the function returns -1.
 */
int DockPanel::indexOfFirstOpenDockWidget() const
{
    Q_D(const DockPanel);
    for (int i = 0; i < d->m_contentsLayout->count(); ++i) {
        DockWidget *w = dockWidget(i);
        if (w && !w->isClosed()) {
            return i;
        }
    }

    return -1;
}

void DockPanel::saveState(QXmlStreamWriter &s) const
{
    Q_D(const DockPanel);
    s.writeStartElement("Area");
    s.writeAttribute("Tabs", QString::number(d->m_contentsLayout->count()));
    auto currentDockWidget = this->currentDockWidget();
    QString name = currentDockWidget ? currentDockWidget->objectName() : "";
    s.writeAttribute("Current", name);

    if (d->m_allowedAreas != s_defaultAllowedAreas) {
        s.writeAttribute("AllowedAreas", QString::number(d->m_allowedAreas, 16));
    }

    if (d->m_flags != DefaultFlags) {
        s.writeAttribute("Flags", QString::number(d->m_flags, 16));
    }
    for (int i = 0; i < d->m_contentsLayout->count(); ++i) {
        dockWidget(i)->saveState(s);
    }
    s.writeEndElement();
}

bool DockPanel::restoreState(DockStateReader &s, DockPanel *&createdWidget, bool testing,
                             DockContainer *parentContainer)
{
    bool ok;

    QString currentDockWidget = s.attributes().value("Current").toString();

    auto dockWindow = parentContainer->dockWindow();
    DockPanel *panel = nullptr;
    if (!testing) {
        panel = new DockPanel(dockWindow, parentContainer);
        const auto allowedAreasAttribute = s.attributes().value("AllowedAreas");
        if (!allowedAreasAttribute.isEmpty()) {
            panel->setAllowedAreas((Qx::DockWidgetArea)allowedAreasAttribute.toInt(nullptr, 16));
        }

        const auto flagsAttribute = s.attributes().value("Flags");
        if (!flagsAttribute.isEmpty()) {
            panel->setDockAreaFlags((DockPanel::DockAreaFlags)flagsAttribute.toInt(nullptr, 16));
        }
    }

    while (s.readNextStartElement()) {
        if (s.name() != QLatin1String("Widget")) {
            continue;
        }

        auto objectName = s.attributes().value("Name");
        if (objectName.isEmpty()) {
            return false;
        }

        bool closed = s.attributes().value("Closed").toInt(&ok);
        if (!ok) {
            return false;
        }

        s.skipCurrentElement();
        DockWidget *dockWidget = dockWindow->findDockWidget(objectName.toString());
        if (!dockWidget || testing) {
            continue;
        }
        if (dockWidget->autoHideContainer()) {
            dockWidget->autoHideContainer()->cleanupAndDelete();
        }

        // We hide the panel here to prevent the short display (the flashing)
        // of the dock areas during application startup
        panel->hide();
        panel->addDockWidget(dockWidget);
        dockWidget->setToggleViewActionChecked(!closed);
        dockWidget->setClosedState(closed);
        dockWidget->setProperty(internal::ClosedProperty, closed);
        dockWidget->setProperty(internal::DirtyProperty, false);
    }

    if (testing) {
        return true;
    }

    if (!panel->dockWidgetsCount()) {
        delete panel;
        panel = nullptr;
    } else {
        panel->setProperty("currentDockWidget", currentDockWidget);
    }

    createdWidget = panel;
    return true;
}

/**
 * Returns the largest minimumSizeHint() of the dock widgets in this panel,
 * plus the height of the title bar when the panel has one. The minimum size
 * hint is updated if a dock widget is removed or added.
 */
QSize DockPanel::minimumSizeHint() const
{
    Q_D(const DockPanel);
    if (!d->m_minSizeHint.isValid()) {
        return Super::minimumSizeHint();
    }

    // Asked as "does the panel have a title bar", which is a question about
    // intent, so it takes isHidden() and not isVisible(). A panel inside a
    // window that has not been shown yet reports isVisible() false while the
    // title bar is very much there, and a layout that asks during that window
    // would get a bound a title bar too short for what it has to place.
    if (!d->m_titleBar->isHidden()) {
        return d->m_minSizeHint + QSize(0, d->m_titleBar->minimumSizeHint().height());
    } else {
        return d->m_minSizeHint;
    }
}

QX_DOCK_END_NAMESPACE
