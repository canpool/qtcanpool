#include "mainwindow.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonpagecontext.h"
#include "qxribbon/ribbonquickaccessbar.h"

#include <QAction>
#include <QKeySequence>
#include <QToolButton>
#include <QButtonGroup>
#include <QRadioButton>
#include <QCheckBox>
#include <QDebug>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

QX_RIBBON_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : RibbonMainWindow(parent)
{
    QIcon icon(":/logo");

    createQuickAccessBar();

    RibbonBar *rb = ribbonBar();

    RibbonPage *page = rb->addPage(tr("page1"));
    createStyleGroup(page);
    createScreenTipGroup(page);

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

    // B8: the QAB's "Customize Quick Access Toolbar..." entry (after a separator,
    // at the end of the customize menu) emits this signal; the host opens its own
    // customize dialog here.
    connect(qab, &RibbonQuickAccessBar::customizeDialogRequested, this, [this]() {
        QDialog dlg(this);
        dlg.setWindowTitle(tr("Customize Quick Access Toolbar"));
        auto *layout = new QVBoxLayout(&dlg);
        layout->addWidget(new QLabel(tr("Host-provided customize dialog.")));
        auto *ok = new QPushButton(tr("Close"), &dlg);
        connect(ok, &QPushButton::clicked, &dlg, &QDialog::accept);
        layout->addWidget(ok);
        dlg.exec();
    });
}

void MainWindow::createStyleGroup(QxRibbon::RibbonPage *page)
{
    RibbonGroup *groupStyle = page->addGroup(tr("ribbon style"));

    QButtonGroup *g = new QButtonGroup(this);

    QRadioButton *r = new QRadioButton();
    r->setText(tr("tab bar below"));
    r->setWindowTitle(r->text());
    r->setObjectName(("tab bar below"));
    r->setChecked(true);
    groupStyle->addSmallWidget(r);
    g->addButton(r, RibbonBar::TabBarBelowStyle);

    r = new QRadioButton();
    r->setObjectName(("tab bar inline"));
    r->setText(tr("tab bar inline"));
    r->setWindowTitle(r->text());
    r->setChecked(false);
    groupStyle->addSmallWidget(r);
    g->addButton(r, RibbonBar::TabBarInlineStyle);

    r = new QRadioButton();
    r->setObjectName(("tab bar below, 2 rows"));
    r->setText(tr("tab bar below, 2 rows"));
    r->setWindowTitle(r->text());
    r->setChecked(false);
    groupStyle->addSmallWidget(r);
    g->addButton(r, RibbonBar::TabBarBelowTwoRowStyle);

    r = new QRadioButton();
    r->setObjectName(("tab bar inline, 2 rows"));
    r->setText(tr("tab bar inline, 2 rows"));
    r->setWindowTitle(r->text());
    r->setChecked(false);
    groupStyle->addSmallWidget(r);
    g->addButton(r, RibbonBar::TabBarInlineTwoRowStyle);

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(g, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::idClicked), this, &MainWindow::onStyleClicked);
#else
    connect(g, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked), this,
            &MainWindow::onStyleClicked);
#endif

    QCheckBox *cb = nullptr;

    cb = new QCheckBox();
    cb->setObjectName(QStringLiteral("left QAB in inline style"));
    cb->setText(tr("left QAB in inline style"));
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

void MainWindow::createScreenTipGroup(RibbonPage *page)
{
    // B6: the rich screen tip (title + description + shortcut) shows on hover only
    // when the action carries a statusTip() or a shortcut(); an action with neither
    // keeps the plain tooltip. The three buttons below cover both cases.
    RibbonGroup *group = page->addGroup(tr("screen tip"));

    QAction *saveAction = new QAction(tr("save"), this);
    saveAction->setStatusTip(tr("Save the current document to disk"));
    saveAction->setShortcut(QKeySequence(QKeySequence::Save));
    group->addLargeAction(saveAction);

    QAction *undoAction = new QAction(tr("undo"), this);
    undoAction->setStatusTip(tr("Undo the last edit"));
    undoAction->setShortcut(QKeySequence(QKeySequence::Undo));
    group->addSmallAction(undoAction);

    QAction *plainAction = new QAction(tr("plain"), this);
    plainAction->setToolTip(tr("no statusTip, no shortcut: stays a plain tooltip"));
    group->addSmallAction(plainAction);
}
