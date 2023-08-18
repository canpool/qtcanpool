#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qcanpool/ribbonwindow.h"

class QToolButton;

QCANPOOL_USE_NAMESPACE

class MainWindow : public RibbonWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QToolButton *createToolButton(const QIcon &icon, const QString &text);

private slots:
    void slotChangeStyle(QAction *action);
    void slotSetFrameless(bool frameless);
};

#endif // MAINWINDOW_H
