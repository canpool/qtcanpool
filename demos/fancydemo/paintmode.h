#ifndef PAINTMODE_H
#define PAINTMODE_H

#include "qcanpool/imode.h"

class TinyTabWidget;
class MiniStack;
class StateProgressBar;
class PaintMode : public IMode
{
    Q_OBJECT
public:
    explicit PaintMode(QWidget *parent = 0);

private:
    void createWindow();

    void createProgressWidget();
    void createProgressBar();
    void createDial();

signals:

public slots:

private slots:

private:
    TinyTabWidget *m_pMainTabWidget;

    // progress widget
    QWidget *m_pProgressWidget;
    MiniStack *m_pProgressBarStack;
    MiniStack *m_pDialStack;
};

#endif // PAINTMODE_H
