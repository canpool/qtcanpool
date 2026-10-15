#include "tst_global.h"

#include "qxribbon/ribbongallery.h"
#include "qxribbon/ribbongallerygroup.h"

#include <QAction>
#include <QApplication>
#include <QKeyEvent>
#include <QSignalSpy>

QX_RIBBON_USE_NAMESPACE

class tst_RibbonGallery : public QObject
{
    Q_OBJECT
private slots:
    void group();
    void keyboard();
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

void tst_RibbonGallery::keyboard()
{
    RibbonGallery gallery;
    QList<QAction *> actions = {
        new QAction(tr("action1"), this),
        new QAction(tr("action2"), this),
        new QAction(tr("action3"), this),
    };
    gallery.addGalleryGroup(tr("group"), actions);

    RibbonGalleryGroup *view = gallery.currentViewGroup();
    QVERIFY(view);
    QCOMPARE(view->model()->rowCount(), 3);

    auto pressKey = [&gallery](int key) {
        QKeyEvent event(QEvent::KeyPress, key, Qt::NoModifier);
        QApplication::sendEvent(&gallery, &event);
    };

    // With no current item yet, the first Down selects the first one, the second moves on.
    pressKey(Qt::Key_Down);
    QCOMPARE(view->currentIndex().row(), 0);
    pressKey(Qt::Key_Down);
    QCOMPARE(view->currentIndex().row(), 1);

    // Enter activates the current item, exactly as a click would.
    QSignalSpy triggered(actions.at(1), &QAction::triggered);
    pressKey(Qt::Key_Return);
    QCOMPARE(triggered.count(), 1);

    // The movement clamps at both ends instead of wrapping or falling off the model.
    pressKey(Qt::Key_Down);
    QCOMPARE(view->currentIndex().row(), 2);
    pressKey(Qt::Key_Down);
    QCOMPARE(view->currentIndex().row(), 2);
    pressKey(Qt::Key_Up);
    QCOMPARE(view->currentIndex().row(), 1);

    // PageUp / PageDown are the keyboard form of the two scroll buttons; they must not
    // change the current item.
    pressKey(Qt::Key_PageDown);
    pressKey(Qt::Key_PageUp);
    QCOMPARE(view->currentIndex().row(), 1);
}

TEST_ADD(tst_RibbonGallery)

#include "tst_ribbongallery.moc"
