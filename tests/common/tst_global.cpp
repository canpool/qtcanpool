#include "tst_global.h"

QVector<TestInfo *> UnitTest::s_testInfos;

void UnitTest::add(TestInfo *info)
{
    if (info) {
        s_testInfos.append(info);
    }
}

void UnitTest::run(int argc, char *argv[])
{
    for (TestInfo *info : s_testInfos) {
        QObject *obj = info->testObject();
        if (obj) {
            QTest::qExec(obj, argc, argv);
        }
    }
    qDeleteAll(s_testInfos);
    s_testInfos.clear();
}

TestInfo::TestInfo(QObject *obj)
{
    m_object = obj;
    UnitTest::add(this);
}

TestInfo::~TestInfo()
{
    if (m_object) {
        delete m_object;
    }
}

QObject *TestInfo::testObject() const
{
    return m_object;
}
