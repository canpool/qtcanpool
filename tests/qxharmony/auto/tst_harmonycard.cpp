/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxharmony/harmonycard.h"
#include "qxharmony/harmonytokens.h"

#include <QAccessible>
#include <QApplication>
#include <QImage>

QX_HARMONY_USE_NAMESPACE

namespace
{
// The card's face is inset by the room its shadow needs, so the corner being
// checked is measured from there.
constexpr int kShadowRoom = 8;

QColor pixel(QWidget *widget, int x, int y)
{
    return widget->grab().toImage().pixelColor(x, y);
}

/*! How much ink there is in a part of the widget: anything that is not the page
 *  it sits on. */
int inkedPixels(QWidget *widget, const QRect &area)
{
    const QColor page = widget->palette().color(QPalette::Window);
    const QImage image = widget->grab().toImage();
    int count = 0;
    for (int y = area.top(); y < qMin(area.bottom(), image.height()); ++y) {
        for (int x = area.left(); x < qMin(area.right(), image.width()); ++x) {
            if (image.pixelColor(x, y) != page) {
                ++count;
            }
        }
    }
    return count;
}
}   // namespace

class tst_HarmonyCard : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void theCornerIsTheFamilys();
    void theFaceIsASurfaceOfItsOwn();
    void aHigherLevelCastsMoreShadow();
    void beingHeldLaysAStateOverTheFace();
    void whatItIs();
};

void tst_HarmonyCard::cleanup()
{
    QAccessible::setActive(false);
}

/*! The corner is the family's large one and not a number of the card's own: a
 *  radius of sixteen turns the corner by about that much, so the page still
 *  shows through a few points in from the face's corner and the face has begun a
 *  few points further in. */
void tst_HarmonyCard::theCornerIsTheFamilys()
{
    HarmonyCard card;
    QCOMPARE(card.sizeHint(), QSize(200, 120));
    card.resize(200, 120);

    const QColor face = card.palette().color(QPalette::Base);
    const qreal radius = HarmonyShape::radius(HarmonyShape::Radius::Large);
    QVERIFY(radius > 12.0);

    // Just inside the face's corner there is no face - the corner has been cut,
    // and what is there is the shadow under it.
    QVERIFY(pixel(&card, kShadowRoom + 2, kShadowRoom + 2) != face);
    // Far enough in, the face is there.
    QCOMPARE(pixel(&card, kShadowRoom + int(radius), kShadowRoom + int(radius)), face);
}

/*! A card is a surface rather than a hole in the page: its face is the
 *  toolkit's own surface colour, and it fills the middle of the widget. */
void tst_HarmonyCard::theFaceIsASurfaceOfItsOwn()
{
    HarmonyCard card;
    card.resize(200, 120);

    QCOMPARE(pixel(&card, 100, 60), card.palette().color(QPalette::Base));
    QVERIFY(inkedPixels(&card, QRect(60, 40, 80, 40)) > 0);
}

/*! The level is the whole of what tells two cards apart, and what it buys is
 *  the shadow: a card put higher casts more ink around its face than one at
 *  rest. */
void tst_HarmonyCard::aHigherLevelCastsMoreShadow()
{
    HarmonyCard resting;
    resting.resize(200, 120);
    HarmonyCard lifted;
    lifted.resize(200, 120);
    lifted.setElevation(HarmonyElevation::Level::Sheet);
    QCOMPARE(lifted.elevation(), HarmonyElevation::Level::Sheet);

    // The room the shadow is drawn in, under the face and beside it.
    const QRect floor(20, 120 - kShadowRoom, 160, kShadowRoom + 4);
    QVERIFY(inkedPixels(&lifted, floor) > inkedPixels(&resting, floor));
}

/*! Held, the card takes a state layer: the face changes colour and keeps its
 *  size, because a press here is a layer over the surface rather than a scale -
 *  the page does not move under the user's hand. Which way the layer moves the
 *  lightness is the palette's business: it darkens a light face and lightens a
 *  dark one, and both are the same statement. */
void tst_HarmonyCard::beingHeldLaysAStateOverTheFace()
{
    HarmonyCard card;
    card.resize(200, 120);
    const QSize size = card.size();
    const QColor resting = pixel(&card, 100, 60);

    QTest::mousePress(&card, Qt::LeftButton, Qt::NoModifier, QPoint(100, 60));
    const QColor held = pixel(&card, 100, 60);
    QTest::mouseRelease(&card, Qt::LeftButton, Qt::NoModifier, QPoint(100, 60));
    // The pointer is still over the card after a release, and a hover is a state
    // as well: the face is back to itself only once the pointer has left.
    QEvent leave(QEvent::Leave);
    QApplication::sendEvent(&card, &leave);

    // Held, it is a different colour at the same size - and let go it is the
    // colour it was.
    QVERIFY(held != resting);
    QCOMPARE(card.size(), size);
    QCOMPARE(pixel(&card, 100, 60), resting);
}

/*! What a card is, for an assistive tool: a panel of the page rather than a
 *  control that does something. */
void tst_HarmonyCard::whatItIs()
{
    QAccessible::setActive(true);
    HarmonyCard card;
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&card);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::Client);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

TEST_ADD(tst_HarmonyCard)

#include "tst_harmonycard.moc"
