/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 * **/
#include "dockwidget_p.h"
#include "docktab.h"
#include "docksidetab.h"

#include <QAction>
#include <QToolBar>

QX_DOCK_BEGIN_NAMESPACE

QList<QAction *> DockWidget::titleBarActions() const
{
    Q_D(const DockWidget);
    return d->m_titleBarActions;
}

/**
 * Set the actions that will be shown in the dock title bar
 * if this dock widget is the active tab.
 * You should not add to many actions to the title bar, because this
 * will remove the available space for the tabs. If you have a number
 * of actions, just add an action with a menu to show a popup menu
 * button in the title bar.
 */
void DockWidget::setTitleBarActions(QList<QAction *> actions)
{
    Q_D(DockWidget);
    d->m_titleBarActions = actions;
}

/**
 * Get the minimum size hint mode configured by setMinimumSizeHintMode
 */
DockWidget::MinimumSizeHintMode DockWidget::minimumSizeHintMode() const
{
    Q_D(const DockWidget);
    return d->m_minimumSizeHintMode;
}

/**
 * Configures the minimum size hint that is returned by the
 * minimumSizeHint() function.
 * \see MinimumSizeHintMode for a detailed description
 */
void DockWidget::setMinimumSizeHintMode(DockWidget::MinimumSizeHintMode mode)
{
    Q_D(DockWidget);
    d->m_minimumSizeHintMode = mode;
}

QSize DockWidget::minimumSizeHint() const
{
    Q_D(const DockWidget);
    if (!d->m_widget) {
        return QSize(60, 40);
    }

    switch (d->m_minimumSizeHintMode) {
    case MinimumSizeHintFromDockWidget:
        return QSize(60, 40);
    case MinimumSizeHintFromContent:
        return d->m_widget->minimumSizeHint();
    case MinimumSizeHintFromDockWidgetMinimumSize:
        return minimumSize();
    case MinimumSizeHintFromContentMinimumSize:
        return d->m_widget->minimumSize();
    }

    return d->m_widget->minimumSizeHint();
}

QIcon DockWidget::icon() const
{
    Q_D(const DockWidget);
    return d->m_tab->icon();
}

void DockWidget::setIcon(const QIcon &icon)
{
    Q_D(DockWidget);
    d->m_tab->setIcon(icon);

    if (d->m_sideTab) {
        d->m_sideTab->setIcon(icon);
    }

    if (!d->m_toggleViewAction->isCheckable()) {
        d->m_toggleViewAction->setIcon(icon);
    }
}

/**
 * This function returns the dock widget top tool bar.
 * a default empty toolbar is assigned or you need to assign your custom
 * toolbar via setToolBar().
 */
QToolBar *DockWidget::toolBar() const
{
    if (!d_ptr->m_toolBar) {
        d_ptr->setupToolBar();
    }
    return d_ptr->m_toolBar;
}

/**
 * Assign a new tool bar that is shown above the content widget.
 * The dock widget will become the owner of the tool bar and deletes it
 * on destruction
 */
void DockWidget::setToolBar(QToolBar *toolBar)
{
    Q_D(DockWidget);
    if (d->m_toolBar) {
        delete d->m_toolBar;
    }

    d->m_toolBar = toolBar;
    d->m_layout->insertWidget(0, d->m_toolBar);
    this->connect(this, SIGNAL(topLevelChanged(bool)), SLOT(setToolBarFloatingStyle(bool)));
    setToolBarFloatingStyle(isFloating());
}

DockWidget::ToolBarStyleSource DockWidget::toolBarStyleSource() const
{
    Q_D(const DockWidget);
    return d->m_toolBarStyleSource;
}

/**
 * Configures, if the dock widget uses the global tool bar styles from
 * dock window or if it uses its own tool bar style
 */
void DockWidget::setToolBarStyleSource(DockWidget::ToolBarStyleSource source)
{
    Q_D(DockWidget);
    d->m_toolBarStyleSource = source;
    if (ToolBarStyleFromDockWindow == d->m_toolBarStyleSource) {
        d->setToolBarStyleFromDockWindow();
    }
}

Qt::ToolButtonStyle DockWidget::toolBarStyle(DockWidget::State state) const
{
    Q_D(const DockWidget);
    if (StateFloating == state) {
        return d->m_toolBarStyleFloating;
    } else {
        return d->m_toolBarStyleDocked;
    }
}

/**
 * This function sets the tool button style for the given dock widget state.
 * It is possible to switch the tool button style depending on the state.
 * If a dock widget is floating, then here are more space and it is
 * possible to select a style that requires more space like
 * Qt::ToolButtonTextUnderIcon. For the docked state Qt::ToolButtonIconOnly
 * might be better.
 */
void DockWidget::setToolBarStyle(Qt::ToolButtonStyle style, DockWidget::State state)
{
    Q_D(DockWidget);
    if (StateFloating == state) {
        d->m_toolBarStyleFloating = style;
    } else {
        d->m_toolBarStyleDocked = style;
    }

    setToolBarFloatingStyle(isFloating());
}

QSize DockWidget::toolBarIconSize(DockWidget::State state) const
{
    Q_D(const DockWidget);
    if (StateFloating == state) {
        return d->m_toolBarIconSizeFloating;
    } else {
        return d->m_toolBarIconSizeDocked;
    }
}

/**
 * This function sets the tool button icon size for the given state.
 * If a dock widget is floating, there is more space an increasing the
 * icon size is possible. For docked widgets, small icon sizes, eg. 16 x 16
 * might be better.
 */
void DockWidget::setToolBarIconSize(const QSize &iconSize, DockWidget::State state)
{
    Q_D(DockWidget);
    if (StateFloating == state) {
        d->m_toolBarIconSizeFloating = iconSize;
    } else {
        d->m_toolBarIconSizeDocked = iconSize;
    }

    setToolBarFloatingStyle(isFloating());
}

/**
 * Adjusts the toolbar icon sizes according to the floating state
 */
void DockWidget::setToolBarFloatingStyle(bool floating)
{
    Q_D(DockWidget);
    if (!d->m_toolBar) {
        return;
    }

    auto iconSize = floating ? d->m_toolBarIconSizeFloating : d->m_toolBarIconSizeDocked;
    if (iconSize != d->m_toolBar->iconSize()) {
        d->m_toolBar->setIconSize(iconSize);
    }

    auto buttonStyle = floating ? d->m_toolBarStyleFloating : d->m_toolBarStyleDocked;
    if (buttonStyle != d->m_toolBar->toolButtonStyle()) {
        d->m_toolBar->setToolButtonStyle(buttonStyle);
    }
}

QX_DOCK_END_NAMESPACE
