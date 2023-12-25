/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALSTATETRANSITION_H
#define QXMATERIALSTATETRANSITION_H

#include "materialstatetransitionevent.h"
#include <QAbstractTransition>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialStateTransition : public QAbstractTransition
{
    Q_OBJECT
public:
    MaterialStateTransition(MaterialStateTransitionType type);
protected:
    virtual bool eventTest(QEvent *event);
    virtual void onTransition(QEvent *);
private:
    MaterialStateTransitionType m_type;
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALSTATETRANSITION_H
