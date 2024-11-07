/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QSplitter>

QX_DOCK_BEGIN_NAMESPACE

class DockSplitterPrivate;

class QX_DOCK_EXPORT DockSplitter : public QSplitter
{
    Q_OBJECT
public:
    explicit DockSplitter(QWidget *parent = nullptr);
    explicit DockSplitter(Qt::Orientation orientation, QWidget *parent = nullptr);
    virtual ~DockSplitter();

    bool hasVisibleContent() const;

    bool isResizingWithContainer() const;
private:
    QX_DECLARE_PRIVATE(DockSplitter)
};

QX_DOCK_END_NAMESPACE
