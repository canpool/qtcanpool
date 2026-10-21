/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALCARD_H
#define MATERIALCARD_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxcontrol.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialCardPrivate;

/*!
 * Material's card: a surface that holds something, in one of three readings.
 *
 * A card is a place on the page where one thing is put together - a message, a
 * setting, a result - and Qt has a group box for that, which arrives with a
 * frame and a title and reads as a form. This has neither: what it is, is a
 * surface, and which of the three it is says how far it stands out from the
 * page:
 *
 * - \c Elevated casts a shadow, and is the page's own colour - it stands out
 *   by being above the page, not by being a different one;
 * - \c Filled is a surface one step from the page, and casts nothing;
 * - \c Outlined is the page's own colour with a line round it, and casts
 *   nothing.
 *
 * That is not the Fluent card's answer, and the difference is what each family
 * thinks a card *is*: Fluent's says how far above the page it sits, so the
 * lift is the message and the card is asked for an elevation; this one says
 * which of three surfaces it is, so the variant is the message and only one of
 * the three casts a shadow at all. A Fluent card is lit from wherever the
 * pointer is; this one answers the pointer by lifting, which is one level, not
 * a light.
 *
 * What goes inside is the application's: add a layout and fill it. The card
 * draws the surface, keeps room for the shadow it casts, and reports a
 * grouping to an assistive tool so its contents are announced as a section
 * rather than as loose widgets.
 */
class QX_MATERIAL_EXPORT MaterialCard : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(Variant variant READ variant WRITE setVariant)
public:
    enum class Variant {
        Elevated, /*!< Casts a shadow: stands out by being above the page. */
        Filled,   /*!< A surface one step from the page; casts nothing. */
        Outlined  /*!< The page's own colour with a line round it. */
    };
    Q_ENUM(Variant)

    explicit MaterialCard(Variant variant = Variant::Elevated, QWidget *parent = Q_NULLPTR);
    ~MaterialCard() override;

    Variant variant() const;

    /*! How much room the card keeps round its surface for the shadow it casts
     *  - the number its contents margin is set to. A card that casts nothing
     *  owes no room. */
    static int shadowMargin(Variant variant);

public Q_SLOTS:
    void setVariant(Variant variant);
protected:
    qreal cornerRadius() const override;
    QColor washColor() const override;
    bool hitTest(const QPointF &pos) const override;
    void paintSurface(QPainter *painter) override;

    QAccessible::Role accessibleRole() const override;
private:
    Q_DISABLE_COPY(MaterialCard)
    QX_DECLARE_PRIVATE(MaterialCard)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALCARD_H
