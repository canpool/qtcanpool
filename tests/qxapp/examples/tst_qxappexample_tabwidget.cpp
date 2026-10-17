/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

/*!
 * \file
 *
 * In-process smoke test for the Example_App_TabWidget example. It builds the
 * example's own MainWindow source and stands the window up offscreen, proving
 * the example still compiles and that both tab classes it demonstrates - the
 * widget and the standalone bar - really are in the window.
 */
#include "tst_global.h"
#include "mainwindow.h"

#include "qxapp/qxtabbar.h"
#include "qxapp/qxtabwidget.h"

#include <QtWidgets/QWidget>
#include <QtTest/QTest>

class tst_QxAppExample_TabWidget : public QObject
{
    Q_OBJECT
private slots:
    void constructsAndShows();
};

void tst_QxAppExample_TabWidget::constructsAndShows()
{
    MainWindow w;
    w.show();

    QVERIFY(w.findChild<QxApp::QxTabWidget *>() != nullptr);
    QVERIFY(w.findChild<QxApp::QxTabBar *>() != nullptr);

    QTest::qWait(30);
}

TEST_ADD(tst_QxAppExample_TabWidget)

#include "tst_qxappexample_tabwidget.moc"
