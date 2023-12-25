/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#include "materialcheckbox.h"
#include "materialcheckable_p.h"
#include "materialcheckbox_p.h"
#include <QPropertyAnimation>
#include <QSignalTransition>
#include <QState>

QX_MATERIAL_BEGIN_NAMESPACE

/*!
 *  \class MaterialCheckBoxPrivate
 *  \internal
 */

/*!
 *  \internal
 */
MaterialCheckBoxPrivate::MaterialCheckBoxPrivate(MaterialCheckBox *q)
    : MaterialCheckablePrivate(q)
{
}

/*!
 *  \internal
 */
MaterialCheckBoxPrivate::~MaterialCheckBoxPrivate()
{
}

/*!
 *  \internal
 */
void MaterialCheckBoxPrivate::init()
{
    Q_Q(MaterialCheckBox);

    checkedState->assignProperty(checkedIcon, "iconSize", 24);
    uncheckedState->assignProperty(checkedIcon, "iconSize", 0);

    QPropertyAnimation *animation;

    animation = new QPropertyAnimation(checkedIcon, "iconSize", q);
    animation->setDuration(300);
    uncheckedTransition->addAnimation(animation);

    animation = new QPropertyAnimation(checkedIcon, "iconSize", q);
    animation->setDuration(1300);
    checkedTransition->addAnimation(animation);

    animation = new QPropertyAnimation(checkedIcon, "opacity", q);
    animation->setDuration(440);
    checkedTransition->addAnimation(animation);

    animation = new QPropertyAnimation(checkedIcon, "opacity", q);
    animation->setDuration(440);
    uncheckedTransition->addAnimation(animation);

    animation = new QPropertyAnimation(uncheckedIcon, "opacity", q);
    animation->setDuration(440);
    checkedTransition->addAnimation(animation);

    animation = new QPropertyAnimation(uncheckedIcon, "opacity", q);
    animation->setDuration(440);
    uncheckedTransition->addAnimation(animation);

    animation = new QPropertyAnimation(uncheckedIcon, "color", q);
    animation->setDuration(440);
    checkedTransition->addAnimation(animation);

    animation = new QPropertyAnimation(uncheckedIcon, "color", q);
    animation->setDuration(440);
    uncheckedTransition->addAnimation(animation);
}

/*!
 *  \class MaterialCheckBox
 */

MaterialCheckBox::MaterialCheckBox(QWidget *parent) : MaterialCheckable(*new MaterialCheckBoxPrivate(this), parent)
{
    d_func()->init();
}

MaterialCheckBox::~MaterialCheckBox()
{
}

QX_MATERIAL_END_NAMESPACE
