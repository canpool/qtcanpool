/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef SIGNINPAGE_H
#define SIGNINPAGE_H

#include "demopage.h"

#include "qxmaterial/qxmaterial_global.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialTextField;
QX_MATERIAL_END_NAMESPACE

/*!
 * Sign in: one card in the middle of an empty page, and nothing else.
 *
 * The template is the one screen whose whole job is a decision, so it gets no
 * navigation, no list and no chrome of its own - what it has is a surface, two
 * fields, the switch that decides how long the decision lasts, and the way in.
 * It is here because it is the smallest screen in the set and the one where a
 * control set is judged hardest: a text field that is not comfortable to type
 * into is not made up for by anything else on the page.
 */
class SignInPage : public DemoPage
{
    Q_OBJECT
public:
    explicit SignInPage(QWidget *parent = Q_NULLPTR);
private:
    void attempt();
    QX_MATERIAL_PREPEND_NAMESPACE(MaterialTextField) *m_email = Q_NULLPTR;
    QX_MATERIAL_PREPEND_NAMESPACE(MaterialTextField) *m_password = Q_NULLPTR;
};

#endif   // SIGNINPAGE_H
