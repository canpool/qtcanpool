#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QWidgetAction>
#include <QToolBar>

#include "qxdock/qxdock_global.h"

QX_DOCK_BEGIN_NAMESPACE
class DockWindow;
class DockPanel;
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
    void savePerspective();
protected:
    virtual void closeEvent(QCloseEvent *event) override;
private:
    void createPerspectiveUi();
private:
    DockWindow *m_window;
    QToolBar *m_toolBar;
    QAction *m_savePerspectiveAction = nullptr;
    QWidgetAction *m_perspectiveListAction = nullptr;
    QComboBox *m_perspectiveComboBox = nullptr;
};
#endif // MAINWINDOW_H
