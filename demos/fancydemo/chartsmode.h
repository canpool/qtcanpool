#ifndef CHARTSMODE_H
#define CHARTSMODE_H

#include "qcanpool/imode.h"

class ChartsMode : public IMode
{
    Q_OBJECT
public:
    explicit ChartsMode(QWidget *parent = nullptr);
};

#endif // CHARTSMODE_H
