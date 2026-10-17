/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxapp/qxsplashscreen.h"
#include "qxapp/qxtoast.h"

#include <QAccessible>
#include <QPixmap>
#include <QtWidgets/QWidget>

QX_APP_USE_NAMESPACE

class tst_AppAccessible : public QObject
{
    Q_OBJECT
private slots:
    void toastRole();
    void splashName();
};

void tst_AppAccessible::toastRole()
{
    // Accessibility (B2): a toast is a transient notification, but a plain QWidget
    // as far as the default accessibility is concerned, so it would be announced
    // as a Client - or not at all, since it is not focusable. Constructing the
    // toast installs the factory that reports the Notification role.
    QWidget host;
    QxToast toast(QStringLiteral("Saved"), QxToast::Information, &host);

    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&toast);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::Notification);
}

void tst_AppAccessible::splashName()
{
    // Accessibility (B2): the splash message is drawn into the pixmap rather than
    // placed in a child widget, so it is mirrored into the accessible name.
    QxSplashScreen splash(QPixmap(16, 16), QStringLiteral("App"), QStringLiteral("1.0"));
    QVERIFY(splash.accessibleName().isEmpty());

    splash.setMessage(QStringLiteral("Loading plugins"));
    QCOMPARE(splash.accessibleName(), QStringLiteral("Loading plugins"));

    splash.setMessage(QStringLiteral("Ready"));
    QCOMPARE(splash.accessibleName(), QStringLiteral("Ready"));
}

TEST_ADD(tst_AppAccessible)

#include "tst_appaccessible.moc"
