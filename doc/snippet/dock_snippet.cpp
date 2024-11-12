/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "docksnippet.h"

QX_DOCK_BEGIN_NAMESPACE

class DockSnippetPrivate
{
public:
    QX_DECLARE_PUBLIC(DockSnippet)
public:
    DockSnippetPrivate();
};

DockSnippetPrivate::DockSnippetPrivate()
{
}

DockSnippet::DockSnippet(QObject *parent)
    : QObject(parent)
{
    QX_INIT_PRIVATE(DockSnippet);
}

DockSnippet::~DockSnippet()
{
    QX_FINI_PRIVATE();
}

QX_DOCK_END_NAMESPACE
