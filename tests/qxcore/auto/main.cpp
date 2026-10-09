#include <QCoreApplication>

#include "tst_global.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // Returns the number of failed test objects, so CTest fails when a test does.
    TEST_RUN_ALL();
}
