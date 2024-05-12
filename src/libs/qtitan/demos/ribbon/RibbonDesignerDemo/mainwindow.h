#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtnRibbonMainWindow.h>

#include "ui_RibbonDesignerDemo.h"


/* MainWindow */
class MainWindow : public Qtitan::RibbonMainWindow
{
  Q_OBJECT
public:
    MainWindow(QWidget* parent = Q_NULL);
    virtual ~MainWindow();

private:
    Ui::RibbonMainWindow ui;
};

#endif // MAINWINDOW_H
