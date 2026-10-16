/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "qxtoast.h"

#include <QAccessibleWidget>

QX_APP_BEGIN_NAMESPACE

namespace
{

/*!
 * \brief Accessible interface for a toast.
 *
 * A toast is a transient notification, but it is a plain QWidget as far as the
 * default accessibility is concerned, so it is announced as a Client - or not at
 * all, since it is not focusable. Report Notification instead, which is the role
 * assistive tools treat as something to announce without being asked.
 */
class QxToastAccessible : public QAccessibleWidget
{
public:
    explicit QxToastAccessible(QxToast *toast)
        : QAccessibleWidget(toast, QAccessible::Notification)
    {
    }
};

/*!
 * \brief Creates the app-specific interfaces; returns null for anything else so
 *        the built-in factories keep handling the remaining widgets.
 */
QAccessibleInterface *appAccessibleFactory(const QString &className, QObject *object)
{
    Q_UNUSED(className)
    if (auto *toast = qobject_cast<QxToast *>(object)) {
        return new QxToastAccessible(toast);
    }
    return Q_NULLPTR;
}

}   // namespace

void installQxAppAccessibility()
{
    static bool installed = false;
    if (installed) {
        return;
    }
    installed = true;
    QAccessible::installFactory(&appAccessibleFactory);
}

QX_APP_END_NAMESPACE
