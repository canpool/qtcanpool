/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXCONTROL_H
#define QXCONTROL_H

#include "qxcontrols_global.h"
#include "qxtheme/qxtheme.h"

#include <QtGui/QAccessible>
#include <QtWidgets/QWidget>

class QPainter;

QX_CONTROLS_BEGIN_NAMESPACE

class QxControlPrivate;

/*!
 * The base a self-drawn control builds on: what every control has to answer
 * the same way, answered once.
 *
 * A control that paints itself still has to track where the pointer is, what
 * is being pressed, whether the keyboard is on it and whether it is enabled;
 * it has to draw a wash for those states between its surface and its content,
 * a focus ring over both, and it has to know the tokens the family gave the
 * theme. All of that is mechanism, and it lives here - a subclass says what
 * its surface looks like, what its content is, and when the pointer counts as
 * being on it, and inherits the rest.
 *
 * The paint order is fixed and is the reason the surface and the content are
 * two hooks rather than one: surface, then the state wash on top of it, then
 * the content on top of the wash, then the focus ring over everything. A
 * control that drew its glyphs in the surface hook would have the wash land
 * on them, which is the difference between a hover that tints the control and
 * one that dims its label.
 */
class QX_CONTROLS_EXPORT QxControl : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
public:
    /*!
     * Where the control stands with the pointer. \c Rest is the control as it
     * sits; \c Hover is the pointer on it; \c Pressed is a press that started
     * on it and has not been released yet. Being enabled and having the focus
     * are QWidget's own answers and are not repeated here.
     */
    enum class State {
        Rest,
        Hover,
        Pressed
    };
    Q_ENUM(State)

    explicit QxControl(QWidget *parent = Q_NULLPTR);
    ~QxControl() override;

    /*! Where the control stands with the pointer, as the base tracked it. */
    State state() const;
    /*! True while the state is anything but \c Rest. */
    bool isActive() const;

    /*!
     * The density tier the theme in force asks for, read from its
     * registration; \c QxTheme::Density::Comfortable while no theme is in
     * force, which is the tier the sheets speak.
     */
    QxTheme::Density density() const;

    /*!
     * The colour the control's text and glyphs paint with - the palette's
     * WindowText, which is what a plain QWidget would use.
     */
    QColor textColor() const;
    /*! The accent of the theme in force, through the theme manager. */
    QColor accentColor() const;

    /*!
     * What the control is, for an assistive tool. \c QAccessible::Client for a
     * control that does not say - which is the role a plain widget gets, and
     * the honest answer while a control has not been given one. Public because
     * the accessible face of a control is not the control: it asks, the way a
     * painter asks for the surface.
     */
    virtual QAccessible::Role accessibleRole() const;

    /*!
     * What is true of the control beyond what any widget already answers -
     * enabled, focusable, focused - which \a state arrives holding. The base
     * adds nothing; a control that is checked, pressed or selected says so
     * here rather than by writing an accessible object of its own.
     */
    virtual void fillAccessibleState(QAccessible::State *state) const;

Q_SIGNALS:
    /*! Emitted after the state changed. The wash a subclass draws follows it. */
    void stateChanged(QxControl::State state);
    /*! The press was released on the control: a click, as widgets mean it. */
    void clicked();
protected:
    /*!
     * How far the control's corners turn, in the theme's shape tokens. The
     * base uses it for the wash and the focus ring; a subclass paints its
     * surface with it as well. Zero by default - a square control.
     */
    virtual qreal cornerRadius() const;

    /*!
     * True when \a pos (in widget coordinates) counts as being on the
     * control. The default is the widget's rectangle; a control whose click
     * area is not its paint area - a small box with a wide label - overrides
     * this rather than lying about its geometry.
     */
    virtual bool hitTest(const QPointF &pos) const;

    /*!
     * The part of the control the wash and the focus ring belong to: the whole
     * widget by default.
     *
     * A control is often a small thing with a sentence beside it - a switch's
     * track, a check box's box - and the wash and the ring belong to the small
     * thing: tinting the sentence is a hover that dims the label, and a ring
     * round the whole row is a ring round nothing. A control that is all one
     * thing does not override this.
     */
    virtual QRectF chromeRect() const;

    /*! The wash colour for the current state; the text colour by default. */
    virtual QColor washColor() const;

    /*! Draw the control's own surface - the fill under everything else. */
    virtual void paintSurface(QPainter *painter);
    /*! Draw the control's content - glyphs, labels, indicators - over the wash. */
    virtual void paintContent(QPainter *painter);

    void paintEvent(QPaintEvent *event) override;
    // The entering event gained a payload in Qt 6; the handler does not read it.
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *event) override;
#else
    void enterEvent(QEvent *event) override;
#endif
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void changeEvent(QEvent *event) override;
private:
    void setState(State state);

    Q_DISABLE_COPY(QxControl)
    QX_DECLARE_PRIVATE(QxControl)
};

QX_CONTROLS_END_NAMESPACE

#endif   // QXCONTROL_H
