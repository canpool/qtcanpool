#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ribbonwindow.h"


/* MainWindow */
class MainWindow : public RibbonWindow
{
  Q_OBJECT
public:
    MainWindow(QWidget* parent = Q_NULL);
    virtual ~MainWindow();

private:
    void createBackstage();
    void createQuickAccessBar();
    void createRibbon();
    void createDockWindows();

Q_SIGNALS:
    void updateRecentFileActions(const QStringList& files);

protected Q_SLOTS:
    void open();
    bool save();
    void openRecentFile(const QString& file);

    void print();
    void printSetup();
    void hideStatusBar(int state);
    void pressButton();

    void showBackstage();
    void hideBackstage();
    void setDPIToggled(bool);
private:
    void setCurrentFile(const QString &fileName);

protected:
    Qtitan::RibbonStyle* m_ribbonStyle;
};

#endif // MAINWINDOW_H
