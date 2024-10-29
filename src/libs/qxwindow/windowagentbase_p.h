/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
**/
#ifndef WINDOWAGENTBASEPRIVATE_H
#define WINDOWAGENTBASEPRIVATE_H

//
//  W A R N I N G !!!
//  -----------------
//
// This file is not part of the QxWindow API. It is used purely as an
// implementation detail. This header file may change from version to
// version without notice, or may even be removed.
//

#include "windowcontext_p.h"
#include "windowagentbase.h"

QX_WINDOW_BEGIN_NAMESPACE

class QX_WINDOW_EXPORT WindowAgentBasePrivate
{
    Q_DECLARE_PUBLIC(WindowAgentBase)
public:
    WindowAgentBasePrivate();
    virtual ~WindowAgentBasePrivate();

    void init();

    WindowAgentBase *q_ptr;   // no need to initialize

    virtual WindowContext *createContext() const;

    void setup(QObject *host, WindowItemDelegate *delegate);

    std::unique_ptr<WindowContext> context;
private:
    Q_DISABLE_COPY(WindowAgentBasePrivate)
};

QX_WINDOW_END_NAMESPACE

#endif   // WINDOWAGENTBASEPRIVATE_H
