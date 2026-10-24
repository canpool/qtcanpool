/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialdrawer.h"

#include "qxcontrols/qxstatelayer.h"

#include "qxtheme/qxthememotion.h"
#include "qxtheme/qxthememanager.h"

#include <QtGui/QPainter>
#include <QAccessible>
#include <QAction>

QX_MATERIAL_USE_NAMESPACE

namespace
{
// The drawer's own measures, read the way a reader reads them.
constexpr int kPanelWidth = 256;
constexpr int kItemHeight = 48;
constexpr int kSectionHeight = 32;

/*! Less motion for as long as this lives: a drawer that slides takes a quarter
 *  of a second to arrive, and reading where it arrived is not a test of the
 *  sliding. The switch is put back to what it was, because it is the whole
 *  process's - the tests after this one are not made silent by it. */
class QuietMotion
{
public:
    QuietMotion()
        : m_was(QxTheme::isMotionReduced())
    {
        // The theme manager installs the theme's own motion when it comes up,
        // so it is brought up first - a switch set before that would be a
        // switch set on something not yet listening.
        QxTheme::QxThemeManager::instance();
        QxTheme::setMotionReduced(true);
    }
    ~QuietMotion()
    {
        QxTheme::setMotionReduced(m_was);
    }
private:
    bool m_was;
};

/*! How many pixels of \a room are the accent itself, allowance made for the
 *  one or two of anti-aliasing either side of it. */
int accentPixels(const QImage &image, const QRect &room)
{
    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
    int count = 0;
    const QRect room_ = room.intersected(image.rect());
    for (int y = room_.top(); y <= room_.bottom(); ++y) {
        for (int x = room_.left(); x <= room_.right(); ++x) {
            const QColor pixel = image.pixelColor(x, y);
            if (qAbs(pixel.red() - accent.red()) <= 8 && qAbs(pixel.green() - accent.green()) <= 8 &&
                qAbs(pixel.blue() - accent.blue()) <= 8) {
                ++count;
            }
        }
    }
    return count;
}

/*! Whether \a pixel is \a expected, allowing for the anti-aliasing of whatever
 *  was drawn over it. */
bool alike(const QColor &pixel, const QColor &expected)
{
    return qAbs(pixel.red() - expected.red()) <= 3 && qAbs(pixel.green() - expected.green()) <= 3 &&
           qAbs(pixel.blue() - expected.blue()) <= 3;
}

/*! The sheet a temporary drawer comes out over: the one child of \a host that
 *  covers all of it and is not the drawer itself. */
QWidget *sheetOf(QWidget *host, QWidget *drawer)
{
    for (QWidget *child : host->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly)) {
        if (child != drawer && child->isVisible() && child->geometry() == host->rect()) {
            return child;
        }
    }
    return Q_NULLPTR;
}
}   // namespace

class tst_MaterialDrawer : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void theDrawerIsAPanelThatIsClosedUntilAsked();
    void aSectionPushesTheDestinationsDown();
    void aPersistentDrawerGivesThePageItsRoomBack();
    void aTemporaryDrawerComesOutOverThePage();
    void pressingTheSheetPutsTheDrawerAway();
    void theDestinationInForceWearsThePill();
    void pressingADestinationPutsItInForce();
    void theKeyboardWalksTheDestinations();
    void theDrawerAnswersAsAList();
    void aPanelNobodyCanUseIsWrittenFainter();
};

namespace
{
/*! A page to put drawers on: wide enough for the panel and the rest of it. */
QWidget *makePage()
{
    auto *page = new QWidget;
    page->resize(600, 400);
    page->show();
    QTest::qWaitForWindowExposed(page);
    return page;
}

/*! A drawer of three destinations, the size Material gives a panel. */
MaterialDrawer *makeDrawer(QWidget *page)
{
    auto *drawer = new MaterialDrawer(page);
    drawer->addDestination(QStringLiteral("Inbox"));
    drawer->addDestination(QStringLiteral("Starred"));
    drawer->addDestination(QStringLiteral("Settings"));
    return drawer;
}
}   // namespace

void tst_MaterialDrawer::initTestCase()
{
    // The names are drawn with whatever font the machine has, and the tests
    // read pixels, so there has to be one.
    qtcGiveFonts();
}

