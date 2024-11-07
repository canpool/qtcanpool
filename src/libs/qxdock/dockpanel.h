/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock/dockwidget.h"
#include "qxdock_global.h"
#include <QWidget>

class QAbstractButton;

QX_DOCK_BEGIN_NAMESPACE

class DockWindow;
class DockContainer;
class DockSplitter;

class DockPanelPrivate;

class QX_DOCK_EXPORT DockPanel : public QWidget
{
    Q_OBJECT
public:
    explicit DockPanel(DockWindow *window, DockContainer *parent);
    ~DockPanel();

    DockWindow *dockWindow() const;
    DockContainer *dockContainer() const;

    DockSplitter *parentSplitter() const;

    int dockWidgetsCount() const;
    QList<DockWidget *> dockWidgets() const;
    QList<DockWidget *> openedDockWidgets() const;
    DockWidget *dockWidget(int index) const;

    int currentIndex() const;

    DockWidget *currentDockWidget() const;
    void setCurrentDockWidget(DockWidget *w);

    QAbstractButton *titleBarButton(Qx::DockTitleBarButton which) const;

    virtual void setVisible(bool visible) override;

    bool isCentralWidgetArea() const;

public Q_SLOTS:
    void setCurrentIndex(int index);
    void closeArea();

protected Q_SLOTS:
    void toggleView(bool open);

Q_SIGNALS:
    void currentChanging(int index);
    void currentChanged(int index);
    void viewToggled(bool open);

protected:
    void addDockWidget(DockWidget *w);
    void insertDockWidget(int index, DockWidget *w, bool activate = true);
    void removeDockWidget(DockWidget *w);
    DockWidget *nextOpenDockWidget(DockWidget *w) const;
    void toggleDockWidgetView(DockWidget *w, bool open);
    int indexOf(DockWidget *w) const;
    void hideAreaWithNoVisibleContent();
    void internalSetCurrentDockWidget(DockWidget *w);

private:
    QX_DECLARE_PRIVATE(DockPanel)
    friend class DockContainerPrivate;
    friend class DockWidget;
    friend class DockWidgetPrivate;
    friend class DockContainer;
};

QX_DOCK_END_NAMESPACE
