#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qribbon/ribbon.h"
#include <QTimer>

namespace QRibbon
{
class WidgetGalleryItems;
class OfficeWidgetGallery;
}

/* MainWindow */
class MainWindow : public QRibbon::RibbonMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = Q_NULL);
    virtual ~MainWindow();
private:
    void createMenuFile();
    void createQuickAccessBar();
    void createRibbon();
    void createStatusBar();
    void createOptions();

protected Q_SLOTS:
    void open();
    bool save();

    void print();
    void printSetup();
    void about();
    void optionsTheme(QAction *);
    void pressButton();
    void maximizeToggle();
    void minimizationChanged(bool minimized);
    void optionsFont(QAction *act);
    void setDPIToggled(bool);
private:
    QAction *m_actionRibbonMinimize;
    QAction *m_actionDefault;
    QAction *m_actionNormal;
    QAction *m_actionLarge;
    QAction *m_actionExLarge;

    int m_defaultFont;

    QRibbon::RibbonStyle *m_ribbonStyle;
};

#endif   // MAINWINDOW_H
