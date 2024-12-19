#include "mainwindow.h"

#include "qxribbon/ribbonapplicationbutton.h"
#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonbutton.h"
#include "qxribbon/ribboncolorbutton.h"
#include "qxribbon/ribbonbuttongroup.h"
#include "qxribbon/ribboncontrols.h"
#include "qxribbon/ribbonmenu.h"
#include "qxribbon/ribboncustomizedialog.h"
#include "qxribbon/ribbonactionsmanager.h"
#include "qxribbon/ribbonmanager.h"
#include "qxribbon/ribbongallery.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribboncontainers.h"
#include "qxribbon/ribbonquickaccessbar.h"
#include "qxribbon/ribbonutils.h"

#include <QAbstractButton>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCalendarWidget>
#include <QDebug>
#include <QElapsedTimer>
#include <QFile>
#include <QFileDialog>
#include <QFontComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QToolButton>
#include <QRadioButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QStatusBar>
#include <QTextEdit>
#include <QTextStream>
#include <QXmlStreamWriter>
#include <QDialog>
#include <QActionGroup>
#include <QMdiArea>
#include <QSplitter>

#include "aboutdialog.h"
#include "editablecontainer.h"

QX_RIBBON_USE_NAMESPACE

#define COST_START()                                                                                                   \
    QElapsedTimer __TMP_COST;                                                                                          \
    __TMP_COST.start();                                                                                                \
    int __TMP_LASTTIMES = 0

#define COST_PRINT(STR)                                                                                                \
    do {                                                                                                               \
        int ___TMP_INT = __TMP_COST.elapsed();                                                                         \
        qDebug() << STR << " cost " << ___TMP_INT - __TMP_LASTTIMES << " ms (" << ___TMP_INT << ")";                   \
        m_edit->append(QString("%1 cost %2 ms(%3)").arg(STR).arg(___TMP_INT - __TMP_LASTTIMES).arg(___TMP_INT));       \
        __TMP_LASTTIMES = ___TMP_INT;                                                                                  \
    } while (0)

#define QXRIBBON_TEST_MDIAREA    0
#define QXRIBBON_TEST_CUSTOMIZE_APPBTN  1

MainWindow::MainWindow(QWidget *par)
    : RibbonMainWindow(par)
    , m_themeGroup(nullptr)
{
    m_customizeXml = qApp->applicationDirPath() + QDir::separator() + "customize.xml";

    COST_START();

    createCentralWidget();
    COST_PRINT("createCentralWidget");
    createStatusBar();
    COST_PRINT("createStatusBar");
    createRibbon();
    COST_PRINT("createRibbon");

    setWindowTitle(tr("ribbon mainwindow demo"));
    setWindowIcon(QIcon(":/icon/res/logo.svg"));
    setMinimumWidth(500);

    changeRibbonTheme(1);

    qDebug() << RibbonElementStyleOpt;

#if defined(Q_OS_LINUX)
    QRadioButton *r = findChild<QRadioButton *>("use wps style");
    if (r) {
        r->click();
    }
    QCheckBox *framelessCB = findChild<QCheckBox *>("use frameless");
    if (framelessCB) {
        framelessCB->click();
    }
#endif

    QxRibbonCustomizeApplyFromXmlFile(m_customizeXml, ribbonBar(), m_actMgr);
}

void MainWindow::setRibbonTheme(int theme)
{
    switch (theme) {
    case Office2016BlueTheme:
        loadTheme(":/theme/res/office2016blue.css");
        break;
    default:
        break;
    }
    RibbonMainWindow::setRibbonTheme(theme);
}

void MainWindow::createCentralWidget()
{
    m_edit = new QTextEdit(this);
    EditableContainer *ec = new EditableContainer(this);
    ec->setWidget(m_edit);
#if QXRIBBON_TEST_MDIAREA
    QMdiArea *area = new QMdiArea(this);
    QTextEdit *te = new QTextEdit(this);
    area->addSubWindow(te);
    QSplitter *splitter = new QSplitter(this);
    splitter->addWidget(ec);
    splitter->addWidget(area);
    splitter->setSizes({400, 200});
    setCentralWidget(splitter);
#else
    setCentralWidget(ec);
#endif
}

void MainWindow::createStatusBar()
{
    setStatusBar(new QStatusBar());
}

