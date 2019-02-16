#ifndef CUSTOMSTYLES_H
#define CUSTOMSTYLES_H

#include <QMainWindow>

class QMdiArea;
class QTextEdit;
class QActionGroup;

namespace Qtitan {class OfficeStyle;}

/* Widgets */
class Widgets : public QMainWindow
{
    Q_OBJECT

public:
    Widgets(QWidget* parent = 0);
    virtual ~Widgets();

protected:
    void createActions();
    void createMenubar();
    void createToolbar();
    void createDockWindows();

protected:
    virtual void closeEvent(QCloseEvent* event);

protected:
    void updateStyle(QAction* act);
    void updateTheme(QAction* act);

    void readSettings();
    void writeSettings();

private slots:
    void createStyleDialog();
    QTextEdit* createMdiChild();

    void triggeredStyles(QAction* action);
    void triggeredThemes(QAction* action);
    void about();

private:
    QMenu* m_menuView;
    QMenu* m_menuTheme;

    // File
    QAction* m_actNew;
    QAction* m_actOpen;
    QAction* m_actClose;
    QAction* m_actSave;
    QAction* m_actSaveAs;
    QAction* m_actPrint;
    QAction* m_actPrintPreview;
    QAction* m_actExit;
    // Custom Styles
    QAction* m_actDefaultStyle;
    QAction* m_actOffice2007Style;
    QAction* m_actRibbonStyle;

    // Group themes
    QActionGroup* m_groupThemesActions;
    QAction* m_actToggleThemes;
    QAction* m_actBlueTheme;
    QAction* m_actBlackTheme;
    QAction* m_actSilverTheme;
    QAction* m_actAquaTheme;
    QAction* m_actScenicTheme;
    QAction* m_actBlue2010Theme;
    QAction* m_actSilver2010Theme;
    QAction* m_actBlack2010Theme;

    // Group help
    QAction* m_actHelp;

    QMdiArea* m_mdiArea;
    QDockWidget* m_styleDock;
    QDockWidget* m_treeDock;
    QDockWidget* m_propDock;
    QDockWidget* m_toolBoxDock;

    Qtitan::OfficeStyle* m_officeStyle;
private:
    QString m_styleName;
};

#endif // CUSTOMSTYLES_H
