/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALTOOLTIP_H
#define MATERIALTOOLTIP_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxtooltip.h"

QX_MATERIAL_BEGIN_NAMESPACE

/*!
 * Material's tooltip: a small plain chip in the inverse pair - the inverse
 * surface under the inverse on-surface - that says a word about the control
 * it waits beside, and says it without an arrow.
 *
 * The mechanism - the watching, the arrival delay, the stay, the four sides,
 * the page turned inside out - is the shared answer, in \c qxcontrols. What
 * is Material's here is only the corner the chip turns by.
 */
class QX_MATERIAL_EXPORT MaterialTooltip : public QxControls::QxToolTip
{
public:
    /*! Which side of the anchor the chip stands on. */
    using Placement = QxControls::QxToolTip::Placement;

    explicit MaterialTooltip(QWidget *parent = Q_NULLPTR);
    ~MaterialTooltip() override;
protected:
    qreal cornerRadius() const override;

    Q_DISABLE_COPY(MaterialTooltip)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALTOOLTIP_H
