/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FEEDBACKPAGE_H
#define FEEDBACKPAGE_H

#include "demopage.h"

#include "qxmaterial/qxmaterial_global.h"

class QPushButton;

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialBadge;
class MaterialProgressRing;
class MaterialSlider;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE

/*! The screen where the application says what it is doing: that it is waiting,
 *  that something has not arrived yet, that there is a count the user has not
 *  looked at, and that something happened. */
class FeedbackPage : public DemoPage
{
    Q_OBJECT
public:
    explicit FeedbackPage(QWidget *parent = Q_NULLPTR);

Q_SIGNALS:
    /*! Asked for by the page, shown by the window: a snackbar belongs to the
     *  window it rises in, and not to the screen that had something to say. */
    void messageRequested(const QString &message);
private:
    MaterialProgressRing *m_known = Q_NULLPTR;
    MaterialProgressRing *m_unknown = Q_NULLPTR;
    MaterialSlider *m_drive = Q_NULLPTR;
    MaterialBadge *m_badge = Q_NULLPTR;
    QPushButton *m_add = Q_NULLPTR;
    QPushButton *m_clear = Q_NULLPTR;
    QPushButton *m_say = Q_NULLPTR;
};

#endif   // FEEDBACKPAGE_H
