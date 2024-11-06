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

class QX_DOCK_EXPORT DockTabBar : public QScrollArea
{
    Q_OBJECT
public:
    explicit DockTabBar(DockPanel *parent = nullptr);
    virtual ~DockTabBar();

    void insertTab(int index, DockTab *tab);

    int count() const;

    DockTab *tab(int index) const;

    virtual QSize minimumSizeHint() const override;
    virtual QSize sizeHint() const override;

public Q_SLOTS:
    void setCurrentIndex(int index);

private:
    QX_DECLARE_PRIVATE(DockTabBar)
};

QX_DOCK_END_NAMESPACE
