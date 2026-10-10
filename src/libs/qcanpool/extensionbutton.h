/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef EXTENSIONBUTTON_H
#define EXTENSIONBUTTON_H

#include "qcanpool.h"
#include "qxapp/qxextensionbutton.h"

QCANPOOL_BEGIN_NAMESPACE

/*!
 * Compatibility shim for 3.0 code (decision K5): the widget is now
 * QxApp::QxExtensionButton. See fancytoolbutton.h for the rationale; the name
 * disappears again in 3.2.
 */
class QCANPOOL_SHARED_EXPORT QCANPOOL_DEPRECATED_X("use QxApp::QxExtensionButton instead") ExtensionButton :
    public QX_APP_PREPEND_NAMESPACE(QxExtensionButton)
{
    Q_OBJECT
public:
    explicit ExtensionButton(QWidget *parent = nullptr)
        : QX_APP_PREPEND_NAMESPACE(QxExtensionButton)(parent)
    {
    }
};

QCANPOOL_END_NAMESPACE

#endif   // EXTENSIONBUTTON_H
