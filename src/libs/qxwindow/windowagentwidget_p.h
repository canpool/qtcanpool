/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
**/
#ifndef WINDOWAGENTWIDGETPRIVATE_H
#define WINDOWAGENTWIDGETPRIVATE_H

//
//  W A R N I N G !!!
//  -----------------
//
// This file is not part of the QxWindow API. It is used purely as an
// implementation detail. This header file may change from version to
// version without notice, or may even be removed.
//

#include "windowagentbase_p.h"
#include "windowagentwidget.h"

QX_WINDOW_BEGIN_NAMESPACE

class WindowAgentWidgetPrivate : public WindowAgentBasePrivate
{
    Q_DECLARE_PUBLIC(WindowAgentWidget)
public:
    WindowAgentWidgetPrivate();
    ~WindowAgentWidgetPrivate();

    void init();

    // Host
    QWidget *hostWidget{};

#ifdef Q_OS_MAC
    QWidget *systemButtonAreaWidget{};
    std::unique_ptr<QObject> systemButtonAreaWidgetEventFilter;
#endif

#if defined(Q_OS_WINDOWS) && defined(QX_WINDOW_ENABLE_SYSTEM_BORDERS) && defined(QX_WINDOW_NATIVE)
    void setupWindows10BorderWorkaround();
    std::unique_ptr<QObject> borderHandler;
#endif
};

QX_WINDOW_END_NAMESPACE

#endif   // WINDOWAGENTWIDGETPRIVATE_H
