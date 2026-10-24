/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentbadge.h"
#include "qxfluent/fluentcard.h"

#include "qxtheme/qxthememanager.h"

#include <QAccessible>
#include <QHBoxLayout>
#include <QImage>

QX_FLUENT_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
const QColor kAccent(0, 0, 255);

QColor pixel(QWidget *widget, int x, int y)
{
    return widget->grab().toImage().pixelColor(x, y);
}

/*! How many pixels of a row are darker than  page: a shadow is a falloff, so
 *  where it is faintest one pixel is a coin toss, and a count is not. The
 *  comparison is against the page the card drew, not against white, because
 *  the page is whatever the theme says it is. */
int darkerThan(QWidget *widget, int y, int from, int to, const QColor &page)
{
    const QImage image = widget->grab().toImage();
    int count = 0;
    for (int x = from; x < to; ++x) {
        if (image.pixelColor(x, y).red() < page.red()) {
            ++count;
        }
    }
    return count;
}

/*! Reaches the corner radius the base keeps to itself. What a surface is
 *  rounded by is the badge's shape, and there is one rule for a rectangle that
 *  is not square: half its short side. */
class ProbeBadge : public FluentBadge
{
public:
    using FluentBadge::cornerRadius;
};
}   // namespace

class tst_FluentBadge : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void aCountIsAPillAndNoCountIsADot();
    void oneFigureIsACircleAndTwoAreAPill();
    void aRowWithRoomToSpareDoesNotStretchIt();
    void theCornersTurnHalfTheShortSide();
    void theBadgeIsTheAccent();
    void pastTheMaximumTheCountIsClipped();
    void whatItSays();
private:
    bool m_hadOverride = false;
};

void tst_FluentBadge::init()
{
    m_hadOverride = QxTheme::QxThemeManager::instance()->hasAccentOverride();
    QxTheme::QxThemeManager::instance()->setAccentColor(kAccent);
}

void tst_FluentBadge::cleanup()
{
    if (!m_hadOverride) {
        QxTheme::QxThemeManager::instance()->setAccentColor(QColor());
    }
}

/*! A badge with nothing to count is the dot; a badge with a number is the
 *  pill, and the pill is tall enough for the number to sit in. */
void tst_FluentBadge::aCountIsAPillAndNoCountIsADot()
{
    FluentBadge dot;
    QCOMPARE(dot.sizeHint(), QSize(6, 6));

    FluentBadge counted;
    counted.setCount(5);
    const QSize pill = counted.sizeHint();
    QCOMPARE(pill.height(), 16);
    QVERIFY(pill.width() >= 16);
}

/*! One figure is read at a glance, so it is a circle; two are already a number
 *  and the badge grows sideways for them. The count decides the shape. */
void tst_FluentBadge::oneFigureIsACircleAndTwoAreAPill()
{
    FluentBadge one;
    one.setCount(5);
    QCOMPARE(one.sizeHint().width(), one.sizeHint().height());

    FluentBadge two;
    two.setCount(12);
    QVERIFY(two.sizeHint().width() > two.sizeHint().height());
    QCOMPARE(two.sizeHint().height(), one.sizeHint().height());
}

/*! A round end is half the short side. Asking for "as round as it gets" is the
 *  same answer on a square and a different one on everything else: past half
 *  the short side the corner keeps the radii it was given and is drawn as an
 *  oval arc, so a badge holding "12" came out an ellipse rather than a pill. */
void tst_FluentBadge::theCornersTurnHalfTheShortSide()
{
    ProbeBadge one;
    one.setCount(5);
    QCOMPARE(one.cornerRadius(), one.sizeHint().height() / 2.0);
    // One figure in a square cell: half the side is what makes it a circle.
    QCOMPARE(one.sizeHint().width(), one.sizeHint().height());

    ProbeBadge two;
    two.setCount(12);
    QVERIFY(two.sizeHint().width() > two.sizeHint().height());
    QCOMPARE(two.cornerRadius(), two.sizeHint().height() / 2.0);
    QVERIFY(two.cornerRadius() * 2 <= two.sizeHint().width());
}

