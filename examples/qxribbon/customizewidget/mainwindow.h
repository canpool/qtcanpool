#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qxribbon/ribbonwindow.h"

QX_RIBBON_BEGIN_NAMESPACE
class RibbonActionsManager;
QX_RIBBON_END_NAMESPACE

class MainWindow : public QxRibbon::RibbonWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void onActionCustomizeTriggered();
private:
    void createActionsManager();
private:
    QxRibbon::RibbonActionsManager *m_actMgr;
    int m_actionTagText;
    int m_actionTagWithIcon;
};
#endif   // MAINWINDOW_H
