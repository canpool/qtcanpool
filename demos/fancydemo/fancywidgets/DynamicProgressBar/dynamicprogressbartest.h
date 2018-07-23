#ifndef DYNAMICPROGRESSBARTEST_H
#define DYNAMICPROGRESSBARTEST_H

#include <QWidget>

namespace Ui {
class DynamicProgressBarTest;
}

class DynamicProgressBarTest : public QWidget
{
    Q_OBJECT

public:
    explicit DynamicProgressBarTest(QWidget *parent = 0);
    ~DynamicProgressBarTest();

private:
    Ui::DynamicProgressBarTest *ui;
};

#endif // DYNAMICPROGRESSBARTEST_H
