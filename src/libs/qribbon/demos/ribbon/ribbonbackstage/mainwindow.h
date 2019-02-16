#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTimer>
#include <QtitanRibbon.h>


class QProgressBar;

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
    void createBackstage();
    void createQuickAccessBar();
    void createRibbon();
    void createOptions();

Q_SIGNALS:
    void updateRecentFileActions(const QStringList& files);

protected slots:
    void open();
    bool save();
    void openRecentFile(const QString& file);

#ifndef QT_NO_PRINTER
    void print();
    void printSetup();
#endif
    void about();
    void options(QAction*);
    void hideStatusBar(int state);
    void pressButton();
    void maximizeToggle();
    void minimizationChanged(bool minimized);

    void showBackstage();
    void hideBackstage();
    void setDPIToggled(bool);
private:
    void setCurrentFile(const QString &fileName);

private:
    QAction* m_actionRibbonMinimize;
    QMenu* m_menuOptions;

    Qtitan::RibbonStyle* m_ribbonStyle;
};

#endif // MAINWINDOW_H
