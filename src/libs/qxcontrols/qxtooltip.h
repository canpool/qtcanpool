/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXTOOLTIP_H
#define QXTOOLTIP_H

#include "qxcontrols_global.h"

#include "qxcontrol.h"

QX_CONTROLS_BEGIN_NAMESPACE

class QxToolTipPrivate;

/*!
 * The note a control explains itself with, answered the same way by every
 * family: it waits beside its anchor until the pointer or the keyboard
 * arrives, and goes away when either does.
 *
 * The page's own tooltip machinery draws through the style; the families
 * want their own, and their answers to what a tooltip *is* are the same
 * answer. It watches its anchor rather than being driven: pointer arrives,
 * it waits a moment in case the arrival was only passing through; the
 * arrival stays, it opens; the pointer leaves, the keyboard leaves or the
 * anchor is hidden, it closes. It is not interactive - it takes no mouse,
 * no focus and no keyboard - so unlike a popup it never stands between the
 * user and what they were doing.
 *
 * The face is the page turned inside out - the ink the page writes with as
 * the surface, the page's colour as the text - which is what says "this is
 * a note about the page, not a part of it". A family that reads its inverse
 * pair from other tokens overrides \c cornerRadius, and that is the whole
 * of what it carries.
 */
class QX_CONTROLS_EXPORT QxToolTip : public QxControl
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(Placement placement READ placement WRITE setPlacement)
public:
    /*!
     * Which side of the anchor the tooltip stands on. \c Top is where a note
     * about a control goes unless the control says otherwise.
     */
    enum class Placement {
        Top,
        Bottom,
        Left,
        Right
    };
    Q_ENUM(Placement)

    explicit QxToolTip(QWidget *parent = Q_NULLPTR);
    ~QxToolTip() override;

    /*! The widget the tooltip waits beside and takes its cues from. */
    void setAnchor(QWidget *anchor);
    QWidget *anchor() const;

    /*! The words the tooltip says. */
    QString text() const;
    /*! Which side of the anchor the tooltip stands on. */
    Placement placement() const;

    QSize sizeHint() const override;

public Q_SLOTS:
    void setText(const QString &text);
    void setPlacement(Placement placement);
    /*! Places itself beside the anchor and shows now, without the arrival
     *  delay; it still goes away on its own after the stay is spent. */
    void popup();
protected:
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

    QAccessible::Role accessibleRole() const override;
private:
    void place();
    void stopWatching();

    Q_DISABLE_COPY(QxToolTip)
    QX_DECLARE_PRIVATE(QxToolTip)
};

QX_CONTROLS_END_NAMESPACE

#endif   // QXTOOLTIP_H
