/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxcupertino/cupertinolistsection.h"

#include <QtGui/QImage>
#include <QtWidgets/QWidget>

QX_CUPERTINO_USE_NAMESPACE

namespace
{
/*! The ground a row is rendered onto, so that "is the card here" is a question
 *  about the row and not about the page behind it. */
const QColor kGround(255, 255, 255);
/*! The card's own colour, given to the row so that it is not the page's: the
 *  palette's base is white in a bare test application, which is the ground. */
const QColor kCard(200, 210, 220);

/*! A row of the asked position, sized and drawn on a ground of its own. */
CupertinoListTile *rowWith(CupertinoListTile::Position position, const QString &title = QStringLiteral("Row"))
{
    auto *row = new CupertinoListTile;
    QPalette palette = row->palette();
    palette.setColor(QPalette::Base, kCard);
    palette.setColor(QPalette::WindowText, Qt::black);
    row->setPalette(palette);
    row->setTitle(title);
    row->setPosition(position);
    row->resize(320, row->sizeHint().height());
    return row;
}

QImage frameOf(QWidget *widget)
{
    QImage image(widget->size(), QImage::Format_ARGB32_Premultiplied);
    image.fill(kGround);
    widget->render(&image);
    return image;
}

/*! How far \a first is from \a second, channel by channel - the answer a "did
 *  the ink land here" question needs, since a wash is a few steps off the face
 *  rather than a different colour. */
int distance(const QColor &first, const QColor &second)
{
    return qAbs(first.red() - second.red()) + qAbs(first.green() - second.green()) + qAbs(first.blue() - second.blue());
}
}   // namespace

class tst_CupertinoListSection : public QObject
{
    Q_OBJECT
private slots:
    void aRowIsOneLineOrTwo();
    void aRowTurnsOnlyTheCornersItsPlaceTurns();
    void theLineBetweenRowsStaysInsideItsRow();
    void aPressOnARowIsAClick();
    void theSectionTellsEveryRowWhereItSits();
};

/*! A row with a second line under the title is taller: the subtitle is a line
 *  of its own and not a smaller title. */
void tst_CupertinoListSection::aRowIsOneLineOrTwo()
{
    QScopedPointer<CupertinoListTile> oneLine(rowWith(CupertinoListTile::Position::Only));
    const int oneLineHeight = oneLine->sizeHint().height();

    QScopedPointer<CupertinoListTile> twoLines(rowWith(CupertinoListTile::Position::Only));
    twoLines->setSubtitle(QStringLiteral("Under it"));
    QVERIFY(twoLines->sizeHint().height() > oneLineHeight);
    QCOMPARE(twoLines->minimumSizeHint().height(), twoLines->sizeHint().height());
}

/*! The card is drawn row by row, so the corners it turns are the row's answer:
 *  the one at the top turns the two above it, the one at the bottom the two
 *  below, and a row between them is a plain band. */
void tst_CupertinoListSection::aRowTurnsOnlyTheCornersItsPlaceTurns()
{
    QScopedPointer<CupertinoListTile> first(rowWith(CupertinoListTile::Position::First));
    const QImage firstFrame = frameOf(first.data());
    // Cut above, square below.
    QVERIFY(distance(firstFrame.pixelColor(0, 0), kCard) > 12);
    QCOMPARE(firstFrame.pixelColor(0, firstFrame.height() - 1), kCard);

    QScopedPointer<CupertinoListTile> last(rowWith(CupertinoListTile::Position::Last));
    const QImage lastFrame = frameOf(last.data());
    // The other way round.
    QCOMPARE(lastFrame.pixelColor(0, 0), kCard);
    QVERIFY(distance(lastFrame.pixelColor(0, lastFrame.height() - 1), kCard) > 12);

    QScopedPointer<CupertinoListTile> middle(rowWith(CupertinoListTile::Position::Middle));
    const QImage middleFrame = frameOf(middle.data());
    // Square to its neighbours on both sides: a card has no cut in its middle.
    QCOMPARE(middleFrame.pixelColor(0, 0), kCard);
    QCOMPARE(middleFrame.pixelColor(0, middleFrame.height() - 2), kCard);

    QScopedPointer<CupertinoListTile> alone(rowWith(CupertinoListTile::Position::Only));
    const QImage aloneFrame = frameOf(alone.data());
    // Both ends cut, because a row alone is the whole card.
    QVERIFY(distance(aloneFrame.pixelColor(0, 0), kCard) > 12);
    QVERIFY(distance(aloneFrame.pixelColor(0, aloneFrame.height() - 1), kCard) > 12);
}

/*! The line under a row separates it from the row below, so it is drawn inside
 *  the row and starts where the text starts - a line to the card's edge would
 *  cut the card in two rather than separate two rows on it. */
void tst_CupertinoListSection::theLineBetweenRowsStaysInsideItsRow()
{
    QScopedPointer<CupertinoListTile> middle(rowWith(CupertinoListTile::Position::Middle));
    const QImage middleFrame = frameOf(middle.data());
    const int bottom = middleFrame.height() - 1;

    // Ink on the line under the text, and none of it in the inset before it.
    QVERIFY2(distance(middleFrame.pixelColor(middleFrame.width() / 2, bottom), kCard) >= 6,
             qPrintable(QStringLiteral("the line under the row is %1 away from the card")
                            .arg(distance(middleFrame.pixelColor(middleFrame.width() / 2, bottom), kCard))));
    QCOMPARE(middleFrame.pixelColor(4, bottom), kCard);

    // The last row has nothing under it, so it draws nothing.
    QScopedPointer<CupertinoListTile> last(rowWith(CupertinoListTile::Position::Last));
    const QImage lastFrame = frameOf(last.data());
    QCOMPARE(lastFrame.pixelColor(lastFrame.width() / 2, lastFrame.height() - 1), kCard);
}

/*! A row is a thing to press: the press and the release on it are one click,
 *  and what the click means is the application's. */
void tst_CupertinoListSection::aPressOnARowIsAClick()
{
    QScopedPointer<CupertinoListTile> row(rowWith(CupertinoListTile::Position::Only));
    row->show();
    QCoreApplication::processEvents();

    QSignalSpy clicked(row.data(), &CupertinoListTile::clicked);
    QTest::mouseClick(row.data(), Qt::LeftButton, Qt::NoModifier, QPoint(row->width() / 2, row->height() / 2));
    QCOMPARE(clicked.count(), 1);

    // Enter is the keyboard's way of saying the same thing.
    QTest::keyClick(row.data(), Qt::Key_Return);
    QCOMPARE(clicked.count(), 2);
}

/*! The group is what knows the order, so it is the group that tells each row
 *  where it sits - the application never assigns a position by hand. */
void tst_CupertinoListSection::theSectionTellsEveryRowWhereItSits()
{
    CupertinoListSection section;
    CupertinoListTile *first = section.addTile(QStringLiteral("First"));
    CupertinoListTile *middle = section.addTile(QStringLiteral("Middle"));
    CupertinoListTile *last = section.addTile(QStringLiteral("Last"));

    QCOMPARE(first->position(), CupertinoListTile::Position::First);
    QCOMPARE(middle->position(), CupertinoListTile::Position::Middle);
    QCOMPARE(last->position(), CupertinoListTile::Position::Last);

    // A group of one is a card of one: the row alone turns all four corners.
    CupertinoListSection one;
    QCOMPARE(one.addTile(QStringLiteral("Only"))->position(), CupertinoListTile::Position::Only);
}

TEST_ADD(tst_CupertinoListSection)

#include "tst_cupertinolistsection.moc"
