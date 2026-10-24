/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTTEXTBOX_H
#define FLUENTTEXTBOX_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
QT_END_NAMESPACE

QX_FLUENT_BEGIN_NAMESPACE

class FluentTextBoxPrivate;

/*!
 * Fluent's text box: one line of text in a face the page can see, with the
 * room for an icon before the words and an answer after them.
 *
 * The typing itself stays QLineEdit's - the cursor, the selection, the input
 * method and the clipboard are a platform's work, and a face drawn over them
 * is all a family owes. What the family owes is the face: the box slightly
 * inked under a hairline, the prompt that stands where the words will while
 * there are none, an icon at the left for what the box is for, and - once
 * there is something to erase - the clear at the right that answers a click
 * by emptying the box and saying so.
 *
 * The keyboard's arrival is drawn by the accent, as a two-pixel underline
 * along the box's foot: a text box does not want the wash and the ring a
 * pressed control gets, it wants the line that says "here is where the words
 * go".
 */
class QX_FLUENT_EXPORT FluentTextBox : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString placeholderText READ placeholderText WRITE setPlaceholderText)
    Q_PROPERTY(QIcon leadingIcon READ leadingIcon WRITE setLeadingIcon)
public:
    explicit FluentTextBox(QWidget *parent = Q_NULLPTR);
    ~FluentTextBox() override;

    /*! What the box says. */
    QString text() const;
    /*! The prompt that stands where the words will while there are none. */
    QString placeholderText() const;
    /*! The icon before the words, saying what the box is for; a null icon is
     *  no icon and the words start where the box starts. */
    QIcon leadingIcon() const;

    /*! The line editor inside the face, for the work that is QLineEdit's:
     *  validators, masks, the input method. The face keeps owning how it
     *  looks. */
    QLineEdit *lineEdit() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setText(const QString &text);
    void setPlaceholderText(const QString &text);
    void setLeadingIcon(const QIcon &icon);
    /*! Empties the box; \c cleared says so, whether the emptying was asked
     *  for here or typed over. */
    void clear();
Q_SIGNALS:
    /*! What the box says changed, whether typed or set. */
    void textChanged(const QString &text);
    /*! The box was emptied: by the clear at the right, or by \c clear(). */
    void cleared();
    /*! The box was left, or Enter was pressed: the words are finished. */
    void editingFinished();
    void returnPressed();
protected:
    qreal cornerRadius() const override;
    /*!
     * Nothing: the box's focus is the accent underline, which it draws
     * itself, and a wash over the face would sit under the editor's own
     * words. An empty chrome is the base's way of being told there is
     * nothing of that kind to draw.
     */
    QRectF chromeRect() const override;
    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;

    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

    QAccessible::Role accessibleRole() const override;
private:
    /*! The editor's room: after the icon, before the clear, the height of
     *  the box. */
    QRectF editorRect() const;
    /*! The clear's room, while there is something to erase and the box may
     *  be worked; empty otherwise. */
    QRectF clearRect() const;
    bool clearWanted() const;
    /*! Hands the editor the room it has now. */
    void updateEditor();

    Q_DISABLE_COPY(FluentTextBox)
    QX_DECLARE_PRIVATE(FluentTextBox)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTTEXTBOX_H
