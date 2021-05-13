#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qlite/litewindow.h"

class QStackedWidget;
class QActionGroup;

QLITE_BEGIN_NAMESPACE
class LiteModeBar;
class LiteTabWidget;
class LiteTabBar;
QLITE_END_NAMESPACE


class MainWindow : public QLITE_PREPEND_NAMESPACE(LiteWindow)
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
    QLITE_PREPEND_NAMESPACE(LiteModeBar) *m_pModeBar;
    QStackedWidget *m_pStackedWidget;

    QLITE_PREPEND_NAMESPACE(LiteTabWidget) *m_pTabWidget;
    QLITE_PREPEND_NAMESPACE(LiteTabBar) *m_pTabBar;
    QActionGroup *m_pActionGroup;
};

#endif // MAINWINDOW_H
