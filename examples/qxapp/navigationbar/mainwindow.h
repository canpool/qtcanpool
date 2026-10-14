#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStackedWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private Q_SLOTS:
    void onNavCurrentChanged(int index);
private:
    QStackedWidget *m_stack = nullptr;
};

#endif   // MAINWINDOW_H
