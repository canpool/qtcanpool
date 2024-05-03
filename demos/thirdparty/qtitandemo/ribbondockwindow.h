#ifndef RIBBONDOCKWINDOW_H
#define RIBBONDOCKWINDOW_H

#include <QtnDockMainWindow.h>
#include <QtitanRibbon.h>
#include <QtitanDocking.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QTextEdit>

/* RibbonDockWindow */
class RibbonDockWindow : public Qtitan::DockMainWindow
{
public:
    RibbonDockWindow(QWidget *parent = Q_NULL);
    virtual ~RibbonDockWindow();

private:
    Q_DISABLE_COPY(RibbonDockWindow)
};

#endif // RIBBONDOCKWINDOW_H
