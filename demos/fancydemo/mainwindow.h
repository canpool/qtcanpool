#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qcanpool/fancywindow.h"

QCANPOOL_USE_NAMESPACE

class QActionGroup;

QCANPOOL_BEGIN_NAMESPACE
class FancyTabWidget;
QCANPOOL_END_NAMESPACE

class MainWindow : public FancyWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    void createWindow();
    void createQuickAccessBar();
    void createMenuBar();
    void createSystemMenu();
    void createCentralWidget();

private:
    void addThemeStyleItem(QActionGroup *group, QAction *action, const QString &qss);
    void setThemeStyle(const QString &style);
    void addWindowStyleItem(QActionGroup *group, QAction *action, int style);
    void addTabPositionItem(QActionGroup *group, QAction *action, int position);

    void readSettings();
    void writeSettings();

private slots:
    void slotNew();
    void slotChangeThemeStyle();
    void slotChangeWindowStyle();
    void slotSetTabPosition();

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    FancyTabWidget *m_pTabWidget;
    QString m_themeStyle;
};
#endif // MAINWINDOW_H
