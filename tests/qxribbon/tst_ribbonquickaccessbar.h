#pragma once

#include <QtTest/QtTest>

class tst_RibbonQuickAccessBar: public QObject
{
    Q_OBJECT

private slots:
    void visibleCount();
    void addAction();
};
