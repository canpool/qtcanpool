/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include <QtTest/QtTest>
#include <QLineEdit>

class tst_QLineEdit : public QObject
{
    Q_OBJECT

private slots:
    void setText();
};

void tst_QLineEdit::setText()
{
    QLineEdit lineEdit;
    QTest::keyClicks(&lineEdit, "hello world");
    QCOMPARE(lineEdit.text(), QString("hello world"));
}

QTEST_MAIN(tst_QLineEdit)

#include "tst_qlineedit.moc"
