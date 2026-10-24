/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef SIGNUPPAGE_H
#define SIGNUPPAGE_H

#include "demopage.h"

#include "qxmaterial/qxmaterial_global.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialCheckBox;
class MaterialTextField;
class MaterialToggleButtonGroup;
QX_MATERIAL_END_NAMESPACE

/*!
 * Making an account: the form the screen before this one signs into.
 *
 * Four rows and two answers, and the difference between this screen and signing
 * in is that it is asking for things nobody has yet. So it is the one form in
 * the demo that asks a question about the account rather than about the person:
 * whether it is one person's or a team's, which is what decides everything the
 * account can hold.
 *
 * That question is a toggle button group and not a segmented button, and the two
 * look alike enough to be worth saying why: nothing has to be on in a group, and
 * a screen that started with neither kind chosen would be a screen asking a
 * question with no answer in force. This one starts with the personal kind on,
 * because that is the answer almost everyone gives.
 */
class SignUpPage : public DemoPage
{
    Q_OBJECT
public:
    explicit SignUpPage(QWidget *parent = Q_NULLPTR);
private:
    void create();
    QX_MATERIAL_PREPEND_NAMESPACE(MaterialTextField) *m_name = Q_NULLPTR;
    QX_MATERIAL_PREPEND_NAMESPACE(MaterialTextField) *m_surname = Q_NULLPTR;
    QX_MATERIAL_PREPEND_NAMESPACE(MaterialTextField) *m_email = Q_NULLPTR;
    QX_MATERIAL_PREPEND_NAMESPACE(MaterialTextField) *m_password = Q_NULLPTR;
    QX_MATERIAL_PREPEND_NAMESPACE(MaterialCheckBox) *m_terms = Q_NULLPTR;
    QX_MATERIAL_PREPEND_NAMESPACE(MaterialToggleButtonGroup) *m_kind = Q_NULLPTR;
};

#endif   // SIGNUPPAGE_H
