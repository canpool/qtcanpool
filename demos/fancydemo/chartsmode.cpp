#include "chartsmode.h"
#include "cwidgets.h"

ChartsMode::ChartsMode(QWidget *parent)
    : FancyMode(parent)
{
    setObjectName(QLatin1String("ChartsMode"));
    setDisplayName(tr("Charts"));
    setIcon(QIcon(":/mode/chart"));

}
