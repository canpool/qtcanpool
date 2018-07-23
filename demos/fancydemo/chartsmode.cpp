#include "chartsmode.h"
#include "cwidgets.h"

ChartsMode::ChartsMode(QWidget *parent)
    : IMode(parent)
{
    setObjectName(QLatin1String("ChartsMode"));
    setDisplayName(tr("Charts"));
    setIcon(QIcon(":/mode/chart"));

}
