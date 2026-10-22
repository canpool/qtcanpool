/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTTOGGLESPLITBUTTON_H
#define FLUENTTOGGLESPLITBUTTON_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

#include <QtGui/QIcon>

class QMenu;

QX_FLUENT_BEGIN_NAMESPACE

class FluentToggleSplitButtonPrivate;

/*!
 * A command that stays down, with a menu beside it: doing and choosing more
 * are two things.
 *
 * The left half is the command - it goes down and stays down, and the button
 * says the command is in force - and the right half is the chevron that opens
 * the menu. That split is the whole of what this class is: QToolButton's
 * MenuButtonPopup has the two halves and gives the menu to the arrow alone,
 * which is the same shape saying something else, and neither it nor a button
 * with a menu beside it can say "this is on" without the menu being the answer
 * to why. Here a press is answered by the half it started on and by nothing
 * else: a press that begins on the body and is released over the chevron has
 * not chosen more, and a press that begins on the chevron has not done the
 * command.
 *
 * A button that is on is washed in the accent, which is the surface's answer
 * and not the content's - the label is still the label, the way it is on a
 * command in force in FluentCommandBar. For an assistive tool it is a check
 * box: the question it answers is "is this in force", and that is the state a
 * check box carries.
 */
class QX_FLUENT_EXPORT FluentToggleSplitButton : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY toggled)
public:
    explicit FluentToggleSplitButton(QWidget *parent = Q_NULLPTR);
    ~FluentToggleSplitButton() override;

    QString text() const;
    QIcon icon() const;
    bool isChecked() const;
    /*! The menu the chevron opens. It is the button's child, as on
     *  FluentDropDownButton, so filling it is a call and owning it is not the
     *  application's problem. */
    QMenu *menu() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setText(const QString &text);
    void setIcon(const QIcon &icon);
    void setChecked(bool checked);
    /*! Puts the command the other way, as the left half does. */
    void toggle();
    /*! Opens the menu where the button ends, as the chevron's half does. */
    void showMenu();

Q_SIGNALS:
    void textChanged(const QString &text);
    void iconChanged(const QIcon &icon);
    /*! The command went down or came back up. */
    void toggled(bool checked);
    /*! Emitted just before the menu opens: the place to fill it with what is
     *  true now. */
    void aboutToShowMenu();
protected:
    qreal cornerRadius() const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void fillAccessibleState(QAccessible::State *state) const override;
    QAccessible::Role accessibleRole() const override;
private:
    /*! Which half a point falls in. \c None is a press that has left the half
     *  it started on, which is not a choice of either. */
    enum class Half {
        None,
        Body,
        Chevron
    };

    /*! The right end of the face, where the chevron stands. */
    QRectF chevronRect() const;
    /*! What is left for the icon and the text: the half that means the
     *  command. */
    QRectF contentRect() const;
    QRectF iconRect() const;
    Half halfAt(const QPointF &pos) const;
    void openMenu();

    Q_DISABLE_COPY(FluentToggleSplitButton)
    QX_DECLARE_PRIVATE(FluentToggleSplitButton)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTTOGGLESPLITBUTTON_H
