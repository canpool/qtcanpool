/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALSWITCH_H
#define MATERIALSWITCH_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxcontrol.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialSwitchPrivate;

/*!
 * Material's switch: a track with a handle that crosses it.
 *
 * Qt has a check box and no switch, and they are not the same answer - a
 * switch says what it stands for is in force now, a check box says a choice
 * was made and will be collected later. Material's switch is not Fluent's,
 * either: where Fluent's is a small pill with a knob that keeps its size,
 * this one has a track that fills with the primary colour and a handle that
 * carries a mark once it is on - so the state is readable from the handle
 * alone, without comparing it against the other end of the track.
 */
class QX_MATERIAL_EXPORT MaterialSwitch : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(bool on READ isOn WRITE setOn)
public:
    explicit MaterialSwitch(QWidget *parent = Q_NULLPTR);
    ~MaterialSwitch() override;

    bool isOn() const;

    QString text() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setOn(bool on);
    void toggle();
    void setText(const QString &text);

Q_SIGNALS:
    /*! Emitted when the switch turns on or off, whoever turned it. */
    void toggled(bool on);
protected:
    qreal cornerRadius() const override;
    QRectF chromeRect() const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
    void fillAccessibleState(QAccessible::State *state) const override;
private:
    Q_DISABLE_COPY(MaterialSwitch)
    QX_DECLARE_PRIVATE(MaterialSwitch)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALSWITCH_H