/*! A drawer is closed until asked, and it is as wide as Material's panel. */
void tst_MaterialDrawer::theDrawerIsAPanelThatIsClosedUntilAsked()
{
    QWidget *page = makePage();
    MaterialDrawer *drawer = makeDrawer(page);

    QCOMPARE(MaterialDrawer::panelWidth(), kPanelWidth);
    QVERIFY(drawer->variant() == MaterialDrawer::Variant::Temporary);
    QVERIFY(!drawer->isOpen());
    QVERIFY(!drawer->isVisible());

    // Three destinations and the padding above and below them, and nothing
    // else: a drawer asks for what its list needs.
    QCOMPARE(drawer->sizeHint().width(), kPanelWidth);
    QCOMPARE(drawer->sizeHint().height(), 8 + 3 * kItemHeight + 8);
    // It may be made shorter than what it asks for, but not shorter than one
    // destination and the padding.
    QCOMPARE(drawer->minimumSizeHint().height(), 8 + kItemHeight + 8);
}

/*! A section heading is a row of its own, and the destinations under it start
 *  below it rather than where they would have without one. */
void tst_MaterialDrawer::aSectionPushesTheDestinationsDown()
{
    QWidget *page = makePage();
    auto *drawer = new MaterialDrawer(page);
    drawer->setVariant(MaterialDrawer::Variant::Persistent);
    drawer->addDestination(QStringLiteral("Inbox"));
    drawer->addSection(QStringLiteral("Labels"));
    drawer->addDestination(QStringLiteral("Starred"));
    drawer->open();

    const QRect first = drawer->destinationRect(0);
    const QRect second = drawer->destinationRect(1);
    QVERIFY(first.isValid());
    QVERIFY(second.isValid());
    QCOMPARE(first.height(), kItemHeight);
    // Rects count their bottom edge as part of themselves, so the measure is
    // from top to top: a destination, then the section under it.
    QCOMPARE(second.top() - first.top(), kItemHeight + kSectionHeight);
}

/*! A persistent drawer is part of the page: open it and the page has less
 *  room, close it and the room is back - at once, because what moved was the
 *  page and not the drawer. */
void tst_MaterialDrawer::aPersistentDrawerGivesThePageItsRoomBack()
{
    QWidget *page = makePage();
    MaterialDrawer *drawer = makeDrawer(page);
    drawer->setVariant(MaterialDrawer::Variant::Persistent);

    int changes = 0;
    connect(drawer, &MaterialDrawer::openChanged, this, [&changes]() {
        ++changes;
    });

    drawer->open();
    QVERIFY(drawer->isOpen());
    QVERIFY(drawer->isVisible());
    QCOMPARE(drawer->width(), kPanelWidth);
    QCOMPARE(changes, 1);

    drawer->close();
    QVERIFY(!drawer->isOpen());
    QCOMPARE(drawer->width(), 0);
    QCOMPARE(changes, 2);

    // Asking for what is already so is not a change.
    drawer->close();
    QCOMPARE(changes, 2);
}

/*! A temporary drawer comes out over the page it was put on, fills its height,
 *  and dims what is under it with a sheet. */
void tst_MaterialDrawer::aTemporaryDrawerComesOutOverThePage()
{
    QuietMotion quiet;
    QWidget *page = makePage();
    MaterialDrawer *drawer = makeDrawer(page);

    drawer->open();

    QVERIFY(drawer->isOpen());
    QVERIFY(drawer->isVisible());
    // It slides in, so where it arrives is asked for rather than assumed: with
    // motion reduced it is there already, and with motion it is there in a
    // quarter of a second.
    QTRY_COMPARE(drawer->x(), 0);
    QCOMPARE(drawer->width(), kPanelWidth);
    QCOMPARE(drawer->height(), page->height());
    QVERIFY2(sheetOf(page, drawer) != Q_NULLPTR, "no sheet under the drawer");

    drawer->close();
    QTRY_VERIFY(!drawer->isVisible());
    QTRY_COMPARE(sheetOf(page, drawer), static_cast<QWidget *>(Q_NULLPTR));
}

/*! The sheet is the way out a temporary drawer gives: pressing the dimmed page
 *  puts the drawer away, the way pressing outside a flyout does. */
void tst_MaterialDrawer::pressingTheSheetPutsTheDrawerAway()
{
    QuietMotion quiet;
    QWidget *page = makePage();
    MaterialDrawer *drawer = makeDrawer(page);
    drawer->open();

    QWidget *sheet = sheetOf(page, drawer);
    QVERIFY(sheet);
    QTest::mouseClick(sheet, Qt::LeftButton, Qt::NoModifier, sheet->rect().center());

    QVERIFY(!drawer->isOpen());
    QTRY_VERIFY(!drawer->isVisible());
}

/*! The destination in force wears the pill - the accent's own shape - and the
 *  ones that are not do not. */
