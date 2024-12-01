#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qxribbon/ribbonwindow.h"

QX_RIBBON_BEGIN_NAMESPACE
class RibbonPage;
QX_RIBBON_END_NAMESPACE

class MainWindow : public QxRibbon::RibbonWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void createQuickAccessBar();
    void createStyleGroup(QxRibbon::RibbonPage *page);

private slots:
    void onStyleClicked(int id);
};
#endif   // MAINWINDOW_H
