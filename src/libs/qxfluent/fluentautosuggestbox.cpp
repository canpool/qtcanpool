/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentautosuggestbox.h"

#include "fluentmenuflyout.h"

#include <QtGui/QKeyEvent>
#include <QtWidgets/QLineEdit>

QX_FLUENT_BEGIN_NAMESPACE

class FluentAutoSuggestBoxPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentAutoSuggestBox)

    QStringList suggestions;
    FluentMenuFlyout *flyout = nullptr;
    /*! True while the box is being filled from its own list: the filling
     *  writes, and the writing must not ask the list to open again. */
    bool taking = false;
};

FluentAutoSuggestBox::FluentAutoSuggestBox(QWidget *parent)
    : FluentTextBox(parent)
    , d_ptr(new FluentAutoSuggestBoxPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    d_ptr->flyout = new FluentMenuFlyout(this);
    connect(d_ptr->flyout, &FluentMenuFlyout::triggered, this, [this](QAction *action) {
        takeSuggestion(action);
    });
    connect(d_ptr->flyout, &QxControls::QxPopupSurface::dismissed, this, [this]() {
        d_ptr->flyout->clear();
    });
    // The writing drives the list: the editor's events are the box's cues.
    lineEdit()->installEventFilter(this);
    connect(this, &FluentTextBox::textChanged, this, [this]() {
        updateSuggestions();
    });
}

FluentAutoSuggestBox::~FluentAutoSuggestBox(){QX_FINI_PRIVATE()}

QStringList FluentAutoSuggestBox::suggestions() const
{
    return d_ptr->suggestions;
}

FluentMenuFlyout *FluentAutoSuggestBox::flyout() const
{
    return d_ptr->flyout;
}

void FluentAutoSuggestBox::setSuggestions(const QStringList &suggestions)
{
    if (d_ptr->suggestions == suggestions) {
        return;
    }
    d_ptr->suggestions = suggestions;
    Q_EMIT suggestionsChanged();
    updateSuggestions();
}

bool FluentAutoSuggestBox::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == lineEdit() && event->type() == QEvent::KeyPress) {
        auto *key = static_cast<QKeyEvent *>(event);
        FluentMenuFlyout *flyout = d_ptr->flyout;
        switch (key->key()) {
        case Qt::Key_Down:
            if (flyout->isVisible()) {
                // The keyboard walks into the list: the command after the
                // one in hand, from where the writing left it.
                const QList<QAction *> actions = flyout->actions();
                const int at = actions.indexOf(flyout->currentAction());
                for (int walked = 1; walked <= actions.count(); ++walked) {
                    QAction *next = actions.at((at + walked) % actions.count());
                    if (next->isEnabled()) {
                        flyout->setCurrentAction(next);
                        break;
                    }
                }
                key->accept();
                return true;
            }
            break;
        case Qt::Key_Up:
            if (flyout->isVisible()) {
                const QList<QAction *> actions = flyout->actions();
                const int at = actions.indexOf(flyout->currentAction());
                for (int walked = 1; walked <= actions.count(); ++walked) {
                    QAction *previous = actions.at((at - walked + actions.count()) % actions.count());
                    if (previous->isEnabled()) {
                        flyout->setCurrentAction(previous);
                        break;
                    }
                }
                key->accept();
                return true;
            }
            break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (flyout->isVisible() && flyout->currentAction()) {
                takeSuggestion(flyout->currentAction());
                key->accept();
                return true;
            }
            break;
        case Qt::Key_Escape:
            if (flyout->isVisible()) {
                flyout->close();
                key->accept();
                return true;
            }
            break;
        default:
            break;
        }
    }
    return FluentTextBox::eventFilter(watched, event);
}

void FluentAutoSuggestBox::updateSuggestions()
{
    FluentMenuFlyout *flyout = d_ptr->flyout;
    if (d_ptr->taking) {
        return;
    }
    const QString written = text();
    flyout->clear();
    if (written.isEmpty()) {
        if (flyout->isVisible()) {
            flyout->close();
        }
        return;
    }
    int offered = 0;
    for (const QString &suggestion : d_ptr->suggestions) {
        if (suggestion.contains(written, Qt::CaseInsensitive)) {
            flyout->addAction(suggestion);
            ++offered;
        }
    }
    if (offered == 0) {
        // A list with nothing to offer is not a list: it does not open.
        if (flyout->isVisible()) {
            flyout->close();
        }
        return;
    }
    flyout->open();
}

void FluentAutoSuggestBox::takeSuggestion(QAction *action)
{
    if (!action) {
        return;
    }
    d_ptr->taking = true;
    setText(action->text());
    d_ptr->taking = false;
    Q_EMIT suggestionAccepted(action->text());
    d_ptr->flyout->clear();
    if (d_ptr->flyout->isVisible()) {
        d_ptr->flyout->close();
    }
}

QX_FLUENT_END_NAMESPACE
