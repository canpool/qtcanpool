#include <QApplication>
#include <QDesktopWidget>

#include <QtnRibbonDef.h>

#include "aboutdialog.h"
#include "ribbonwindow.h"

#define Font_Default        "System Default"
#define Font_Normal         "Normal"
#define Font_Large          "Large"
#define Font_ExLarge        "Extra Large"
#define DPI_100_percent     "DPI 100%"


/* RibbonWindow */
RibbonWindow::RibbonWindow(QWidget* parent)
    : Qtitan::RibbonMainWindow(parent)
    , m_ribbonStyle(qobject_cast<Qtitan::RibbonStyle*>(qApp->style()))
{
//    m_ribbonStyle->setDPIAware(false);
    RibbonToolTip::setWrapMode(RibbonToolTip::NativeWrap);
    createOptions();

    QAction* actionAbout = ribbonBar()->addAction(QIcon(":/shared/res/about.png"), tr("About"), Qt::ToolButtonIconOnly);
    actionAbout->setToolTip(tr("Display program<br />information, version number and copyright"));
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    m_actionRibbonMinimize = ribbonBar()->addAction(QIcon(":/shared/res/ribbonMinimize.png"), tr("Minimize the Ribbon"), Qt::ToolButtonIconOnly);
    m_actionRibbonMinimize->setStatusTip(tr("Show only the tab names on the Ribbon"));
    m_actionRibbonMinimize->setShortcut(tr("Ctrl+F1"));
    connect(m_actionRibbonMinimize, SIGNAL(triggered()), this, SLOT(maximizeToggle()));
    connect(ribbonBar(), SIGNAL(minimizationChanged(bool)), this, SLOT(minimizationChangedIcon(bool)));

    m_actionRibbonMinimizeMenu = new QAction(tr("Minimize the Ribbon"), this);
    m_actionRibbonMinimizeMenu->setCheckable(true);
    connect(m_actionRibbonMinimizeMenu, SIGNAL(triggered()), this, SLOT(maximizeToggle()));
    connect((QWidget*)ribbonBar()->quickAccessBar(), SIGNAL(showCustomizeMenu(QMenu*)), this, SLOT(showQuickAccessCustomizeMenu(QMenu*)));
    connect(ribbonBar(), SIGNAL(showRibbonContextMenu(QMenu*, QContextMenuEvent*)), this, SLOT(showRibbonContextMenu(QMenu*, QContextMenuEvent*)));
    updateActionsTheme();
}

