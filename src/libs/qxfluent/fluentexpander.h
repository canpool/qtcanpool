/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTEXPANDER_H
#define FLUENTEXPANDER_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

QX_FLUENT_BEGIN_NAMESPACE

class FluentExpanderPrivate;

/*!
 * Fluent's expander: a header that keeps a body, and opens to show it.
 *
 * Qt's answer to this is a QToolButton next to a widget the application hides
 * and shows itself, which is why the two disagree about where the clickable
 * part ends and why the movement is instant - a body that appears and a body
 * that is revealed are not the same thing to watch. Here the header is the
 * control: it is what the pointer can reach and what the keyboard stops at,
 * and the body travels out from under it on a Fluent duration.
 *
 * The body is the application's widget; the expander gives it room and
 * geometry, and takes the room back when it closes.
 */
class QX_FLUENT_EXPORT FluentExpander : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(bool expanded READ isExpanded WRITE setExpanded NOTIFY expandedChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle)
public:
    explicit FluentExpander(QWidget *parent = Q_NULLPTR);
    ~FluentExpander() override;

    bool isExpanded() const;
    QString title() const;

    /*! The widget the header keeps; null while none was given. */
    QWidget *content() const;

    QSize sizeHint() const override;

public Q_SLOTS:
    void setTitle(const QString &title);
    /*! Gives the header a body to keep. The expander lays it out and does not
     *  own it. */
    void setContent(QWidget *content);
    void setExpanded(bool expanded);
    void toggle();

Q_SIGNALS:
    /*! Emitted whenever the header opens or closes, whatever did it. */
    void expandedChanged(bool expanded);
protected:
    QRectF chromeRect() const override;
    bool hitTest(const QPointF &pos) const override;
    void paintContent(QPainter *painter) override;
    void resizeEvent(QResizeEvent *event) override;

    QAccessible::Role accessibleRole() const override;
    void fillAccessibleState(QAccessible::State *state) const override;
private:
    Q_DISABLE_COPY(FluentExpander)
    QX_DECLARE_PRIVATE(FluentExpander)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTEXPANDER_H
