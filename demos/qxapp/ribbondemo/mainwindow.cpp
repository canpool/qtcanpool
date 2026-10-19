/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "mainwindow.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonbutton.h"
#include "qxribbon/ribbonbuttongroup.h"
#include "qxribbon/ribbonmenu.h"
#include "qxribbon/ribboncustomizedialog.h"
#include "qxribbon/ribbonactionsmanager.h"
#include "qxribbon/ribbonmanager.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbonquickaccessbar.h"
#include "qxribbon/ribbonbackstageview.h"
#include "qxribbon/ribbonstatusbar.h"

#include "applypage.h"
#include "contextpage1.h"
#include "contextpage2.h"
#include "custompage.h"
#include "homepage.h"
#include "otherpage.h"
#include "qxtheme/qxtheme.h"
#include "qxtheme/qxthemeicon.h"
#include "qxtheme/qxthemepalette.h"

#include <QAbstractButton>
#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QFile>
#include <QFileDialog>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPalette>
#include <QPushButton>
#include <QSize>
#include <QToolButton>
#include <QCheckBox>
#include <QComboBox>
#include <QStatusBar>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QTextStream>
#include <QXmlStreamWriter>
#include <QDialog>
#include <QActionGroup>

#include "aboutdialog.h"

QX_RIBBON_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

#define COST_START()                                                                                                   \
    QElapsedTimer costTimer;                                                                                           \
    costTimer.start();                                                                                                 \
    int costLast = 0

#define COST_PRINT(STR)                                                                                                \
    do {                                                                                                               \
        int costNow = costTimer.elapsed();                                                                             \
        qDebug() << STR << " cost " << costNow - costLast << " ms (" << costNow << ")";                                \
        if (!m_screenshotMode) {                                                                                       \
            m_edit->append(QString("%1 cost %2 ms(%3)").arg(STR).arg(costNow - costLast).arg(costNow));                \
        }                                                                                                              \
        costLast = costNow;                                                                                            \
    } while (0)

namespace
{

struct DemoTheme {
    QString id;
    QString name;
    bool dark;   // Drives the context page tab; see MainWindow::applyTheme().
    QString file;
};

/*
 * The themes this demo offers, and they are the demo's own to offer: the ribbon
 * library ships no style sheet of its own, so what a ribbon looks like is decided
 * here. Every family comes in a light and a dark scheme, all of them from
 * res/themes.qrc - which this demo builds in like any other application that
 * wants the same set.
 */
QVector<DemoTheme> demoThemes()
{
    return {
        {QStringLiteral("soft-light"), MainWindow::tr("SoftLight"), false, QStringLiteral(":/themes/soft_light.css")},
        {QStringLiteral("soft-dark"), MainWindow::tr("SoftDark"), true, QStringLiteral(":/themes/soft_dark.css")},
        {QStringLiteral("classic-light"), MainWindow::tr("ClassicLight"), false,
         QStringLiteral(":/themes/classic_light.css")},
        {QStringLiteral("classic-dark"), MainWindow::tr("ClassicDark"), true,
         QStringLiteral(":/themes/classic_dark.css")},
        {QStringLiteral("flat-light"), MainWindow::tr("FlatLight"), false, QStringLiteral(":/themes/flat_light.css")},
        {QStringLiteral("flat-dark"), MainWindow::tr("FlatDark"), true, QStringLiteral(":/themes/flat_dark.css")},
        {QStringLiteral("slate-light"), MainWindow::tr("SlateLight"), false,
         QStringLiteral(":/themes/slate_light.css")},
        {QStringLiteral("slate-dark"), MainWindow::tr("SlateDark"), true, QStringLiteral(":/themes/slate_dark.css")},
        {QStringLiteral("accent-light"), MainWindow::tr("AccentLight"), false,
         QStringLiteral(":/themes/accent_light.css")},
        {QStringLiteral("accent-dark"), MainWindow::tr("AccentDark"), true, QStringLiteral(":/themes/accent_dark.css")},
        {QStringLiteral("fluent-light"), MainWindow::tr("FluentLight"), false,
         QStringLiteral(":/themes/fluent_light.css")},
        {QStringLiteral("fluent-dark"), MainWindow::tr("FluentDark"), true, QStringLiteral(":/themes/fluent_dark.css")},
        {QStringLiteral("material-light"), MainWindow::tr("MaterialLight"), false,
         QStringLiteral(":/themes/material_light.css")},
        {QStringLiteral("material-dark"), MainWindow::tr("MaterialDark"), true,
         QStringLiteral(":/themes/material_dark.css")},
    };
}

}   // namespace

