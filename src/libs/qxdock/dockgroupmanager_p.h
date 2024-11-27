/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"

#include <QAction>

QX_DOCK_BEGIN_NAMESPACE

class DockGroup;
class DockWidget;
class DockSubWindow;

class CreateChildDockAction : public QAction
{
    Q_OBJECT
public:
    CreateChildDockAction(DockGroup *group, QMenu *menu);

public slots:
    void createGroup();
private:
    DockGroup *m_group;
};

class DestroyGroupAction : public QAction
{
    Q_OBJECT
public:
    DestroyGroupAction(DockGroup *group, QMenu *menu);

public slots:
    void destroyGroup();
private:
    DockGroup *m_group;
};

class MoveDockWidgetAction : public QAction
{
    Q_OBJECT
public:
    MoveDockWidgetAction(DockWidget *widget, DockSubWindow *moveTo, QMenu *menu);

    static void move(DockWidget *widget, DockSubWindow *moveTo);

public slots:
    void move();
private:
    DockWidget *m_widget;
    DockSubWindow *m_moveTo;
};

class LoadPerspectiveAction : public QAction
{
    Q_OBJECT
public:
    LoadPerspectiveAction(QMenu *parent, const QString &name, DockGroup *group);

public slots:
    void load();
private:
    QString name;
    DockGroup *m_group;
};

class RemovePerspectiveAction : public QAction
{
    Q_OBJECT
public:
    RemovePerspectiveAction(QMenu *parent, const QString &name, DockGroup *group);

public slots:
    void remove();
private:
    QString name;
    DockGroup *m_group;
};
QX_DOCK_END_NAMESPACE
