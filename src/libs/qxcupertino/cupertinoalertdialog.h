/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef CUPERTINOALERTDIALOG_H
#define CUPERTINOALERTDIALOG_H

#include "qxcupertino_global.h"

#include <QtCore/QString>
#include <QtWidgets/QDialog>

QX_CUPERTINO_BEGIN_NAMESPACE

class CupertinoAlertDialogPrivate;

/*!
 * Cupertino's alert: a card the width of a hand, standing in the middle of a
 * dimmed page, with the answers along its bottom edge.
 *
 * What makes it this family's and not another's is that the answers are part of
 * the card: they are cells of it, separated from the question and from each
 * other by hairlines, and they are read as a row rather than as a row of
 * commands. A Material dialog sets its answers out as text in a row with room
 * between them; here they are as wide as the card and the line between them is
 * the only thing that says where one ends.
 *
 * One answer is a cell across the card, two are two cells side by side, and
 * three are three cells stacked - which is the platform's own rule and the
 * reason the control takes three and refuses a fourth: a card that stacked four
 * would be taller than the question it asks.
 *
 * Qt's QDialog is the window underneath, because modality, Escape, exec() and a
 * result code are the toolkit's. The answers ask and do not decide: pressing
 * one emits actionChosen() and nothing else; whether the alert was answered,
 * and which way that counts, is the application's - which is what the
 * application's slot does with the index and with the dialog's result.
 */
class QX_CUPERTINO_EXPORT CupertinoAlertDialog : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString message READ message WRITE setMessage)
public:
    /*! What an answer is worth. The role decides the ink and nothing else: the
     *  affirmative and the way out are both the accent, the one answer that
     *  destroys what the alert names is the family's red. */
    enum class ActionRole {
        Default,    /*!< The answer the alert is asking for. */
        Cancel,     /*!< The way out, in the heavier cut: the one that leaves. */
        Destructive /*!< The answer that cannot be taken back. */
    };
    Q_ENUM(ActionRole)

    explicit CupertinoAlertDialog(QWidget *parent = Q_NULLPTR);
    ~CupertinoAlertDialog() override;

    QString title() const;
    QString message() const;

    /*! The number of answers, at most three. */
    static int maximumActionCount();
    /*! Adds an answer at the end and answers whether it took it: a fourth is
     *  refused, and refusing is the answer rather than an error. */
    bool addAction(const QString &text, ActionRole role = ActionRole::Default);
    int actionCount() const;
    QString actionText(int index) const;
    ActionRole actionRole(int index) const;
    /*! Where \a index's answer sits, in the dialog's own coordinates; empty for
     *  an index that has no answer. A dialog that paints its own answers has to
     *  say where it painted them - the application anchors to them and a test
     *  aims at them. */
    QRect actionRect(int index) const;

    /*! The card the alert draws, in the dialog's own coordinates. */
    QRect cardRect() const;

    QSize sizeHint() const override;

public Q_SLOTS:
    void setTitle(const QString &title);
    void setMessage(const QString &message);
    /*! Takes every answer away, as if none had been added. */
    void clearActions();

Q_SIGNALS:
    /*! An answer was pressed, by index in the order it was added. What it means
     *  is the application's. */
    void actionChosen(int index);
protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void changeEvent(QEvent *event) override;
private:
    /*! Recomputes the card and the answers after anything that changes either. */
    void relayout();
    /*! Grows the dialog to the page it was opened over, so that the veil covers
     *  it; a dialog with no page under it stays the size of its card. */
    void coverThePage();
    /*! Which answer is at \a pos, or -1 for the card itself or the veil. */
    int actionAt(const QPoint &pos) const;
    /*! The height the card wants at its own width, before the dialog clamps it
     *  to the room there is. */
    int cardHeight() const;
    /*! The rectangle the question sits in, inside the card. */
    QRect textRect() const;

    Q_DISABLE_COPY(CupertinoAlertDialog)
    QX_DECLARE_PRIVATE(CupertinoAlertDialog)
};

QX_CUPERTINO_END_NAMESPACE

#endif   // CUPERTINOALERTDIALOG_H
