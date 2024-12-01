#include "mainwindow.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonpagecontext.h"
#include "qxribbon/ribbonquickaccessbar.h"

#include <QAction>
#include <QToolButton>
#include <QButtonGroup>
#include <QRadioButton>
#include <QCheckBox>
#include <QDebug>

QX_RIBBON_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : RibbonWindow(parent)
{
    QIcon icon(":/logo");

    createQuickAccessBar();

    RibbonBar *rb = ribbonBar();

    RibbonPage *page = rb->addPage(tr("page1"));
    createStyleGroup(page);

    rb->addPage(tr("loooooooooooongpage2"));

    RibbonPageContext *pc = rb->addPageContext(tr("pagecontext1"));
    pc->addPage(tr("page1"));
    pc->addPage(tr("page2"));
    rb->showPageContext(pc);

    resize(800, 400);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createQuickAccessBar()
{
    RibbonQuickAccessBar *qab = ribbonBar()->quickAccessBar();
    connect(qab, &RibbonQuickAccessBar::customizeActionChanged, this, [this]() {
        this->ribbonBar()->resizeRibbon();
    });
    QIcon icon(":/logo");
    qab->addAction(icon, tr("qa1"));
    qab->addAction(icon, tr("qa2"));

    QAction *otherAction = new QAction(tr("other"), this);
    connect(qab, &RibbonQuickAccessBar::showCustomizeMenu, this, [otherAction](QMenu *menu) {
        menu->addSeparator();
        menu->addAction(otherAction);
    });
}

void MainWindow::createStyleGroup(QxRibbon::RibbonPage *page)
{
    RibbonGroup *groupStyle = page->addGroup(tr("ribbon style"));

    QButtonGroup *g = new QButtonGroup(this);

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
    cb->setObjectName(QStringLiteral("show group title"));
    cb->setText(tr("show group title"));
    cb->setWindowTitle(cb->text());
    cb->setChecked(true);
    groupStyle->addSmallWidget(cb);
    connect(cb, &QCheckBox::clicked, this, [this](bool checked) {
        RibbonGroup::setTitleVisible(checked);
        this->ribbonBar()->updateRibbonGeometry();
    });
}

void MainWindow::onStyleClicked(int id)
{
    ribbonBar()->setRibbonStyle(static_cast<RibbonBar::RibbonStyle>(id));
}
