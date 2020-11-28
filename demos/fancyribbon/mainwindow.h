#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcanpool/qcanpool_global.h"

QCANPOOL_USE_NAMESPACE

QCANPOOL_BEGIN_NAMESPACE
class FancyRibbon;
QCANPOOL_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    FancyRibbon *m_ribbon;
};

#endif // MAINWINDOW_H
