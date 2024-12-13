#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qxribbon/ribbonwindow.h"

class ColorButton;

class MainWindow : public QxRibbon::RibbonWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    ColorButton *createColorButton(const QColor &color, const QString &text);
};
#endif   // MAINWINDOW_H
