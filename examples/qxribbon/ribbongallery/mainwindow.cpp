#include "mainwindow.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbongallery.h"
#include "qxribbon/ribbongallerygroup.h"

#include <QAction>
#include <QToolButton>
#include <QDebug>

QX_RIBBON_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : RibbonWindow(parent)
{
    QIcon icon(":/logo");

    RibbonBar *rb = ribbonBar();

    RibbonPage *page = rb->addPage(tr("page1"));
    RibbonGroup *group = page->addGroup(tr("group1"));

    RibbonGallery *gallery = group->addGallery();
    QList<QAction *> galleryActions;
    auto lambdaCreateGalleryAction = [this](const QString &text, const QIcon &icon) -> QAction * {
        QAction *act = new QAction(icon, text, this);
        return act;
    };
    for (int i = 0; i < 30; ++i) {
        galleryActions.append(lambdaCreateGalleryAction(tr("action%1").arg(i), icon));
    }
    RibbonGalleryGroup *galleryGroup1 = gallery->addGalleryGroup(tr("gallery-group-1"), galleryActions);
    galleryGroup1->setGridMinimumWidth(40);
    galleryActions.clear();
    for (int i = 0; i < 30; ++i) {
        galleryActions.append(lambdaCreateGalleryAction(tr("gallery action %1").arg(i), icon));
    }
    RibbonGalleryGroup *galleryGroup2 = gallery->addGalleryGroup(tr("gallery-group-2"), galleryActions);
    galleryGroup2->setGalleryGroupStyle(RibbonGalleryGroup::IconWithWordWrapText);
    galleryGroup2->setGridMinimumWidth(40);
    galleryActions.clear();
    for (int i = 0; i < 30; ++i) {
        galleryActions.append(lambdaCreateGalleryAction(tr("gallery action %1").arg(i), icon));
    }
    RibbonGalleryGroup *galleryGroup3 = gallery->addGalleryGroup(tr("gallery-group-3"), galleryActions);
    galleryGroup3->setGalleryGroupStyle(RibbonGalleryGroup::IconOnly);
    galleryGroup3->setDisplayRow(RibbonGalleryGroup::DisplayTwoRow);
    galleryGroup3->setGridMinimumWidth(40);
    galleryActions.clear();
    for (int i = 0; i < 100; ++i) {
        galleryActions.append(lambdaCreateGalleryAction(tr("gallery action %1").arg(i), icon));
    }
    RibbonGalleryGroup *galleryGroup4 = gallery->addGalleryGroup(tr("gallery-group-4"), galleryActions);
    galleryGroup4->setGalleryGroupStyle(RibbonGalleryGroup::IconOnly);
    galleryGroup4->setDisplayRow(RibbonGalleryGroup::DisplayThreeRow);
    // galleryGroup4->setGridMinimumWidth(40);
    gallery->setCurrentViewGroup(galleryGroup1);

    rb->addPage(tr("loooooooooooongpage2"));

    resize(800, 400);
}

MainWindow::~MainWindow()
{
}
