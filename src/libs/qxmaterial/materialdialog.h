/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALDIALOG_H
#define MATERIALDIALOG_H

#include "qxmaterial_global.h"

#include <QtWidgets/QDialog>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialDialogPrivate;

/*!
 * Material's dialog, in the two shapes M3 gives it: a card that asks over a
 * dimmed page, and a place that takes the whole page to itself.
 *
 * The difference between them is not size, it is what the thing is. A basic
 * dialog is a question with an answer - it holds a sentence and at most two
 * text buttons, and it does not want the page to be looked at while it is up.
 * A full-screen dialog is a place to do something in: it is full-bleed from
 * the top of the page, its title sits in a bar with a cross that leaves it,
 * and anything the application wants to put in it goes in the middle.
 *
 * Qt's QDialog is the window underneath, because modality, Escape, exec() and
 * a result code are the toolkit's; what is drawn here is the surface - the
 * scrim, the card at the 28 corner of the shape scale, or the whole-page
 * sheet - and the text buttons, which are text and not commands: a Material
 * dialog does not fill an answer in, it names it in the accent and lets the
 * press be the whole of it.
 *
 * The answers ask and do not decide. Pressing one emits buttonClicked() and
 * nothing else happens; whether the question was answered, and which way that
 * counts, is the application's. The cross of a full-screen dialog is the one
 * exception, and it is not an answer: it is the dialog being put away, which
 * is what a frameless window otherwise has no button for.
 */
class QX_MATERIAL_EXPORT MaterialDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString message READ message WRITE setMessage)
    Q_PROPERTY(Variant variant READ variant WRITE setVariant)
public:
    /*! The two shapes. \c Basic is a card in the middle of the page; \c
     *  FullScreen is the page, from the top down, with a bar at the top. */
    enum class Variant {
        Basic,     /*!< A question, asked over a dimmed page. */
        FullScreen /*!< A place to do something in, the whole page of it. */
    };
    Q_ENUM(Variant)

    /*! The answers a dialog can have, in the order they mean: the affirmative
     *  first. M3 gives a dialog at most two; the row is read to the right, so
     *  the answer to the question is the last thing in it. */
    enum class Role {
        Primary,  /*!< The affirmative: Save, Delete, Discard. */
        Secondary /*!< The other answer: Cancel, Not now. */
    };
    Q_ENUM(Role)

    explicit MaterialDialog(QWidget *parent = Q_NULLPTR);
    ~MaterialDialog() override;

    QString title() const;
    QString message() const;
    Variant variant() const;

    /*! Hands \a widget the room under the message. It becomes the dialog's
     *  child and is kept there; handing another one replaces it, and the one
     *  that was there is orphaned rather than deleted. */
    void setContent(QWidget *widget);
    QWidget *contentWidget() const;

    /*! The words on \a role's answer; empty means that role has no answer. */
    QString buttonText(Role role) const;
    /*! Where \a role's answer sits, in the dialog's own coordinates; an empty
     *  rectangle while that role has nothing to say. A dialog that draws its
     *  own answers has to say where it drew them - the application anchors
     *  things to them and a test aims at them. */
    QRect buttonRect(Role role) const;

    /*! The surface the dialog draws, in the dialog's own coordinates: the card
     *  in \c Basic, the whole dialog in \c FullScreen. */
    QRect cardRect() const;

    QSize sizeHint() const override;

public Q_SLOTS:
    void setTitle(const QString &title);
    void setMessage(const QString &message);
    void setVariant(Variant variant);
    void setButtonText(Role role, const QString &text);

Q_SIGNALS:
    /*! An answer was pressed. What it means is the application's. */
    void buttonClicked(MaterialDialog::Role role);
protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
private:
    /*! The height the card wants at its own width, before the dialog clamps it
     *  to the room there is. */
    int cardHeight() const;
    void relayout();
    void coverThePage();
    void beginTheEntrance();

    /*! Which answer, if any, is at \a pos: -1 for none, 0 and 1 for the roles
     *  in the order of the enum. */
    int answerAt(const QPoint &pos) const;
    bool isOnTheCross(const QPoint &pos) const;

    Q_DISABLE_COPY(MaterialDialog)
    QX_DECLARE_PRIVATE(MaterialDialog)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALDIALOG_H