void RibbonWindow::createOptions()
{
    Qtitan::RibbonStyle::Theme themeId = Qtitan::RibbonStyle::Office2007Blue;
    if (m_ribbonStyle)
        themeId = m_ribbonStyle->getTheme();

    m_menuOptions = ribbonBar()->addMenu(tr("Options"));
    QAction* actionStyle = m_menuOptions->addAction(tr("Style"));

    QMenu* menuStyle = new QMenu(ribbonBar());
    m_styleActions = new QActionGroup(this);

    QMenu* menuStyle2007 = new QMenu(tr("Office 2007"), this);
    QAction* actionBlue = menuStyle2007->addAction(tr("Blue"));
    actionBlue->setCheckable(true);
    actionBlue->setChecked(themeId == RibbonStyle::Office2007Blue);
    actionBlue->setObjectName("Office2007Blue");

    QAction* actionBlack = menuStyle2007->addAction(tr("Black"));
    actionBlack->setObjectName("Office2007Black");
    actionBlack->setCheckable(true);
    actionBlack->setChecked(themeId == RibbonStyle::Office2007Black);

    QAction* actionSilver = menuStyle2007->addAction(tr("Silver"));
    actionSilver->setObjectName("Office2007Silver");
    actionSilver->setCheckable(true);
    actionSilver->setChecked(themeId == RibbonStyle::Office2007Silver);

    QAction* actionAqua = menuStyle2007->addAction(tr("Aqua"));
    actionAqua->setObjectName("Office2007Aqua");
    actionAqua->setCheckable(true);
    actionAqua->setChecked(themeId == RibbonStyle::Office2007Aqua);
    menuStyle->addMenu(menuStyle2007);

    menuStyle->addSeparator();

    QAction* actionScenic = menuStyle->addAction(tr("Windows 7 Scenic"));
    actionScenic->setObjectName("Windows7Scenic");
    actionScenic->setCheckable(true);
    actionScenic->setChecked(themeId == RibbonStyle::Windows7Scenic);

    menuStyle->addSeparator();

    QMenu* menuStyle2010 = new QMenu(tr("Office 2010"), this);
    QAction* action2010Blue = menuStyle2010->addAction(tr("Blue"));
    action2010Blue->setObjectName("Office2010Blue");
    action2010Blue->setCheckable(true);
    action2010Blue->setChecked(themeId == RibbonStyle::Office2010Blue);

    QAction* action2010Silver = menuStyle2010->addAction(tr("Silver"));
    action2010Silver->setObjectName("Office2010Silver");
    action2010Silver->setCheckable(true);
    action2010Silver->setChecked(themeId == RibbonStyle::Office2010Silver);

    QAction* action2010Black = menuStyle2010->addAction(tr("Black"));
    action2010Black->setObjectName("Office2010Black");
    action2010Black->setCheckable(true);
    action2010Black->setChecked(themeId == RibbonStyle::Office2010Black);
    menuStyle->addMenu(menuStyle2010);

    menuStyle->addSeparator();

    QMenu* menuStyle2013 = new QMenu(tr("Office 2013"), this);
    QAction* action2013White = menuStyle2013->addAction(tr("White"));
    action2013White->setObjectName("Office2013White");
    action2013White->setCheckable(true);
    action2013White->setChecked(themeId == RibbonStyle::Office2013White);

    QAction* action2013Gray = menuStyle2013->addAction(tr("Light Gray"));
    action2013Gray->setObjectName("Office2013Gray");
    action2013Gray->setCheckable(true);
    action2013Gray->setChecked(themeId == RibbonStyle::Office2013Gray);
    menuStyle->addMenu(menuStyle2013);

    QAction* action2013Dark = menuStyle2013->addAction(tr("Dark Gray"));
    action2013Dark->setObjectName("Office2013Dark");
    action2013Dark->setCheckable(true);
    action2013Dark->setChecked(themeId == RibbonStyle::Office2013Dark);
    menuStyle->addMenu(menuStyle2013);

    menuStyle->addSeparator();

    //QMenu* menuStyle2016 = new QMenu(tr("Office 2016"), this);
    //QAction* action2016Colorful = menuStyle2016->addAction(tr("Colorful"));
    //action2016Colorful->setObjectName("Office2016Colorful");
    //action2016Colorful->setCheckable(true);
    //action2016Colorful->setChecked(themeId == RibbonStyle::Office2016Colorful);

    //QAction* action2016White = menuStyle2016->addAction(tr("White"));
    //action2016White->setObjectName("Office2016White");
    //action2016White->setCheckable(true);
    //action2016White->setChecked(themeId == RibbonStyle::Office2016White);

    //QAction* action2016DarkGray = menuStyle2016->addAction(tr("Dark Gray"));
    //action2016DarkGray->setObjectName("Office2016DarkGray");
    //action2016DarkGray->setCheckable(true);

    //QAction* action2016Black = menuStyle2016->addAction(tr("Black"));
    //action2016Black->setObjectName("Office2016Black");
    //action2016Black->setCheckable(true);

    //menuStyle->addMenu(menuStyle2016);

    m_styleActions->addAction(actionBlue);
    m_styleActions->addAction(actionBlack);
    m_styleActions->addAction(actionSilver);
    m_styleActions->addAction(actionAqua);
    m_styleActions->addAction(actionScenic);
    m_styleActions->addAction(action2010Blue);
    m_styleActions->addAction(action2010Silver);
    m_styleActions->addAction(action2010Black);
    m_styleActions->addAction(action2013White);
    m_styleActions->addAction(action2013Gray);
    m_styleActions->addAction(action2013Dark);
    //m_styleActions->addAction(action2016Colorful);
    //m_styleActions->addAction(action2016White);
    //m_styleActions->addAction(action2016DarkGray);
    //m_styleActions->addAction(action2016Black);

    actionStyle->setMenu(menuStyle);
    connect(m_styleActions, SIGNAL(triggered(QAction*)), this, SLOT(optionsTheme(QAction*)));

    m_menuOptions->addSeparator();

    QAction* actionAnimation = m_menuOptions->addAction(tr("Animation"));
    actionAnimation->setCheckable(true);
    actionAnimation->setChecked(true);
    connect(actionAnimation, SIGNAL(triggered(bool)), this, SLOT(includingAnimation(bool)));

    m_menuOptions->addSeparator();

    QAction* actionTitleGroupsVisible = m_menuOptions->addAction(tr("Title Groups Visible"));
    actionTitleGroupsVisible ->setCheckable(true);
    actionTitleGroupsVisible ->setChecked(true);
    connect(actionTitleGroupsVisible , SIGNAL(triggered(bool)), this, SLOT(setTitleGroupsVisible(bool)));

#ifdef Q_WIN
    QAction* actionFrameTheme = m_menuOptions->addAction(tr("Frame Theme"));
    actionFrameTheme->setCheckable(true);
    actionFrameTheme->setChecked(true);
    connect(actionFrameTheme, SIGNAL(triggered(bool)), this, SLOT(frameTheme(bool)));
#endif // Q_WIN

    m_menuOptions->addSeparator();

    QAction* actionMenu = m_menuOptions->addAction(tr("Font"));

    QMenu* menuFont = new QMenu(ribbonBar());
    QActionGroup* fontActions = new QActionGroup(this);

    m_actionDefault = menuFont->addAction(Font_Default);
    m_actionDefault->setCheckable(true);
    m_actionDefault->setChecked(true);
    m_actionDefault->setObjectName(Font_Default);
    fontActions->addAction(m_actionDefault);

    menuFont->addSeparator();

    m_actionNormal = menuFont->addAction(Font_Normal);
    m_actionNormal->setCheckable(true);
    m_actionNormal->setObjectName(Font_Normal);
    fontActions->addAction(m_actionNormal);

    m_actionLarge = menuFont->addAction(Font_Large);
    m_actionLarge->setCheckable(true);
    m_actionLarge->setObjectName(Font_Large);
    fontActions->addAction(m_actionLarge);

    m_actionExLarge = menuFont->addAction(Font_ExLarge);
    m_actionExLarge->setCheckable(true);
    m_actionExLarge->setObjectName(Font_ExLarge);
    fontActions->addAction(m_actionExLarge);
    actionMenu->setMenu(menuFont);
    connect(fontActions, SIGNAL(triggered(QAction*)), this, SLOT(optionsFont(QAction*)));

    menuFont->addSeparator();
    QAction* actionDPI_100_percent = menuFont->addAction(tr("DPI Aware (new)"));
    actionDPI_100_percent->setObjectName(DPI_100_percent);
    actionDPI_100_percent->setCheckable(true);
    actionDPI_100_percent->setChecked(m_ribbonStyle->isDPIAware());
    connect(actionDPI_100_percent, SIGNAL(toggled(bool)), this, SLOT(setDPIToggled(bool)));

    m_menuOptions->addSeparator();
    QAction* actionNativeScrollBars = m_menuOptions->addAction(tr("Native Scroll Bars (new)"));
    actionNativeScrollBars->setCheckable(true);
#ifdef Q_OS_MAC
    m_ribbonStyle->setScrollBarsIgnored(true);
#endif
    actionNativeScrollBars->setChecked(m_ribbonStyle->isScrollBarsIgnored());
    connect(actionNativeScrollBars, SIGNAL(toggled(bool)), this, SLOT(setNativeScrollBarsToggled(bool)));

    QAction* actionNativeDialogs = m_menuOptions->addAction(tr("Native Dialogs"));
    actionNativeDialogs->setCheckable(true);
    actionNativeDialogs->setChecked(m_ribbonStyle->isDialogsIgnored());
    connect(actionNativeDialogs, SIGNAL(toggled(bool)), this, SLOT(setNativeDialogsToggled(bool)));
    m_menuOptions->addSeparator();

    QAction* actionCusomize = m_menuOptions->addAction(tr("Cusomize..."));
    actionCusomize->setEnabled(false);

    m_actionShowBelowRibbon = new QAction(tr("Show Below the Ribbon"), this);
    m_actionShowAboveRibbon = new QAction(tr("Show Above the Ribbon"), this);
    connect(m_actionShowBelowRibbon, SIGNAL(triggered()), this, SLOT(switchQuickAccessBarPosition()));
    connect(m_actionShowAboveRibbon, SIGNAL(triggered()), this, SLOT(switchQuickAccessBarPosition()));
}

