/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALRADIO_H
#define MATERIALRADIO_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxcontrol.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialRadioPrivate;

/*!
 * Material's radio: one choice of several, where the choice itself is the
 * answer and the others stand or fall with it.
 *
 * The circle is eighteen across, and a checked one is the primary's ring
 * with the primary's own dot inside it. The ripple answers from the circle's
 * own centre and stays inside it, and the brothers that share the parent
 * cannot be on while one of them is.
 */
class QX_MATERIAL_EXPORT MaterialRadio : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY toggled)
public:
    explicit MaterialRadio(QWidget *parent = Q_NULLPTR);
    ~MaterialRadio() override;

    QString text() const;
    bool isChecked() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setText(const QString &text);
    /*! Puts this one in force. The brother buttons that share its parent
     *  come out of force with it, and a button already in force stays so. */
    void setChecked(bool checked);
Q_SIGNALS:
    void textChanged(const QString &text);
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
    Q_DISABLE_COPY(MaterialRadio)
    QX_DECLARE_PRIVATE(MaterialRadio)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALRADIO_H
