/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTBADGE_H
#define FLUENTBADGE_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

QX_FLUENT_BEGIN_NAMESPACE

class FluentBadgePrivate;

/*!
 * Fluent's info badge: a dot that says there is something, or a number that
 * says how much.
 *
 * Qt has no badge at all, which is why the count an application wants to show
 * ends up painted into a label, a button, or a pixmap made at run time. Those
 * three answers disagree with each other about size, colour and what happens
 * past 99; this one is one answer, and it is the accent, because a badge is
 * the application pointing at something.
 */
class QX_FLUENT_EXPORT FluentBadge : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(int count READ count WRITE setCount)
    Q_PROPERTY(int maximumCount READ maximumCount WRITE setMaximumCount)
public:
    explicit FluentBadge(QWidget *parent = Q_NULLPTR);
    ~FluentBadge() override;

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
    Q_DISABLE_COPY(FluentBadge)
    QX_DECLARE_PRIVATE(FluentBadge)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTBADGE_H
