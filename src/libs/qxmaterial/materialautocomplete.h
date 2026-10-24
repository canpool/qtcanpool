/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALAUTOCOMPLETE_H
#define MATERIALAUTOCOMPLETE_H

#include "qxmaterial_global.h"

#include "materialtextfield.h"

class QKeyEvent;
class QWidget;

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialAutocompletePrivate;

/*!
 * Material's autocomplete: a field that answers before the answer is finished.
 *
 * It stands on the text field - the same filled or outlined face, the same
 * floating label - and adds the one thing a field cannot do: as the writing
 * goes in, the answers that still fit it are offered under the field, and one
 * of them can be taken instead of being typed out. The list is the family's
 * own rows - forty-eight tall, the pointer's wash over one, the keyboard's
 * row tinted - opening below the field, turning above when below has no room,
 * and going away on Escape or on a click outside, which is the shared surface
 * doing what it does for every family.
 *
 * What is written is not the same question as what was answered, and the two
 * are kept apart: \c text() is what the editor holds, \c currentText() is the
 * answer in force. Typing clears the answer - a value that no longer matches
 * what the field says is not a value - and taking a row or pressing Enter on
 * it puts one back.
 *
 * A field that only accepts what is in its list is a select with typing as a
 * way of finding a row; a field that accepts anything is what \c freeSolo is
 * for. With it, Enter on writing that matches nothing takes the writing.
 *
 * Multiple answers - the chips-inside-the-field variant Material also draws -
 * are not here: the chips would have to live inside the editor's own room, and
 * the field has no room inside it to give. That is a shape of its own, and a
 * half of one would be worse than none.
 *
 * \sa MaterialSelect
 */
class QX_MATERIAL_EXPORT MaterialAutocomplete : public MaterialTextField
{
    Q_OBJECT
    Q_PROPERTY(QString currentText READ currentText NOTIFY currentTextChanged)
    Q_PROPERTY(bool freeSolo READ isFreeSolo WRITE setFreeSolo)
    Q_PROPERTY(bool openOnFocus READ opensOnFocus WRITE setOpenOnFocus)
public:
    explicit MaterialAutocomplete(QWidget *parent = Q_NULLPTR);
    ~MaterialAutocomplete() override;

    QStringList options() const;
    void setOptions(const QStringList &options);

    /*! Whether writing that matches no option may be taken as the answer. */
    bool isFreeSolo() const;
    void setFreeSolo(bool on);

    /*! Whether the field offers every answer the moment the keyboard arrives
     *  at it. True by default: a field that waits for a letter before it has
     *  anything to say makes a reader type to find out what there is. */
    bool opensOnFocus() const;
    void setOpenOnFocus(bool on);

    /*! What the editor holds. */
    QString text() const;
    void setText(const QString &text);

    /*! The answer in force: the option taken, or - when the field is free -
     *  the writing. Empty while nothing has been answered. */
    QString currentText() const;

    bool isListOpen() const;

    /*! The answer the keyboard is on, or an empty string while it is on none:
     *  what Enter would take. */
    QString highlightedOption() const;

    /*! The surface the answers open into; applications read it to re-style it,
     *  and the tests read it to know where it stood. */
    QWidget *popup() const;

public Q_SLOTS:
    /*! Opens the list on every answer, which is what the mark at the field's
     *  right end is for. */
    void showAll();
    void hideList();
    /*! Answers with the row in force, if there is one. */
    void acceptCurrent();
    void clear();

Q_SIGNALS:
    /*! The editing changed, by the user's typing rather than by setText(). */
    void textEdited(const QString &text);
    /*! A row was taken, by a click or by Enter on it. */
    void optionChosen(const QString &option);
    /*! The answer in force changed - by a choice, by typing, or by a setter. */
    void currentTextChanged(const QString &text);
protected:
    void focusInEvent(QFocusEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    void refilter();
    void answer(const QString &text);
    void takeRow(int row);

    Q_DISABLE_COPY(MaterialAutocomplete)
    QX_DECLARE_PRIVATE(MaterialAutocomplete)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALAUTOCOMPLETE_H
