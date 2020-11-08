#ifndef MENUMODE_H
#define MENUMODE_H

#include "qcanpool/fancymode.h"

class MenuMode : public FancyMode
{
    Q_OBJECT
public:
    explicit MenuMode(QWidget *parent = nullptr);
};

#endif // MENUMODE_H
