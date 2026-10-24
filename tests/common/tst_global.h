/**
 * Copyright (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include <QtWidgets/QApplication>
#include <QtGui/QFontDatabase>
#include <QObject>
#include <QVector>
#include <QtTest/QtTest>

/*!
 * An offscreen platform without a font directory of its own writes no words:
 * every glyph comes out as the empty box, so a judgement read off a frame -
 * the prompt moved, the echo is not the words - cannot be read. Gives the
 * platform a font to write with when it has none; a platform that has fonts
 * answers differently and loses nothing.
 *
 * The font is only put in the database, not made the application's: making
 * it the application's would remeasure every row of every test that runs
 * after this one, and a judgement that reads a frame at a fixed point would
 * then be reading a different layout. A test that needs to name the family
 * asks the database for it.
 */
inline void qtcGiveFonts()
{
    QStringList families;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    families = QFontDatabase::families();
#else
    families = QFontDatabase().families();
#endif
    if (families.isEmpty()) {
        QFontDatabase::addApplicationFont(QStringLiteral("C:/Windows/Fonts/segoeui.ttf"));
        QFontDatabase::addApplicationFont(QStringLiteral("C:/Windows/Fonts/arial.ttf"));
    }
}

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
