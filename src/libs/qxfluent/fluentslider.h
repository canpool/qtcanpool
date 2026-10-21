/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTSLIDER_H
#define FLUENTSLIDER_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

QX_FLUENT_BEGIN_NAMESPACE

class FluentSliderPrivate;
class FluentSliderAccessible;

/*!
 * Fluent's slider: a rail with a thumb on it, and - unlike Material's - the
 * marks that say where the rail is divided.
 *
 * QSlider is a styled widget, which means its look belongs to the QStyle in
 * force and a Fluent application gets whatever the platform drew. This one is
 * drawn: the rail fills with the accent up to the thumb, the thumb is a disc
 * that rides it, and the ticks underneath are Fluent's way of saying the rail
 * is divided - a slider without them is a slider that will not tell you where
 * to stop.
 *
 * A drag is answered at once, because a thumb that lags the pointer is a
 * slider that feels broken; a step from the keyboard travels, because there
 * the movement is the feedback.
 */
class QX_FLUENT_EXPORT FluentSlider : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(int minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(int maximum READ maximum WRITE setMaximum)
    Q_PROPERTY(int singleStep READ singleStep WRITE setSingleStep)
    Q_PROPERTY(int tickInterval READ tickInterval WRITE setTickInterval)
public:
    explicit FluentSlider(QWidget *parent = Q_NULLPTR);
    ~FluentSlider() override;

    int value() const;
    int minimum() const;
    int maximum() const;
    int singleStep() const;
    int pageStep() const;
    /*! 0 draws no ticks; any other value draws one every \a interval. */
    int tickInterval() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setValue(int value);
    void setMinimum(int minimum);
    void setMaximum(int maximum);
    void setRange(int minimum, int maximum);
    void setSingleStep(int step);
    void setPageStep(int step);
    void setTickInterval(int interval);

Q_SIGNALS:
    /*! Emitted whenever the value changes, whatever changed it. */
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
    Q_DISABLE_COPY(FluentSlider)
    QX_DECLARE_PRIVATE(FluentSlider)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTSLIDER_H
