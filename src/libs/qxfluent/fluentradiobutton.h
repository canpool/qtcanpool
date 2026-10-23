/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTRADIOBUTTON_H
#define FLUENTRADIOBUTTON_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

QX_FLUENT_BEGIN_NAMESPACE

class FluentRadioButtonPrivate;

/*!
 * Fluent's radio button: one choice of several, where the choice itself is
 * the answer and the others stand or fall with it.
 *
 * The circle is twenty across; a checked one is the accent's ring and the
 * accent's own dot inside it. The label sits on the right and the whole row
 * answers the pointer, and the brothers it was born with - the radio buttons
 * that share its parent - cannot be on while it is.
 */
class QX_FLUENT_EXPORT FluentRadioButton : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY toggled)
public:
    explicit FluentRadioButton(QWidget *parent = Q_NULLPTR);
    ~FluentRadioButton() override;

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
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
    void fillAccessibleState(QAccessible::State *state) const override;
private:
    Q_DISABLE_COPY(FluentRadioButton)
    QX_DECLARE_PRIVATE(FluentRadioButton)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTRADIOBUTTON_H