MainWindow::MainWindow(QWidget *par, bool screenshotMode)
    : MyRibbonWindow(par)
    , m_screenshotMode(screenshotMode)
{
    setWindowTitle(tr("ribbon mainwindow demo"));
    setWindowIcon(QIcon(":/icon/res/logo.svg"));
    // It needs to be consistent with the border size in qss
    // setContentsMargins(1, 1, 1, 1);

    m_customizeXml = qApp->applicationDirPath() + QDir::separator() + "customize.xml";
    // The set every page hands its glyphs to, so a theme change repaints them.
    m_icons = new QxThemeIcons(this);

    COST_START();

    createCentralWidget();
    COST_PRINT("createCentralWidget");
    createStatusBar();
    COST_PRINT("createStatusBar");
    createRibbon();
    COST_PRINT("createRibbon");

    setMinimumWidth(500);

    // The default theme goes on here, in the constructor, instead of on the first turn of
    // the event loop where it used to wait. Three things are installed by it - the color
    // scheme, the palette and the sheet - and all three describe how the window is drawn,
    // so installing them after show() meant one frame in the system's colors (a dark frame
    // around a light theme) and a ribbon laid out from the hints it had before the sheet
    // landed: with the windows11 style that alone left the quick access bar three pixels
    // short of its own size hint, and the bar clips what does not fit, so one button
    // disappeared from the strip.
    changeRibbonTheme(0);

    qDebug() << RibbonElementStyleOpt;

#if defined(Q_OS_LINUX)
    // The inline tab bar is the layout that fits Linux window chrome. Ask the bar;
    // the Home page's radios follow through RibbonBar::ribbonStyleChanged.
    ribbonBar()->setRibbonStyle(RibbonBar::TabBarInlineStyle);
#endif

    QxRibbonCustomizeApplyFromXmlFile(m_customizeXml, ribbonBar(), m_actMgr);
}

MainWindow::~MainWindow()
{
    // The backstage answers its host's teardown like any other child: it is hidden with
    // the window, which emits aboutToHide into the status bar - already gone by then. Cutting
    // the links first keeps the hints from firing into a half-destroyed window.
    if (m_backstage) {
        disconnect(m_backstage, nullptr, this, nullptr);
    }
}

void MainWindow::applyTheme(const QString &themeId)
{
    const QVector<DemoTheme> themes = demoThemes();
    for (const DemoTheme &theme : themes) {
        if (theme.id != themeId) {
            continue;
        }
        QFile file(theme.file);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return;
        }
        // Three things have to happen and their order matters. The color scheme first,
        // because it is what the platform reads for the parts it draws itself: the
        // windows11 style on Windows 11 paints from a light or dark table chosen by the
        // scheme and never looks at the palette, and the window frame follows the scheme
        // too - a light theme under a dark system would otherwise come out with a dark
        // title bar and dark scroll bars. Then one sheet for the whole application,
        // because a sheet on the window would win over the application's wherever both
        // describe the same widget - which is how the ribbon used to stay light while the
        // rest went dark. And the matching palette, because a style sheet cannot repaint
        // an icon: every glyph would keep the colour it was drawn in, black on a dark
        // ribbon.
        //
        // The palette goes in *before* the sheet. An application style sheet makes Qt
        // hand each widget a palette of its own, and from then on application palette
        // changes no longer reach those widgets; installing the sheet is also what
        // re-polishes them, so it has to be the last thing to land. The other way round
        // left the editor and every other styled control in the previous theme.
        QxTheme::applyColorScheme(theme.dark);
        qApp->setPalette(QxTheme::paletteFor(theme.dark));
        qApp->setStyleSheet(QString::fromUtf8(file.readAll()));
        m_icons->refresh();
        // A dark ribbon needs the context page to leave its tab alone; a light one
        // draws over it. Office 2016 Blue is dark enough to count.
        ribbonBar()->setPageContextCoverTab(!theme.dark);
        return;
    }
}

void MainWindow::createCentralWidget()
{
    m_edit = new QTextEdit(this);
    m_edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_edit->setLineWrapMode(QTextEdit::NoWrap);
    setCentralWidget(m_edit);
}