void RibbonWindow::about()
{
    Qtitan::AboutDialog::show(this, tr("About Qtitan Ribbon Controls Sample"), 
        tr("QtitanRibbon"), QLatin1String(QTN_VERSION_RIBBON_STR));
}

void RibbonWindow::optionsDialog()
{
}

void RibbonWindow::optionsTheme(QAction* action)
{
    Qtitan::RibbonStyle::Theme themeId = Qtitan::RibbonStyle::Office2007Blue;
    if (action->objectName() == "Office2007Black")
        themeId = RibbonStyle::Office2007Black;
    else if (action->objectName() == "Office2007Silver")
        themeId = RibbonStyle::Office2007Silver;
    else if (action->objectName() == "Office2007Aqua")
        themeId = RibbonStyle::Office2007Aqua;
    else if (action->objectName() == "Windows7Scenic")
        themeId = OfficeStyle::Windows7Scenic;
    else if (action->objectName() == "Office2010Blue")
        themeId = OfficeStyle::Office2010Blue;
    else if (action->objectName() == "Office2010Silver")
        themeId = OfficeStyle::Office2010Silver;
    else if (action->objectName() == "Office2010Black")
        themeId = OfficeStyle::Office2010Black;
    else if (action->objectName() == "Office2013White")
        themeId = OfficeStyle::Office2013White;
    else if (action->objectName() == "Office2013Gray")
        themeId = OfficeStyle::Office2013Gray;
    else if (action->objectName() == "Office2013Dark")
        themeId = OfficeStyle::Office2013Dark;
    else if (action->objectName() == "Office2016Colorful")
        themeId = OfficeStyle::Office2016Colorful;
    else if (action->objectName() == "Office2016White")
        themeId = OfficeStyle::Office2016White;
    else if (action->objectName() == "Office2016DarkGray")
        themeId = OfficeStyle::Office2016DarkGray;
    else if (action->objectName() == "Office2016Black")
        themeId = OfficeStyle::Office2016Black;

    setStyleTheme(themeId);
}

