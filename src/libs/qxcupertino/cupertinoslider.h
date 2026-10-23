/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef CUPERTINOSLIDER_H
#define CUPERTINOSLIDER_H

#include "qxcupertino_global.h"

#include "qxcontrols/qxcontrol.h"

QX_CUPERTINO_BEGIN_NAMESPACE

class CupertinoSliderPrivate;

/*!
 * Cupertino's slider: a thin rail, a round handle that rides it, and the part
 * the handle has reached filled in.
 *
 * QSlider is a style's idea of a slider, and every one of them draws a groove
 * the toolkit owns. This one is drawn here, and what it draws is the least a
 * slider can say: a four pixel rail with capsule ends, the filled part in the
 * family's blue and the rest in its grey, and no marks under either - a mark
 * every interval would be a second answer to a question the handle has already
 * answered.
 *
 * The handle is the thing taken hold of, and it says so: it is seven times the
 * height of the rail, it lifts off it with the family's raised shadow, and it
 * grows a little while it is being dragged - the one movement here that only
 * happens under the pointer.
 */
class QX_CUPERTINO_EXPORT CupertinoSlider : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue)
public:
    explicit CupertinoSlider(QWidget *parent = Q_NULLPTR);
    ~CupertinoSlider() override;

    int value() const;
    int minimum() const;
    int maximum() const;
    int singleStep() const;
    int pageStep() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setValue(int value);
    void setMinimum(int minimum);
    void setMaximum(int maximum);
    void setRange(int minimum, int maximum);
    void setSingleStep(int step);
    void setPageStep(int step);

Q_SIGNALS:
    void valueChanged(int value);
protected:
    qreal cornerRadius() const override;
    QRectF chromeRect() const override;
    void paintSurface(QPainter *painter) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    QAccessible::Role accessibleRole() const override;
    QAccessibleInterface *accessibleInterface() override;
private:
    Q_DISABLE_COPY(CupertinoSlider)
    QX_DECLARE_PRIVATE(CupertinoSlider)
};

QX_CUPERTINO_END_NAMESPACE

#endif   // CUPERTINOSLIDER_H
