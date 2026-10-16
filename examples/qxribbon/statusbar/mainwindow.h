#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qxribbon/ribbonmainwindow.h"

QX_RIBBON_BEGIN_NAMESPACE
class RibbonStatusBar;
QX_RIBBON_END_NAMESPACE

class MainWindow : public QxRibbon::RibbonMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    // The line the cursor is on, for the left segment that reports it. It has to
    // live on the object: the action outlives the function that made it, so a
    // counter captured by reference from there would dangle.
    int m_line = 1;
};

#endif   // MAINWINDOW_H
