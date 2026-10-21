/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTCARD_H
#define FLUENTCARD_H

#include "qxfluent_global.h"

#include "fluenttokens.h"
#include "qxcontrols/qxcontrol.h"

QX_FLUENT_BEGIN_NAMESPACE

class FluentCardPrivate;

/*!
 * Fluent's card: a surface that sits above the page, and the lift is what
 * says how far above.
 *
 * A card is the one place in a Fluent application where elevation is the
 * message rather than decoration, so the shadow is not a border pretending to
 * be one: it is the shadow of the elevation the card is given, painted by the
 * base and cached, and the card keeps room for it round its edges - which is
 * why a card is a control that owns a margin.
 *
 * What goes inside is the application's: add a layout and fill it. The card
 * draws the surface, casts the shadow, and reports a grouping to an assistive
 * tool so its contents are announced as a section rather than as loose
 * widgets.
 */
class QX_FLUENT_EXPORT FluentCard : public QxControls::QxControl
{
    Q_OBJECT
public:
    explicit FluentCard(QWidget *parent = Q_NULLPTR);
    ~FluentCard() override;

    /*! The elevation the card sits at, as a Fluent level. */
    FluentElevation::Level elevation() const;

    /*! How much room the card keeps round its surface for the shadow it
     *  casts - the number its contents margin is set to. */
    static int shadowMargin(FluentElevation::Level level);

public Q_SLOTS:
    void setElevation(FluentElevation::Level level);
protected:
    qreal cornerRadius() const override;
    void paintSurface(QPainter *painter) override;

    QAccessible::Role accessibleRole() const override;
private:
    Q_DISABLE_COPY(FluentCard)
    QX_DECLARE_PRIVATE(FluentCard)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTCARD_H
