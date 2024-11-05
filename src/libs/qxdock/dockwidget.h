/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include <QWidget>

QX_DOCK_BEGIN_NAMESPACE

class DockWindow;
class DockContainer;
class DockPanel;
class DockTab;

class DockWidgetPrivate;

class QX_DOCK_EXPORT DockWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DockWidget(const QString &title, QWidget *parent = nullptr);
    ~DockWidget();

    DockTab *tab() const;

protected:
    void setDockWindow(DockWindow *window);
    void setDockPanel(DockPanel *panel);
private:
    QX_DECLARE_PRIVATE(DockWidget)
    friend class DockContainer;
    friend class DockPanel;
};

QX_DOCK_END_NAMESPACE
