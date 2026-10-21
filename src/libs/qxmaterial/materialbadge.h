/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALBADGE_H
#define MATERIALBADGE_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxcontrol.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialBadgePrivate;

/*!
 * Material's badge: a dot that says there is something, or a number that says
 * how much.
 *
 * Qt has no badge, which is why a count ends up painted into a label, a button
 * or a pixmap made at run time. This one is one answer - and it is not the
 * Fluent one: a Fluent badge is the application pointing at something and so
 * wears the accent, while a Material badge is the error colour, because what
 * it says is that something needs attention. The colour is the family's own,
 * the way an info bar's severities are: a theme has a role for a highlight and
 * none for "this needs attention".
 */
class QX_MATERIAL_EXPORT MaterialBadge : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(int count READ count WRITE setCount)
    Q_PROPERTY(int maximumCount READ maximumCount WRITE setMaximumCount)
public:
    explicit MaterialBadge(QWidget *parent = Q_NULLPTR);
    ~MaterialBadge() override;

    /*! The count it shows; 0 draws the dot. */
    int count() const;
    /*! The count past which the badge shows "n+" instead of a number. */
    int maximumCount() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setCount(int count);
    void setMaximumCount(int maximum);
protected:
    qreal cornerRadius() const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;

    QAccessible::Role accessibleRole() const override;
private:
    Q_DISABLE_COPY(MaterialBadge)
    QX_DECLARE_PRIVATE(MaterialBadge)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALBADGE_H
