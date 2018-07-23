#include "menumode.h"

MenuMode::MenuMode(QWidget *parent)
    : IMode(parent)
{
    setObjectName(QLatin1String("MenuMode"));
    setDisplayName(tr("Tools"));
    setIcon(QIcon(":/mode/menu"));

    QMenu *menu = new QMenu();
    menu->addAction(tr("Action1"));
    menu->addAction(tr("Action2"));
    menu->addAction(tr("Action3"));
    menu->addAction(tr("Action4"));
    menu->setToolTip(tr("qcanpool menu mode"));

    setMenu(menu);
}
