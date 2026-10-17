#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#if defined(QXRIBBON_USE_APPWINDOW)
#include "qxapp/ribbonappwindow.h"
QX_APP_USE_NAMESPACE
#define MyRibbonWindow RibbonAppWindow
#else
#include "qxribbon/ribbonmainwindow.h"
#define MyRibbonWindow RibbonMainWindow
#endif

class QTextEdit;
class QActionGroup;
class QAbstractButton;

QX_RIBBON_BEGIN_NAMESPACE
class RibbonPage;
class RibbonPageContext;
class RibbonActionsManager;
class RibbonQuickAccessBar;
class RibbonButtonGroup;
class RibbonBackstageView;
QX_RIBBON_END_NAMESPACE

QX_RIBBON_USE_NAMESPACE

class MainWindow : public MyRibbonWindow
{
    Q_OBJECT
public:
    /*!
     * @param screenshotMode keeps the construction log out of the central
     * editor. The log carries elapsed milliseconds, so its text - and with it
     * the capture - changes from run to run; a screenshot has to render the
     * same pixels every time for the golden to mean anything. The numbers are
     * still printed, they just go to the log file instead of the window.
     */
    MainWindow(QWidget *par = nullptr, bool screenshotMode = false);
    ~MainWindow();
private:
    void createCentralWidget();
    void createStatusBar();
    void createRibbon();

    void createPageHome();
    void createPageOther();
    void createPageCustom();
    void createPageApply();
    void createPageContext1();
    void createPageContext2();
    void createPageContext1(RibbonPage *page);
    void createPageContext2(RibbonPage *page);
    void createQuickAccessBar();
    void createRightButtonGroup();
    void createApplicationButton();
    void createBackstage();
    void addSomeOtherAction();

    void setApplicationButton(QAbstractButton *btn);

    QAction *createAction(const QString &text, const QString &iconurl, const QString &objName);
    QAction *createAction(const QString &text, const QString &iconurl);
    QAction *addThemeAction(QAction *action, const QString &themeId);
    void applyTheme(const QString &themeId);

    void changeRibbonTheme(int index);

public slots:
    void applyStyleSheet(const QString &styleSheet);

private slots:
    void onMenuButtonPopupCheckableTest(bool b);
    void onDelayedPopupCheckableTest(bool b);
    void onMenuButtonPopupCheckabletriggered(bool b);
    void onInstantPopupCheckabletriggered(bool b);
    void onDelayedPopupCheckabletriggered(bool b);
    void onShowContextPage(bool on);
    void onStyleClicked(int id);
    void onActionCustomizeAndSaveTriggered(bool b);
    void onActionHelpTriggered();
    void onActionRemoveAppBtnTriggered(bool b);
    void onActionUseQssTriggered();
    void onActionLoadCustomizeXmlFileTriggered();
    void onActionWindowFlagNormalButtonTriggered(bool b);
    void onActionChangeThemeTriggered();

    void onFontComWidgetCurrentFontChanged(const QFont &f);
    void onActionFontLargerTriggered();
    void onActionFontSmallerTriggered();
    void onButtonGroupActionTriggered(QAction *act);
private:
    RibbonPageContext *m_pageContext = nullptr;
    RibbonPageContext *m_pageContext2 = nullptr;
    QTextEdit *m_edit = nullptr;
    RibbonBackstageView *m_backstage = nullptr;
    QActionGroup *m_themeGroup = nullptr;
    RibbonActionsManager *m_actMgr = nullptr;
    int m_actionTagText = 0;
    int m_actionTagWithIcon = 0;
    QString m_customizeXml;
    bool m_screenshotMode = false;
};

#endif   // MAINWINDOW_H
