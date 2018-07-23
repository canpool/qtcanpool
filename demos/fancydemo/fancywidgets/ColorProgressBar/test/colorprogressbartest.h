#ifndef COLORPROGRESSBARTEST_H
#define COLORPROGRESSBARTEST_H

#include <QWidget>

namespace Ui {
class ColorProgressBarTest;
}

class ColorProgressBarTest : public QWidget
{
    Q_OBJECT

public:
    explicit ColorProgressBarTest(QWidget *parent = 0);
    ~ColorProgressBarTest();

private:
    Ui::ColorProgressBarTest *ui;
};

#endif // COLORPROGRESSBARTEST_H