void tst_MaterialDrawer::theDestinationInForceWearsThePill()
{
    QWidget *page = makePage();
    MaterialDrawer *drawer = makeDrawer(page);
    drawer->setVariant(MaterialDrawer::Variant::Persistent);
    drawer->open();
    drawer->setCurrentIndex(0);

    const QImage frame = drawer->grab().toImage();
    const QColor page_ = drawer->palette().color(QPalette::Window);
    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
    const QColor pill =
        QxControls::QxStateLayer::layered(page_, accent, QxControls::QxStateLayer::selectedFillOpacityFor(page_));

    // Between the icon and the name, on the pill's own line: at sixty it is the
    // one in force, at sixty through the next row it is not.
    const QRect first = drawer->destinationRect(0);
    const QRect second = drawer->destinationRect(1);
    const QPoint inForce(60, first.center().y());
    const QPoint resting(60, second.center().y());
    QVERIFY2(alike(frame.pixelColor(inForce), pill),
             qPrintable(QStringLiteral("pill %1 vs %2").arg(frame.pixelColor(inForce).name()).arg(pill.name())));
    QVERIFY(alike(frame.pixelColor(resting), page_));

    // And its name turns to the accent with it.
    QVERIFY(accentPixels(frame, QRect(first.left() + 68, first.top() + 12, 120, 24)) > 0);
}

/*! Pressing a destination puts it in force, and says so once. */
void tst_MaterialDrawer::pressingADestinationPutsItInForce()
{
    QWidget *page = makePage();
    MaterialDrawer *drawer = makeDrawer(page);
    drawer->setVariant(MaterialDrawer::Variant::Persistent);
    drawer->open();

    int changes = 0;
    connect(drawer, &MaterialDrawer::currentIndexChanged, this, [&changes]() {
        ++changes;
    });

    const QRect second = drawer->destinationRect(1);
    QTest::mouseClick(drawer, Qt::LeftButton, Qt::NoModifier, second.center());
    QCOMPARE(drawer->currentIndex(), 1);
    QCOMPARE(changes, 1);

    // Pressing the one already in force is not a change.
    QTest::mouseClick(drawer, Qt::LeftButton, Qt::NoModifier, second.center());
    QCOMPARE(changes, 1);
}

/*! The keyboard walks the destinations and puts what it walks to in force: in
 *  a drawer the walking is choosing, as it is along the rail. */
void tst_MaterialDrawer::theKeyboardWalksTheDestinations()
{
    QWidget *page = makePage();
    MaterialDrawer *drawer = makeDrawer(page);
    drawer->setVariant(MaterialDrawer::Variant::Persistent);
    drawer->open();
    drawer->setCurrentIndex(0);
    drawer->setFocus();

    QTest::keyClick(drawer, Qt::Key_Down);
    QCOMPARE(drawer->currentIndex(), 1);
    QTest::keyClick(drawer, Qt::Key_Down);
    QCOMPARE(drawer->currentIndex(), 2);
    // The end of the list is the end: it does not wrap round.
    QTest::keyClick(drawer, Qt::Key_Down);
    QCOMPARE(drawer->currentIndex(), 2);
    QTest::keyClick(drawer, Qt::Key_Up);
    QCOMPARE(drawer->currentIndex(), 1);
}

/*! To an assistive tool the drawer is a list of destinations. */
void tst_MaterialDrawer::theDrawerAnswersAsAList()
{
    QWidget *page = makePage();
    MaterialDrawer *drawer = makeDrawer(page);

    QAccessibleInterface *face = QAccessible::queryAccessibleInterface(drawer);
    QVERIFY(face);
    QCOMPARE(face->role(), QAccessible::List);

    QCOMPARE(drawer->destinations().count(), 3);
    QCOMPARE(drawer->currentDestination(), drawer->destinations().at(0));
    QCOMPARE(drawer->destinations().at(1)->text(), QStringLiteral("Starred"));
}

/*! A panel nobody can use is written a step down: the destinations, the section
 *  heads and the pill together. Pinned first, so the frame that differs is the
 *  panel's own drawing.
 */
void tst_MaterialDrawer::aPanelNobodyCanUseIsWrittenFainter()
{
    QWidget *page = makePage();
    MaterialDrawer *drawer = makeDrawer(page);
    drawer->setVariant(MaterialDrawer::Variant::Persistent);
    drawer->setOpen(true);
    QTest::qWaitForWindowExposed(drawer);
    qtcPinDisabledColours(drawer);

    const QImage live = drawer->grab().toImage();
    drawer->setEnabled(false);
    const QImage faded = drawer->grab().toImage();
    QVERIFY(faded != live);

    delete page;
}

TEST_ADD(tst_MaterialDrawer)

#include "tst_materialdrawer.moc"
