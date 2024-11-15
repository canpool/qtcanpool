/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QFrame>

class QAbstractButton;

QX_DOCK_BEGIN_NAMESPACE

class DockPanel;
class DockTabBar;
class DockLabel;

class DockTitleBarPrivate;

class QX_DOCK_EXPORT DockTitleBar : public QFrame
{
    Q_OBJECT
public:
    using Super = QFrame;
public:
    explicit DockTitleBar(DockPanel *parent = nullptr);
    virtual ~DockTitleBar();

    DockTabBar *tabBar() const;
    DockPanel *dockPanel() const;

    QAbstractButton *button(Qx::DockTitleBarButton which) const;

    DockLabel *autoHideTitleLabel() const;
    void showAutoHideControls(bool show);

    void updateDockWidgetActionsButtons();

    int indexOf(QWidget *widget) const;
    QString titleBarButtonToolTip(Qx::DockTitleBarButton id) const;

    bool isAutoHide() const;

Q_SIGNALS:
    void tabBarClicked(int index);

public Q_SLOTS:
    void markTabsMenuOutdated();

private Q_SLOTS:
    void onTabsMenuAboutToShow();
    void onTabsMenuActionTriggered(QAction *a);
    void onCurrentTabChanged(int index);
    void onCloseButtonClicked();
    void onUndockButtonClicked();
    void onAutoHideButtonClicked();
    void minimizeAutoHideContainer();
    void onAutoHideDockAreaActionClicked();
    void onAutoHideToActionClicked();
    void onAutoHideCloseActionTriggered();

protected:
    virtual void mousePressEvent(QMouseEvent *e) override;
    virtual void mouseReleaseEvent(QMouseEvent *e) override;
    virtual void mouseMoveEvent(QMouseEvent *e) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *e) override;
    virtual void contextMenuEvent(QContextMenuEvent *e) override;
    virtual void resizeEvent(QResizeEvent *e) override;

private:
    QX_DECLARE_PRIVATE(DockTitleBar)
};

QX_DOCK_END_NAMESPACE