void MainWindow::createStatusBar()
{
    // The ribbon-styled status bar: plain-text segments on the left, a view
    // switch on the right. The segments are driven by their actions' text, so the
    // demo keeps them up to date the way it would for any other action, and the
    // switch stays put while showMessage() scrolls its text across the left.
    RibbonStatusBar *bar = new RibbonStatusBar();
    setStatusBar(bar);

    QAction *document = bar->addAction(tr("Ready"));
    bar->addSeparator();
    QAction *position = bar->addAction(tr("Ln 1, Col 1"));

    QAction *viewSwitch = createAction(tr("Toggle view"), QStringLiteral(":/icon/res/tabler/eye.svg"));
    viewSwitch->setCheckable(true);
    bar->addPermanentAction(viewSwitch);

    connect(viewSwitch, &QAction::triggered, this, [this, document](bool on) {
        document->setText(on ? tr("View: switched") : tr("Ready"));
        m_edit->append(tr("View switched: %1").arg(on ? tr("on") : tr("off")));
    });
    connect(m_edit, &QTextEdit::cursorPositionChanged, this, [this, position] {
        QTextCursor cursor = m_edit->textCursor();
        position->setText(tr("Ln %1, Col %2").arg(cursor.blockNumber() + 1).arg(cursor.positionInBlock() + 1));
    });
}

void MainWindow::createRibbon()
{
    COST_START();

    ribbonBar()->setContentsMargins(0, 0, 0, 0);

    // The "Home" page builds itself; the window only wires the few things that
    // leave the page - the context tabs it owns, and the lines logged to the editor.
    auto *homePage = new HomePage(m_icons, this);
    connect(homePage, &HomePage::contextPageVisibleRequested, this, [this](bool on) {
        // The first context tab can be torn down from the page, so it is rebuilt
        // lazily here if it is gone.
        if (m_pageContext == nullptr) {
            createPageContext1();
        }
        if (on) {
            ribbonBar()->showPageContext(m_pageContext);
        } else {
            ribbonBar()->hidePageContext(m_pageContext);
        }
    });
    connect(homePage, &HomePage::contextPageDestroyRequested, this, [this]() {
        if (m_pageContext) {
            ribbonBar()->destroyPageContext(m_pageContext);
            m_pageContext = nullptr;
        }
    });
    connect(homePage, &HomePage::context2VisibleRequested, this, [this](bool on) {
        if (m_pageContext2) {
            ribbonBar()->setPageContextVisible(m_pageContext2, on);
        }
    });
    connect(homePage, &HomePage::messageRequested, this, [this](const QString &message) {
        if (m_edit) {
            m_edit->append(message);
        }
    });
    ribbonBar()->addPage(homePage);
    COST_PRINT("createPageHome");
    auto *otherPage = new OtherPage(m_icons, this);
    connect(otherPage, &OtherPage::messageRequested, this, [this](const QString &message) {
        if (m_edit) {
            m_edit->append(message);
        }
    });
    ribbonBar()->addPage(otherPage);
    COST_PRINT("createPageOther");
    createPageCustom();
    COST_PRINT("createPageCustom");
    createPageApply();
    COST_PRINT("createPageApply");
    createPageContext1();
    COST_PRINT("createPageContext1");
    createPageContext2();
    COST_PRINT("createPageContext2");
    createQuickAccessBar();
    COST_PRINT("createQuickAccessBar");
    createRightButtonGroup();
    COST_PRINT("createRightButtonGroup");
    createApplicationButton();
    COST_PRINT("createApplicationButton");

    addSomeOtherAction();
    COST_PRINT("addSomeOtherAction");
}

/**
 * @brief Builds the "Custom" page.
 *
 * The page builds itself; the window only connects what it wants to hear about.
 */
void MainWindow::createPageCustom()
{
    auto *page = new CustomPage(m_icons, this);
    connect(page, &CustomPage::useQssRequested, this, &MainWindow::onActionUseQssTriggered);
    connect(page, &CustomPage::loadCustomizeRequested, this, &MainWindow::onActionLoadCustomizeXmlFileTriggered);
    ribbonBar()->addPage(page);
}

/**
 * @brief Builds the "Apply" page.
 *
 * The page builds itself; the window only connects what it wants to hear about.
 */
