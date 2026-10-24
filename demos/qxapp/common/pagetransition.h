/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef PAGETRANSITION_H
#define PAGETRANSITION_H

#include "qxtheme/qxthememotion.h"

#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtCore/QPointer>

class QGraphicsOpacityEffect;
class QPropertyAnimation;
class QStackedWidget;
class QWidget;

/*!
 * A screen arriving rather than appearing.
 *
 * A stack swaps pages between two frames, and the eye reads that as a cut: the
 * new screen is simply there, with nothing saying it came from anywhere. What
 * makes a change of place legible is that the new screen moves the way a thing
 * moves - it rises the last few pixels and settles, while it comes up from
 * nothing. Both demos draw this from their own motion family, which is why the
 * time and the curve are handed in rather than picked here: Fluent moves a
 * control at its own pace, Material at another, and the user's answer to "less
 * motion" is already folded into what the family answers.
 *
 * One transition per stack, and it will not run two at once: a second change
 * while the first is still arriving is answered by settling the first where it
 * was going and starting the second, so that a hand on the arrow keys leaves no
 * queue of animations behind it.
 */
class PageTransition : public QObject
{
    Q_OBJECT
public:
    /*! Takes over how \a stack changes page; nothing happens until show(). */
    explicit PageTransition(QStackedWidget *stack, QObject *parent = Q_NULLPTR);

    /*!
     * Puts \a page in front of the stack, arriving over \a motion.
     *
     * A motion whose duration is zero - which is what a family answers once the
     * application has said the user wants less motion - puts the page in front
     * with no animation at all, and the stack is left exactly as a plain
     * setCurrentWidget would have left it.
     */
    void show(QWidget *page, const QxTheme::QxMotion &motion);

    /*! Whether a page is still on its way in. */
    bool isRunning() const;

    /*! How long the last show() was asked to take; zero for a page put there
     *  without animation, which is what less motion asks for. */
    int duration() const;
private:
    /*! Finishes whatever is running at once - the page lands where it was
     *  going - and leaves the stack as if no transition had ever been used. */
    void settle();

    QStackedWidget *m_stack = Q_NULLPTR;
    QPointer<QWidget> m_page;
    /*! Where the page stands when it is not on its way: the rise starts eight
     *  pixels below it and lands here. */
    QPoint m_home;
    /*! Put on the page for the length of the transition and taken off after:
     *  an effect left behind is a page painted through a proxy for the rest of
     *  the run. */
    QPointer<QGraphicsOpacityEffect> m_opacity;
    QPointer<QPropertyAnimation> m_fade;
    QPointer<QPropertyAnimation> m_rise;
    int m_duration = 0;
};

#endif   // PAGETRANSITION_H
