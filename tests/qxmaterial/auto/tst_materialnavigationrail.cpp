/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialnavigationrail.h"

#include <QAccessible>
#include <QAction>
#include <QSignalSpy>
#include <QWidget>

QX_MATERIAL_USE_NAMESPACE

namespace
{
using QxMaterial::MaterialNavigationRail;

// The measures the rail lays out with: eighty wide, twelve down to the first
// destination, and fifty-six per destination with the indicator in the top
// thirty-two of it. Aiming at one means knowing where it stands.
constexpr int kRailWidth = 80;
constexpr int kTopPadding = 12;
constexpr int kItemHeight = 56;
constexpr int kHeaderGap = 12;

QPoint itemPoint(int index)
{
    return QPoint(kRailWidth / 2, kTopPadding + index * kItemHeight + kItemHeight / 2);
}

MaterialNavigationRail *railWith(const QStringList &labels)
{
    MaterialNavigationRail *rail = new MaterialNavigationRail;
    for (const QString &label : labels) {
        rail->addDestination(label);
    }
    rail->resize(rail->sizeHint());
    return rail;
}
}   // namespace

class tst_MaterialNavigationRail : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void theRailHoldsWhatItIsGiven();
    void aClickPutsTheDestinationInForce();
    void theHeaderStandsAboveThem();
    void theColumnKeepsItsWidthAndStaysInTheMiddle();
    void aDestinationNobodyCanUseIsNotPutInForce();
    void theKeyboardWalksTheColumn();
    void whatAnAssistiveToolReads();
};

void tst_MaterialNavigationRail::cleanup()
{
    QAccessible::setActive(false);
}

/*! A destination is a QAction, so a rail is a column of things an application
 *  already knows how to use. */
void tst_MaterialNavigationRail::theRailHoldsWhatItIsGiven()
{
    std::unique_ptr<MaterialNavigationRail> rail(railWith({QStringLiteral("Home"), QStringLiteral("Mail")}));
    QCOMPARE(rail->destinations().count(), 2);
    // Material puts the first destination in force: a rail with nothing in
    // force is a rail that has not answered where you are.
    QCOMPARE(rail->currentIndex(), 0);

    rail->removeDestination(rail->destinations().first());
    QCOMPARE(rail->destinations().count(), 1);

    rail->clear();
    QCOMPARE(rail->destinations().count(), 0);
    QVERIFY(!rail->currentDestination());
}

void tst_MaterialNavigationRail::aClickPutsTheDestinationInForce()
{
    std::unique_ptr<MaterialNavigationRail> rail(railWith({QStringLiteral("Home"), QStringLiteral("Mail")}));
    QAction *mail = rail->destinations().at(1);
    QSignalSpy changed(rail.get(), &MaterialNavigationRail::currentIndexChanged);

    QTest::mouseClick(rail.get(), Qt::LeftButton, Qt::NoModifier, itemPoint(1));

    QCOMPARE(rail->currentIndex(), 1);
    QCOMPARE(rail->currentDestination(), mail);
    QCOMPARE(changed.count(), 1);
}

/*! The top of the rail is not a destination: it is where the one thing that
 *  belongs above them goes, and it is given rather than drawn. */
void tst_MaterialNavigationRail::theHeaderStandsAboveThem()
{
    std::unique_ptr<MaterialNavigationRail> rail(railWith({QStringLiteral("Home")}));
    QVERIFY(!rail->headerWidget());

    QWidget *header = new QWidget;
    header->setFixedSize(48, 48);
    rail->setHeaderWidget(header);

    QCOMPARE(rail->headerWidget(), header);
    QCOMPARE(header->parent(), rail.get());
    // Above them, and clear of them: the first destination starts under it.
    QVERIFY(header->geometry().bottom() < kTopPadding + 48 + kItemHeight);
    QVERIFY(rail->sizeHint().height() > 48 + kItemHeight);
}

/*! A rail is a column beside the room the destinations open into, so it is the
 *  room that grows and not the column: it is fixed across, and a rail made
 *  wider than the token by hand stands its column in the middle of the room
 *  rather than against the left edge of it. Where the column stands is worked
 *  out when it is asked for, because a widget that has never been shown is not
 *  told it was resized - and a column laid out when the room changed answers
 *  from a room it no longer has. */
void tst_MaterialNavigationRail::theColumnKeepsItsWidthAndStaysInTheMiddle()
{
    std::unique_ptr<MaterialNavigationRail> rail(railWith({QStringLiteral("Home"), QStringLiteral("Mail")}));
    QCOMPARE(rail->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);

    rail->resize(240, 200);
    QWidget *header = new QWidget;
    header->setFixedSize(48, 48);
    rail->setHeaderWidget(header);

    const int middle = rail->width() / 2;
    QCOMPARE(header->geometry().x() + header->width() / 2, middle);

    // Aiming at the middle of the room it was given lands on the column: the
    // second destination is the one under the header, and it is the one that
    // answers.
    QTest::mouseClick(rail.get(), Qt::LeftButton, Qt::NoModifier,
                      QPoint(middle, kTopPadding + 48 + kHeaderGap + kItemHeight + kItemHeight / 2));
    QCOMPARE(rail->currentIndex(), 1);
}

void tst_MaterialNavigationRail::aDestinationNobodyCanUseIsNotPutInForce()
{
    std::unique_ptr<MaterialNavigationRail> rail(railWith({QStringLiteral("Home"), QStringLiteral("Mail")}));
    rail->setCurrentIndex(0);
    rail->destinations().at(1)->setEnabled(false);

    QTest::mouseClick(rail.get(), Qt::LeftButton, Qt::NoModifier, itemPoint(1));
    QCOMPARE(rail->currentIndex(), 0);

    QTest::mouseClick(rail.get(), Qt::LeftButton, Qt::NoModifier, itemPoint(0));
    QCOMPARE(rail->currentIndex(), 0);
}

void tst_MaterialNavigationRail::theKeyboardWalksTheColumn()
{
    std::unique_ptr<MaterialNavigationRail> rail(
        railWith({QStringLiteral("Home"), QStringLiteral("Mail"), QStringLiteral("Notes")}));

    QTest::keyClick(rail.get(), Qt::Key_Down);
    QCOMPARE(rail->currentIndex(), 1);
    QTest::keyClick(rail.get(), Qt::Key_Up);
    QCOMPARE(rail->currentIndex(), 0);
    // The top is a wall.
    QTest::keyClick(rail.get(), Qt::Key_Up);
    QCOMPARE(rail->currentIndex(), 0);
}

void tst_MaterialNavigationRail::whatAnAssistiveToolReads()
{
    std::unique_ptr<MaterialNavigationRail> rail(railWith({QStringLiteral("Home")}));
    QAccessible::setActive(true);
    QAccessibleInterface *face = QAccessible::queryAccessibleInterface(rail.get());
    QVERIFY(face);
    // A short column of destinations is a list, and it is the column that an
    // assistive tool is meant to walk - not the room beside it.
    QCOMPARE(face->role(), QAccessible::List);
}

TEST_ADD(tst_MaterialNavigationRail)

#include "tst_materialnavigationrail.moc"
