/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef CUPERTINOACTIVITYINDICATOR_H
#define CUPERTINOACTIVITYINDICATOR_H

#include "qxcupertino_global.h"

#include "qxcontrols/qxcontrol.h"

QX_CUPERTINO_BEGIN_NAMESPACE

class CupertinoActivityIndicatorPrivate;

/*!
 * Cupertino's activity indicator: eight spokes fading round a centre.
 *
 * What makes it this family's is the shape of the wait rather than the spin:
 * there is no track and no arc, only spokes of equal length, each one fainter
 * than the one before it, turning as a whole. The fade is fixed to the spokes
 * and not to the direction of travel - the wheel is what moves, the light on
 * it does not - so the eye reads a rotation without the control ever saying
 * how far along the work is. That is the difference between this and a
 * progress bar: a bar answers "how much longer", an indicator answers only
 * "still going".
 *
 * The spokes are drawn in the family's grey on both faces of the palette, so
 * the control sits under the pointer's expectation of a waiting mark rather
 * than on top of whatever accent the application chose.
 *
 * The turn asks the family's motion scale for its step, which means reduce
 * motion silences it: the spokes stay where they are and are still drawn, so
 * a page that is waiting still looks like it. Nothing is drawn when the
 * widget is hidden, because a turn nobody sees is a timer nobody needs.
 */
class QX_CUPERTINO_EXPORT CupertinoActivityIndicator : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(bool animating READ isAnimating WRITE setAnimating NOTIFY animatingChanged)
public:
    explicit CupertinoActivityIndicator(QWidget *parent = Q_NULLPTR);
    ~CupertinoActivityIndicator() override;

    /*! True while the wheel is turning - which it stops doing under reduce
     *  motion while staying drawn. */
    bool isAnimating() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    /*! Starts or stops the turn. The indicator is drawn in both cases. */
    void setAnimating(bool animating);

Q_SIGNALS:
    void animatingChanged(bool animating);
protected:
    bool hitTest(const QPointF &pos) const override;
    void paintContent(QPainter *painter) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

    QAccessible::Role accessibleRole() const override;
private:
    void restart();

    Q_DISABLE_COPY(CupertinoActivityIndicator)
    QX_DECLARE_PRIVATE(CupertinoActivityIndicator)
};

QX_CUPERTINO_END_NAMESPACE

#endif   // CUPERTINOACTIVITYINDICATOR_H
