/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTTOOLTIP_H
#define FLUENTTOOLTIP_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

QX_FLUENT_BEGIN_NAMESPACE

class FluentToolTipPrivate;

/*!
 * Fluent's tooltip: a word of explanation that waits beside its anchor until
 * the pointer or the keyboard arrives, and goes away when either does.
 *
 * The page's own tooltip machinery draws through the style, and the style has
 * none of Fluent in it. This one is the family's answer: a small surface in
 * the ink the page writes with, saying its words in the page's colour - the
 * page turned inside out, which is what says "this is a note about the page,
 * not a part of it".
 *
 * It watches its anchor rather than being driven: pointer arrives, it waits
 * a moment in case the arrival was only passing through; the arrival stays,
 * it opens; the pointer leaves, the keyboard leaves or the anchor is hidden,
 * it closes. It is not interactive - it takes no mouse, no focus and no
 * keyboard - so unlike a popup it never stands between the user and what
 * they were doing.
 */
class QX_FLUENT_EXPORT FluentToolTip : public QxControls::QxControl
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

    explicit FluentToolTip(QWidget *parent = Q_NULLPTR);
    ~FluentToolTip() override;

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

    Q_DISABLE_COPY(FluentToolTip)
    QX_DECLARE_PRIVATE(FluentToolTip)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTTOOLTIP_H
