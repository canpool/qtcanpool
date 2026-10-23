/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTCHECKBOX_H
#define FLUENTCHECKBOX_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

#include <QtCore/QFlags>

QX_FLUENT_BEGIN_NAMESPACE

class FluentCheckBoxPrivate;

/*!
 * Fluent's check box: a choice that stands beside other choices, each of
 * which is its own.
 *
 * The box is twenty across and the tick is drawn, not typed; the label sits
 * on the right and the whole row answers the pointer, because the row is
 * what looks like the control. Three states carry the three answers a list
 * of choices can give - none, some, all - when the application asks for the
 * third one.
 */
class QX_FLUENT_EXPORT FluentCheckBox : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(Qt::CheckState checkState READ checkState WRITE setCheckState NOTIFY stateChanged)
    Q_PROPERTY(bool tristate READ isTristate WRITE setTristate)
public:
    explicit FluentCheckBox(QWidget *parent = Q_NULLPTR);
    ~FluentCheckBox() override;

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
    qreal cornerRadius() const override;
    QRectF chromeRect() const override;
    bool hitTest(const QPointF &pos) const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
    void fillAccessibleState(QAccessible::State *state) const override;
private:
    Q_DISABLE_COPY(FluentCheckBox)
    QX_DECLARE_PRIVATE(FluentCheckBox)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTCHECKBOX_H
