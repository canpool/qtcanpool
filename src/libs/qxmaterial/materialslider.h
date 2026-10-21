/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALSLIDER_H
#define MATERIALSLIDER_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxcontrol.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialSliderPrivate;

/*!
 * Material's slider: a track, a handle that rides it, and nothing under it.
 *
 * QSlider is a style's idea of a slider, and every one of them draws a groove
 * the toolkit owns. This one is drawn here, and the difference that matters
 * between the two families is what it *does not* draw: Fluent's slider says
 * where its rail is divided, with a mark at every interval, and Material's
 * says nothing of the kind - a Material slider has no ticks, because the value
 * is the handle's position and the marks would be a second answer to a
 * question the handle already answered.
 *
 * The handle is bigger than the track and lifts off it, which is the other
 * half of the reading: what the user takes hold of is not part of the rail.
 */
class QX_MATERIAL_EXPORT MaterialSlider : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue)
public:
    explicit MaterialSlider(QWidget *parent = Q_NULLPTR);
    ~MaterialSlider() override;

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
    Q_DISABLE_COPY(MaterialSlider)
    QX_DECLARE_PRIVATE(MaterialSlider)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALSLIDER_H
