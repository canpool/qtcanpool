/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef HARMONYCARD_H
#define HARMONYCARD_H

#include "qxharmony_global.h"

#include "qxcontrols/qxcontrol.h"

#include "harmonytokens.h"

QX_HARMONY_BEGIN_NAMESPACE

class HarmonyCardPrivate;

/*!
 * Harmony's card: a face laid on the page, standing on a shadow.
 *
 * The card is the shape this family is built out of - a surface with a corner
 * you can see and a lift under it - so the two things it carries are the
 * family's large corner and one of the family's elevation levels. The level is
 * the whole of what tells two cards apart: the same face at a higher level is a
 * card the pointer is on, or one that has been opened over the page.
 *
 * Held, the card dims: the state a press draws is a layer over the surface, not
 * a scale. A card that shrank under the hand would be moving the page under the
 * user's finger, and this family moves things when they arrive, not when they
 * are touched.
 */
class QX_HARMONY_EXPORT HarmonyCard : public QxControls::QxControl
{
    Q_OBJECT
public:
    explicit HarmonyCard(QWidget *parent = Q_NULLPTR);
    ~HarmonyCard() override;

    /*! The level the card stands at; \c HarmonyElevation::Level::Raised by
     *  default, which is a card at rest on the page. */
    HarmonyElevation::Level elevation() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    /*! Puts the card at \a level. */
    void setElevation(HarmonyElevation::Level level);
protected:
    qreal cornerRadius() const override;
    void paintSurface(QPainter *painter) override;

    QAccessible::Role accessibleRole() const override;
private:
    Q_DISABLE_COPY(HarmonyCard)
    QX_DECLARE_PRIVATE(HarmonyCard)
};

QX_HARMONY_END_NAMESPACE

#endif   // HARMONYCARD_H
