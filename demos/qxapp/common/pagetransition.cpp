/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "pagetransition.h"

#include <QtCore/QPropertyAnimation>
#include <QtWidgets/QGraphicsOpacityEffect>
#include <QtWidgets/QLayout>
#include <QtWidgets/QStackedWidget>

namespace
{

// How far the arriving page has still to rise: eight pixels is enough to read
// as movement and not enough to read as the page being laid out wrongly.
constexpr int kRise = 8;

}   // namespace

PageTransition::PageTransition(QStackedWidget *stack, QObject *parent)
    : QObject(parent)
    , m_stack(stack)
{
}

void PageTransition::show(QWidget *page, const QxTheme::QxMotion &motion)
{
    if (!m_stack || !page) {
        return;
    }
    // A page that is already in front is not an arrival: a second click on the
    // destination in force asks for nothing, and animating it again would make
    // the screen flicker for a keystroke that changed no place. Neither is the
    // first page of all - the window opens on a screen, and a page rising into
    // an empty stack would be an arrival from nowhere.
    const bool samePage = m_stack->currentWidget() == page;
    const bool nothingYet = m_stack->currentWidget() == Q_NULLPTR;
    settle();
    m_duration = motion.duration;
    if (samePage || nothingYet || m_duration <= 0) {
        m_stack->setCurrentWidget(page);
        return;
    }

    m_stack->setCurrentWidget(page);
    // The page's own place, asked for after it was put in front and the stack
    // has said where that is: the rise is from eight pixels below it, and a
    // position read before the layout ran would move the page out of the stack.
    if (QLayout *layout = m_stack->layout()) {
        layout->activate();
    }
    const QPoint home = page->pos();
    m_home = home;

    m_opacity = new QGraphicsOpacityEffect(page);
    m_opacity->setOpacity(0.0);
    page->setGraphicsEffect(m_opacity);
    page->move(home + QPoint(0, kRise));

    m_page = page;
    m_fade = new QPropertyAnimation(m_opacity, "opacity", this);
    m_fade->setDuration(m_duration);
    m_fade->setEasingCurve(motion.curve);
    m_fade->setStartValue(0.0);
    m_fade->setEndValue(1.0);

    m_rise = new QPropertyAnimation(page, "pos", this);
    m_rise->setDuration(m_duration);
    m_rise->setEasingCurve(motion.curve);
    m_rise->setStartValue(home + QPoint(0, kRise));
    m_rise->setEndValue(home);

    // The fade is what tells the story; the rise is told alongside it. Either
    // one finishing means the arrival is over, and neither is left half run.
    connect(m_fade, &QPropertyAnimation::finished, this, &PageTransition::settle);
    m_fade->start();
    m_rise->start();
}

bool PageTransition::isRunning() const
{
    return m_fade && m_fade->state() == QAbstractAnimation::Running;
}

int PageTransition::duration() const
{
    return m_duration;
}

void PageTransition::settle()
{
    if (!m_page) {
        return;
    }
    if (m_fade) {
        m_fade->stop();
        m_fade->deleteLater();
        m_fade = Q_NULLPTR;
    }
    if (m_rise) {
        m_rise->stop();
        m_rise->deleteLater();
        m_rise = Q_NULLPTR;
    }
    // The page is left where the arrival was going, and painted directly again:
    // an effect taken off a widget hands it back the way it was found, which is
    // what the next page change counts on.
    m_page->move(m_home);
    m_page->setGraphicsEffect(Q_NULLPTR);
    m_opacity = Q_NULLPTR;
    m_page = Q_NULLPTR;
    m_home = QPoint();
}
