/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "docksplitter.h"

QX_DOCK_BEGIN_NAMESPACE

class DockSplitterPrivate
{
public:
    QX_DECLARE_PUBLIC(DockSplitter)
public:
    DockSplitterPrivate();
};

DockSplitterPrivate::DockSplitterPrivate()
{

}

DockSplitter::DockSplitter(QWidget *parent)
    : QSplitter(parent)
{
    QX_INIT_PRIVATE(DockSplitter);
}

DockSplitter::~DockSplitter()
{
    QX_FINI_PRIVATE()
}

QX_DOCK_END_NAMESPACE
