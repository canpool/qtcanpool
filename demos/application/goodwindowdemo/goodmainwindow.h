#ifndef GOODMAINWINDOW_H
#define GOODMAINWINDOW_H

#include "qgood/qgoodwindow.h"

class GoodMainWindow : public QGoodWindow
{
    Q_OBJECT
public:
    GoodMainWindow(QWidget *parent = nullptr);
    ~GoodMainWindow();

signals:
    void windowStateChanged(Qt::WindowStates state);
protected:
    virtual bool event(QEvent *e) Q_DECL_OVERRIDE;
};

#endif // MAINWINDOW_H
