#include "tst_global.h"

#include "qxribbon/ribbongallery.h"
#include "qxribbon/ribbongallerygroup.h"

#include <QAction>

QX_RIBBON_USE_NAMESPACE

class tst_RibbonGallery : public QObject
{
    Q_OBJECT
private slots:
    void group();
};

void tst_RibbonGallery::group()
{
    RibbonGallery gallery;

    QCOMPARE(gallery.currentViewGroup(), nullptr);

    RibbonGalleryGroup *g1 = gallery.addGalleryGroup();

    QVERIFY(gallery.currentViewGroup() != nullptr);
    QVERIFY(gallery.currentViewGroup() != g1);

    QList<QAction *> actions = {
        new QAction(tr("action1"), this),
        new QAction(tr("action2"), this),
        new QAction(tr("action3"), this),
        new QAction(tr("action4"), this),
    };
    RibbonGalleryGroup *g2 = gallery.addGalleryGroup(tr("group2"), actions);

    QVERIFY(gallery.currentViewGroup() != nullptr);
    QVERIFY(gallery.currentViewGroup() != g1);
    QVERIFY(gallery.currentViewGroup() != g2);

    RibbonGalleryGroup *g3 = new RibbonGalleryGroup();
    gallery.addGalleryGroup(g3);

    QVERIFY(gallery.currentViewGroup() != nullptr);
    QVERIFY(gallery.currentViewGroup() != g1);
    QVERIFY(gallery.currentViewGroup() != g2);
    QVERIFY(gallery.currentViewGroup() != g3);

    RibbonGalleryGroup *vg = gallery.currentViewGroup();
    gallery.setCurrentViewGroup(g2);
    QCOMPARE(gallery.currentViewGroup(), vg);
}

TEST_ADD(tst_RibbonGallery)

#include "tst_ribbongallery.moc"
