#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "qxdock/qxdock_global.h"
#include <memory>

QX_DOCK_BEGIN_NAMESPACE
class DockGroup;
class DockGroupManager;
QX_DOCK_END_NAMESPACE

QX_DOCK_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    DockGroup *m_group;
    std::unique_ptr<DockGroupManager> m_groupManager;
};
#endif   // MAINWINDOW_H