void MainWindow::createPageApply()
{
    auto *page = new ApplyPage(m_icons, this);
    ribbonBar()->addPage(page);

    connect(page, &ApplyPage::fontSelected, this, &MainWindow::onFontComWidgetCurrentFontChanged);
    connect(page, &ApplyPage::largerFontRequested, this, &MainWindow::onActionFontLargerTriggered);
    connect(page, &ApplyPage::smallerFontRequested, this, &MainWindow::onActionFontSmallerTriggered);
}

/**
 * @brief Creates the context pages.
 */
void MainWindow::createPageContext1()
{
    RibbonBar *ribbon = ribbonBar();
    m_pageContext = ribbon->addPageContext(tr("context"), QColor());

    auto *contextPage1 = new ContextPage1(m_icons, this);
    contextPage1->setPageName(tr("Page1"));
    m_pageContext->addPage(contextPage1);

    auto *contextPage2 = new ContextPage2(m_icons, this);
    contextPage2->setPageName(tr("Page2"));
    m_pageContext->addPage(contextPage2);
}

void MainWindow::createPageContext2()
{
    RibbonBar *ribbon = ribbonBar();
    m_pageContext2 = ribbon->addPageContext(tr("context2"), QColor());
    m_pageContext2->addPage(tr("context2 Page1"));
    m_pageContext2->addPage(tr("context2 Page2"));
}

void MainWindow::createQuickAccessBar()
{
    RibbonQuickAccessBar *quickAccessBar = ribbonBar()->quickAccessBar();
    connect(quickAccessBar, &RibbonQuickAccessBar::customizeActionChanged, this, [this]() {
        this->ribbonBar()->resizeRibbon();
    });
    // The customize menu's trailing entry opens the same customization dialog as the
    // "customize and save" button (B8).
    connect(quickAccessBar, &RibbonQuickAccessBar::customizeDialogRequested, this, [this]() {
        onActionCustomizeAndSaveTriggered(false);
    });
    QAction *saveQab = createAction(tr("save"), QStringLiteral(":/icon/res/tabler/device-floppy.svg"), "save-quickbar");
    saveQab->setStatusTip(tr("Save the current document to disk"));
    quickAccessBar->addAction(saveQab);
    quickAccessBar->addAction(createAction(tr("printer"), QStringLiteral(":/icon/res/tabler/printer.svg")));

    QAction *undoQab = createAction(tr("undo"), QStringLiteral(":/icon/res/tabler/arrow-back-up.svg"));
    undoQab->setStatusTip(tr("Undo the last action"));
    quickAccessBar->addAction(undoQab);
    QAction *redoQab = createAction(tr("redo"), QStringLiteral(":/icon/res/tabler/arrow-forward-up.svg"));
    redoQab->setStatusTip(tr("Redo the last undone action"));
    quickAccessBar->addAction(redoQab);

    QAction *menuAction = createAction(tr("file"), QStringLiteral(":/icon/res/tabler/file.svg"));
    RibbonMenu *m = new RibbonMenu(tr("file"), this);
    for (int i = 0; i < 10; ++i) {
        m->addAction(createAction(tr("file%1").arg(i + 1), QStringLiteral(":/icon/res/tabler/file.svg")));
    }
    menuAction->setMenu(m);
    quickAccessBar->addAction(menuAction);

    QAction *actionCustomizeAndSave =
        createAction(tr("customize and save"), QStringLiteral(":/icon/res/tabler/adjustments.svg"));
    quickAccessBar->addAction(actionCustomizeAndSave);
    connect(actionCustomizeAndSave, &QAction::triggered, this, &MainWindow::onActionCustomizeAndSaveTriggered);

    QAction *otherAction = new QAction(tr("other"), this);
    connect(quickAccessBar, &RibbonQuickAccessBar::showCustomizeMenu, this, [otherAction](QMenu *menu) {
        menu->addSeparator();
        menu->addAction(otherAction);
    });
}

