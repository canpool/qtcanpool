#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtitanRibbon.h>
#include <QMdiArea>

class TextEditChild;
class PieChartChild;
class SketchChild;
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiSubWindow;
class QSignalMapper;
class QRadioButton; 
QT_END_NAMESPACE


/* MdiArea */
class MdiArea : public QMdiArea
{
    Q_OBJECT
public:
    MdiArea(QWidget* parent);
    virtual ~MdiArea();

public:
    void setupMdiArea();

Q_SIGNALS:
    void tabCloseRequested(int index);

protected:
    virtual void paintEvent(QPaintEvent* paintEvent);
};

/* MainWindow */
class MainWindow : public Qtitan::RibbonMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    virtual ~MainWindow();

protected:
    virtual bool eventFilter(QObject*, QEvent* e);
    virtual void closeEvent(QCloseEvent* event);

private slots:
    void newTextFile();
    void newSketchFile();
    void open();
    void save();
    void saveAs();
    void cut();
    void copy();
    void paste();
    void about();
    void updateMenus();
    void updateWindowMenu();
    TextEditChild* createMdiTextChild();
    SketchChild* createSketchChild();
    void switchLayoutDirection();
    void setActiveSubWindow(QWidget* window);
    void options(QAction*);
    void hideStatusBar(int);
    void switchViewMode(int);
    void switchFrameTheme(int);
    void setTabPositionLeft(bool state);
    void setTabPositionTop(bool state);
    void setTabPositionBottom(bool state);
    void setTabPositionRight(bool state);

    void focusChanged(QWidget*, QWidget*);

    void maximizeToggle();
    void minimizationChanged(bool minimized);
    void fullScreen(bool);
    void setDPIToggled(bool);
    void tabCloseRequested(int);

private:
    void createActions();
    void createMenuFile();

    void createRibbon();
    void createOptions();
    void createStatusBar();

    void readSettings();
    void writeSettings();

    QWidget* activeMdiChild();
    QMdiSubWindow* findMdiChild(const QString& fileName);

protected:
    virtual void keyPressEvent(QKeyEvent* event);

private:
    int m_typeDocCurrent;
    QWidget* m_childCurrent;

    QMdiArea* m_mdiArea;
    QSignalMapper* m_windowMapper;

    Qtitan::RibbonGroup* m_groupMdiTabs;
    QRadioButton* m_posTop;
    QRadioButton* m_posBottom;
    QRadioButton* m_posLeft;
    QRadioButton* m_posRight;

    QMenu* m_windowMenu;
    QAction* m_newTextAct;
    QAction* m_newSketchAct;
    QAction* m_openAct;
    QAction* m_saveAct;
    QAction* m_saveAsAct;
    QAction* m_exitAct;
    QAction* m_closeAct;
    QAction* m_appCloseAct;
    QAction* m_closeAllAct;
    QAction* m_tileAct;
    QAction* m_cascadeAct;
    QAction* m_nextAct;
    QAction* m_previousAct;
    QAction* m_separatorAct;
    QAction* m_aboutAct;
    QAction* m_actionRibbonMinimize;
    QAction* m_actionFullScreen;

    Qtitan::RibbonStyle* m_ribbonStyle;
};

#endif
