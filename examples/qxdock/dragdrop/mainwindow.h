#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "qxdock/qxdock_global.h"

class QAction;

QX_DOCK_BEGIN_NAMESPACE
class DockWindow;
class DockWidget;
QX_DOCK_END_NAMESPACE

QX_DOCK_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startNativeDrag();
    void onDockWidgetDropped(const QString &name, const QPoint &globalPos);
private:
    DockWindow *m_window;
    DockWidget *m_notes;
};

#endif   // MAINWINDOW_H
