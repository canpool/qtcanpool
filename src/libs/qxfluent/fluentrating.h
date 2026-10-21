/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTRATING_H
#define FLUENTRATING_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

QX_FLUENT_BEGIN_NAMESPACE

class FluentRatingPrivate;

/*!
 * Fluent's rating: a row of stars, filled up to the one the user chose.
 *
 * Qt has no rating control at all - the survey in the study found nothing
 * between a row of buttons and a label with stars in it - and the two things
 * a rating has to answer are not answered by either: how much is it now, and
 * what would it be if I clicked here. The second is the hover, and it is not
 * decoration: a rating that does not show what a click would do is a control
 * the user has to click to find out about.
 *
 * The keyboard walks it, because a rating is a value in a range and an
 * assistive tool reads it that way.
 */
class QX_FLUENT_EXPORT FluentRating : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(int count READ count WRITE setCount)
public:
    explicit FluentRating(QWidget *parent = Q_NULLPTR);
    ~FluentRating() override;

    /*! How many stars are filled; 0 is "not rated". */
    int value() const;
    /*! How many stars there are. */
    int count() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setValue(int value);
    void setCount(int count);

Q_SIGNALS:
    /*! Emitted whenever the rating changes, whatever changed it. */
    void valueChanged(int value);
protected:
    void paintContent(QPainter *painter) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
    QAccessibleInterface *accessibleInterface() override;
private:
    Q_DISABLE_COPY(FluentRating)
    QX_DECLARE_PRIVATE(FluentRating)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTRATING_H
