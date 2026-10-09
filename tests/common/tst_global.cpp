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

int UnitTest::count() const
{
    return s_testObjects.count();
}

int UnitTest::run(int argc, char *argv[])
{
    // QTest can select a single test function on the command line, but the same
    // arguments are handed to every registered test object, so the other
    // objects would just report "Function not found". QTC_ONLY_TEST=<class>
    // limits the run to one object, which makes debugging a suite practical.
    const QByteArray only = qgetenv("QTC_ONLY_TEST");

    int failed = 0;
    int executed = 0;
    for (QObject *obj : s_testObjects) {
        if (!only.isEmpty() && only != obj->metaObject()->className()) {
            continue;
        }
        ++executed;
        if (QTest::qExec(obj, argc, argv) != 0) {
            ++failed;
        }
    }
    qDeleteAll(s_testObjects);
    s_testObjects.clear();

    // A filter that matches nothing is a typo, not a passing run.
    if (!only.isEmpty() && executed == 0) {
        return 1;
    }
    return failed;
}

TestInfo::TestInfo(QObject *obj)
{
    UnitTest::instance().add(obj);
}
