#pragma once

#include <QObject>
#include <QVector>
#include <QtTest/QtTest>

#define TEST_RUN(TestObject)                                                                                           \
    TestObject TestObject##_var;                                                                                       \
    QTest::qExec(&TestObject##_var, argc, argv);

/* UnitTest */
class UnitTest
{
public:
    static void add(QObject *obj)
    {
        s_testObjects.append(obj);
    }

    static void run(int argc, char *argv[])
    {
        for (QObject *obj : s_testObjects) {
            QTest::qExec(obj, argc, argv);
        }
    }
private:
    static QVector<QObject *> s_testObjects;
};

/* TestInfo */
class TestInfo
{
public:
    TestInfo(QObject *obj)
    {
        UnitTest::add(obj);
    };
};

#define TEST_ADD(TestObject)                                                                                           \
    class TestObject##_TestClass                                                                                       \
    {                                                                                                                  \
    public:                                                                                                            \
        static TestInfo *const _info;                                                                                  \
    };                                                                                                                 \
    TestInfo *const TestObject##_TestClass::_info = new TestInfo(new TestObject);

#define TEST_RUN_ALL() UnitTest::run(argc, argv);