void MainWindow::createRightButtonGroup()
{
    RibbonButtonGroup *rightBar = ribbonBar()->cornerButtonGroup(Qt::BottomRightCorner);
    m_themeGroup = new QActionGroup(this);
    QAction *actionTheme = createAction(tr("theme"), QStringLiteral(":/icon/res/tabler/palette.svg"));
    RibbonMenu *menu = new RibbonMenu(tr("theme"), this);
    actionTheme->setMenu(menu);
    // One entry per theme the demo offers, in the order they are listed above.
    for (const DemoTheme &theme : demoThemes()) {
        m_themeGroup->addAction(addThemeAction(menu->addAction(theme.name), theme.id));
    }
    rightBar->addAction(actionTheme);

    QAction *actionHelp = createAction(tr("help"), QStringLiteral(":/icon/res/tabler/help-circle.svg"));
    connect(actionHelp, &QAction::triggered, this, &MainWindow::onActionHelpTriggered);
    rightBar->addAction(actionHelp);

    RibbonButtonGroup *topLeftBar = ribbonBar()->cornerButtonGroup(Qt::TopLeftCorner);
    QLineEdit *le = new QLineEdit(this);
    le->setPlaceholderText(tr("Search..."));
    topLeftBar->addWidget(le);

    RibbonButtonGroup *bottomLeftBar = ribbonBar()->cornerButtonGroup(Qt::BottomLeftCorner);
    QMenu *m1 = new QMenu(tr("menu"), this);
    m1->addAction(tr("action1"));
    m1->addAction(tr("action2"));
    QMenu *m2 = m1->addMenu(tr("submenu"));
    m2->addAction(tr("action1"));
    m2->addAction(tr("action2"));
    bottomLeftBar->addMenu(m1);

    bottomLeftBar->addSeparator();

    le = new QLineEdit(this);
    le->setMinimumWidth(150);
    le->setClearButtonEnabled(true);
    le->setPlaceholderText(tr("Search..."));
    bottomLeftBar->addWidget(le);

    RibbonButtonGroup *topRightBar = ribbonBar()->cornerButtonGroup(Qt::TopRightCorner);
    QComboBox *combox = new QComboBox(this);
    combox->setMinimumWidth(100);
    combox->addItems(QStringList() << "apple" << "banana" << "orange");
    topRightBar->addWidget(combox);
}

void MainWindow::createApplicationButton()
{
    // NOTE: the theme stylesheets dress the application button through the
    // property setApplicationButton() maintains, so this button carries the
    // theme's look - pressed included - whatever it is called. Before 4.6 the
    // sheets answered to the ribbon's own object name only, and a named button
    // was left on the platform's press look.
    QToolButton *appBtn = new QToolButton(this);
    appBtn->setAutoRaise(true);
    appBtn->setFocusPolicy(Qt::NoFocus);
    appBtn->setText(tr("&File"));
    appBtn->setIcon(windowIcon());
    appBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    appBtn->setMinimumWidth(40);
    ribbonBar()->setApplicationButton(appBtn);
    // B4: the Backstage view takes over the application-button click (no menu needed).
    createBackstage();
}

void MainWindow::createBackstage()
{
    // B4: the Office-style application menu is replaced by a RibbonBackstageView that covers
    // the window when the application button is clicked. The view wires itself to
    // RibbonBar::applicationButtonClicked, so the button needs no menu of its own.
    RibbonBackstageView *backstage = new RibbonBackstageView(ribbonBar());

    // Each command carries its own glyph now: the palette tint applies to backstage entries
    // too, so they stay readable when the window is dark.
    auto addCommand = [this, backstage](const QString &glyph, const QString &text) {
        QAction *action = backstage->addAction(QIcon(), text);
        m_icons->add(action, QStringLiteral(":/icon/res/tabler/%1.svg").arg(glyph), QPalette::ButtonText,
                     QSize(24, 24));
        return action;
    };

    QAction *saveAction = addCommand(QStringLiteral("device-floppy"), tr("Save"));
    QAction *openAction = addCommand(QStringLiteral("folder-open"), tr("Open"));
    backstage->addSeparator();
    addCommand(QStringLiteral("printer"), tr("Print"));
    backstage->addSeparator();
    QAction *exitAction = addCommand(QStringLiteral("logout"), tr("Exit"));

    // Pages are arbitrary widgets; the view reparents the active one and sizes it to the
    // area right of the command column.
    QLabel *infoPage = new QLabel(tr("The Info page.\n\n"
                                     "A page is any widget the host hands over: the view reparents it, "
                                     "keeps it hidden while another page is up, and sizes it to the area "
                                     "right of the column."));
    infoPage->setWordWrap(true);
    infoPage->setContentsMargins(16, 16, 16, 16);
    infoPage->setWindowTitle(tr("Info"));
    backstage->addPage(infoPage);

    QWidget *optionsPage = new QWidget();
    optionsPage->setWindowTitle(tr("Options"));
    QVBoxLayout *optionsLayout = new QVBoxLayout(optionsPage);
    QCheckBox *preventClose = new QCheckBox(tr("Prevent the backstage from closing"));
    optionsLayout->addWidget(preventClose);
    optionsLayout->addStretch();
    backstage->addPage(optionsPage);

    QLabel *aboutPage = new QLabel(tr("The About page.\n\n"
                                      "Clicking an entry in the column shows its page and checks the "
                                      "entry; the one before it is hidden and unchecked."));
    aboutPage->setWordWrap(true);
    aboutPage->setContentsMargins(16, 16, 16, 16);
    aboutPage->setWindowTitle(tr("About"));
    backstage->addPage(aboutPage);

    backstage->setActivePage(infoPage);

    connect(saveAction, &QAction::triggered, this, [this] {
        m_edit->append(tr("Save triggered"));
    });
    connect(openAction, &QAction::triggered, this, [this] {
        m_edit->append(tr("Open triggered"));
    });
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);
    connect(preventClose, &QCheckBox::toggled, backstage, &RibbonBackstageView::setClosePrevented);
    connect(backstage, &RibbonBackstageView::aboutToShow, this, [this] {
        statusBar()->showMessage(tr("Backstage open"), 3000);
    });
    connect(backstage, &RibbonBackstageView::aboutToHide, this, [this] {
        statusBar()->showMessage(tr("Backstage closed"), 3000);
    });

    m_backstage = backstage;
}

