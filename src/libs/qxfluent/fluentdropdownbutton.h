/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTDROPDOWNBUTTON_H
#define FLUENTDROPDOWNBUTTON_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

#include <QtGui/QIcon>

class QMenu;

QX_FLUENT_BEGIN_NAMESPACE

class FluentDropDownButtonPrivate;

/*!
 * A command that is a menu: the whole face opens it.
 *
 * QToolButton can be told to do this - \c MenuButtonPopup, \c InstantPopup -
 * and then draws itself with a style that a Fluent application replaces anyway;
 * what it cannot be told is what this one says with the space, which is that
 * there is nothing here to press and then something else to choose. The face
 * is one thing, and pressing anywhere on it opens the menu: there is no half
 * of it that means "do the command" - that is what FluentToggleSplitButton is,
 * and the two are separate classes because they are separate promises.
 *
 * The menu belongs to the button. An application fills it - \c menu() is
 * never null, and aboutToShowMenu() is the place to put what is true now - and
 * never owns it, which is the one ownership question this class answers so that
 * callers do not have to.
 */
class QX_FLUENT_EXPORT FluentDropDownButton : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY iconChanged)
public:
    explicit FluentDropDownButton(QWidget *parent = Q_NULLPTR);
    ~FluentDropDownButton() override;

    QString text() const;
    QIcon icon() const;
    /*! The menu this button opens. It is the button's child and is created
     *  with it, so filling it is one call and owning it is nobody's problem. */
    QMenu *menu() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setText(const QString &text);
    void setIcon(const QIcon &icon);
    /*! Opens the menu where the button ends, the way a click does - for a
     *  caller that has a reason of its own. */
    void showMenu();

Q_SIGNALS:
    void textChanged(const QString &text);
    void iconChanged(const QIcon &icon);
    /*! Emitted just before the menu opens: the place to fill it with the
     *  commands that are true now rather than the ones that were true when it
     *  was built. */
    void aboutToShowMenu();
protected:
    qreal cornerRadius() const override;
    void paintContent(QPainter *painter) override;
    void keyPressEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
private:
    /*! The right end of the face, where the chevron stands. */
    QRectF chevronRect() const;
    /*! What is left for the icon and the text: from the leading inset to the
     *  chevron, which is the part of the face that is not the promise. */
    QRectF contentRect() const;
    QRectF iconRect() const;
    void openMenu();

    Q_DISABLE_COPY(FluentDropDownButton)
    QX_DECLARE_PRIVATE(FluentDropDownButton)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTDROPDOWNBUTTON_H
