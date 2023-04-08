#include "ribbonwindow.h"
#include "aboutdialog.h"
#include "qribbon/ribbon_def.h"
#include <QApplication>
#include <QDesktopWidget>

#define Font_Default    "System Default"
#define Font_Normal     "Normal"
#define Font_Large      "Large"
#define Font_ExLarge    "Extra Large"
#define DPI_100_percent "DPI 100%"

/* RibbonWindow */
RibbonWindow::RibbonWindow(QWidget *parent)
    : QRibbon::RibbonMainWindow(parent)
    , m_ribbonStyle(qobject_cast<QRibbon::RibbonStyle *>(qApp->style()))
{
    // m_ribbonStyle->setDPIAware(false);
    RibbonToolTip::setWrapMode(RibbonToolTip::NativeWrap);
    createOptions();

    QAction *actionAbout = ribbonBar()->addAction(QIcon(":/shared/res/about.png"), tr("About"), Qt::ToolButtonIconOnly);
    actionAbout->setToolTip(tr("Display program<br />information, version number and copyright"));
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    m_actionRibbonMinimize = ribbonBar()->addAction(QIcon(":/shared/res/ribbonMinimize.png"), tr("Minimize the Ribbon"),
                                                    Qt::ToolButtonIconOnly);
    m_actionRibbonMinimize->setStatusTip(tr("Show only the tab names on the Ribbon"));
    m_actionRibbonMinimize->setShortcut(tr("Ctrl+F1"));
    connect(m_actionRibbonMinimize, SIGNAL(triggered()), this, SLOT(maximizeToggle()));
    connect(ribbonBar(), SIGNAL(minimizationChanged(bool)), this, SLOT(minimizationChangedIcon(bool)));

    m_actionRibbonMinimizeMenu = new QAction(tr("Minimize the Ribbon"), this);
    m_actionRibbonMinimizeMenu->setCheckable(true);
    connect(m_actionRibbonMinimizeMenu, SIGNAL(triggered()), this, SLOT(maximizeToggle()));
    connect((QWidget *)ribbonBar()->quickAccessBar(), SIGNAL(showCustomizeMenu(QMenu *)), this,
            SLOT(showQuickAccessCustomizeMenu(QMenu *)));
    connect(ribbonBar(), SIGNAL(showRibbonContextMenu(QMenu *, QContextMenuEvent *)), this,
            SLOT(showRibbonContextMenu(QMenu *, QContextMenuEvent *)));
    updateActionsTheme();
}

