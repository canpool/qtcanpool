#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <windows.h>
#include <windowsx.h>
#include <QtGui>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *);
    bool winEvent(MSG *message, long *result);

private:
    void drawWindowShadow(QPainter &p);
};

#endif // MAINWINDOW_H
