#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qlite/litewindow.h"

class QStackedWidget;
class QActionGroup;

namespace QLite
{
    class LiteModeBar;
    class LiteTabWidget;
    class LiteTabBar;
}

class MainWindow : public QLite::LiteWindow
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
    QLite::LiteModeBar *m_pModeBar;
    QStackedWidget *m_pStackedWidget;

    QLite::LiteTabWidget *m_pTabWidget;
    QLite::LiteTabBar *m_pTabBar;

    QActionGroup *m_pActionGroup;
};

#endif // MAINWINDOW_H
