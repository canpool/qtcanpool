#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtitanRibbon.h>

#include "ribbonwindow.h"


/* MainWindow */
class MainWindow : public RibbonWindow
{
  Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = Q_NULL);
    virtual ~MainWindow();
private:
    void createLargeButtons(RibbonPage* page);
    void createSmallButtons(RibbonPage* page);
    void createLargePlusSmallButtons(RibbonPage* page);

protected Q_SLOTS:
private:
    Q_DISABLE_COPY(MainWindow)
};

#endif // MAINWINDOW_H
