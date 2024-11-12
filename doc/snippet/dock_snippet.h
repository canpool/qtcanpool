/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include <QObject>

QX_DOCK_BEGIN_NAMESPACE

class DockSnippetPrivate;

class QX_DOCK_EXPORT DockSnippet : public QObject
{
    Q_OBJECT
public:
    explicit DockSnippet(QObject *parent = nullptr);
    virtual ~DockSnippet();

private:
    QX_DECLARE_PRIVATE(DockSnippet)
};

QX_DOCK_END_NAMESPACE