void MainWindow::addSomeOtherAction()
{
    RibbonBar *bar = this->ribbonBar();
    // Actions the ribbon does not manage: they are here for the customize widget to offer.
    QAction *acttext1 = new QAction(tr("text action1"), bar);
    QAction *acttext2 = new QAction(tr("text action2"), bar);
    QAction *acttext3 = new QAction(tr("text action3"), bar);
    QAction *acttext4 = new QAction(tr("text action4"), bar);
    QAction *acttext5 = new QAction(tr("text action5"), bar);

    QAction *actIcon1 = new QAction(QIcon(QStringLiteral(":/icon/res/tabler/file.svg")), tr("action with icon"), bar);

    m_actionTagText = RibbonActionsManager::UserDefineActionTag + 1;
    m_actionTagWithIcon = RibbonActionsManager::UserDefineActionTag + 2;

    m_actMgr = new RibbonActionsManager(bar);   // constructing it registered every action that exists

    // The extra registrations.
    m_actMgr->registerAction(acttext1, RibbonActionsManager::CommonlyUsedActionTag);
    m_actMgr->registerAction(acttext3, RibbonActionsManager::CommonlyUsedActionTag);
    m_actMgr->registerAction(acttext5, RibbonActionsManager::CommonlyUsedActionTag);
    m_actMgr->registerAction(actIcon1, RibbonActionsManager::CommonlyUsedActionTag);

    m_actMgr->registerAction(acttext1, m_actionTagText);
    m_actMgr->registerAction(acttext2, m_actionTagText);
    m_actMgr->registerAction(acttext3, m_actionTagText);
    m_actMgr->registerAction(acttext4, m_actionTagText);
    m_actMgr->registerAction(acttext5, m_actionTagText);

    m_actMgr->registerAction(actIcon1, m_actionTagWithIcon);

    m_actMgr->setTagName(RibbonActionsManager::CommonlyUsedActionTag, tr("in common use"));
    m_actMgr->setTagName(m_actionTagText, tr("no icon action"));
    m_actMgr->setTagName(m_actionTagWithIcon, tr("have icon action"));
}

QAction *MainWindow::createAction(const QString &text, const QString &iconurl, const QString &objName)
{
    QAction *act = new QAction(this);
    act->setText(text);
    if (m_icons) {
        m_icons->add(act, iconurl, QPalette::ButtonText, QSize(24, 24));
    } else {
        act->setIcon(QIcon(iconurl));
    }
    act->setObjectName(objName);
    return act;
}

QAction *MainWindow::createAction(const QString &text, const QString &iconurl)
{
    // The object name is the text when the caller does not name the action itself.
    return createAction(text, iconurl, text);
}

