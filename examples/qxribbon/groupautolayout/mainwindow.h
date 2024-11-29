#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qxribbon/ribbonwindow.h"

QX_RIBBON_BEGIN_NAMESPACE
class RibbonGroup;
QX_RIBBON_END_NAMESPACE

class MainWindow : public QxRibbon::RibbonWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QAction *createAction(const QIcon &icon, const QString &text);
    QAction *createAction(const QString &text);

    void addMediumAction(QxRibbon::RibbonGroup *group, QAction *action);
    void addSmallAction(QxRibbon::RibbonGroup *group, QAction *action);
};

#endif   // MAINWINDOW_H
