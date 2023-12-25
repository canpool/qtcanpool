/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#include "materialradiobutton.h"
#include "materialcheckable_p.h"
#include "materialradiobutton_p.h"
#include <QColor>
#include <QPropertyAnimation>
#include <QState>
#include <QStateMachine>

QX_MATERIAL_BEGIN_NAMESPACE

/*!
 *  \class MaterialRadioButtonPrivate
 *  \internal
 */

MaterialRadioButtonPrivate::MaterialRadioButtonPrivate(MaterialRadioButton *q)
    : MaterialCheckablePrivate(q)
{
}

MaterialRadioButtonPrivate::~MaterialRadioButtonPrivate()
{
}

void MaterialRadioButtonPrivate::init()
{
    Q_Q(MaterialRadioButton);

    q->setAutoExclusive(true);

    q->setCheckedIcon(QIcon(":/res/icons/toggle/svg/production/ic_radio_button_checked_24px.svg"));
    q->setUncheckedIcon(QIcon(":/res/icons/toggle/svg/production/ic_radio_button_unchecked_24px.svg"));

    uncheckedState->assignProperty(checkedIcon, "iconSize", 0);
    uncheckedState->assignProperty(uncheckedIcon, "iconSize", 24);

    disabledUncheckedState->assignProperty(checkedIcon, "iconSize", 0);
    disabledUncheckedState->assignProperty(uncheckedIcon, "iconSize", 24);

    checkedState->assignProperty(uncheckedIcon, "iconSize", 0);
    checkedState->assignProperty(checkedIcon, "iconSize", 24);

    disabledCheckedState->assignProperty(uncheckedIcon, "iconSize", 0);
    disabledCheckedState->assignProperty(checkedIcon, "iconSize", 24);

    uncheckedState->assignProperty(checkedIcon, "opacity", 0);
    uncheckedState->assignProperty(uncheckedIcon, "opacity", 1);

    checkedState->assignProperty(uncheckedIcon, "opacity", 0);
    checkedState->assignProperty(checkedIcon, "opacity", 1);

    checkedIcon->setIconSize(0);

    //

    checkedState->assignProperty(checkedIcon, "color", q->checkedColor());
    checkedState->assignProperty(uncheckedIcon, "color", q->uncheckedColor());
    uncheckedState->assignProperty(uncheckedIcon, "color", q->uncheckedColor());

    QPropertyAnimation *animation;

    animation = new QPropertyAnimation(checkedIcon, "iconSize", q);
    animation->setDuration(250);
    stateMachine->addDefaultAnimation(animation);

    animation = new QPropertyAnimation(uncheckedIcon, "iconSize", q);
    animation->setDuration(250);
    stateMachine->addDefaultAnimation(animation);

    animation = new QPropertyAnimation(uncheckedIcon, "opacity", q);
    animation->setDuration(250);
    stateMachine->addDefaultAnimation(animation);

    animation = new QPropertyAnimation(checkedIcon, "opacity", q);
    animation->setDuration(250);
    stateMachine->addDefaultAnimation(animation);
}

/*!
 *  \class MaterialRadioButton
 */

MaterialRadioButton::MaterialRadioButton(QWidget *parent)
    : MaterialCheckable(*new MaterialRadioButtonPrivate(this), parent)
{
    d_func()->init();
}

MaterialRadioButton::~MaterialRadioButton()
{
}

void MaterialRadioButton::setupProperties()
{
    MaterialCheckable::setupProperties();

    Q_D(MaterialRadioButton);

    d->checkedState->assignProperty(d->checkedIcon, "color", checkedColor());
    d->checkedState->assignProperty(d->uncheckedIcon, "color", uncheckedColor());
    d->uncheckedState->assignProperty(d->uncheckedIcon, "color", uncheckedColor());
}

QX_MATERIAL_END_NAMESPACE
