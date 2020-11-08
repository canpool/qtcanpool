#ifndef CUSTOMMODE_H
#define CUSTOMMODE_H

#include "qcanpool/fancymode.h"

class MiniCard;
class TinyTabWidget;

class CustomMode : public FancyMode
{
    Q_OBJECT
public:
    explicit CustomMode(QWidget *parent = 0);

private:
    void createWindow();
    void createCards();

private:
    TinyTabWidget *m_pTabWidget;
    QList<MiniCard *> m_infoCards;
};

#endif // CUSTOMMODE_H
