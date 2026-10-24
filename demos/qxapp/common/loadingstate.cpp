/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "loadingstate.h"

#include <QtCore/QEvent>
#include <QtCore/QTimer>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QWidget>

LoadingState::LoadingState(QStackedWidget *stack, QWidget *waiting, QWidget *content, QObject *parent)
    : QObject(parent)
    , m_stack(stack)
    , m_waiting(waiting)
    , m_content(content)
    , m_timer(new QTimer(this))
{
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &LoadingState::finish);

    // The screen opens on the shape of what is coming rather than on nothing:
    // the caller decides when that shape goes up, but a stack whose first page
    // were the content would flash the answer and then take it away.
    if (m_stack && m_waiting) {
        m_stack->setCurrentWidget(m_waiting);
    }
}

LoadingState::~LoadingState() = default;

bool LoadingState::isLoading() const
{
    return m_stack && m_stack->currentWidget() == m_waiting;
}

QWidget *LoadingState::waitingWidget() const
{
    return m_waiting;
}

QWidget *LoadingState::contentWidget() const
{
    return m_content;
}

void LoadingState::start(int afterMs)
{
    if (!m_stack || !m_waiting) {
        return;
    }

    m_stack->setCurrentWidget(m_waiting);
    m_timer->start(qMax(afterMs, 0));
}

void LoadingState::finish()
{
    m_timer->stop();

    if (!m_stack || !m_content) {
        return;
    }

    if (m_stack->currentWidget() == m_content) {
        return;
    }

    m_stack->setCurrentWidget(m_content);
    Q_EMIT finished();
}

void LoadingState::startOnFirstShow(QWidget *screen, int afterMs)
{
    if (!m_stack || !m_waiting) {
        return;
    }

    // The placeholder goes up now rather than at the show, so that a screen
    // shown for the first time is already waiting when it appears - a wait that
    // began after the first paint would first show the content and then take it
    // away.
    m_stack->setCurrentWidget(m_waiting);

    m_screen = screen;
    m_firstShowWait = qMax(afterMs, 0);
    if (!m_screen) {
        return;
    }

    m_screen->installEventFilter(this);
}

bool LoadingState::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_screen && event->type() == QEvent::Show && m_firstShowWait >= 0) {
        const int afterMs = m_firstShowWait;
        m_firstShowWait = -1;
        if (m_screen) {
            m_screen->removeEventFilter(this);
        }
        start(afterMs);
    }

    return QObject::eventFilter(watched, event);
}
