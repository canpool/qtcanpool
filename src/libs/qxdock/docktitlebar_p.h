/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QToolButton>

QX_DOCK_BEGIN_NAMESPACE

class TitleBarButton : public QToolButton
{
    Q_OBJECT
public:
    TitleBarButton(bool showInTitleBar, bool hideWhenDisabled, Qx::DockTitleBarButton id,
                   QWidget *parent = nullptr);

public:
    Qx::DockTitleBarButton m_id;
    bool m_showInTitleBar = true;
    bool m_hideWhenDisabled = false;
};

QX_DOCK_END_NAMESPACE
