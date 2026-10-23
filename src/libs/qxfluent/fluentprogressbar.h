/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTPROGRESSBAR_H
#define FLUENTPROGRESSBAR_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

QX_FLUENT_BEGIN_NAMESPACE

class FluentProgressBarPrivate;

/*!
 * Fluent's progress bar: a track four tall that says how far along something
 * is, or a block that only says something is happening.
 *
 * QProgressBar draws both of its modes through the style, and the style has
 * none of Fluent in it; the bar here is the family's own drawing - a pill of
 * a track in thinned ink, and along it either a fill that ends at the value
 * or a block that slides and says the waiting has not stopped.
 *
 * The slide is the one motion here that is not a transition - nothing is
 * moving from one state to another, it is a loop - so it reads the
 * reduce-motion switch and stops: the block holds still rather than
 * vanishing, because a bar that says nothing at all is not progress, it is
 * an empty track.
 */
class QX_FLUENT_EXPORT FluentProgressBar : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(int minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(int maximum READ maximum WRITE setMaximum)
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(bool indeterminate READ isIndeterminate WRITE setIndeterminate)
public:
    explicit FluentProgressBar(QWidget *parent = Q_NULLPTR);
    ~FluentProgressBar() override;

    /*! Where the range starts; the value never goes below it. */
    int minimum() const;
    /*! Where the range ends; the fill reaches the far edge there. */
    int maximum() const;
    /*! How far along, between the minimum and the maximum; meaningless while indeterminate. */
    int value() const;
    /*! True while the bar says only that something is happening. */
    bool isIndeterminate() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setMinimum(int minimum);
    void setMaximum(int maximum);
    void setValue(int value);
    void setIndeterminate(bool on);
Q_SIGNALS:
    /*! The value moved; \a value is the new one, already bounded. */
    void valueChanged(int value);
protected:
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;
    void changeEvent(QEvent *event) override;

    QColor washColor() const override;

    QAccessible::Role accessibleRole() const override;
    QAccessibleInterface *accessibleInterface() override;
private:
    void updateSliding();

    Q_DISABLE_COPY(FluentProgressBar)
    QX_DECLARE_PRIVATE(FluentProgressBar)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTPROGRESSBAR_H
