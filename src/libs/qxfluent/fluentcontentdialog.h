/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTCONTENTDIALOG_H
#define FLUENTCONTENTDIALOG_H

#include "qxfluent_global.h"

#include <QtWidgets/QDialog>

class QPushButton;

QX_FLUENT_BEGIN_NAMESPACE

class FluentContentDialogPrivate;

/*!
 * Fluent's content dialog: a question asked over a dimmed page.
 *
 * Qt's QDialog is a window with a title bar and a QDialogButtonBox in it, and
 * what a Fluent dialog is made of is none of those: it is a card that sits in
 * the middle of the page it was opened over, on a veil that says the page is
 * not the thing to look at now, with no chrome of the platform's anywhere in
 * it. So the surface is drawn here - the veil, the shadow of the 64 level, the
 * rounded card - and the window underneath is still a QDialog, because
 * modality, Escape, exec() and a result code are the toolkit's and writing them
 * again would only produce a worse copy.
 *
 * A dialog is a place, not a moment, so it is not a popup: it is opened by the
 * application, it stays until the application - or Escape - closes it, and it
 * takes the whole of the page it was opened over. A dialog with no page under
 * it has nothing to dim and stands at its own size, veil and all.
 *
 * The buttons are the toolkit's QPushButtons and the dialog does not answer for
 * them: pressing one emits clicked() and nothing else happens. Whether the
 * question was answered, and which way that counts, is the application's to
 * decide - a dialog sits on top of the write it is asking about, and a control
 * that closed itself would take that decision away. At most three exist, one of
 * each role, and each one is made the first time it is asked for.
 */
class QX_FLUENT_EXPORT FluentContentDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString message READ message WRITE setMessage)
public:
    /*! Which of the three a button is. \c Primary is the one the keyboard
     *  reaches on Return and the one that sits first in the row, \c Close is
     *  the way out - Fluent reads the row from the affirmative, so the answer
     *  to the question comes before the way to leave it unanswered. */
    enum class Role {
        Primary,   /*!< The affirmative: Save, Delete, Continue. */
        Secondary, /*!< The other answer: Not now, keep the old one. */
        Close      /*!< The way out that answers nothing. */
    };
    Q_ENUM(Role)

    explicit FluentContentDialog(QWidget *parent = Q_NULLPTR);
    ~FluentContentDialog() override;

    QString title() const;
    QString message() const;

    /*! Hands \a widget the room under the message. It becomes the card's child
     *  and is kept there; handing another one replaces it, and the one that was
     *  there is orphaned rather than deleted. */
    void setContent(QWidget *widget);
    QWidget *contentWidget() const;

    /*! The button of \a role, made the first time it is asked for - so a dialog
     *  that never asks for one has none, which is how "at most three" is kept.
     *  Give it its text before showing the dialog; the dialog lays the row out
     *  again when it is resized, not when a button changes. */
    QPushButton *primaryButton();
    QPushButton *secondaryButton();
    QPushButton *closeButton();

    /*! The card the dialog draws, in the dialog's own coordinates. The card is
     *  what a caller measures against: the buttons and the content all sit
     *  inside this rectangle. */
    QRect cardRect() const;

    QSize sizeHint() const override;

public Q_SLOTS:
    void setTitle(const QString &title);
    void setMessage(const QString &message);
protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
private:
    /*! The height the card wants at its own width, before the dialog clamps it
     *  to the room there is. */
    int cardHeight() const;
    void relayout();
    void coverThePage();
    void beginTheEntrance();
    QPushButton *ensureButton(Role role);

    Q_DISABLE_COPY(FluentContentDialog)
    QX_DECLARE_PRIVATE(FluentContentDialog)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTCONTENTDIALOG_H
