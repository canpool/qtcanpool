#ifndef CHARTSMODE_H
#define CHARTSMODE_H

#include "qcanpool/fancymode.h"

QCANPOOL_USE_NAMESPACE

class ChartsMode : public FancyMode
{
    Q_OBJECT
public:
    explicit ChartsMode(QWidget *parent = nullptr);
};

#endif // CHARTSMODE_H
