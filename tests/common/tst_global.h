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
    /*!
     * Runs every registered test object and returns the number of objects that
     * reported a failure, so main() can turn it into a process exit code.
     * Returning 0 unconditionally would make CTest, and therefore CI, green
     * even when assertions fail.
     *
     * Setting QTC_ONLY_TEST=<class name> restricts the run to that object.
     */
    int run(int argc, char *argv[]);
    /*! Number of currently registered test objects. */
    int count() const;
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

/*
 * Runs all registered test objects and leaves main() with a non-zero exit code
 * when at least one of them failed, which is what CTest looks at. It must be
 * used as the last statement of main().
 */
#define TEST_RUN_ALL() return UnitTest::instance().run(argc, argv);
