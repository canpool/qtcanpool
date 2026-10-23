/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXPOPUPSURFACE_H
#define QXPOPUPSURFACE_H

#include "qxcontrols_global.h"

#include <QtWidgets/QWidget>

class QPainter;

QX_CONTROLS_BEGIN_NAMESPACE

class QxPopupSurfacePrivate;

/*!
 * The surface a set of answers opens into: one answer, owed to every family
 * the same way.
 *
 * A combo box, a select, a menu flyout and a tooltip all stand on the same
 * four promises - the surface is drawn and lifted off the page, it opens
 * below its anchor and turns above when below has no room, it keeps inside
 * the screen when neither side has room, and it goes away when the keyboard
 * asks it to or the pointer lands outside it. Those four are mechanism, and
 * mechanism is here. What belongs to a family is what the surface carries:
 * its radius, and the content painted over it.
 *
 * The surface is a frameless popup window; a click outside it closes it, an
 * Escape closes it, and \c dismissed says so either way. What was chosen is
 * the content's business, not the surface's.
 */
class QX_CONTROLS_EXPORT QxPopupSurface : public QWidget
{
    Q_OBJECT
public:
    explicit QxPopupSurface(QWidget *anchor);
    ~QxPopupSurface() override;

    /*! The widget the surface stands beside when it opens. */
    QWidget *anchor() const;

    /*! The radius the surface's corners turn by; a family asks for its own
     *  before opening. Zero is square. */
    qreal surfaceRadius() const;
    void setSurfaceRadius(qreal radius);

    /*! Sizes itself from the content's answer, places itself beside the
     *  anchor, and shows. */
    void open();

    /*! The part of the widget the content paints in - the surface's own
     *  room, with the margin its shadow keeps taken off. */
    QRectF contentRect() const;

Q_SIGNALS:
    /*! The surface went away - by Escape or by a click outside it. What was
     *  chosen before it went is nobody's here. */
    void dismissed();
protected:
    /*! The room the content asks for, without the surface's shadow margin. */
    virtual QSize contentSizeHint() const = 0;
    /*! The content, over the surface, inside \a content's room. */
    virtual void paintContent(QPainter *painter, const QRectF &content) = 0;

    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void hideEvent(QHideEvent *event) override;
private:
    void place();
    Q_DISABLE_COPY(QxPopupSurface)
    QX_DECLARE_PRIVATE(QxPopupSurface)
};

QX_CONTROLS_END_NAMESPACE

#endif   // QXPOPUPSURFACE_H
