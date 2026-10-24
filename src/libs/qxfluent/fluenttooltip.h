/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTTOOLTIP_H
#define FLUENTTOOLTIP_H

#include "qxfluent_global.h"

#include "qxcontrols/qxtooltip.h"

QX_FLUENT_BEGIN_NAMESPACE

/*!
 * Fluent's tooltip: a word of explanation that waits beside its anchor until
 * the pointer or the keyboard arrives, and goes away when either does.
 *
 * The mechanism - the watching, the arrival delay, the stay, the four sides,
 * the page turned inside out - is the shared answer, in \c qxcontrols. What
 * is Fluent's here is only the corner the note turns by.
 */
class QX_FLUENT_EXPORT FluentToolTip : public QxControls::QxToolTip
{
public:
    /*! Which side of the anchor the tooltip stands on. */
    using Placement = QxControls::QxToolTip::Placement;

    explicit FluentToolTip(QWidget *parent = Q_NULLPTR);
    ~FluentToolTip() override;
protected:
    qreal cornerRadius() const override;

    Q_DISABLE_COPY(FluentToolTip)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTTOOLTIP_H
