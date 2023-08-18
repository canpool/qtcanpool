#pragma once

#include <QtTest/QtTest>

class TestQuickAccessBar: public QObject
{
    Q_OBJECT
private slots:
    void visibleCount();
    void addAction();
};
