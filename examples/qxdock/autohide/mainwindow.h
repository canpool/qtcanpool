#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QWidgetAction>

#include "qxdock/dockwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    virtual void closeEvent(QCloseEvent *event) override;
private:
    Ui::MainWindow *ui;
    QxDock::DockWindow *m_window;
};
#endif   // MAINWINDOW_H
