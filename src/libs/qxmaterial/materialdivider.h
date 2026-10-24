/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALDIVIDER_H
#define MATERIALDIVIDER_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxcontrol.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialDividerPrivate;

/*!
 * Material's divider: one hairline that separates, and says nothing else.
 *
 * It is a line, and Qt draws lines with QFrame - but a frame arrives with
 * frames' opinions: bevels, sunken states, a style underneath. This one is
 * the family's hairline in the outline the family reads its edges with, and
 * where it runs is the variant:
 *
 * - \c Full runs the width it was given;
 * - \c Inset keeps the content's indent on the left, so a line under a row
 *   of text starts where the text starts;
 * - \c Middle keeps the indent at both ends, so a line between two blocks of
 *   content touches neither.
 */
class QX_MATERIAL_EXPORT MaterialDivider : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(Variant variant READ variant WRITE setVariant)
public:
    /*! Where the line runs. */
    enum class Variant {
        Full,  /*!< Across the width it was given. */
        Inset, /*!< The content's indent on the left. */
        Middle /*!< The content's indent at both ends. */
    };
    Q_ENUM(Variant)

    explicit MaterialDivider(QWidget *parent = Q_NULLPTR);
    ~MaterialDivider() override;

    Variant variant() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setVariant(Variant variant);
protected:
    void paintSurface(QPainter *painter) override;

    QAccessible::Role accessibleRole() const override;
private:
    Q_DISABLE_COPY(MaterialDivider)
    QX_DECLARE_PRIVATE(MaterialDivider)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALDIVIDER_H