QAction *MainWindow::addThemeAction(QAction *action, const QString &themeId)
{
    action->setCheckable(true);
    action->setData(QVariant(themeId));
    connect(action, &QAction::triggered, this, &MainWindow::onActionChangeThemeTriggered);

    return action;
}

void MainWindow::changeRibbonTheme(int index)
{
    if (m_themeGroup == nullptr) {
        return;
    }
    if (m_themeGroup->actions().count() > index) {
        m_themeGroup->actions().at(index)->trigger();
    } else {
        m_themeGroup->actions().at(0)->trigger();
    }
}

void MainWindow::applyStyleSheet(const QString &styleSheet)
{
    // Same reasoning as in applyTheme(): a sheet picked from a file belongs to the
    // application, not to this window, or it would shadow the application's own.
    qApp->setStyleSheet(styleSheet);
    // NOTE: the theme menu does not reflect the custom theme - there is no "Custom" entry in it, and
    // adding one only to track this state is not worth it for a demo. Known imperfection, not a bug.
}

void MainWindow::onActionCustomizeAndSaveTriggered(bool b)
{
    Q_UNUSED(b);
    RibbonCustomizeDialog dlg(this->ribbonBar(), this);
    dlg.setupActionsManager(m_actMgr);
    dlg.fromXml(m_customizeXml);
    if (RibbonCustomizeDialog::Accepted == dlg.exec()) {
        QByteArray str;
        QXmlStreamWriter xml(&str);
        xml.setAutoFormatting(true);
        xml.setAutoFormattingIndent(2);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)   // QXmlStreamWriter always encodes XML in UTF-8.
        xml.setCodec("utf-8");
#endif
        xml.writeStartDocument();
        bool isOk = dlg.toXml(&xml);
        xml.writeEndDocument();
        if (isOk) {
            QFile f(m_customizeXml);
            if (f.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
                QTextStream s(&f);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)   // QTextStream always encodes XML in UTF-8.
                s.setCodec("utf-8");
#endif
                s << str;
                s.flush();
            }
            m_edit->append("write xml:");
            m_edit->append(QString::fromLatin1(str));
        }
    }
}

void MainWindow::onActionHelpTriggered()
{
    QString url("https://gitee.com/icanpool/qtcanpool");

    AboutDialog ad(QLatin1String(":/icon/res/logo64.svg"), tr("QxRibbon"), QLatin1String(QX_RIBBON_VERSION_STR),
                   tr("Copyleft (C) 2023 maminjie &lt;canpool@163.com&gt;<br/>"
                      "<br/>QxRibbon is a Qx componet like to Microsoft Ribbon style. "
                      "For more information, please visit <a href=\"%1\">%1</a>")
                       .arg(url),
                   this);
    ad.exec();
}

void MainWindow::onActionUseQssTriggered()
{
    QFile f("ribbon.qss");
    if (!f.exists()) {
        QString fdir = QFileDialog::getOpenFileName(this, tr("select stylesheet file"), "", tr("Files (*.qss *.css)"));
        if (fdir.isEmpty()) {
            return;
        }
        f.setFileName(fdir);
    }
    if (!f.open(QIODevice::ReadWrite)) {
        return;
    }
    QString qss(QString::fromLatin1(f.readAll()));
    f.close();
    m_edit->setText(qss);
    applyStyleSheet(qss);
}

void MainWindow::onActionLoadCustomizeXmlFileTriggered()
{
    // Only the first call does anything.
    static bool has_call = false;
    if (!has_call) {
        has_call = QxRibbonCustomizeApplyFromXmlFile("customize.xml", this->ribbonBar(), m_actMgr);
    }
}

void MainWindow::onActionChangeThemeTriggered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        applyTheme(action->data().toString());
    }
}

void MainWindow::onFontComWidgetCurrentFontChanged(const QFont &f)
{
    qDebug() << "set font:" << f;
    ribbonBar()->setFont(f);
    update();
}

void MainWindow::onActionFontLargerTriggered()
{
    QFont f = font();
    f.setPointSize(f.pointSize() + 1);
    ribbonBar()->setFont(f);
    update();
    qDebug() << "set font:" << f;
}

void MainWindow::onActionFontSmallerTriggered()
{
    QFont f = font();
    f.setPointSize(f.pointSize() - 1);
    ribbonBar()->setFont(f);
    update();
    qDebug() << "set font:" << f;
}