void RibbonWindow::createOptions()
{
    QRibbon::RibbonStyle::Theme themeId = QRibbon::RibbonStyle::Office2013White;
    if (m_ribbonStyle)
        themeId = m_ribbonStyle->getTheme();

    m_menuOptions = ribbonBar()->addMenu(tr("Options"));
    QAction *actionStyle = m_menuOptions->addAction(tr("Style"));

    QMenu *menuStyle = new QMenu(ribbonBar());
    m_styleActions = new QActionGroup(this);

    QMenu *menuStyle2013 = new QMenu(tr("Office 2013"), this);
    QAction *action2013White = menuStyle2013->addAction(tr("White"));
    action2013White->setObjectName("Office2013White");
    action2013White->setCheckable(true);
    action2013White->setChecked(themeId == RibbonStyle::Office2013White);

    QAction *action2013Gray = menuStyle2013->addAction(tr("Light Gray"));
    action2013Gray->setObjectName("Office2013Gray");
    action2013Gray->setCheckable(true);
    action2013Gray->setChecked(themeId == RibbonStyle::Office2013Gray);
    menuStyle->addMenu(menuStyle2013);

    QAction *action2013Dark = menuStyle2013->addAction(tr("Dark Gray"));
    action2013Dark->setObjectName("Office2013Dark");
    action2013Dark->setCheckable(true);
    action2013Dark->setChecked(themeId == RibbonStyle::Office2013Dark);
    menuStyle->addMenu(menuStyle2013);

    menuStyle->addSeparator();

    m_styleActions->addAction(action2013White);
    m_styleActions->addAction(action2013Gray);
    m_styleActions->addAction(action2013Dark);

    actionStyle->setMenu(menuStyle);
    connect(m_styleActions, SIGNAL(triggered(QAction *)), this, SLOT(optionsTheme(QAction *)));

    m_menuOptions->addSeparator();

    QAction *actionAnimation = m_menuOptions->addAction(tr("Animation"));
    actionAnimation->setCheckable(true);
    actionAnimation->setChecked(true);
    connect(actionAnimation, SIGNAL(triggered(bool)), this, SLOT(includingAnimation(bool)));

    m_menuOptions->addSeparator();

    QAction *actionTitleGroupsVisible = m_menuOptions->addAction(tr("Title Groups Visible"));
    actionTitleGroupsVisible->setCheckable(true);
    actionTitleGroupsVisible->setChecked(true);
    connect(actionTitleGroupsVisible, SIGNAL(triggered(bool)), this, SLOT(setTitleGroupsVisible(bool)));

#ifdef Q_WIN
    QAction *actionFrameTheme = m_menuOptions->addAction(tr("Frame Theme"));
    actionFrameTheme->setCheckable(true);
    actionFrameTheme->setChecked(true);
    connect(actionFrameTheme, SIGNAL(triggered(bool)), this, SLOT(frameTheme(bool)));
#endif   // Q_WIN

    m_menuOptions->addSeparator();

    QAction *actionMenu = m_menuOptions->addAction(tr("Font"));

    QMenu *menuFont = new QMenu(ribbonBar());
    QActionGroup *fontActions = new QActionGroup(this);

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
    connect(fontActions, SIGNAL(triggered(QAction *)), this, SLOT(optionsFont(QAction *)));

    menuFont->addSeparator();
    QAction *actionDPI_100_percent = menuFont->addAction(tr("DPI Aware (new)"));
    actionDPI_100_percent->setObjectName(DPI_100_percent);
    actionDPI_100_percent->setCheckable(true);
    actionDPI_100_percent->setChecked(m_ribbonStyle->isDPIAware());
    connect(actionDPI_100_percent, SIGNAL(toggled(bool)), this, SLOT(setDPIToggled(bool)));

    m_menuOptions->addSeparator();
    QAction *actionNativeScrollBars = m_menuOptions->addAction(tr("Native Scroll Bars (new)"));
    actionNativeScrollBars->setCheckable(true);
#ifdef Q_OS_MAC
    m_ribbonStyle->setScrollBarsIgnored(true);
#endif
    actionNativeScrollBars->setChecked(m_ribbonStyle->isScrollBarsIgnored());
    connect(actionNativeScrollBars, SIGNAL(toggled(bool)), this, SLOT(setNativeScrollBarsToggled(bool)));

    QAction *actionNativeDialogs = m_menuOptions->addAction(tr("Native Dialogs"));
    actionNativeDialogs->setCheckable(true);
    actionNativeDialogs->setChecked(m_ribbonStyle->isDialogsIgnored());
    connect(actionNativeDialogs, SIGNAL(toggled(bool)), this, SLOT(setNativeDialogsToggled(bool)));
    m_menuOptions->addSeparator();

    QAction *actionCusomize = m_menuOptions->addAction(tr("Cusomize..."));
    actionCusomize->setEnabled(false);

    m_actionShowBelowRibbon = new QAction(tr("Show Below the Ribbon"), this);
    m_actionShowAboveRibbon = new QAction(tr("Show Above the Ribbon"), this);
    connect(m_actionShowBelowRibbon, SIGNAL(triggered()), this, SLOT(switchQuickAccessBarPosition()));
    connect(m_actionShowAboveRibbon, SIGNAL(triggered()), this, SLOT(switchQuickAccessBarPosition()));
}

void RibbonWindow::about()
{
    QRibbon::AboutDialog::show(this, tr("About QRibbon"), tr("QRibbon"), QLatin1String(getRibbonVersion()));
}

void RibbonWindow::optionsDialog() {}

void RibbonWindow::optionsTheme(QAction *action)
{
    QRibbon::RibbonStyle::Theme themeId = QRibbon::RibbonStyle::Office2013White;
    if (action->objectName() == "Office2013White")
        themeId = OfficeStyle::Office2013White;
    else if (action->objectName() == "Office2013Gray")
        themeId = OfficeStyle::Office2013Gray;
    else if (action->objectName() == "Office2013Dark")
        themeId = OfficeStyle::Office2013Dark;

    setStyleTheme(themeId);
}

