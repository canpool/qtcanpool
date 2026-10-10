/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxapp_global.h"
#include "qxtoastmanager.h"

#include <QtCore/QList>
#include <QtCore/QPointer>

QX_APP_BEGIN_NAMESPACE

/*!
 * The state behind QxToastManager: the host, the stack and the placement rules.
 *
 * The stack is a plain list of live toasts, oldest first - the order they are
 * stacked in, and the order eviction takes them out in. It is the only thing
 * that decides what is on screen, which is why every other part of the class is
 * built around keeping it exactly in step with the widgets.
 */
class QxToastManagerPrivate
{
    QX_DECLARE_PUBLIC(QxToastManager)
public:
    QxToastManagerPrivate();
    /*! Wires \a toast up and appends it to the stack. */
    void adopt(QxToast *toast);
    /*! Takes \a toast out of the stack and lets it fade out. Does nothing when it is not in it. */
    void remove(QxToast *toast);
    /*! Places every toast of the stack along the anchored edges of the host. */
    void relayout();
    /*! Width of the plates, the host and the requested maximum taken into account. */
    int plateWidth() const;
public:
    QPointer<QWidget> m_host;
    QList<QxToast *> m_toasts;
    int m_maxVisible = 3;
    QxToastManager::Position m_position = QxToastManager::TopRight;
    int m_margin = 12;
    int m_spacing = 8;
    int m_maxWidth = 360;
    int m_fadeDuration = QxToast::DefaultFadeDuration;
};

QX_APP_END_NAMESPACE
