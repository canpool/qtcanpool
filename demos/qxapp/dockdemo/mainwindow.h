#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qxdock/qxdock_global.h"
#include <QMainWindow>

QX_DOCK_BEGIN_NAMESPACE
class DockWindow;
QX_DOCK_END_NAMESPACE

QX_DOCK_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

    /*!
     * The demo's arrangement goes in and out of a settings file, and the two
     * are named for that rather than saveState()/restoreState(): those are
     * QMainWindow's own, they take arguments, and a same-named pair here
     * hides them - which is exactly why this class could not call its base's
     * without writing the class name in front of every call.
     */
    void saveToSettings();
    void restoreFromSettings();
protected:
    virtual void closeEvent(QCloseEvent *event) override;
private:
    DockWindow *m_dockWindow = nullptr;
};
#endif   // MAINWINDOW_H
