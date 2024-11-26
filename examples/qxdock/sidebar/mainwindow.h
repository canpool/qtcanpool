#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qxdock/dockwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

QX_DOCK_USE_NAMESPACE

/**
 * This example shows, how to place a dock widget container and a static
 * sidebar into a QMainWindow
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    virtual void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;
    DockWindow *m_window;
};

#endif // MAINWINDOW_H
