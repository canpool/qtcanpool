/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

/*!
 * \file
 *
 * In-process smoke test for the Example_App_NavigationBar example. It builds
 * the example's own MainWindow source and stands the window up offscreen,
 * proving the example still compiles and that both navigation classes it
 * demonstrates really are in the window - the vertical rail and the tab bar
 * derived one are different classes despite the similar names.
 */
#include "tst_global.h"
#include "mainwindow.h"

#include "qxapp/qxnavtabbar.h"
#include "qxapp/qxnavigationbar.h"

#include <QtWidgets/QWidget>
#include <QtTest/QTest>

class tst_QxAppExample_NavigationBar : public QObject
{
    Q_OBJECT
private slots:
    void constructsAndShows();
};

void tst_QxAppExample_NavigationBar::constructsAndShows()
{
    MainWindow w;
    w.show();

    QVERIFY(w.findChild<QxApp::QxNavigationBar *>() != nullptr);
    QVERIFY(w.findChild<QxApp::QxNavTabBar *>() != nullptr);

    QTest::qWait(30);
}

TEST_ADD(tst_QxAppExample_NavigationBar)

#include "tst_qxappexample_navigationbar.moc"
