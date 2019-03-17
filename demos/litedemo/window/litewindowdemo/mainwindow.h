#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "litewindow.h"

class LiteModeBar;
class LiteTabWidget;
class LiteTabBar;

class QStackedWidget;
class QActionGroup;

class MainWindow : public LiteWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void createWindow();

    void addSkinItem(QAction *action, const QString &qss);

private slots:
    void slotLiteDialog();
    void slotChangeSkin();

private:
    LiteModeBar *m_pModeBar;
    QStackedWidget *m_pStackedWidget;

    LiteTabWidget *m_pTabWidget;
    LiteTabBar *m_pTabBar;

    QActionGroup *m_pActionGroup;
};

#endif // MAINWINDOW_H
