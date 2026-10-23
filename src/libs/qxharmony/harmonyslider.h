/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef HARMONYSLIDER_H
#define HARMONYSLIDER_H

#include "qxharmony_global.h"

#include "qxcontrols/qxcontrol.h"

QX_HARMONY_BEGIN_NAMESPACE

class HarmonySliderPrivate;

/*!
 * Harmony's slider: a rail, a handle that rides it, and a bubble that says the
 * number while the handle is being held.
 *
 * Qt's QSlider is a style's idea of a slider and every one of them draws a
 * groove the toolkit owns. This one is drawn here, and the drawing is quiet: a
 * rail in the family's grey with the part the handle has reached in its blue,
 * and no marks under either, because a mark every interval would be a second
 * answer to a question the handle has already answered.
 *
 * What makes it this family's is what happens while the pointer is down: a
 * bubble floats over the handle with the value in it, and fades out when the
 * handle is let go. The reading is that the exact number is what the hand is
 * asking for at that moment and nothing else is - a slider that printed the
 * number or kept the bubble there would be answering after the question.
 *
 * The widget is taller than its rail because the bubble needs somewhere to be:
 * the room above the handle belongs to this control, and is empty the rest of
 * the time.
 */
class QX_HARMONY_EXPORT HarmonySlider : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue)
public:
    explicit HarmonySlider(QWidget *parent = Q_NULLPTR);
    ~HarmonySlider() override;

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
    void paintContent(QPainter *painter) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    QAccessible::Role accessibleRole() const override;
    QAccessibleInterface *accessibleInterface() override;
private:
    Q_DISABLE_COPY(HarmonySlider)
    QX_DECLARE_PRIVATE(HarmonySlider)
};

QX_HARMONY_END_NAMESPACE

#endif   // HARMONYSLIDER_H
