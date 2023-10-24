#pragma once

#include <QtTest/QtTest>

class TestRibbonQuickAccessBar: public QObject
{
    Q_OBJECT
private slots:
    void visibleCount();
    void addAction();
};
