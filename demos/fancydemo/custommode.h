#ifndef CUSTOMMODE_H
#define CUSTOMMODE_H

#include "qcanpool/fancymode.h"

QCANPOOL_USE_NAMESPACE

QCANPOOL_BEGIN_NAMESPACE
class TinyTabWidget;
QCANPOOL_END_NAMESPACE

class CustomMode : public FancyMode
{
    Q_OBJECT
public:
    explicit CustomMode(QWidget *parent = 0);

private:
    void createWindow();

private:
    TinyTabWidget *m_pTabWidget;
};

#endif // CUSTOMMODE_H
