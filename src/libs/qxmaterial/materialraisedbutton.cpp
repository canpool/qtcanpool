/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#include "materialraisedbutton.h"
#include "materialraisedbutton_p.h"
#include <QEventTransition>
#include <QPropertyAnimation>
#include <QStateMachine>
#include <QtWidgets/QGraphicsDropShadowEffect>

QX_MATERIAL_BEGIN_NAMESPACE

/*!
 *  \class MaterialRaisedButtonPrivate
 *  \internal
 */

/*!
 *  \internal
 */
MaterialRaisedButtonPrivate::MaterialRaisedButtonPrivate(MaterialRaisedButton *q)
    : MaterialFlatButtonPrivate(q)
{
}

/*!
 *  \internal
 */
MaterialRaisedButtonPrivate::~MaterialRaisedButtonPrivate()
{
}

/*!
 *  \internal
 */
void MaterialRaisedButtonPrivate::init()
{
    Q_Q(MaterialRaisedButton);

    shadowStateMachine = new QStateMachine(q);
    normalState = new QState;
    pressedState = new QState;
    effect = new QGraphicsDropShadowEffect;

    effect->setBlurRadius(7);
    effect->setOffset(QPointF(0, 2));
    effect->setColor(QColor(0, 0, 0, 75));

    q->setBackgroundMode(Qt::OpaqueMode);
    q->setMinimumHeight(42);
    q->setGraphicsEffect(effect);
    q->setBaseOpacity(0.3);

    shadowStateMachine->addState(normalState);
    shadowStateMachine->addState(pressedState);

    normalState->assignProperty(effect, "offset", QPointF(0, 2));
    normalState->assignProperty(effect, "blurRadius", 7);

    pressedState->assignProperty(effect, "offset", QPointF(0, 5));
    pressedState->assignProperty(effect, "blurRadius", 29);

    QAbstractTransition *transition;

    transition = new QEventTransition(q, QEvent::MouseButtonPress);
    transition->setTargetState(pressedState);
    normalState->addTransition(transition);

    transition = new QEventTransition(q, QEvent::MouseButtonDblClick);
    transition->setTargetState(pressedState);
    normalState->addTransition(transition);

    transition = new QEventTransition(q, QEvent::MouseButtonRelease);
    transition->setTargetState(normalState);
    pressedState->addTransition(transition);

    QPropertyAnimation *animation;

    animation = new QPropertyAnimation(effect, "offset", q);
    animation->setDuration(100);
    shadowStateMachine->addDefaultAnimation(animation);

    animation = new QPropertyAnimation(effect, "blurRadius", q);
    animation->setDuration(100);
    shadowStateMachine->addDefaultAnimation(animation);

    shadowStateMachine->setInitialState(normalState);
    shadowStateMachine->start();
}

/*!
 *  \class MaterialRaisedButton
 */

MaterialRaisedButton::MaterialRaisedButton(QWidget *parent)
    : MaterialFlatButton(*new MaterialRaisedButtonPrivate(this), parent)
{
    d_func()->init();
}

MaterialRaisedButton::MaterialRaisedButton(const QString &text, QWidget *parent)
    : MaterialFlatButton(*new MaterialRaisedButtonPrivate(this), parent)
{
    d_func()->init();

    setText(text);
}

MaterialRaisedButton::~MaterialRaisedButton()
{
}

MaterialRaisedButton::MaterialRaisedButton(MaterialRaisedButtonPrivate &d, QWidget *parent)
    : MaterialFlatButton(d, parent)
{
    d_func()->init();
}

bool MaterialRaisedButton::event(QEvent *event)
{
    Q_D(MaterialRaisedButton);

    if (QEvent::EnabledChange == event->type()) {
        if (isEnabled()) {
            d->shadowStateMachine->start();
            d->effect->setEnabled(true);
        } else {
            d->shadowStateMachine->stop();
            d->effect->setEnabled(false);
        }
    }
    return MaterialFlatButton::event(event);
}

QX_MATERIAL_END_NAMESPACE
