/**
 * Copyright (C) 2022-2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FANCYTOOLBUTTON_H
#define FANCYTOOLBUTTON_H

#include "qcanpool.h"
#include "qxapp/qxtoolbutton.h"

QCANPOOL_BEGIN_NAMESPACE

/*!
 * Compatibility shim for 3.0 code (decision K5).
 *
 * The widget itself moved to the application framework in 3.1 and is now
 * QxApp::QxToolButton; qcanpool keeps only the old name. This class derives
 * from the new one and forwards every constructor, so it cannot drift away
 * from it - and because it adds nothing, there is no behaviour to keep in
 * sync. The name disappears again in 3.2.
 */
class QCANPOOL_SHARED_EXPORT QCANPOOL_DEPRECATED_X("use QxApp::QxToolButton instead") FancyToolButton :
    public QX_APP_PREPEND_NAMESPACE(QxToolButton)
{
    Q_OBJECT
public:
    explicit FancyToolButton(QWidget *parent = nullptr)
        : QX_APP_PREPEND_NAMESPACE(QxToolButton)(parent)
    {
    }
    explicit FancyToolButton(const QString &text, QWidget *parent = nullptr)
        : QX_APP_PREPEND_NAMESPACE(QxToolButton)(text, parent)
    {
    }
    FancyToolButton(const QIcon &icon, const QString &text, QWidget *parent = nullptr)
        : QX_APP_PREPEND_NAMESPACE(QxToolButton)(icon, text, parent)
    {
    }
};

QCANPOOL_END_NAMESPACE

#endif   // FANCYTOOLBUTTON_H
