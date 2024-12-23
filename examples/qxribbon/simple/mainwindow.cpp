#include "mainwindow.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbongallery.h"
#include "qxribbon/ribbongallerygroup.h"

#include <QAction>

QX_RIBBON_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : RibbonWindow(parent)
{
    QIcon icon(":/logo");

    RibbonBar *rb = ribbonBar();

    RibbonPage *page = rb->addPage(tr("page1"));
    RibbonGroup *group = page->addGroup(tr("group1"));
    group->addLargeAction(new QAction(icon, tr("La"), this));
    group->addMediumAction(new QAction(icon, tr("Ma1"), this));
    group->addMediumAction(new QAction(icon, tr("Maaaaaaa2"), this));
    group->addSmallAction(new QAction(tr("Sa11111111"), this));
    group->addSmallAction(new QAction(tr("Sa2222"), this));
    group->addSmallAction(new QAction(tr("Sa3333333"), this));

    page = rb->addPage(tr("loooooooooooongpage2"));
    group = page->addGroup(tr("looooooooooonggroup1"));
    RibbonGallery *gallery = group->addGallery();
    RibbonGalleryGroup *galleryGrp = gallery->addGalleryGroup();
    galleryGrp->addItem(tr("item1"), icon);

    // setRibbonTheme(RibbonTheme::WpsdarkTheme);

    resize(800, 400);
}

MainWindow::~MainWindow()
{
}