RibbonStyle::Theme RibbonWindow::styleTheme() const
{
    if (m_ribbonStyle == Q_NULL)
    {
        Q_ASSERT(false);
        return (RibbonStyle::Theme)-1;
    }
    return m_ribbonStyle->getTheme();
}

void RibbonWindow::setStyleTheme(RibbonStyle::Theme themeId)
{
    if (m_ribbonStyle == Q_NULL)
    {
        Q_ASSERT(false);
        return;
    }
    setButtonStyleTheme(themeId);
    m_ribbonStyle->setTheme(themeId);
    updateActionsTheme();
}

void RibbonWindow::setButtonStyleTheme(RibbonStyle::Theme themeId)
{
    if (QToolButton* button = ribbonBar()->getSystemButton())
    {
        if (themeId == OfficeStyle::Windows7Scenic   || 
            themeId == OfficeStyle::Office2010Blue   ||
            themeId == OfficeStyle::Office2010Silver ||
            themeId == OfficeStyle::Office2010Black  ||
            themeId == OfficeStyle::Office2013White  ||
            themeId == OfficeStyle::Office2013Gray   ||
            themeId == OfficeStyle::Office2013Dark   ||
            themeId == OfficeStyle::Office2016Colorful ||
            themeId == OfficeStyle::Office2016White  ||
            themeId == OfficeStyle::Office2016DarkGray ||
            themeId == OfficeStyle::Office2016Black)
            button->setToolButtonStyle(Qt::ToolButtonTextOnly);
        else
            button->setToolButtonStyle(Qt::ToolButtonFollowStyle);
    }
}

