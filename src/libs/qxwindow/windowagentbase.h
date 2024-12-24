/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
**/
#ifndef WINDOWAGENTBASE_H
#define WINDOWAGENTBASE_H

#include "qxwindow_global.h"
#include <QtCore/QObject>

QX_WINDOW_BEGIN_NAMESPACE

class WindowAgentBasePrivate;

class QX_WINDOW_EXPORT WindowAgentBase : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WindowAgentBase)
public:
    ~WindowAgentBase() override;

    enum SystemButton {
        Unknown,
        WindowIcon,
        Help,
        Minimize,
        Maximize,
        Close,
    };
    Q_ENUM(SystemButton)

    QVariant windowAttribute(const QString &key) const;
    Q_INVOKABLE bool setWindowAttribute(const QString &key, const QVariant &attribute);

public Q_SLOTS:
    void showSystemMenu(const QPoint &pos);   // Only available on Windows now
    void centralize();
    void raise();
protected:
    explicit WindowAgentBase(WindowAgentBasePrivate &d, QObject *parent = nullptr);

    WindowAgentBasePrivate *d_ptr;
};

QX_WINDOW_END_NAMESPACE

#endif   // WINDOWAGENTBASE_H
