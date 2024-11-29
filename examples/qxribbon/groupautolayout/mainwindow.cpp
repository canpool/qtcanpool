#include "mainwindow.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonbutton.h"

#include <QAction>

QX_RIBBON_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : RibbonWindow(parent)
{
    QIcon icon(":/logo");

    RibbonBar *rb = ribbonBar();

    RibbonPage *page = rb->addPage(tr("page1"));

    RibbonGroup *group = page->addGroup(tr("auto layout"));
    group->addAction(createAction(icon, tr("Large")), QToolButton::InstantPopup, RibbonGroup::Large);
    group->addAction(createAction(icon, tr("Small")), QToolButton::InstantPopup, RibbonGroup::Small);
    group->addAction(createAction(icon, tr("Small")), QToolButton::InstantPopup, RibbonGroup::Small);
    group->addSeparator();
    group->addAction(createAction(icon, tr("Small")), QToolButton::InstantPopup, RibbonGroup::Small);
    group->addAction(createAction(icon, tr("Small")), QToolButton::InstantPopup, RibbonGroup::Small);
    group->addAction(createAction(icon, tr("Small")), QToolButton::InstantPopup, RibbonGroup::Small);
    group->addAction(createAction(icon, tr("Small")), QToolButton::InstantPopup, RibbonGroup::Small);
    group->addSeparator();
    group->addAction(createAction(icon, tr("Large")), QToolButton::InstantPopup, RibbonGroup::Large);
    group->addAction(createAction(icon, tr("Medium")), QToolButton::InstantPopup, RibbonGroup::Medium);
    group->addAction(createAction(icon, tr("Medium")), QToolButton::InstantPopup, RibbonGroup::Medium);
    group->addAction(createAction(icon, tr("Small")), QToolButton::InstantPopup, RibbonGroup::Small);
    group->addAction(createAction(icon, tr("Medium")), QToolButton::InstantPopup, RibbonGroup::Medium);
    group->addAction(createAction(icon, tr("Large")), QToolButton::InstantPopup, RibbonGroup::Large);
    group->addAction(createAction(icon, tr("Medium")), QToolButton::InstantPopup, RibbonGroup::Medium);
    group->addAction(createAction(icon, tr("Medium")), QToolButton::InstantPopup, RibbonGroup::Medium);
    group->addAction(createAction(icon, tr("Large")), QToolButton::InstantPopup, RibbonGroup::Large);
    group->addSeparator();
    group->addAction(createAction(icon, tr("Medium")), QToolButton::InstantPopup, RibbonGroup::Medium);
    group->addAction(createAction(icon, tr("Large")), QToolButton::InstantPopup, RibbonGroup::Large);
    group->addAction(createAction(icon, tr("Medium")), QToolButton::InstantPopup, RibbonGroup::Medium);
    group->addAction(createAction(icon, tr("Small")), QToolButton::InstantPopup, RibbonGroup::Small);

    page = rb->addPage(tr("page2"));

    group = page->addGroup(tr("medium"));
    addMediumAction(group, createAction(icon, tr("Medium")));
    addMediumAction(group, createAction(icon, tr("Medium")));
    addMediumAction(group, createAction(icon, tr("Medium")));
    group->addSeparator();
    addMediumAction(group, createAction(icon, tr("Medium")));
    addMediumAction(group, createAction(icon, tr("Medium")));
    addMediumAction(group, createAction(icon, tr("Medium")));
    addMediumAction(group, createAction(icon, tr("Medium")));
    addMediumAction(group, createAction(icon, tr("Medium")));
    addMediumAction(group, createAction(icon, tr("Medium")));
    addMediumAction(group, createAction(icon, tr("Medium")));
    addMediumAction(group, createAction(icon, tr("Medium")));

    group = page->addGroup(tr("small"));
    addSmallAction(group, createAction(icon, tr("Small")));
    addSmallAction(group, createAction(icon, tr("Small")));
    group->addSeparator();
    addSmallAction(group, createAction(icon, tr("Small")));
    addSmallAction(group, createAction(icon, tr("Small")));
    addSmallAction(group, createAction(icon, tr("Small")));
    addSmallAction(group, createAction(icon, tr("Small")));
    addSmallAction(group, createAction(icon, tr("Small")));
    addSmallAction(group, createAction(icon, tr("Small")));
    addSmallAction(group, createAction(icon, tr("Small")));
    addSmallAction(group, createAction(icon, tr("Small")));
    addSmallAction(group, createAction(icon, tr("Small")));


    rb->addPage(tr("loooooooooooongpage"));

    resize(800, 400);
}

MainWindow::~MainWindow()
{
}

QAction *MainWindow::createAction(const QIcon &icon, const QString &text)
{
    return new QAction(icon, text, this);
}

QAction *MainWindow::createAction(const QString &text)
{
    return new QAction(text, this);
}

void MainWindow::addMediumAction(QxRibbon::RibbonGroup *group, QAction *action)
{
    RibbonButton *btn = group->addMediumAction(action);
    btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
}

void MainWindow::addSmallAction(QxRibbon::RibbonGroup *group, QAction *action)
{
    RibbonButton *btn = group->addSmallAction(action);
    btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
}
