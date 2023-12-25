/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#include "materialstatetransition.h"

QX_MATERIAL_BEGIN_NAMESPACE

MaterialStateTransition::MaterialStateTransition(MaterialStateTransitionType type)
    : m_type(type)
{
}

bool MaterialStateTransition::eventTest(QEvent *event)
{
    if (event->type() != QEvent::Type(QEvent::User + 1)) {
        return false;
    }
    MaterialStateTransitionEvent *transition = static_cast<MaterialStateTransitionEvent *>(event);
    return (m_type == transition->type);
}

void MaterialStateTransition::onTransition(QEvent *)
{
}

QX_MATERIAL_END_NAMESPACE