void RibbonWindow::updateActionsTheme()
{
    OfficeStyle::Theme theme = m_ribbonStyle->getTheme();

    QString strTheme = "Office2007Blue";
    if (theme == OfficeStyle::Office2007Black)
        strTheme = "Office2007Black";
    else if (theme == OfficeStyle::Office2007Silver)
        strTheme = "Office2007Silver";
    else if (theme == OfficeStyle::Office2007Aqua)
        strTheme = "Office2007Aqua";
    else if (theme == OfficeStyle::Windows7Scenic)
        strTheme = "Windows7Scenic";
    else if (theme == OfficeStyle::Office2010Silver)
        strTheme = "Office2010Silver";
    else if (theme == OfficeStyle::Office2010Blue)
        strTheme = "Office2010Blue";
    else if (theme == OfficeStyle::Office2010Black)
        strTheme = "Office2010Black";
    else if (theme == OfficeStyle::Office2013White)
        strTheme = "Office2013White";
    else if (theme == OfficeStyle::Office2013Gray)
        strTheme = "Office2013Gray";
    else if (theme == OfficeStyle::Office2013Dark)
        strTheme = "Office2013Dark";
    else if (theme == OfficeStyle::Office2016Colorful)
        strTheme = "Office2016Colorful";
    else if (theme == OfficeStyle::Office2016White)
        strTheme = "Office2016White";
    else if (theme == OfficeStyle::Office2016DarkGray)
        strTheme = "Office2016DarkGray";
    else if (theme == OfficeStyle::Office2016Black)
        strTheme = "Office2016Black";

    QList<QAction*> list = m_styleActions->actions();
    for (int i = 0, count = list.count(); count > i; i++)
        list[i]->setChecked(list[i]->objectName() == strTheme);
}

void RibbonWindow::includingAnimation(bool checked)
{
    if (Qtitan::OfficeStyle* st = qobject_cast<Qtitan::OfficeStyle*>(qApp->style()))
        st->setAnimationEnabled(checked);
}

void RibbonWindow::setTitleGroupsVisible(bool checked)
{
    ribbonBar()->setTitleGroupsVisible(checked);
}

void RibbonWindow::maximizeToggle()
{
    ribbonBar()->setMinimized(!ribbonBar()->isMinimized());
}

void RibbonWindow::minimizationChangedIcon(bool minimized)
{
    m_actionRibbonMinimize->setChecked(minimized);
    m_actionRibbonMinimizeMenu->setChecked(minimized);
    m_actionRibbonMinimize->setIcon(minimized ? QIcon(":/shared/res/ribbonMaximize.png") :  
        QIcon(":/shared/res/ribbonMinimize.png"));
}

void RibbonWindow::optionsFont(QAction* act)
{
    QFont fnt = m_ribbonStyle->font(ribbonBar());
    int psz = fnt.pointSize();

    if (m_actionNormal == act)
        fnt.setPointSize(psz);
    else if (m_actionLarge == act)
        fnt.setPointSize(psz + 3);
    else if (m_actionExLarge == act)
        fnt.setPointSize(psz + 5);

    ribbonBar()->setFont(fnt);
}

void RibbonWindow::setDPIToggled(bool on)
{
    m_ribbonStyle->setDPIAware(on);

    QAction* actChecked =  m_actionDefault;
    if (m_actionNormal->isChecked())
        actChecked = m_actionNormal;
    if (m_actionLarge->isChecked())
        actChecked = m_actionLarge;
    if (m_actionExLarge->isChecked())
        actChecked = m_actionExLarge;

    optionsFont(actChecked);
}

void RibbonWindow::setNativeScrollBarsToggled(bool on)
{
    m_ribbonStyle->setScrollBarsIgnored(on);
}

void RibbonWindow::setNativeDialogsToggled(bool on)
{
    m_ribbonStyle->setDialogsIgnored(on);
}

void RibbonWindow::showQuickAccessCustomizeMenu(QMenu* menu)
{
    menu->addSeparator()->setText(tr("Common Customization"));
    menu->addAction(ribbonBar()->quickAccessBarPosition() == RibbonBar::QATopPosition ? m_actionShowBelowRibbon : m_actionShowAboveRibbon);
    menu->addSeparator();
    menu->addAction(m_actionRibbonMinimizeMenu);
}

void RibbonWindow::switchQuickAccessBarPosition()
{
    ribbonBar()->setQuickAccessBarPosition(ribbonBar()->quickAccessBarPosition() == RibbonBar::QATopPosition ? 
        RibbonBar::QABottomPosition : RibbonBar::QATopPosition);
}

void RibbonWindow::showRibbonContextMenu(QMenu* menu, QContextMenuEvent* event)
{
    Q_UNUSED(menu)
    event->setAccepted(false);
}

#ifdef Q_OS_WIN
void RibbonWindow::frameTheme(bool checked)
{
    ribbonBar()->setFrameThemeEnabled(checked);
}
#endif // Q_WIN

