#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class StateProgressBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void createWindow();
    void createStateProgressBar();

private:
    StateProgressBar *m_pStateProgressBar;
};

#endif // MAINWINDOW_H
