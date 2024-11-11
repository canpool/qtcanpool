/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QWidget>

class QAbstractButton;

QX_DOCK_BEGIN_NAMESPACE

class DockPanel;
class DockTabBar;

class DockTitleBarPrivate;

class QX_DOCK_EXPORT DockTitleBar : public QWidget
{
    Q_OBJECT
public:
    using Super = QWidget;
public:
    explicit DockTitleBar(DockPanel *parent = nullptr);
    virtual ~DockTitleBar();

    DockTabBar *tabBar() const;
    DockPanel *dockPanel() const;

    QAbstractButton *button(Qx::DockTitleBarButton which) const;

    void updateDockWidgetActionsButtons();

    int indexOf(QWidget *widget) const;
    QString titleBarButtonToolTip(Qx::DockTitleBarButton id) const;

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

protected:
    virtual void mousePressEvent(QMouseEvent *e) override;
    virtual void mouseReleaseEvent(QMouseEvent *e) override;
    virtual void mouseMoveEvent(QMouseEvent *e) override;

private:
    QX_DECLARE_PRIVATE(DockTitleBar)
};

QX_DOCK_END_NAMESPACE
