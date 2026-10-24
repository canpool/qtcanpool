/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef SIGNINSIDEPAGE_H
#define SIGNINSIDEPAGE_H

#include "demopage.h"

#include "qxmaterial/qxmaterial_global.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialAutocomplete;
class MaterialTextField;
QX_MATERIAL_END_NAMESPACE

/*!
 * Signing in, with half the page given to the account rather than to the form.
 *
 * The same decision as the screen before this one, arranged the other way: a
 * band of the accent down one side carrying what the account is for, and the
 * form beside it. What the arrangement buys is that the form stops having to
 * say what it is for, so it can be three rows and a command.
 *
 * The account is an autocomplete rather than a field, because a window that
 * knows whose accounts it can open has answers to give before the writing is
 * finished - and then the difference between what was written and what was
 * taken is the control's to keep, not the page's.
 */
class SignInSidePage : public DemoPage
{
    Q_OBJECT
public:
    explicit SignInSidePage(QWidget *parent = Q_NULLPTR);
private:
    void attempt();
    QX_MATERIAL_PREPEND_NAMESPACE(MaterialAutocomplete) *m_account = Q_NULLPTR;
    QX_MATERIAL_PREPEND_NAMESPACE(MaterialTextField) *m_password = Q_NULLPTR;
};

#endif   // SIGNINSIDEPAGE_H
