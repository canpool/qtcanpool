/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTAUTOSUGGESTBOX_H
#define FLUENTAUTOSUGGESTBOX_H

#include "qxfluent_global.h"

#include "fluenttextbox.h"

QX_FLUENT_BEGIN_NAMESPACE

class FluentMenuFlyout;
class FluentAutoSuggestBoxPrivate;

/*!
 * Fluent's auto-suggest box: the text box that answers while it is being
 * written into, with the words the page has to offer.
 *
 * A QComboBox that is editable looks like the answer and is not quite: it
 * offers a closed list, arrived at before the writing started. The suggest
 * box is the other shape - the writing drives the list, and the list is
 * commands the flyout knows how to carry: what matches what has been written
 * so far is in it, what matches better comes higher, Enter or a click takes
 * what is in hand into the box, and Escape puts the list away without
 * touching what was written.
 */
class QX_FLUENT_EXPORT FluentAutoSuggestBox : public FluentTextBox
{
    Q_OBJECT
    Q_PROPERTY(QStringList suggestions READ suggestions WRITE setSuggestions NOTIFY suggestionsChanged)
public:
    explicit FluentAutoSuggestBox(QWidget *parent = Q_NULLPTR);
    ~FluentAutoSuggestBox() override;

    /*! The words the page has to offer, whatever their order; the list the
     *  flyout carries is what still matches what has been written. */
    QStringList suggestions() const;

    /*! The flyout the suggestions open into, for the reader of the room. */
    FluentMenuFlyout *flyout() const;

public Q_SLOTS:
    void setSuggestions(const QStringList &suggestions);
Q_SIGNALS:
    void suggestionsChanged();
    /*! A suggestion was taken - by Enter or by a click - into the box. */
    void suggestionAccepted(const QString &suggestion);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    /*! Rebuilds the flyout's commands from what still matches, and opens the
     *  flyout when there are any; closes it when there are none. */
    void updateSuggestions();
    void takeSuggestion(QAction *action);

    Q_DISABLE_COPY(FluentAutoSuggestBox)
    QX_DECLARE_PRIVATE(FluentAutoSuggestBox)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTAUTOSUGGESTBOX_H
