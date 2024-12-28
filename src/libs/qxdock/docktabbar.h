/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QScrollArea>

QX_DOCK_BEGIN_NAMESPACE

class DockPanel;
class DockTab;

class DockTabBarPrivate;

/**
 * Custom tabbar implementation for tab area that is shown on top of a
 * dock panel widget.
 * The tabbar displays the tab widgets of the contained dock widgets.
 * We cannot use QTabBar here because it does a lot of fancy animations
 * that will crash the application if a tab is removed while the animation
 * has not finished. And we need to remove a tab, if the user drags a
 * a dock widget out of a group of tabbed widgets
 */
class QX_DOCK_EXPORT DockTabBar : public QScrollArea
{
    Q_OBJECT
public:
    using Super = QScrollArea;
public:
    explicit DockTabBar(DockPanel *parent = nullptr);
    virtual ~DockTabBar();

    void insertTab(int index, DockTab *tab);
    void removeTab(DockTab *tab);

    int count() const;

    int currentIndex() const;

    DockTab *currentTab() const;
    DockTab *tab(int index) const;

    int tabAt(const QPoint &pos) const;
    int tabInsertIndexAt(const QPoint &pos) const;

    bool isTabOpen(int index) const;

    virtual QSize minimumSizeHint() const override;
    virtual QSize sizeHint() const override;

    bool areTabsOverflowing() const;

public Q_SLOTS:
    void setCurrentIndex(int index);
    void closeTab(int index);

private Q_SLOTS:
    void onTabClicked();
    void onTabCloseRequested();
    void onCloseOtherTabsRequested();
    void onTabWidgetMoved(const QPoint &globalPos);

Q_SIGNALS:
    void currentChanging(int index);
    void currentChanged(int index);
    void tabBarClicked(int index);
    void tabCloseRequested(int index);
    void tabClosed(int index);
    void tabOpened(int index);
    void tabMoved(int from, int to);
    void removingTab(int index);
    void tabInserted(int index);
    void elidedChanged(bool elided);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;

private:
    QX_DECLARE_PRIVATE(DockTabBar)
};

QX_DOCK_END_NAMESPACE
