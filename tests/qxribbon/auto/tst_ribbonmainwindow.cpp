#include "tst_global.h"

#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonmainwindow.h"
#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbonquickaccessbar.h"

#include <QAction>
#include <QApplication>

QX_RIBBON_USE_NAMESPACE

/*
 * The ribbon bar tells the window how narrow it may become, and getting that wrong does
 * not surface as a wrong number in a getter: it surfaces as a window that cannot be
 * resized. Windows turns the minimum width of the window into the minimum tracking size
 * of the native window, and RibbonWindow - whose FramelessWidgetDataQt::resizeWidget()
 * clamps the interactive resize against minimumWidth() - reads the value directly, so a
 * window that is left narrower than its own minimum is clamped by the system on the next
 * drag: it jumps instead of following the mouse, and once the minimum has grown past the
 * current width it stops resizing altogether.
 *
 * The minimum is not a constant either. It follows the ribbon content, and the content
 * can change while the window keeps its size - a new quick access entry, a hidden
 * ribbon, another style - which is the case that used to leave the two out of step.
 * These tests pin down both the value and the direction it has to be applied in.
 */

class tst_RibbonMainWindow : public QObject
{
    Q_OBJECT
private slots:
    void minimumWidthCoversRibbonContent();
    void minimumWidthSurvivesMaximizeRoundTrip();
};

static void addRibbonContent(RibbonBar *bar)
{
    for (int i = 0; i < 2; ++i) {
        RibbonPage *page = bar->addPage(QString("Page%1").arg(i));
        RibbonGroup *group = page->addGroup(QString("Group"));
        for (int a = 0; a < 3; ++a) {
            group->addLargeAction(new QAction(QString("Action%1").arg(a), group));
        }
    }
    bar->quickAccessBar()->addAction(new QAction(QString("Quick"), bar));
}

void tst_RibbonMainWindow::minimumWidthCoversRibbonContent()
{
    RibbonMainWindow window;
    addRibbonContent(window.ribbonBar());
    window.resize(900, 600);
    window.show();
    QApplication::processEvents();

    // The window publishes what the ribbon needs, and is at least that wide itself.
    const int initial = window.ribbonBar()->sizeHint().width();
    QCOMPARE(window.minimumWidth(), initial);
    QVERIFY(initial > 0);
    QVERIFY(window.width() >= initial);

    // Shrinking stops at the minimum instead of running past it.
    window.resize(initial + 200, 600);
    QApplication::processEvents();
    QCOMPARE(window.width(), initial + 200);

    window.resize(initial - 1, 600);
    QApplication::processEvents();
    QCOMPARE(window.width(), window.minimumWidth());

    // Now the ribbon asks for more room than the window has, and nothing resizes the
    // window while it happens. Growing the window is part of publishing the new
    // minimum: without it the window stays narrower than it may become and the next
    // resize it is given is the one that jumps.
    const int before = window.width();
    for (int i = 0; i < 12; ++i) {
        window.ribbonBar()->quickAccessBar()->addAction(new QAction(QString("Extra%1").arg(i), window.ribbonBar()));
    }
    QApplication::processEvents();
    QApplication::processEvents();

    const int grown = window.ribbonBar()->sizeHint().width();
    QVERIFY(grown > before);
    QCOMPARE(window.minimumWidth(), grown);
    QVERIFY(window.width() >= grown);

    // And with the window back inside its own minimum, it follows the mouse again.
    window.resize(grown + 100, 600);
    QApplication::processEvents();
    QCOMPARE(window.width(), grown + 100);

    window.resize(grown - 50, 600);
    QApplication::processEvents();
    QCOMPARE(window.width(), grown);
}

void tst_RibbonMainWindow::minimumWidthSurvivesMaximizeRoundTrip()
{
    RibbonMainWindow window;
    addRibbonContent(window.ribbonBar());
    window.resize(900, 600);
    window.show();
    QApplication::processEvents();

    // A maximized window is the size of the screen, so the ribbon minimum is not what
    // limits it there; it has to hold again once the window is back to its normal size.
    window.showMaximized();
    QApplication::processEvents();
    window.showNormal();
    QApplication::processEvents();
    QApplication::processEvents();

    const int needed = window.ribbonBar()->sizeHint().width();
    QCOMPARE(window.minimumWidth(), needed);
    QVERIFY(window.width() >= needed);
}

TEST_ADD(tst_RibbonMainWindow)

#include "tst_ribbonmainwindow.moc"
