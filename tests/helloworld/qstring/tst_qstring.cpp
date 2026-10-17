/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include <QtTest/QtTest>

class tst_QString : public QObject
{
    Q_OBJECT

private slots:
    void toUpper();
};

void tst_QString::toUpper()
{
    QString str = "Hello";
    QVERIFY(str.toUpper() == "HELLO");
}

QTEST_MAIN(tst_QString)

#include "tst_qstring.moc"
