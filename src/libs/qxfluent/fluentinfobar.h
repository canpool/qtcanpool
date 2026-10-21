/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTINFOBAR_H
#define FLUENTINFOBAR_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

QX_FLUENT_BEGIN_NAMESPACE

class FluentInfoBarPrivate;

/*!
 * Fluent's info bar: a message that stays until it is dealt with.
 *
 * A QMessageBox is a window the user has to dismiss before doing anything
 * else, and a status tip is a message that leaves before it has been read;
 * what sits between them - something happened, it matters, it does not stop
 * you - is what this is. It is not modal, it has a severity, and it stays
 * until the application or the user closes it.
 *
 * The severity is what it is about, so the severity is what colours it. The
 * colours are the family's own: a theme has a role for a highlight and none
 * for "warning", so a bar carries them rather than asking the palette for
 * something it does not have.
 */
class QX_FLUENT_EXPORT FluentInfoBar : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString message READ message WRITE setMessage)
    Q_PROPERTY(bool open READ isOpen WRITE setOpen)
public:
    /*! What the bar is about. */
    enum class Severity {
        Informational, /*!< Something to know; the accent, because it is this
                            application speaking. */
        Success,       /*!< What was asked for happened. */
        Warning,       /*!< It happened, but not the way it should. */
        Error          /*!< It did not happen. */
    };
    Q_ENUM(Severity)

    explicit FluentInfoBar(QWidget *parent = Q_NULLPTR);
    ~FluentInfoBar() override;

    QString title() const;
    QString message() const;
    Severity severity() const;
    /*! True while the bar is showing; a closed bar hides itself. */
    bool isOpen() const;

    QSize sizeHint() const override;

public Q_SLOTS:
    void setTitle(const QString &title);
    void setMessage(const QString &message);
    void setSeverity(Severity severity);
    void setOpen(bool open);

Q_SIGNALS:
    /*! Emitted when the bar closes, whoever closed it. */
    void closed();
protected:
    qreal cornerRadius() const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;
    void mousePressEvent(QMouseEvent *event) override;

    QAccessible::Role accessibleRole() const override;
private:
    Q_DISABLE_COPY(FluentInfoBar)
    QX_DECLARE_PRIVATE(FluentInfoBar)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTINFOBAR_H
