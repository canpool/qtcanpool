#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#if defined(QXRIBBON_USE_APPWINDOW)
#include "qxwidget/ribbonappwindow.h"
QX_WIDGET_USE_NAMESPACE
#define MyRibbonWindow RibbonAppWindow
#else
#include "qxribbon/ribbonwindow.h"
#define MyRibbonWindow RibbonWindow
#endif

class QTextEdit;
class QActionGroup;
class QAbstractButton;

QX_RIBBON_BEGIN_NAMESPACE
class RibbonPage;
class RibbonPageContext;
class RibbonCustomizeWidget;
class RibbonActionsManager;
class RibbonQuickAccessBar;
class RibbonButtonGroup;
QX_RIBBON_END_NAMESPACE

QX_RIBBON_USE_NAMESPACE

class MainWindow : public MyRibbonWindow
{
    Q_OBJECT
public:
    enum RibbonThemeCustom {
        Office2016BlueTheme = RibbonTheme::CustomTheme + 1,
        UserCustomTheme,
    };
public:
    MainWindow(QWidget *par = nullptr);
    void setRibbonTheme(int theme);
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
    void addSomeOtherAction();

    void setApplicationButton(QAbstractButton *btn);

    QAction *createAction(const QString &text, const QString &iconurl, const QString &objName);
    QAction *createAction(const QString &text, const QString &iconurl);
    QAction *addThemeAction(QAction *action, int themeId);

    void changeRibbonTheme(int index);

public slots:
    void applyStyleSheet(const QString &styleSheet);

private slots:
    void onMenuButtonPopupCheckableTest(bool b);
    void onInstantPopupCheckableTest(bool b);
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
    void onApplicationButtonClicked();
private:
    RibbonPageContext *m_pageContext;
    RibbonPageContext *m_pageContext2;
    QTextEdit *m_edit;
    QActionGroup *m_themeGroup;
    RibbonActionsManager *m_actMgr;
    int m_actionTagText;
    int m_actionTagWithIcon;
    QString m_customizeXml;
};

#endif   // MAINWINDOW_H
