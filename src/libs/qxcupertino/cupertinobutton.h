/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef CUPERTINOBUTTON_H
#define CUPERTINOBUTTON_H

#include "qxcupertino_global.h"

#include "qxcontrols/qxcontrol.h"

QX_CUPERTINO_BEGIN_NAMESPACE

class CupertinoButtonPrivate;

/*!
 * Cupertino's button: a word in a capsule, at one of the three levels the
 * family gives an action.
 *
 * The levels are the whole of what tells one button from another here - there
 * is no icon, no second line, no arrow: the surface is a word, and what the
 * word is worth is said by how much surface it stands on. \c Filled is the one
 * action on the surface and carries the family's blue at full strength; \c
 * Tinted is the ones beside it, the same blue thinned into the page so a row of
 * two does not read as two primary actions; \c Plain is an action that is only
 * a word, with no fill at all.
 *
 * The capsule casts nothing. A raised thing in this family lies on something -
 * the pill of the segmented control lies on the row's grey - and a button lies
 * on the page, so its lift is the level of the surface underneath and not its
 * own.
 *
 * Pressed, the capsule takes the base's wash rather than shrinking: a control
 * that moved under the finger would be moving the page, and this family moves
 * things when they arrive.
 */
class QX_CUPERTINO_EXPORT CupertinoButton : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(Role role READ role WRITE setRole NOTIFY roleChanged)
public:
    /*!
     * How much surface the action stands on. The three levels are the family's
     * three levels of prominence, in the order it ranks them.
     */
    enum class Role {
        Filled, /*!< The one action on the surface: the blue itself, word reversed out. */
        Tinted, /*!< The ones beside it: the blue thinned into the page, the word in the blue. */
        Plain   /*!< An action that is only a word: no fill, the word in the blue. */
    };
    Q_ENUM(Role)

    explicit CupertinoButton(QWidget *parent = Q_NULLPTR);
    explicit CupertinoButton(const QString &text, QWidget *parent = Q_NULLPTR);
    ~CupertinoButton() override;

    /*! What the button says. */
    QString text() const;
    /*! How much surface it stands on. */
    Role role() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    /*! Replaces what the button says. What it says is also its accessible
     *  name, for as long as the application has not given it a better one. */
    void setText(const QString &text);
    /*! Puts the action at \a role. */
    void setRole(Role role);

Q_SIGNALS:
    /*! Emitted when the word changed. */
    void textChanged(const QString &text);
    /*! Emitted when the level changed. */
    void roleChanged(Role role);
protected:
    qreal cornerRadius() const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
private:
    Q_DISABLE_COPY(CupertinoButton)
    QX_DECLARE_PRIVATE(CupertinoButton)
};

QX_CUPERTINO_END_NAMESPACE

#endif   // CUPERTINOBUTTON_H
