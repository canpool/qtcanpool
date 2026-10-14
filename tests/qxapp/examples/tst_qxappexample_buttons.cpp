/*!
 * \file
 *
 * In-process smoke test for the Example_App_Buttons example. It builds the
 * example's own MainWindow source and stands the window up offscreen, proving
 * the example still compiles and that all four button classes it demonstrates
 * really are in the window.
 */
#include "tst_global.h"
#include "mainwindow.h"

#include "qxapp/qxextensionbutton.h"
#include "qxapp/qxmenuaccessbutton.h"
#include "qxapp/qxmenubutton.h"
#include "qxapp/qxtoolbutton.h"

#include <QtWidgets/QWidget>
#include <QtTest/QTest>

class tst_QxAppExample_Buttons : public QObject
{
    Q_OBJECT
private slots:
    void constructsAndShows();
};

void tst_QxAppExample_Buttons::constructsAndShows()
{
    MainWindow w;
    w.show();

    QVERIFY(w.findChild<QxApp::QxToolButton *>() != nullptr);
    QVERIFY(w.findChild<QxApp::QxMenuButton *>() != nullptr);
    QVERIFY(w.findChild<QxApp::QxMenuAccessButton *>() != nullptr);
    QVERIFY(w.findChild<QxApp::QxExtensionButton *>() != nullptr);

    QTest::qWait(30);
}

TEST_ADD(tst_QxAppExample_Buttons)

#include "tst_qxappexample_buttons.moc"
