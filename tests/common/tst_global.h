#pragma once

#include <QObject>
#include <QVector>
#include <QtTest/QtTest>

#define TEST_RUN(TestObject)                                                                                           \
    TestObject TestObject##_var;                                                                                       \
    QTest::qExec(&TestObject##_var, argc, argv);

class TestInfo;

/* UnitTest */
class UnitTest
{
public:
    UnitTest();
    ~UnitTest();
public:
    static UnitTest &instance();

    void add(QObject *obj);
    void run(int argc, char *argv[]);
private:
    QVector<QObject *> s_testObjects;
};

/* TestInfo */
class TestInfo
{
public:
    TestInfo(QObject *obj);
};

#define TEST_ADD(TestObject)                                                                                           \
    class TestObject##_TestClass                                                                                       \
    {                                                                                                                  \
    public:                                                                                                            \
        static TestInfo _info;                                                                                         \
    };                                                                                                                 \
    TestInfo TestObject##_TestClass::_info = TestInfo(new TestObject);

#define TEST_RUN_ALL() UnitTest::instance().run(argc, argv);
