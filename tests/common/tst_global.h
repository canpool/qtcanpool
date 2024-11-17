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
    static void add(TestInfo *info);
    static void run(int argc, char *argv[]);
private:
    static QVector<TestInfo *> s_testInfos;
};

/* TestInfo */
class TestInfo
{
public:
    TestInfo(QObject *obj);
    ;
    ~TestInfo();

    QObject *testObject() const;
private:
    QObject *m_object = nullptr;
};

#define TEST_ADD(TestObject)                                                                                           \
    class TestObject##_TestClass                                                                                       \
    {                                                                                                                  \
    public:                                                                                                            \
        static TestInfo *const _info;                                                                                  \
    };                                                                                                                 \
    TestInfo *const TestObject##_TestClass::_info = new TestInfo(new TestObject);

#define TEST_RUN_ALL() UnitTest::run(argc, argv);
