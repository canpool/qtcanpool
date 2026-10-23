/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTBUTTON_H
#define FLUENTBUTTON_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

#include <QtGui/QIcon>

QX_FLUENT_BEGIN_NAMESPACE

class FluentButtonPrivate;

/*!
 * Fluent's button: the command the page is built out of.
 *
 * QPushButton draws itself through the style, and a Fluent application
 * replaces the style - which leaves the button either undressed or fighting
 * the dress it was given. This one is drawn from the family's tokens: three
 * kinds say three weights of command, the wash carries hover and press, and
 * the press never moves the face.
 */
class QX_FLUENT_EXPORT FluentButton : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(Kind kind READ kind WRITE setKind)
public:
    /*!
     * The weight of the command. \c Standard is the default one; \c Accent
     * is the one command a view is allowed to point at; \c Subtle is a
     * command that sits down while others speak.
     */
    enum class Kind {
        Standard,
        Accent,
        Subtle
    };
    Q_ENUM(Kind)

    explicit FluentButton(QWidget *parent = Q_NULLPTR);
    ~FluentButton() override;

    QString text() const;
    QIcon icon() const;
    Kind kind() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setText(const QString &text);
    void setIcon(const QIcon &icon);
    void setKind(Kind kind);
Q_SIGNALS:
    void textChanged(const QString &text);
    void iconChanged(const QIcon &icon);
protected:
    qreal cornerRadius() const override;
    QColor washColor() const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;

    QAccessible::Role accessibleRole() const override;
private:
    Q_DISABLE_COPY(FluentButton)
    QX_DECLARE_PRIVATE(FluentButton)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTBUTTON_H
