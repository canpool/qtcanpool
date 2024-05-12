#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ribbonwindow.h"

class QProgressBar;

/* MainWindow */
class MainWindow : public RibbonWindow
{
  Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = Q_NULL);

private:
    void createQuickAccessBar();
    void createRibbon();
    void createStatusBar();

protected Q_SLOTS:
    void pressLogoRight();
    void pressLogoLeft();

protected:
    QAction* m_actionLogoRight;
    QAction* m_actionLogoLeft;
};

#endif // MAINWINDOW_H