void MainWindow::createRibbon()
{
    COST_START();

    ribbonBar()->setContentsMargins(0, 0, 0, 0);

    createPageHome();
    COST_PRINT("createPageHome");
    createPageOther();
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

void MainWindow::createPageHome()
{
    // 通过addPage函数添加标签页
    RibbonPage *page = ribbonBar()->addPage(tr("&Home"));
    page->setObjectName(("pageHome"));

    //! 1
    //! group 1 start
    //!

    // 使用addGroup函数来创建RibbonGroup，效果和new RibbonGroup再addGroup一样
    RibbonGroup *groupStyle = page->addGroup(("ribbon style"));

    QAction *actHideRibbon = createAction(tr("hide ribbon"), ":/icon/res/hideRibbon.svg", "actHideRibbon");
    actHideRibbon->setCheckable(true);
    groupStyle->addLargeAction(actHideRibbon);
    connect(actHideRibbon, &QAction::triggered, this, [this](bool b) {
        this->ribbonBar()->setMinimized(b);
    });
    connect(ribbonBar(), &RibbonBar::minimizationChanged, this, [actHideRibbon](bool minimized) {
        actHideRibbon->setChecked(minimized);
    });

    QAction *actShowMinimumButton =
        createAction(tr("show minimum button"), ":/icon/res/show.svg", "show minimum button");
    actShowMinimumButton->setCheckable(true);
    groupStyle->addSmallAction(actShowMinimumButton);
    connect(actShowMinimumButton, &QAction::triggered, this, [this](bool b) {
        this->ribbonBar()->showMinimumButton(b);   // 显示ribbon最小化按钮
    });
    actShowMinimumButton->trigger();

    QAction *actRemoveAppBtn = createAction(tr("remove application"), ":/icon/res/remove-app-btn.svg");
    actRemoveAppBtn->setCheckable(true);
    groupStyle->addSmallAction(actRemoveAppBtn);
    connect(actRemoveAppBtn, &QAction::toggled, this, &MainWindow::onActionRemoveAppBtnTriggered);

    QAction *actWindowFlagNormalButton = createAction(tr("window maximizable"), ":/icon/res/windowsflag-normal.svg");
    actWindowFlagNormalButton->setCheckable(true);
    actWindowFlagNormalButton->setChecked(true);
    groupStyle->addSmallAction(actWindowFlagNormalButton);
    connect(actWindowFlagNormalButton, &QAction::triggered, this, &MainWindow::onActionWindowFlagNormalButtonTriggered);

    QButtonGroup *g = new QButtonGroup(page);

    QRadioButton *r = new QRadioButton();
    r->setText(tr("use office style"));
    r->setWindowTitle(r->text());
    r->setObjectName(("use office style"));
    r->setChecked(true);
    groupStyle->addSmallWidget(r);
    g->addButton(r, RibbonBar::OfficeStyle);

    r = new QRadioButton();
    r->setObjectName(("use wps style"));
    r->setText(tr("use wps style"));
    r->setWindowTitle(r->text());
    r->setChecked(false);
    groupStyle->addSmallWidget(r);
    g->addButton(r, RibbonBar::WpsLiteStyle);

    r = new QRadioButton();
    r->setObjectName(("use office 2row style"));
    r->setText(tr("use office 2 row style"));
    r->setWindowTitle(r->text());
    r->setChecked(false);
    groupStyle->addSmallWidget(r);
    g->addButton(r, RibbonBar::OfficeStyleTwoRow);

    r = new QRadioButton();
    r->setObjectName(("use wps 2row style"));
    r->setText(tr("use wps 2row style"));
    r->setWindowTitle(r->text());
    r->setChecked(false);
    groupStyle->addSmallWidget(r);
    g->addButton(r, RibbonBar::WpsLiteStyleTwoRow);

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(g, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::idClicked), this, &MainWindow::onStyleClicked);
#else
    connect(g, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this,
            &MainWindow::onStyleClicked);
#endif

    QCheckBox *cb = nullptr;

#if !defined(QXRIBBON_USE_APPWINDOW)
    cb = new QCheckBox();
    cb->setObjectName(QStringLiteral("use frameless"));
    cb->setText(tr("use frameless"));
    cb->setWindowTitle(cb->text());
    cb->setChecked(isFrameless());
    groupStyle->addSmallWidget(cb);
    connect(cb, &QCheckBox::clicked, this, [this](bool checked) {
        this->setFrameless(checked);
    });
    QCheckBox *framelessCB = cb;
#endif

    cb = new QCheckBox();
    cb->setObjectName(QStringLiteral("left QAB in wps style"));
    cb->setText(tr("left QAB in wps style"));
    cb->setWindowTitle(cb->text());
    cb->setChecked(false);
    groupStyle->addSmallWidget(cb);
    connect(cb, &QCheckBox::clicked, this, [this](bool checked) {
        this->ribbonBar()->setQuickAccessBarPosition(checked ? RibbonBar::QABLeftPosition
                                                             : RibbonBar::QABRightPosition);
    });

    cb = new QCheckBox();
    cb->setObjectName(QStringLiteral("center tab bar"));
    cb->setText(tr("center tab bar"));
    cb->setWindowTitle(cb->text());
    cb->setChecked(false);
    groupStyle->addSmallWidget(cb);
    connect(cb, &QCheckBox::clicked, this, [this](bool checked) {
        this->ribbonBar()->setTabBarPosition(checked ? RibbonBar::TBCenterPosition
                                                     : RibbonBar::TBLeftPosition);
    });

    cb = new QCheckBox();
    cb->setObjectName(QStringLiteral("show group title"));
    cb->setText(tr("show group title"));
    cb->setWindowTitle(cb->text());
    cb->setChecked(true);
    groupStyle->addSmallWidget(cb);
    connect(cb, &QCheckBox::clicked, this, [this](bool checked) {
        RibbonGroup::setTitleVisible(checked);
        this->ribbonBar()->updateRibbonGeometry();
    });

    cb = new QCheckBox();
    cb->setObjectName(QStringLiteral("show full screen"));
    cb->setText(tr("show full screen"));
    cb->setWindowTitle(cb->text());
    cb->setChecked(false);
    groupStyle->addSmallWidget(cb);
#if !defined(QXRIBBON_USE_APPWINDOW)
    connect(cb, &QCheckBox::clicked, this, [this, framelessCB](bool checked) {
#else
    connect(cb, &QCheckBox::clicked, this, [this](bool checked) {
#endif
        if (checked) {
            this->showFullScreen();
        } else {
            this->showNormal();
        }
#if !defined(QXRIBBON_USE_APPWINDOW)
        // FIXME: 全屏显示时，如果切换有边框和无边框会影响全屏显示，所以当全屏显示时，暂时先禁止有无边框切换
        framelessCB->setDisabled(checked);
#endif
    });
    connect(this, &MainWindow::windowStateChanged, cb, [cb](Qt::WindowStates s) {
        cb->setChecked(s.testFlag(Qt::WindowFullScreen));
    });

    RibbonGroup *groupToolButtonStyle = page->addGroup(tr("ribbon toolbutton style"));

    RibbonButton *btn;
    RibbonMenu *menu = new RibbonMenu(this);
    QAction *a = nullptr;
    {
        QIcon itemicon = QIcon(":/icon/res/file.svg");
        for (int i = 0; i < 3; ++i) {
            a = menu->addAction(itemicon, tr("item %1").arg(i + 1));
            a->setObjectName(QStringLiteral("item %1").arg(i + 1));
        }
    }

    QAction *act = createAction(tr("test 1"), ":/icon/res/file.svg");
    act->setMenu(menu);
    act->setToolTip(tr("use QToolButton::MenuButtonPopup mode"));
    btn = groupToolButtonStyle->addSmallAction(act);
    btn->setPopupMode(QToolButton::MenuButtonPopup);

    act = createAction(tr("test 2"), ":/icon/res/file.svg");
    act->setMenu(menu);
    act->setToolTip(tr("use QToolButton::InstantPopup mode"));
    btn = groupToolButtonStyle->addSmallAction(act);
    btn->setPopupMode(QToolButton::InstantPopup);

    groupToolButtonStyle->addSeparator();

    act = createAction(tr("test 3"), ":/icon/res/file.svg");
    act->setMenu(menu);
    btn = groupToolButtonStyle->addSmallAction(act);
    btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    act = createAction(tr("test 4"), ":/icon/res/file.svg");
    groupToolButtonStyle->addSmallAction(act);
    groupToolButtonStyle->ribbonButtonForAction(act)->setToolButtonStyle(Qt::ToolButtonIconOnly);

    groupToolButtonStyle->addSeparator();

    act = createAction(tr("Delayed Popup"), ":/icon/res/folder-cog.svg");
    act->setMenu(menu);
    btn = groupToolButtonStyle->addLargeAction(act);
    btn->setPopupMode(QToolButton::DelayedPopup);
    connect(act, &QAction::triggered, this, &MainWindow::onDelayedPopupCheckabletriggered);

    act = createAction(tr("Menu Button Popup"), ":/icon/res/folder-star.svg");
    act->setMenu(menu);
    btn = groupToolButtonStyle->addLargeAction(act);
    btn->setPopupMode(QToolButton::MenuButtonPopup);
    connect(act, &QAction::triggered, this, &MainWindow::onMenuButtonPopupCheckabletriggered);

    act = createAction(tr("Instant Popup"), ":/icon/res/folder-stats.svg");
    act->setMenu(menu);
    btn = groupToolButtonStyle->addLargeAction(act);
    btn->setPopupMode(QToolButton::InstantPopup);
    connect(act, &QAction::triggered, this, &MainWindow::onInstantPopupCheckabletriggered);

    act = createAction(tr("Delayed Popup checkable"), ":/icon/res/folder-table.svg");
    act->setCheckable(true);
    act->setMenu(menu);
    btn = groupToolButtonStyle->addLargeAction(act);
    btn->setPopupMode(QToolButton::DelayedPopup);
    connect(act, &QAction::triggered, this, &MainWindow::onDelayedPopupCheckableTest);

    act = createAction(tr("Menu Button Popup checkable"), ":/icon/res/folder-checkmark.svg");
    act->setCheckable(true);
    act->setMenu(menu);
    btn = groupToolButtonStyle->addLargeAction(act);
    btn->setPopupMode(QToolButton::MenuButtonPopup);
    connect(act, &QAction::triggered, this, &MainWindow::onMenuButtonPopupCheckableTest);

    act = createAction(tr("Menu"), ":/icon/res/folder-add.svg");
    act->setMenu(menu);
    btn = groupToolButtonStyle->addLargeAction(act);
    btn->setPopupMode(QToolButton::InstantPopup);

    act = createAction(tr("Action"), ":/icon/res/folder.svg");
    groupToolButtonStyle->addLargeAction(act);

    act = createAction(tr("Disable Action"), ":/icon/res/disable.svg");
    act->setCheckable(true);
    act->setMenu(menu);
    act->setEnabled(false);
    groupToolButtonStyle->addLargeAction(act);

    act = new QAction(tr("Arrow Button"), this);
    btn = groupToolButtonStyle->addLargeAction(act);
    btn->setArrowType(Qt::DownArrow);

    act = new QAction(QxRibbon::colorIcon(Qt::red, QSize(32, 32)), tr("Color Action"), this);
    groupToolButtonStyle->addLargeAction(act);

    act = createAction(tr("Color Button"), ":/icon/res/long-text.svg");
    RibbonColorButton *colorButton = new RibbonColorButton(this);
    colorButton->setDefaultAction(act);
    colorButton->setButtonType(RibbonButton::LargeButton);
    colorButton->setColor(Qt::blue);
    groupToolButtonStyle->addLargeWidget(colorButton);

    QAction *actionShortText = createAction(tr("Qx"), ":/icon/res/long-text.svg", "short-text");
    groupToolButtonStyle->addLargeAction(actionShortText);
    actionShortText = createAction(tr("Q\nx"), ":/icon/res/long-text.svg", "short-wordwrap-text");
    groupToolButtonStyle->addLargeAction(actionShortText);

    QAction *actionLongText =
        createAction(tr("show very long text in a button,balabalabala etc"), ":/icon/res/long-text.svg", "long-text");
    groupToolButtonStyle->addLargeAction(actionLongText);

    QAction *optAct = new QAction(this);
    connect(optAct, &QAction::triggered, this, [this](bool on) {
        Q_UNUSED(on);
        QMessageBox::information(this, tr("Option Action Triggered"), tr("Option Action Triggered"));
    });
    groupToolButtonStyle->addOptionAction(optAct);

    //! 2
    //! group 2 start
    //!
    RibbonGroup *groupPageContext = page->addGroup(tr("page context"));

    QAction *actShowContext = createAction(tr("show Context1"), ":/icon/res/showContext.svg");
    actShowContext->setCheckable(true);
    groupPageContext->addLargeAction(actShowContext);
    connect(actShowContext, &QAction::triggered, this, &MainWindow::onShowContextPage);

    QAction *actDeleteContext = createAction(tr("delete Context1"), ":/icon/res/deleteContext.svg");
    groupPageContext->addLargeAction(actDeleteContext);
    connect(actDeleteContext, &QAction::triggered, this, [this, actShowContext](bool on) {
        Q_UNUSED(on);
        if (this->m_pageContext) {
            this->ribbonBar()->destroyPageContext(this->m_pageContext);
            this->m_pageContext = nullptr;
            actShowContext->setChecked(false);
        }
    });

    QAction *actShowContext2 = createAction(tr("show Context2"), ":/icon/res/showContext.svg");
    actShowContext2->setCheckable(true);
    connect(actShowContext2, &QAction::triggered, this, [this](bool c) {
        if (m_pageContext2) {
            this->ribbonBar()->setPageContextVisible(m_pageContext2, c);
        }
    });
    groupPageContext->addLargeAction(actShowContext2);
}

void MainWindow::createPageOther()
{
    // 添加其他标签页 - 直接new RibbonPage添加
    RibbonPage *page = new RibbonPage();
    page->setPageName(tr("&Other"));
    page->setObjectName(("pageOther"));
    ribbonBar()->addPage(page);

    //! 1
    //! group 1 start
    //!

    RibbonGroup *group1 = new RibbonGroup(tr("button group and gallery"));
    group1->setObjectName("PageOther-group1");
    page->addGroup(group1);

    // 按钮组
    RibbonButtonGroup *btnGroup1 = new RibbonButtonGroup(group1);
    btnGroup1->addAction(createAction(tr("Copy"), ":/icon/res/copy.svg"));
    btnGroup1->addAction(createAction(tr("Cut"), ":/icon/res/cut.svg"));
    btnGroup1->addAction(createAction(tr("Paste"), ":/icon/res/clipboard.svg"));
    QAction *printerAction = createAction(tr("Printer"), ":/icon/res/printer.svg");
    QMenu *printerMenu = new QMenu();
    printerMenu->addAction(tr("test1"));
    printerMenu->addAction(tr("test2"));
    printerAction->setMenu(printerMenu);
    btnGroup1->addAction(printerAction);
    group1->addWidget(btnGroup1, RibbonGroup::Medium);

    RibbonButtonGroup *btnGroup2 = new RibbonButtonGroup(group1);
    QAction *titleAlgnment = createAction(tr("Title Align Left"), ":/icon/res/align-left.svg");
    titleAlgnment->setProperty("align", (int)Qt::AlignLeft | Qt::AlignVCenter);
    btnGroup2->addAction(titleAlgnment);
    titleAlgnment = createAction(tr("Title Align Center"), ":/icon/res/align-center.svg");
    titleAlgnment->setProperty("align", (int)Qt::AlignCenter);
    btnGroup2->addAction(titleAlgnment);
    titleAlgnment = createAction(tr("Title Align Right"), ":/icon/res/align-right.svg");
    titleAlgnment->setProperty("align", (int)Qt::AlignRight | Qt::AlignVCenter);
    btnGroup2->addAction(titleAlgnment);
    group1->addWidget(btnGroup2, RibbonGroup::Medium);
    connect(btnGroup2, &RibbonButtonGroup::actionTriggered, this, &MainWindow::onButtonGroupActionTriggered);

    // Gallery
    RibbonGallery *gallery = group1->addGallery();
    QList<QAction *> galleryActions;
    auto lambdaCreateGalleryAction = [this](const QString &text, const QString &iconurl) -> QAction * {
        QAction *act = this->createAction(text, iconurl);
        this->connect(act, &QAction::triggered, this, [this, text]() {
            if (this->m_edit) {
                this->m_edit->append(tr("%1 triggered").arg(text));
            }
        });
        return act;
    };
    galleryActions.append(lambdaCreateGalleryAction(tr("Document File"), ":/gallery-icon/res/gallery/Document-File.svg"));
    galleryActions.append(lambdaCreateGalleryAction(tr("Download File"), ":/gallery-icon/res/gallery/Download-File.svg"));
    galleryActions.append(lambdaCreateGalleryAction(tr("Drive File Four Word"), ":/gallery-icon/res/gallery/Drive-File.svg"));
    galleryActions.append(lambdaCreateGalleryAction(tr("Dropbox File"), ":/gallery-icon/res/gallery/Dropbox-File.svg"));
    galleryActions.append(lambdaCreateGalleryAction(tr("Email File"), ":/gallery-icon/res/gallery/Email-File.svg"));
    galleryActions.append(lambdaCreateGalleryAction(tr("Encode File"), ":/gallery-icon/res/gallery/Encode-File.svg"));
    galleryActions.append(lambdaCreateGalleryAction(tr("Favorit File"), ":/gallery-icon/res/gallery/Favorit-File.svg"));
    galleryActions.append(lambdaCreateGalleryAction(tr("File Error"), ":/gallery-icon/res/gallery/File-Error.svg"));
    galleryActions.append(lambdaCreateGalleryAction(tr("File Read Only"), ":/gallery-icon/res/gallery/File-Readonly.svg"));
    galleryActions.append(lambdaCreateGalleryAction(tr("File Settings"), ":/gallery-icon/res/gallery/File-Settings.svg"));
    galleryActions.append(lambdaCreateGalleryAction(tr("Presentation File"), ":/gallery-icon/res/gallery/Presentation-File.svg"));
    RibbonGalleryGroup *galleryGroup1 = gallery->addGalleryGroup(tr("Files"), galleryActions);
    galleryGroup1->setGalleryGroupStyle(RibbonGalleryGroup::IconWithWordWrapText);
    galleryGroup1->setGridMinimumWidth(80);
    galleryActions.clear();
    galleryActions.append(lambdaCreateGalleryAction(tr("Photoshop"), ":/gallery-icon/res/gallery/Photoshop.svg"));
    galleryActions.append(lambdaCreateGalleryAction(tr("Internet-Explorer"), ":/gallery-icon/res/gallery/Internet-Explorer.svg"));
    galleryActions.append(lambdaCreateGalleryAction(tr("Illustrator"), ":/gallery-icon/res/gallery/Illustrator.svg"));
    galleryActions.append(lambdaCreateGalleryAction(tr("Google-Maps"), ":/gallery-icon/res/gallery/Google-Maps.svg"));
    galleryActions.append(lambdaCreateGalleryAction(tr("Adobe"), ":/gallery-icon/res/gallery/Adobe.svg"));
    galleryActions.append(lambdaCreateGalleryAction(tr("Word"), ":/gallery-icon/res/gallery/Word.svg"));
    RibbonGalleryGroup *galleryGroup2 = gallery->addGalleryGroup(tr("Apps"), galleryActions);
    galleryGroup2->setGridMinimumWidth(80);
    gallery->setCurrentViewGroup(galleryGroup1);

    QAction *optAct = new QAction(this);
    optAct->setObjectName(("debug"));
    group1->addOptionAction(optAct);

    //! 2
    //! group 2 start -> widget test
    //!

    RibbonGroup *group2 = new RibbonGroup(tr("widget test"));
    group2->setObjectName("PageOther-group2");
    page->addGroup(group2);

    RibbonComboBox *com = new RibbonComboBox(this);
    com->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    com->setWindowTitle(tr("RibbonComboBox Editable"));
    for (int i = 0; i < 15; ++i) {
        com->addItem(QString("RibbonComboBox test%1").arg(i + 1));
    }
    com->setEditable(true);
    group2->addSmallWidget(com);

    com = new RibbonComboBox(this);
    com->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    com->setWindowTitle(tr("RibbonComboBox"));
    for (int i = 0; i < 15; ++i) {
        com->addItem(QString("item %1").arg(i + 1));
    }
    group2->addSmallWidget(com);

    RibbonLineEdit *lineEdit = new RibbonLineEdit(this);
    lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lineEdit->setText(tr("RibbonLineEdit"));
    lineEdit->setWindowTitle(lineEdit->text());
    group2->addSmallWidget(lineEdit);
    QWidget *w = lineEdit->parentWidget();
    while (w) {
        qDebug() << w->metaObject()->className();
        w = w->parentWidget();
    }

    RibbonCheckBox *checkBox = new RibbonCheckBox(this);
    checkBox->setText(tr("RibbonCheckBox"));
    checkBox->setWindowTitle(checkBox->text());
    group2->addSmallWidget(checkBox);

    QRadioButton *radioButton = new QRadioButton(this);
    radioButton->setText(tr("QRadioButton"));
    radioButton->setWindowTitle(radioButton->text());
    group2->addSmallWidget(radioButton);

    QSpinBox *spinBox = new QSpinBox(this);
    spinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    group2->addSmallWidget(spinBox);

    group2->addSeparator();

    QCalendarWidget *calendarWidget = new QCalendarWidget(this);
    calendarWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    calendarWidget->setObjectName(("calendarWidget"));
    group2->addLargeWidget(calendarWidget);
    // group2->setExpanding();

    optAct = new QAction(this);
    connect(optAct, &QAction::triggered, this, [this](bool on) {
        Q_UNUSED(on);
        QMessageBox::information(this, tr("Option Action Triggered"), tr("Option Action Triggered"));
    });
    group2->addOptionAction(optAct);

    group2->setVisible(true);

    //!
    //! \brief groupUtf8
    //!

    RibbonGroup *groupUtf8 = new RibbonGroup(QStringLiteral(u"中文显示测试"));
    groupUtf8->setObjectName("PageOther-groupUtf8");
    page->addGroup(groupUtf8);

    QAction *actionChangeText =
        createAction(QStringLiteral(u"改变显示为英文"), ":/icon/res/chinese-char.svg", "actionChangeText");
    actionChangeText->setCheckable(true);
    actionChangeText->setChecked(false);
    groupUtf8->addLargeAction(actionChangeText);
    connect(actionChangeText, &QAction::triggered, this, [groupUtf8, actionChangeText](bool on) {
        if (on) {
            groupUtf8->setGroupName(QStringLiteral(u"show chinese char"));
            actionChangeText->setText(QStringLiteral(u"show in chinese"));
        } else {
            groupUtf8->setGroupName(QStringLiteral(u"中文显示测试"));
            actionChangeText->setText(QStringLiteral(u"改变显示为英文"));
        }
    });
}

void MainWindow::createPageCustom()
{
    RibbonPage *page = ribbonBar()->addPage(tr("&Custom"));
    page->setObjectName(("pageCustom"));

    // group1
    RibbonGroup *group1 = new RibbonGroup(tr("quick access bar"));
    QAction *addAct = createAction(tr("add action"), ":/icon/res/action2.svg");
    QAction *removeAct = createAction(tr("remove action"), ":/icon/res/action4.svg");
    group1->addLargeAction(addAct);
    group1->addLargeAction(removeAct);
    removeAct->setDisabled(true);
    QAction *testAct = createAction(tr("custom action"), ":/icon/res/action2.svg");
    connect(addAct, &QAction::triggered, this, [this, addAct, removeAct, testAct]() {
        this->ribbonBar()->quickAccessBar()->addAction(testAct);
        this->ribbonBar()->resizeRibbon();
        addAct->setDisabled(true);
        removeAct->setDisabled(false);
    });
    connect(removeAct, &QAction::triggered, this, [this, addAct, removeAct, testAct]() {
        this->ribbonBar()->quickAccessBar()->removeAction(testAct);
        this->ribbonBar()->resizeRibbon();
        addAct->setDisabled(false);
        removeAct->setDisabled(true);
    });
    page->addGroup(group1);

    // groupStyle
    RibbonGroup *groupStyle = new RibbonGroup(tr("config file"));
    page->addGroup(groupStyle);

    QAction *actionUseQss = createAction(tr("use qss"), ":/icon/res/useqss.svg");
    connect(actionUseQss, &QAction::triggered, this, &MainWindow::onActionUseQssTriggered);
    groupStyle->addSmallAction(actionUseQss);

    QAction *actionLoadCustomizeXmlFile =
        createAction(tr("load customize from xml file"), ":/icon/res/useCustomize.svg");
    connect(actionLoadCustomizeXmlFile, &QAction::triggered, this, &MainWindow::onActionLoadCustomizeXmlFileTriggered);
    groupStyle->addSmallAction(actionLoadCustomizeXmlFile);
}

/**
 * @brief 构建应用页
 * @param page
 */
void MainWindow::createPageApply()
{
    RibbonPage *page = ribbonBar()->addPage(tr("&Apply(example long page)"));
    page->setObjectName(("pageApply"));

    QAction *act = nullptr;
    RibbonGroup *group = page->addGroup(tr("Font"));
    RibbonButtonGroup *group1 = new RibbonButtonGroup(group);
    group1->setObjectName(QStringLiteral(u"group1"));
    QLabel *labelFontSize = new QLabel(group1);
    labelFontSize->setText(tr("select font"));
    labelFontSize->setObjectName(QStringLiteral(u"labelFontSize"));
    group1->addWidget(labelFontSize);
    QFontComboBox *fontComWidget = new QFontComboBox(group1);
    fontComWidget->setObjectName(QStringLiteral(u"fontComboBox"));
    connect(fontComWidget, &QFontComboBox::currentFontChanged, this, &MainWindow::onFontComWidgetCurrentFontChanged);
    group1->addWidget(fontComWidget);
    act = group->addWidget(group1, RibbonGroup::Medium);
    act->setObjectName(labelFontSize->objectName());

    RibbonButtonGroup *group2 = new RibbonButtonGroup(group);
    group2->setObjectName(QStringLiteral(u"group2"));
    group2->addAction(createAction(tr("Bold"), ":/icon/res/bold.svg"));
    group2->addAction(createAction(tr("Italic"), ":/icon/res/italic.svg"));
    group2->addSeparator();
    group2->addAction(createAction(tr("left alignment"), ":/icon/res/align-left.svg"));
    group2->addAction(createAction(tr("center alignment"), ":/icon/res/align-center.svg"));
    group2->addAction(createAction(tr("right alignment"), ":/icon/res/align-right.svg"));
    group2->addAction(createAction(tr("line up on both sides"), ":/icon/res/align-bothside.svg"));
    act = group->addWidget(group2, RibbonGroup::Medium);
    act->setObjectName(group2->objectName());

    group->addSeparator();

    QAction *actLargerFontSize = createAction(tr("Larger"), ":/icon/res/largerFont.svg", "actLargerFontSize");
    group->addLargeAction(actLargerFontSize);

    QAction *actSmallFontSize = createAction(tr("Smaller"), ":/icon/res/smallFont.svg", "actSmallFontSize");
    group->addLargeAction(actSmallFontSize);

    connect(actLargerFontSize, &QAction::triggered, this, &MainWindow::onActionFontLargerTriggered);
    connect(actSmallFontSize, &QAction::triggered, this, &MainWindow::onActionFontSmallerTriggered);
}

/**
 * @brief 创建上下文标签
 */
void MainWindow::createPageContext1()
{
    RibbonBar *ribbon = ribbonBar();
    m_pageContext = ribbon->addPageContext(tr("context"), QColor());

    RibbonPage *page1 = m_pageContext->addPage(tr("Page1"));
    createPageContext1(page1);

    RibbonPage *page2 = m_pageContext->addPage(tr("Page2"));
    createPageContext2(page2);
}

void MainWindow::createPageContext2()
{
    RibbonBar *ribbon = ribbonBar();
    m_pageContext2 = ribbon->addPageContext(tr("context2"), QColor());
    m_pageContext2->addPage(tr("context2 Page1"));
    m_pageContext2->addPage(tr("context2 Page2"));
}

void MainWindow::createPageContext1(RibbonPage *page)
{
    RibbonGroup *group = page->addGroup(tr("show and hide test"));

    QAction *actionHideGroup = createAction(tr("hide group"), ":/icon/res/hide.svg");
    actionHideGroup->setCheckable(true);
    group->addLargeAction(actionHideGroup);

    QAction *actionDisable = createAction(tr("Disable"), ":/icon/res/enableTest.svg");

    actionDisable->setDisabled(true);
    group->addLargeAction(actionDisable);
    connect(actionDisable, &QAction::triggered, this, [actionDisable](bool b) {
        Q_UNUSED(b);
        actionDisable->setDisabled(true);
    });

    QAction *actionUnlock = createAction(tr("unlock"), ":/icon/res/unlock.svg");
    actionUnlock->setShortcut(QKeySequence(QLatin1String("Ctrl+E")));
    group->addLargeAction(actionUnlock);
    connect(actionUnlock, &QAction::triggered, this, [actionDisable](bool b) {
        Q_UNUSED(b);
        actionDisable->setEnabled(true);
        actionDisable->setText(tr("Enabled"));
    });

    QAction *actionSetTextTest = createAction(tr("set text"), ":/icon/res/setText.svg");

    actionSetTextTest->setCheckable(true);
    actionSetTextTest->setShortcut(QKeySequence(QLatin1String("Ctrl+D")));
    group->addLargeAction(actionSetTextTest);

    connect(actionSetTextTest, &QAction::toggled, this, [actionSetTextTest](bool b) {
        if (b) {
            actionSetTextTest->setText(QStringLiteral(u"setText测试"));
        } else {
            actionSetTextTest->setText(QStringLiteral(u"set text"));
        }
    });
    // 隐藏group
    QAction *actionShowTest = createAction(tr("show beside group"), ":/icon/res/show.svg");

    actionShowTest->setCheckable(true);
    group->addLargeAction(actionShowTest);

    RibbonGroup *group2 = page->addGroup(tr("show/hide"));

    group2->addLargeAction(actionSetTextTest);

    connect(actionShowTest, &QAction::toggled, this, [actionShowTest, group2, this](bool b) {
        group2->setVisible(!b);
        if (b) {
            actionShowTest->setText(tr("hide beside group"));
        } else {
            actionShowTest->setText(tr("show beside group"));
        }
        ribbonBar()->repaint();
    });

    RibbonGroup *group3 = page->addGroup(tr("show/hide action test"));

    QAction *actionHideAction2 = createAction(tr("hide action 2"), ":/icon/res/action.svg");
    QAction *actionHideAction4 = createAction(tr("hide action 4"), ":/icon/res/action.svg");
    actionHideAction2->setCheckable(true);
    actionHideAction2->setChecked(true);
    actionHideAction4->setCheckable(true);
    actionHideAction4->setChecked(true);
    QAction *act2 = createAction(tr("action2"), ":/icon/res/action2.svg");
    QAction *act3 = createAction(tr("action3"), ":/icon/res/action3.svg");
    QAction *act4 = createAction(tr("action4"), ":/icon/res/action4.svg");
    group3->addLargeAction(actionHideAction2);
    group3->addLargeAction(actionHideAction4);
    group3->addSmallAction(act2);
    group3->addSmallAction(act3);
    group3->addSmallAction(act4);
    connect(actionHideAction2, &QAction::triggered, this, [actionHideAction2, act2](bool b) {
        if (b) {
            act2->setVisible(true);
            actionHideAction2->setText(tr("hide action2"));
        } else {
            act2->setVisible(false);
            actionHideAction2->setText(tr("show action2"));
        }
    });
    connect(actionHideAction4, &QAction::triggered, this, [actionHideAction4, act4](bool b) {
        if (b) {
            act4->setVisible(true);
            actionHideAction4->setText(tr("hide action4"));
        } else {
            act4->setVisible(false);
            actionHideAction4->setText(tr("show action4"));
        }
    });

    RibbonGroup *group4 = page->addGroup(tr("widget"));
    QSpinBox *spinbox = new QSpinBox(this);
    RibbonCtrlContainer *ctrlContainer1 = new RibbonCtrlContainer(group4);
    ctrlContainer1->setWidget(spinbox);
    ctrlContainer1->setText(tr("spinbox:"));
    ctrlContainer1->setIconVisible(false);
    group4->addMediumWidget(ctrlContainer1);

    QLineEdit *linedit = new QLineEdit(this);
    RibbonCtrlContainer *ctrlContainer2 = new RibbonCtrlContainer(group4);
    ctrlContainer2->setWidget(linedit);
    ctrlContainer2->setText(tr("linedit:"));
    ctrlContainer2->setIconVisible(false);
    group4->addMediumWidget(ctrlContainer2);
}

void MainWindow::createPageContext2(RibbonPage *page)
{
    RibbonGroup *group1 = page->addGroup(QStringLiteral(u"删除PagePage测试"));

    QAction *actionDeleteThisPage = createAction(tr("delete this page"), ":/icon/res/delete.svg");
    group1->addLargeAction(actionDeleteThisPage);
    connect(actionDeleteThisPage, &QAction::triggered, this, [this, page]() {
        this->ribbonBar()->removePage(page);
        page->deleteLater();
    });
    RibbonGroup *groupLayout = page->addGroup(QStringLiteral(u"特殊布局"));

    groupLayout->addAction(createAction(tr("Large"), ":/icon/res/file.svg", "@Large1"), QToolButton::InstantPopup,
                           RibbonGroup::Large);
    groupLayout->addAction(createAction(tr("Small"), ":/icon/res/file.svg", "@Small1"), QToolButton::InstantPopup,
                           RibbonGroup::Small);
    groupLayout->addAction(createAction(tr("Small"), ":/icon/res/file.svg", "@Small2"), QToolButton::InstantPopup,
                           RibbonGroup::Small);
    groupLayout->addSeparator();
    groupLayout->addAction(createAction(tr("Small"), ":/icon/res/file.svg", "@Small3"), QToolButton::InstantPopup,
                           RibbonGroup::Small);
    groupLayout->addAction(createAction(tr("Small"), ":/icon/res/file.svg", "@Small4"), QToolButton::InstantPopup,
                           RibbonGroup::Small);
    groupLayout->addAction(createAction(tr("Small"), ":/icon/res/file.svg", "@Small5"), QToolButton::InstantPopup,
                           RibbonGroup::Small);
    groupLayout->addAction(createAction(tr("Small"), ":/icon/res/file.svg", "@Small6"), QToolButton::InstantPopup,
                           RibbonGroup::Small);
    groupLayout->addSeparator();
    groupLayout->addAction(createAction(tr("Large"), ":/icon/res/file.svg", "@Large2"), QToolButton::InstantPopup,
                           RibbonGroup::Large);
    groupLayout->addAction(createAction(tr("Medium"), ":/icon/res/file.svg", "@Medium1"), QToolButton::InstantPopup,
                           RibbonGroup::Medium);
    groupLayout->addAction(createAction(tr("Medium"), ":/icon/res/file.svg", "@Medium2"), QToolButton::InstantPopup,
                           RibbonGroup::Medium);
    groupLayout->addAction(createAction(tr("Small"), ":/icon/res/file.svg", "@Small7"), QToolButton::InstantPopup,
                           RibbonGroup::Small);
    groupLayout->addAction(createAction(tr("Medium"), ":/icon/res/file.svg", "@Medium3"), QToolButton::InstantPopup,
                           RibbonGroup::Medium);
    groupLayout->addAction(createAction(tr("Large"), ":/icon/res/file.svg", "@Large3"), QToolButton::InstantPopup,
                           RibbonGroup::Large);
    groupLayout->addAction(createAction(tr("Medium"), ":/icon/res/file.svg", "@Medium4"), QToolButton::InstantPopup,
                           RibbonGroup::Medium);
    groupLayout->addAction(createAction(tr("Medium"), ":/icon/res/file.svg", "@Medium5"), QToolButton::InstantPopup,
                           RibbonGroup::Medium);
    groupLayout->addAction(createAction(tr("Large"), ":/icon/res/file.svg", "@Large4"), QToolButton::InstantPopup,
                           RibbonGroup::Large);
    groupLayout->addSeparator();
    groupLayout->addAction(createAction(tr("Medium"), ":/icon/res/file.svg", "@Medium6"), QToolButton::InstantPopup,
                           RibbonGroup::Medium);
    groupLayout->addAction(createAction(tr("Large"), ":/icon/res/file.svg", "@Large5"), QToolButton::InstantPopup,
                           RibbonGroup::Large);
    groupLayout->addAction(createAction(tr("Medium"), ":/icon/res/file.svg", "@Medium7"), QToolButton::InstantPopup,
                           RibbonGroup::Medium);
    groupLayout->addAction(createAction(tr("Small"), ":/icon/res/file.svg", "@Small8"), QToolButton::InstantPopup,
                           RibbonGroup::Small);
}

void MainWindow::createQuickAccessBar()
{
    RibbonQuickAccessBar *quickAccessBar = ribbonBar()->quickAccessBar();
    connect(quickAccessBar, &RibbonQuickAccessBar::customizeActionChanged, this, [this]() {
        this->ribbonBar()->resizeRibbon();
    });
    quickAccessBar->addAction(createAction(tr("save"), ":/icon/res/save.svg", "save-quickbar"));
    quickAccessBar->addAction(createAction(tr("printer"), ":/icon/res/printer.svg"));

    quickAccessBar->addAction(createAction(tr("undo"), ":/icon/res/undo.svg"));
    quickAccessBar->addAction(createAction(tr("redo"), ":/icon/res/redo.svg"));

    QAction *menuAction = createAction(tr("file"), ":/icon/res/file.svg");
    RibbonMenu *m = new RibbonMenu(tr("file"), this);
    for (int i = 0; i < 10; ++i) {
        m->addAction(createAction(tr("file%1").arg(i + 1), ":/icon/res/file.svg"));
    }
    menuAction->setMenu(m);
    quickAccessBar->addAction(menuAction);

    QAction *actionCustomizeAndSave = createAction(tr("customize and save"), ":/icon/res/customize.svg");
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
    QAction *actionTheme = createAction(tr("theme"), ":/icon/res/theme.svg");
    RibbonMenu *menu = new RibbonMenu(tr("theme"), this);
    actionTheme->setMenu(menu);
    m_themeGroup->addAction(addThemeAction(menu->addAction(tr("LightYellow")), RibbonTheme::LightYellowTheme));
    m_themeGroup->addAction(addThemeAction(menu->addAction(tr("LightOffice2013")), RibbonTheme::LightOffice2013Theme));
    m_themeGroup->addAction(addThemeAction(menu->addAction(tr("LightClassic")), RibbonTheme::LightClassicTheme));
    m_themeGroup->addAction(addThemeAction(menu->addAction(tr("LightFancy")), RibbonTheme::LightFancyTheme));
    m_themeGroup->addAction(addThemeAction(menu->addAction(tr("DarkWps")), RibbonTheme::DarkWpsTheme));
    m_themeGroup->addAction(addThemeAction(menu->addAction(tr("Office2016Blue")), MainWindow::Office2016BlueTheme));
    rightBar->addAction(actionTheme);

    QAction *actionHelp = createAction(tr("help"), ":/icon/res/help.svg");
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
#if QXRIBBON_TEST_CUSTOMIZE_APPBTN
    // FIXME: 当前按钮在单击后会出现下沉效果
    QToolButton *appBtn = new QToolButton(this);
    appBtn->setAutoRaise(true);
    appBtn->setFocusPolicy(Qt::NoFocus);
    appBtn->setPopupMode(QToolButton::InstantPopup);
    appBtn->setText(tr("&File"));
    appBtn->setToolButtonStyle(Qt::ToolButtonTextOnly);
    appBtn->setMinimumWidth(40);
    RibbonMenu *appMenu = new RibbonMenu(this);
    // 菜单添加的Action可以来自QuickAccessBar或其它的Action，此处创建新的Action为了方便演示
    appMenu->addAction(createAction(tr("file"), ":/icon/res/file.svg"));
    appMenu->addAction(createAction(tr("save"), ":/icon/res/save.svg"));
    appMenu->addAction(createAction(tr("cut"), ":/icon/res/cut.svg"));
    appBtn->setMenu(appMenu);
    ribbonBar()->setApplicationButton(appBtn);
#else
    setApplicationButton(ribbonBar()->applicationButton());
#endif
}

void MainWindow::addSomeOtherAction()
{
    RibbonBar *bar = this->ribbonBar();
    // 添加其他的action，这些action并不在ribbon管理范围，主要用于RibbonCustomizeWidget自定义用
    QAction *acttext1 = new QAction(tr("text action1"), bar);
    QAction *acttext2 = new QAction(tr("text action2"), bar);
    QAction *acttext3 = new QAction(tr("text action3"), bar);
    QAction *acttext4 = new QAction(tr("text action4"), bar);
    QAction *acttext5 = new QAction(tr("text action5"), bar);

    QAction *actIcon1 = new QAction(QIcon(":/icon/res/file.svg"), tr("action with icon"), bar);

    m_actionTagText = RibbonActionsManager::UserDefineActionTag + 1;
    m_actionTagWithIcon = RibbonActionsManager::UserDefineActionTag + 2;

    m_actMgr = new RibbonActionsManager(bar);   // 声明过程已经自动注册所有action

    // 以下注册特别的action
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

void MainWindow::setApplicationButton(QAbstractButton *btn)
{
    btn->setText(tr("&File"));
    btn->setCheckable(true);
    connect(ribbonBar(), &RibbonBar::applicationButtonClicked, this, &MainWindow::onApplicationButtonClicked);
}

QAction *MainWindow::createAction(const QString &text, const QString &iconurl, const QString &objName)
{
    QAction *act = new QAction(this);
    act->setText(text);
    act->setIcon(QIcon(iconurl));
    act->setObjectName(objName);
    return act;
}

QAction *MainWindow::createAction(const QString &text, const QString &iconurl)
{
    QAction *act = new QAction(this);
    act->setText(text);
    act->setIcon(QIcon(iconurl));
    act->setObjectName(text);
    return act;
}

QAction *MainWindow::addThemeAction(QAction *action, int themeId)
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
    this->setStyleSheet(styleSheet);
    setRibbonTheme(UserCustomTheme);
    // TODO: update theme action state
}

void MainWindow::onShowContextPage(bool on)
{
    if (m_pageContext == nullptr) {
        createPageContext1();
    }
    if (on) {
        ribbonBar()->showPageContext(m_pageContext);
    } else {
        ribbonBar()->hidePageContext(m_pageContext);
    }
}

void MainWindow::onStyleClicked(int id)
{
    ribbonBar()->setRibbonStyle(static_cast<RibbonBar::RibbonStyle>(id));
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
                   tr(
"Copyleft (C) 2023 maminjie &lt;canpool@163.com&gt;<br/>"
"<br/>QxRibbon is a Qx componet like to Microsoft Ribbon style. "
"For more information, please visit <a href=\"%1\">%1</a>").arg(url),
                   this);
    ad.exec();
}

void MainWindow::onActionRemoveAppBtnTriggered(bool b)
{
    if (b) {
        ribbonBar()->setApplicationButton(nullptr);
    } else {
        RibbonApplicationButton *actionRemoveAppBtn = new RibbonApplicationButton();
        ribbonBar()->setApplicationButton(actionRemoveAppBtn);
        setApplicationButton(actionRemoveAppBtn);
    }
}

void MainWindow::onActionUseQssTriggered()
{
    QFile f("ribbon.qss");
    if (!f.exists()) {
        QString fdir = QFileDialog::getOpenFileName(this, tr("select stylesheet file"),
                                                    "", tr("Files (*.qss *.css)"));
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
    // 只能调用一次
    static bool has_call = false;
    if (!has_call) {
        has_call = QxRibbonCustomizeApplyFromXmlFile("customize.xml", this->ribbonBar(), m_actMgr);
    }
}

void MainWindow::onActionWindowFlagNormalButtonTriggered(bool b)
{
    if (b) {
        updateWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
    } else {
        updateWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    }
}

void MainWindow::onActionChangeThemeTriggered()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        int theme = action->data().toInt();
        // 暗色系
        if (theme == RibbonTheme::DarkWpsTheme || theme == MainWindow::Office2016BlueTheme) {
            ribbonBar()->setPageContextCoverTab(false);
        } else {
            // 亮色系
            ribbonBar()->setPageContextCoverTab(true);
        }
        setRibbonTheme(theme);
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

/**
 * @brief 测试RibbonButtonGroup和标题对齐
 * @param act
 */
void MainWindow::onButtonGroupActionTriggered(QAction *act)
{
    QVariant v = act->property("align");
    if (v.isValid()) {
        Qt::Alignment al = static_cast<Qt::Alignment>(v.toInt());
        if (!ribbonBar()) {
            return;
        }
        ribbonBar()->setWindowTitleAligment(al);
        ribbonBar()->repaint();
    }
}

void MainWindow::onApplicationButtonClicked()
{
    // 使用QWidget且setWindowModality(Qt::ApplicationModal)未阻塞，所以用QDialog
    QDialog w(this);
    w.setWindowFlags(Qt::Popup);
    w.resize(200, 100);
    QAbstractButton *btn = ribbonBar()->applicationButton();
    w.move(mapToGlobal(btn->geometry().bottomLeft()));
    w.exec();
    btn->setChecked(false);
}

void MainWindow::onMenuButtonPopupCheckableTest(bool b)
{
    m_edit->append(QString("MenuButtonPopupCheckableTest : %1").arg(b));
}

void MainWindow::onInstantPopupCheckableTest(bool b)
{
    m_edit->append(QString("InstantPopupCheckableTest : %1").arg(b));
}

void MainWindow::onDelayedPopupCheckableTest(bool b)
{
    m_edit->append(QString("DelayedPopupCheckableTest : %1").arg(b));
}

void MainWindow::onMenuButtonPopupCheckabletriggered(bool b)
{
    m_edit->append(QString("MenuButtonPopupCheckabletriggered : %1").arg(b));
}

void MainWindow::onInstantPopupCheckabletriggered(bool b)
{
    m_edit->append(QString("InstantPopupCheckabletriggered : %1").arg(b));
}

void MainWindow::onDelayedPopupCheckabletriggered(bool b)
{
    m_edit->append(QString("DelayedPopupCheckabletriggered : %1").arg(b));
}
