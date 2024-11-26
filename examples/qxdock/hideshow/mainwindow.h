#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "qxdock/qxdock_global.h"

class QStackedLayout;

QX_DOCK_BEGIN_NAMESPACE
class DockWindow;
QX_DOCK_END_NAMESPACE

QX_DOCK_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void openProject();
    void closeProject();
private:
    DockWindow *m_window;
    QWidget *m_welcomeWidget;
    QStackedLayout *m_layout;
    QAction *m_actionOpen;
    QAction *m_actionClose;
    QMenu *m_menuView;
};
#endif // MAINWINDOW_H
