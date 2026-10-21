/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTTEACHINGTIP_H
#define FLUENTTEACHINGTIP_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

QX_FLUENT_BEGIN_NAMESPACE

class FluentTeachingTipPrivate;

/*!
 * Fluent's teaching tip: a flyout that points at the thing it is about.
 *
 * Qt has a tool tip, and a teaching tip is not one: a tool tip is a sentence
 * the toolkit decides to show andhide, it cannot be asked to stay, it holds
 * no buttons, and it does not point at anything - it appears where the
 * pointer is. This one is opened by the application on a target of its
 * choosing, stays until it is dismissed, carries a title, a sentence and a
 * way out, and points at what it is about with a tail - which is what tells
 * the user which of the things on screen the sentence is about.
 *
 * It is a popup: clicking anywhere else dismisses it, which is the Fluent
 * reading of a light dismiss and Qt's reading of Qt::Popup.
 */
class QX_FLUENT_EXPORT FluentTeachingTip : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString message READ message WRITE setMessage)
    Q_PROPERTY(bool open READ isOpen WRITE setOpen)
public:
    /*! Where the tip sits relative to its target, and so where the tail is. */
    enum class Side {
        Auto,   /*!< Below if there is room, above if there is not. */
        Top,    /*!< Above the target, tail at the bottom. */
        Bottom, /*!< Below the target, tail at the top. */
        Left,   /*!< To its left, tail at the right. */
        Right   /*!< To its right, tail at the left. */
    };
    Q_ENUM(Side)

    explicit FluentTeachingTip(QWidget *parent = Q_NULLPTR);
    ~FluentTeachingTip() override;

    QString title() const;
    QString message() const;

    QWidget *target() const;
    Side preferredSide() const;
    /*! Which side the tip ended up on once the room was measured; \a Auto is
     *  never the answer to this. */
    Side side() const;

    bool isOpen() const;

    QSize sizeHint() const override;

public Q_SLOTS:
    void setTitle(const QString &title);
    void setMessage(const QString &message);
    void setTarget(QWidget *target);
    void setPreferredSide(Side side);
    void setOpen(bool open);

Q_SIGNALS:
    /*! Emitted when the tip goes away, whoever dismissed it. */
    void closed();
protected:
    qreal cornerRadius() const override;
    QRectF chromeRect() const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;

    void mousePressEvent(QMouseEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

    QAccessible::Role accessibleRole() const override;
private:
    Q_DISABLE_COPY(FluentTeachingTip)
    QX_DECLARE_PRIVATE(FluentTeachingTip)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTTEACHINGTIP_H
