#include <QtTest/QtTest>
#include <QApplication>

#include "tst_quickaccessbar.h"


#define TEST_RUN(TestObject) \
    TestObject TestObject##_var; \
    QTest::qExec(&TestObject##_var, argc, argv);


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TEST_RUN(TestQuickAccessBar)
}
