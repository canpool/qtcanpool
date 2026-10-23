/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALCHECKBOX_H
#define MATERIALCHECKBOX_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxcontrol.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialCheckBoxPrivate;

/*!
 * Material's check box: a choice that stands beside other choices, each of
 * which is its own.
 *
 * The box is eighteen across with the tick drawn, not typed; the label sits
 * on the right and the whole row answers the pointer. The press is answered
 * from the box, not from where the pointer landed - the ripple starts at the
 * box's own centre and stays inside it, because the box is the control and
 * the row is only what looks like it.
 */
class QX_MATERIAL_EXPORT MaterialCheckBox : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(Qt::CheckState checkState READ checkState WRITE setCheckState NOTIFY stateChanged)
    Q_PROPERTY(bool tristate READ isTristate WRITE setTristate)
public:
    explicit MaterialCheckBox(QWidget *parent = Q_NULLPTR);
    ~MaterialCheckBox() override;

    QString text() const;
    Qt::CheckState checkState() const;
    bool isChecked() const;
    bool isTristate() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setText(const QString &text);
    void setCheckState(Qt::CheckState state);
    void setChecked(bool checked);
    void setTristate(bool tristate);
Q_SIGNALS:
    void textChanged(const QString &text);
    /*! The state moved; \a checked is the on-or-off reading of it. */
    void stateChanged(Qt::CheckState state);
    void toggled(bool checked);
protected:
    QRectF chromeRect() const override;
    bool hitTest(const QPointF &pos) const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
    void fillAccessibleState(QAccessible::State *state) const override;
private:
    Q_DISABLE_COPY(MaterialCheckBox)
    QX_DECLARE_PRIVATE(MaterialCheckBox)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALCHECKBOX_H
