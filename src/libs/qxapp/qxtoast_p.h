/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxapp_global.h"
#include "qxtoast.h"

QT_BEGIN_NAMESPACE
class QGraphicsOpacityEffect;
class QPropertyAnimation;
class QTimer;
class QLabel;
QT_END_NAMESPACE

QX_APP_BEGIN_NAMESPACE

/*!
 * The state behind QxToast: the message, the level, the clock and the two
 * animations.
 *
 * The toast keeps no layout on purpose. Its plate has a width the manager
 * decides - a column of toasts of the same width reads as one column - and a
 * height that only the wrapped text can tell, so the geometry is computed from
 * the text metrics instead of being negotiated with a QLayout. That also makes
 * heightForWidth() cheap enough to call while placing a stack.
 */
class QxToastPrivate
{
    QX_DECLARE_PUBLIC(QxToast)
public:
    QxToastPrivate();
    void init();

    /*! Applies the level to the icon and to the text colour. */
    void applyLevel();
    /*! Places the icon and the label inside the current geometry. */
    void placeChildren();
    /*! Fades the opacity to \a to; \a dismissWhenFaded finishes the dismissal. */
    void startFade(qreal to, bool dismissWhenFaded);
    /*! Hides the widget and tells the owner it may delete it. */
    void finishDismiss();

    /*! Width of the plate for a widget \a outerWidth wide, clamped to the limits. */
    int plateWidth(int outerWidth) const;
    /*! Width left for the text on a plate \a plateWidth pixels wide. */
    static int textWidthFor(int plateWidth);
    /*! Height of the whole widget when it is \a outerWidth wide. */
    int outerHeightFor(int outerWidth) const;
public:
    QString m_text;
    QxToast::Level m_level = QxToast::Information;
    int m_timeout = QxToast::DefaultTimeout;
    int m_fadeDuration = QxToast::DefaultFadeDuration;
    /*! Milliseconds left on the countdown while the pointer holds it back. */
    int m_remaining = 0;
    bool m_dismissing = false;
    bool m_paused = false;
    bool m_dismissWhenFaded = false;
    QLabel *m_icon = Q_NULLPTR;
    QLabel *m_label = Q_NULLPTR;
    QTimer *m_timer = Q_NULLPTR;
    QGraphicsOpacityEffect *m_effect = Q_NULLPTR;
    QPropertyAnimation *m_fade = Q_NULLPTR;
};

QX_APP_END_NAMESPACE
