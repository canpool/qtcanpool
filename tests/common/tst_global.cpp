#include "tst_global.h"

UnitTest::UnitTest()
{
    s_testObjects.clear();
}

UnitTest::~UnitTest()
{
    qDeleteAll(s_testObjects);
    s_testObjects.clear();
}

UnitTest &UnitTest::instance()
{
    static UnitTest s_testInstance;
    return s_testInstance;
}

void UnitTest::add(QObject *obj)
{
    if (obj) {
        s_testObjects.append(obj);
    }
}

void UnitTest::run(int argc, char *argv[])
{
    for (QObject *obj : s_testObjects) {
        QTest::qExec(obj, argc, argv);
    }
    qDeleteAll(s_testObjects);
    s_testObjects.clear();
}

TestInfo::TestInfo(QObject *obj)
{
    UnitTest::instance().add(obj);
}