RibbonStyle::Theme RibbonWindow::styleTheme() const
{
    if (m_ribbonStyle == Q_NULL) {
        Q_ASSERT(false);
        return (RibbonStyle::Theme)-1;
    }
    return m_ribbonStyle->getTheme();
}

void RibbonWindow::setStyleTheme(RibbonStyle::Theme themeId)
{
    if (m_ribbonStyle == Q_NULL) {
        Q_ASSERT(false);
        return;
    }
    setButtonStyleTheme(themeId);
    m_ribbonStyle->setTheme(themeId);
    updateActionsTheme();
}

void RibbonWindow::setButtonStyleTheme(RibbonStyle::Theme themeId)
{
    if (QToolButton *button = ribbonBar()->getSystemButton()) {
        if (themeId == OfficeStyle::Office2013White || themeId == OfficeStyle::Office2013Gray ||
            themeId == OfficeStyle::Office2013Dark)
            button->setToolButtonStyle(Qt::ToolButtonTextOnly);
        else
            button->setToolButtonStyle(Qt::ToolButtonFollowStyle);
    }
}

void RibbonWindow::updateActionsTheme()
{
    OfficeStyle::Theme theme = m_ribbonStyle->getTheme();

    QString strTheme = "Office2013White";
    if (theme == OfficeStyle::Office2013White)
        strTheme = "Office2013White";
    else if (theme == OfficeStyle::Office2013Gray)
        strTheme = "Office2013Gray";
    else if (theme == OfficeStyle::Office2013Dark)
        strTheme = "Office2013Dark";

    QList<QAction *> list = m_styleActions->actions();
    for (int i = 0, count = list.count(); count > i; i++)
        list[i]->setChecked(list[i]->objectName() == strTheme);
}

void RibbonWindow::includingAnimation(bool checked)
{
    if (QRibbon::OfficeStyle *st = qobject_cast<QRibbon::OfficeStyle *>(qApp->style()))
        st->setAnimationEnabled(checked);
}

void RibbonWindow::setTitleGroupsVisible(bool checked) { ribbonBar()->setTitleGroupsVisible(checked); }

void RibbonWindow::maximizeToggle() { ribbonBar()->setMinimized(!ribbonBar()->isMinimized()); }

void RibbonWindow::minimizationChangedIcon(bool minimized)
{
    m_actionRibbonMinimize->setChecked(minimized);
    m_actionRibbonMinimizeMenu->setChecked(minimized);
    m_actionRibbonMinimize->setIcon(minimized ? QIcon(":/shared/res/ribbonMaximize.png")
                                              : QIcon(":/shared/res/ribbonMinimize.png"));
}

void RibbonWindow::optionsFont(QAction *act)
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

    QAction *actChecked = m_actionDefault;
    if (m_actionNormal->isChecked())
        actChecked = m_actionNormal;
    if (m_actionLarge->isChecked())
        actChecked = m_actionLarge;
    if (m_actionExLarge->isChecked())
        actChecked = m_actionExLarge;

    optionsFont(actChecked);
}

void RibbonWindow::setNativeScrollBarsToggled(bool on) { m_ribbonStyle->setScrollBarsIgnored(on); }

void RibbonWindow::setNativeDialogsToggled(bool on) { m_ribbonStyle->setDialogsIgnored(on); }

void RibbonWindow::showQuickAccessCustomizeMenu(QMenu *menu)
{
    menu->addSeparator()->setText(tr("Common Customization"));
    menu->addAction(ribbonBar()->quickAccessBarPosition() == RibbonBar::QATopPosition ? m_actionShowBelowRibbon
                                                                                      : m_actionShowAboveRibbon);
    menu->addSeparator();
    menu->addAction(m_actionRibbonMinimizeMenu);
}

void RibbonWindow::switchQuickAccessBarPosition()
{
    ribbonBar()->setQuickAccessBarPosition(ribbonBar()->quickAccessBarPosition() == RibbonBar::QATopPosition
                                               ? RibbonBar::QABottomPosition
                                               : RibbonBar::QATopPosition);
}

void RibbonWindow::showRibbonContextMenu(QMenu *menu, QContextMenuEvent *event)
{
    Q_UNUSED(menu)
    event->setAccepted(false);
}

#ifdef Q_OS_WIN
void RibbonWindow::frameTheme(bool checked) { ribbonBar()->setFrameThemeEnabled(checked); }
#endif   // Q_WIN
