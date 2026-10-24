/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALSTEPPER_H
#define MATERIALSTEPPER_H

#include "qxmaterial_global.h"

#include <QtWidgets/QWidget>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialStepperPrivate;

/*!
 * Material's stepper: the way a long piece of work is told apart into steps.
 *
 * Every step is a number in a circle with its name under it - and a caption
 * under that, when the application gave one - and the steps stand in a row
 * joined by a line. The step in force is the accent's ring, the ones already
 * walked are the accent's fill with a tick in them, and the ones still ahead
 * are a quiet outline: the row says how far along the work is without a word
 * about it.
 *
 * A step that is behind can be taken again by clicking it, which is what a
 * form asks for when the answer two steps back turns out to be wrong. A step
 * ahead cannot be taken: how far along the work is, is the work's answer, not
 * the pointer's.
 */
class QX_MATERIAL_EXPORT MaterialStepper : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int currentStep READ currentStep WRITE setCurrentStep NOTIFY currentChanged)
public:
    /*! What a step is, from where the work stands. */
    enum class State {
        Completed, /*!< Walked: the accent's fill and a tick. */
        Current,   /*!< The one in force: the accent's ring. */
        Upcoming   /*!< Still ahead: a quiet outline. */
    };
    Q_ENUM(State)

    explicit MaterialStepper(QWidget *parent = Q_NULLPTR);
    ~MaterialStepper() override;

    /*! Adds a step named \a label, saying \a caption under it when it was
     *  given one. */
    void addStep(const QString &label, const QString &caption = QString());
    /*! How many steps there are. */
    int count() const;
    /*! The name of the step at \a index, or an empty string when there is
     *  none. */
    QString stepLabel(int index) const;
    /*! The caption of the step at \a index, empty when it says none. */
    QString stepCaption(int index) const;
    /*! Where the work stands. */
    int currentStep() const;
    /*! What the step at \a index is, from where the work stands. */
    State stepState(int index) const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    /*! Puts the work at \a step. A step past the last one is no answer and
     *  is ignored. */
    void setCurrentStep(int step);
Q_SIGNALS:
    /*! Where the work stands changed. */
    void currentChanged(int step);
protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
private:
    /*! The column the step at \a index stands in. */
    QRectF columnRect(int index) const;
    /*! The circle the step at \a index is numbered in. */
    QRectF circleRect(int index) const;
    /*! The step the point \a pos falls in, or -1 for none. */
    int stepAt(const QPointF &pos) const;

    Q_DISABLE_COPY(MaterialStepper)
    QX_DECLARE_PRIVATE(MaterialStepper)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALSTEPPER_H
