#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qxdock/qxdock_global.h"
#include <QMainWindow>

QX_DOCK_BEGIN_NAMESPACE
class DockWidget;
class DockWindow;
QX_DOCK_END_NAMESPACE

QX_DOCK_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    DockWidget *createColorDockWidget(const QString &title, const QColor &color);
    QWidget *createColorWidget(const QColor &color);

    void saveState();
    void restoreState();

protected:
    virtual void closeEvent(QCloseEvent *event) override;

private:
    DockWindow *m_dockWindow = nullptr;
};
#endif   // MAINWINDOW_H
