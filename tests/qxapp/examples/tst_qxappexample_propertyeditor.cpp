/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

/*!
 * \file
 *
 * In-process smoke test for the Example_App_PropertyEditor example. It builds
 * the example's own MainWindow source and stands the window up offscreen, so
 * the example cannot rot silently: if its source stops compiling or its window
 * starts crashing, this test fails.
 *
 * "mainwindow.h" comes from the example directory, which tests/CMakeLists.txt
 * puts on the include path; the qxapp dependency supplies the rest.
 */
#include "tst_global.h"
#include "mainwindow.h"

#include "qxapp/qxpropertyeditor.h"
#include "qxapp/qxsettingsdialog.h"

#include <QtWidgets/QWidget>
#include <QtTest/QTest>

class tst_QxAppExample_PropertyEditor : public QObject
{
    Q_OBJECT
private slots:
    void constructsAndShows();
};

void tst_QxAppExample_PropertyEditor::constructsAndShows()
{
    MainWindow w;
    w.show();

    // The three classes the example exists for: the form built from QxProperty
    // descriptors, and the dialog that stores them.
    QVERIFY(w.findChild<QxApp::QxPropertyEditor *>() != nullptr);
    QVERIFY(w.findChild<QxApp::QxSettingsDialog *>() != nullptr);

    QTest::qWait(30);
}

TEST_ADD(tst_QxAppExample_PropertyEditor)

#include "tst_qxappexample_propertyeditor.moc"
