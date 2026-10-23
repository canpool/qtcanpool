/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTDIVIDER_H
#define FLUENTDIVIDER_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

QX_FLUENT_BEGIN_NAMESPACE

class FluentDividerPrivate;

/*!
 * Fluent's divider: one hairline that separates, and says nothing else.
 *
 * It is a line, and Qt draws lines with QFrame - but a frame arrives with
 * frames' opinions: bevels, sunken states, a style underneath. This one is
 * the family's hairline, \c Full across the edge it was given or \c Inset
 * past the indent the content beside it keeps.
 */
class QX_FLUENT_EXPORT FluentDivider : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(Kind kind READ kind WRITE setKind)
public:
    /*!
     * Where the line stands. \c Full runs the width it was given; \c Inset
     * keeps the content's indent on the left, so a line under a row of text
     * starts where the text starts.
     */
    enum class Kind {
        Full,
        Inset
    };
    Q_ENUM(Kind)

    explicit FluentDivider(QWidget *parent = Q_NULLPTR);
    ~FluentDivider() override;

    Kind kind() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setKind(Kind kind);
protected:
    void paintSurface(QPainter *painter) override;

    QAccessible::Role accessibleRole() const override;
private:
    Q_DISABLE_COPY(FluentDivider)
    QX_DECLARE_PRIVATE(FluentDivider)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTDIVIDER_H
