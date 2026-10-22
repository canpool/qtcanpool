/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FEEDBACKPAGE_H
#define FEEDBACKPAGE_H

#include "demopage.h"

#include "qxfluent/qxfluent_global.h"

class QPushButton;

QX_FLUENT_BEGIN_NAMESPACE
class FluentBadge;
class FluentInfoBar;
class FluentProgressRing;
class FluentSlider;
QX_FLUENT_END_NAMESPACE

QX_FLUENT_USE_NAMESPACE

/*! The screen where the application says what it is doing: that it is waiting,
 *  that there is a count the user has not looked at, and that something
 *  happened - three things, and three answers that are not the same shape. */
class FeedbackPage : public DemoPage
{
    Q_OBJECT
public:
    explicit FeedbackPage(QWidget *parent = Q_NULLPTR);
private:
    FluentProgressRing *m_known = Q_NULLPTR;
    FluentProgressRing *m_unknown = Q_NULLPTR;
    FluentSlider *m_drive = Q_NULLPTR;
    FluentBadge *m_badge = Q_NULLPTR;
    FluentInfoBar *m_info = Q_NULLPTR;
    QPushButton *m_add = Q_NULLPTR;
    QPushButton *m_clear = Q_NULLPTR;
};

#endif   // FEEDBACKPAGE_H
