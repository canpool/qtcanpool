#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include <QtitanRibbon.h>


namespace Qtitan 
{ 
    class WidgetGalleryItems; 
    class OfficeWidgetGallery;
}

/* MainWindow */
class MainWindow : public Qtitan::RibbonMainWindow
{
  Q_OBJECT
public:
    MainWindow(QWidget* parent = Q_NULL);
    virtual ~MainWindow();

private:
    void createMenuFile();
    void createQuickAccessBar();
    void createRibbon();
    void createStatusBar();
    void createOptions();

protected slots:
    void open();
    bool save();

#ifndef QT_NO_PRINTER
    void print();
    void printSetup();
#endif
    void about();
    void options(QAction*);
    void pressButton();
    void maximizeToggle();
    void minimizationChanged(bool minimized);
    void optionsFont(QAction* act);
    void setDPIToggled(bool);

private:
    QAction* m_actionRibbonMinimize;
    QAction* m_actionDefault;
    QAction* m_actionNormal;
    QAction* m_actionLarge;
    QAction* m_actionExLarge;

    int m_defaultFont;

    Qtitan::RibbonStyle* m_ribbonStyle;
};

#endif // MAINWINDOW_H