/*! Put in a row with room to spare, the badge is still the size its count asked
 *  for: a circle stretched one way only is an ellipse, which is what the demo's
 *  badge page had become - twice. */
void tst_FluentBadge::aRowWithRoomToSpareDoesNotStretchIt()
{
    QWidget row;
    auto *layout = new QHBoxLayout(&row);
    auto *badge = new FluentBadge(&row);
    badge->setCount(5);
    const QSize asked = badge->sizeHint();
    layout->addWidget(badge);
    layout->addStretch(1);
    row.resize(400, 80);
    row.show();
    QVERIFY(QTest::qWaitForWindowExposed(&row));

    QCOMPARE(badge->size(), asked);
}

/*! The accent is what a badge points with, so the accent is what it is. */
void tst_FluentBadge::theBadgeIsTheAccent()
{
    FluentBadge badge;
    badge.setCount(5);
    badge.resize(badge.sizeHint());

    // Inside the pill, off the glyph: the fill.
    QCOMPARE(pixel(&badge, 1, 8), kAccent);
}

/*! "150" is not a number a badge has room for, and a badge that grew to fit
 *  would stop being a badge. */
void tst_FluentBadge::pastTheMaximumTheCountIsClipped()
{
    FluentBadge five;
    five.setCount(5);
    FluentBadge many;
    many.setCount(150);
    const QSize before = many.sizeHint();

    QVERIFY(before.width() > five.sizeHint().width());

    // And the maximum is the application's to move.
    many.setMaximumCount(9);
    QVERIFY(many.sizeHint().width() < before.width());
}

/*! A badge is text an assistive tool reads, not a shape it guesses at. */
void tst_FluentBadge::whatItSays()
{
    FluentBadge badge;
    badge.setCount(5);
    QCOMPARE(badge.accessibleName(), QStringLiteral("5"));

    badge.setCount(150);
    QCOMPARE(badge.accessibleName(), QStringLiteral("99+"));
}

class tst_FluentCard : public QObject
{
    Q_OBJECT
private slots:
    void aCardCastsTheShadowItsElevationAsksFor();
    void theElevationDecidesTheRoomInside();
    void aCardIsAGrouping();
private:
    bool m_wasActive = false;
};

/*! Elevation is what a card says, and a shadow is how it says it: a card at a
 *  level above the page puts ink on the page round its edges, a card on the
 *  page does not. */
void tst_FluentCard::aCardCastsTheShadowItsElevationAsksFor()
{
    FluentCard flat;
    flat.setElevation(FluentElevation::Level::None);
    flat.resize(120, 60);
    const QColor page = pixel(&flat, 60, 30);
    QCOMPARE(darkerThan(&flat, 30, 0, 13, page), 0);

    FluentCard raised;
    raised.setElevation(FluentElevation::Level::Depth8);
    raised.resize(120, 60);
    QVERIFY(darkerThan(&raised, 30, 0, 13, page) > 0);
}

/*! The shadow needs room and the card keeps it, so the contents never sit on
 *  the shadow of the surface they are on. */
void tst_FluentCard::theElevationDecidesTheRoomInside()
{
    FluentCard flat;
    flat.setElevation(FluentElevation::Level::None);
    QCOMPARE(flat.contentsMargins().left(), 0);

    FluentCard raised;
    raised.setElevation(FluentElevation::Level::Depth8);
    QCOMPARE(raised.contentsMargins().left(), FluentCard::shadowMargin(FluentElevation::Level::Depth8));
    QVERIFY(raised.contentsMargins().left() > 0);
}

void tst_FluentCard::aCardIsAGrouping()
{
    m_wasActive = QAccessible::isActive();
    QAccessible::setActive(true);

    FluentCard card;
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&card);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::Grouping);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));

    QAccessible::setActive(m_wasActive);
}

TEST_ADD(tst_FluentBadge)
TEST_ADD(tst_FluentCard)

#include "tst_fluentbadge.moc"
