#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qcanpool/fancywindow.h"
#include "qcanpool/fancydialog.h"

QCANPOOL_USE_NAMESPACE

class MainWindow : public FancyWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};
#endif   // MAINWINDOW_H

class Dialog : public FancyDialog
{
    Q_OBJECT
public:
    Dialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~Dialog();
};
