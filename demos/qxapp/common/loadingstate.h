/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef LOADINGSTATE_H
#define LOADINGSTATE_H

#include <QtCore/QObject>
#include <QtCore/QPointer>

class QEvent;
class QStackedWidget;
class QTimer;
class QWidget;

/*!
 * The wait between a screen that has nothing yet and the same screen holding
 * what it went to fetch.
 *
 * A screen that fetches has two ways to wait. It can show what it has - which
 * is nothing - and then jump when the answer arrives, and the user reads the
 * jump as the screen having been wrong. Or it can show the shape of what is
 * coming: a skeleton where the rows will be, a ring where the work is. That is
 * what this switches, and the shape itself is the family's, not this class's:
 * a Material screen waits with MaterialSkeleton, a Fluent screen with its
 * progress ring.
 *
 * Two widgets and a clock, then, in the order they are handed over: the
 * placeholder, the content, and how long the wait lasts. The wait is a duration
 * the application picks and not a motion token - nothing is moving from one
 * state to another, the screen is simply not ready - so it does not come from
 * the two families' motion vocabulary, which answers a different question.
 *
 * It is driven by a clock a test can also drive: start() begins a wait and
 * finish() ends it, which is what makes "the screen was seen waiting" and "the
 * screen was seen full" facts a case can assert rather than a stopwatch.
 */
class LoadingState : public QObject
{
    Q_OBJECT
public:
    /*!
     * Switches @p stack between @p waiting and @p content. Both are expected to
     * already be pages of @p stack; neither is owned here.
     */
    LoadingState(QStackedWidget *stack, QWidget *waiting, QWidget *content, QObject *parent = Q_NULLPTR);
    ~LoadingState() override;

    /*! True while the placeholder is the page on screen. */
    bool isLoading() const;

    /*! The shape of what is coming, and what arrives in its place. */
    QWidget *waitingWidget() const;
    QWidget *contentWidget() const;

public Q_SLOTS:
    /*! Puts the placeholder up and arms the wait for @p afterMs. A wait that is
     *  already running is replaced, not queued: the second question is the one
     *  the screen is waiting for. */
    void start(int afterMs);
    /*! Ends the wait now, whether or not its time was up. */
    void finish();
    /*! Arms the wait for the first time @p screen is shown, and only then.
     *
     *  A screen that is built at start-up and opened a minute later has already
     *  fetched what it went for, so a wait that ran from construction would be
     *  a wait nobody ever saw. Coming back to a screen does not ask it to fetch
     *  what it already has either, which is why it is the first show and not
     *  every one.
     */
    void startOnFirstShow(QWidget *screen, int afterMs);

Q_SIGNALS:
    /*! The content is on screen: either its time came up or the wait was ended
     *  early. */
    void finished();
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    QPointer<QStackedWidget> m_stack;
    QPointer<QWidget> m_waiting;
    QPointer<QWidget> m_content;
    QPointer<QWidget> m_screen;
    QTimer *m_timer = Q_NULLPTR;
    /*! How long the wait armed for the first show lasts, or -1 once it is armed
     *  and waiting for the show (or never was). */
    int m_firstShowWait = -1;

    Q_DISABLE_COPY(LoadingState)
};

#endif   // LOADINGSTATE_H
