#include <QtTest/QtTest>
#include <QApplication>

#include "tst_ribbonquickaccessbar.h"


#define TEST_RUN(TestObject) \
    TestObject TestObject##_var; \
    QTest::qExec(&TestObject##_var, argc, argv);


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TEST_RUN(tst_RibbonQuickAccessBar)

    return 0;
}
