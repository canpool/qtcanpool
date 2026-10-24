/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialrating.h"

#include "qxcontrols/qxstatelayer.h"

#include "qxtheme/qxthememanager.h"

#include <QtGui/QPainter>
#include <QAccessible>

QX_MATERIAL_USE_NAMESPACE

namespace
{
// The row's own measure, read the way a reader reads it.
constexpr int kStarSize = 24;
constexpr int kStars = 5;

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

/*! How many pixels of \a room are darker than the page: an empty star is a
 *  line, and how many pixels it takes is not the question - that it is drawn
 *  at all is. */
int inkedPixels(const QImage &image, const QRect &room, const QColor &page)
{
    int count = 0;
    const QRect room_ = room.intersected(image.rect());
    for (int y = room_.top(); y <= room_.bottom(); ++y) {
        for (int x = room_.left(); x <= room_.right(); ++x) {
            if (image.pixelColor(x, y).lightness() < page.lightness() - 2) {
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

/*! Where star \a index sits, one box per star with nothing between them. */
QRect starBox(int index)
{
    return QRect(index * kStarSize, 0, kStarSize, kStarSize);
}

/*! The middle of star \a index: a star that is on is a solid there, whatever
 *  its points are doing. */
QPoint starCenter(int index)
{
    return starBox(index).center();
}

/*! Move the pointer to \a pos over \a widget, the way the platform would, so
 *  that a rating can be asked what a click there would do. */
void pointerAt(QWidget *widget, const QPoint &pos)
{
    QMouseEvent move(QEvent::MouseMove, QPointF(pos), QPointF(widget->mapToGlobal(pos)), Qt::NoButton, Qt::NoButton,
                     Qt::NoModifier);
    QApplication::sendEvent(widget, &move);
}

/*! Take the pointer away from \a widget. */
void pointerLeaves(QWidget *widget)
{
    QEvent leave(QEvent::Leave);
    QApplication::sendEvent(widget, &leave);
}
}   // namespace

class tst_MaterialRating : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void theRowIsTwentyFourPerStar();
    void pressingAStarRatesIt();
    void theStarsThatAreRatedAreSolid();
    void anEmptyStarIsTheOneThatIsDrawnAsALine();
    void thePointerShowsWhatAClickWouldDo();
    void theKeyboardWalksTheRating();
    void aScoreThatIsOnlyToBeReadIsNotPressed();
    void aRatingNobodyCanUseAnswersNothing();
    void moreOrFewerStarsIsAWholeRow();
    void theRatingAnswersAsAValue();
};

namespace
{
/*! A rating of five stars with nothing rated yet. */
MaterialRating *makeRating(QWidget *page)
{
    auto *rating = new MaterialRating(page);
    rating->setCount(kStars);
    rating->resize(rating->sizeHint());
    // Away from where the pointer of the offscreen platform parks itself: what
    // the pointer would do is drawn, so a rating built under it would be showing
    // a preview before the test said anything about the pointer.
    rating->move(0, 60);
    rating->show();
    QTest::qWaitForWindowExposed(rating);
    return rating;
}

QWidget *makePage()
{
    auto *page = new QWidget;
    page->resize(320, 200);
    page->show();
    QTest::qWaitForWindowExposed(page);
    return page;
}
}   // namespace

void tst_MaterialRating::initTestCase()
{
    // Nothing here reads a glyph, but the star's box is measured against the
    // theme's tokens, and a platform with no font at all is not the one the
    // rest of the suite runs on.
    qtcGiveFonts();
}

/*! One box per star, twenty-four across, with nothing between them. */
void tst_MaterialRating::theRowIsTwentyFourPerStar()
{
    QWidget *page = makePage();
    MaterialRating *rating = makeRating(page);

    QCOMPARE(rating->count(), kStars);
    QCOMPARE(rating->value(), 0);
    QCOMPARE(rating->height(), kStarSize);
    QCOMPARE(rating->sizeHint(), QSize(kStars * kStarSize, kStarSize));
    QCOMPARE(rating->minimumSizeHint(), rating->sizeHint());
}

/*! Pressing a star rates up to it, and says so once - the same star again is
 *  not a new rating. */
void tst_MaterialRating::pressingAStarRatesIt()
{
    QWidget *page = makePage();
    MaterialRating *rating = makeRating(page);
    QSignalSpy spy(rating, &MaterialRating::valueChanged);

    QTest::mouseClick(rating, Qt::LeftButton, Qt::NoModifier, starCenter(2));
    QCOMPARE(rating->value(), 3);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 3);

    QTest::mouseClick(rating, Qt::LeftButton, Qt::NoModifier, starCenter(2));
    QCOMPARE(spy.count(), 1);

    // Going back down is a rating too: a row of stars is not a one-way scale.
    QTest::mouseClick(rating, Qt::LeftButton, Qt::NoModifier, starCenter(0));
    QCOMPARE(rating->value(), 1);
    QCOMPARE(spy.count(), 2);
}

/*! The stars that are rated are solid accent - the accent is their edge as well
 *  as their inside - and the ones that are not have none of it. */
void tst_MaterialRating::theStarsThatAreRatedAreSolid()
{
    QWidget *page = makePage();
    MaterialRating *rating = makeRating(page);
    rating->setValue(3);

    const QImage frame = rating->grab().toImage();
    const QColor page_ = rating->palette().color(QPalette::Window);
    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();

    for (int i = 0; i < 3; ++i) {
        const QColor middle = frame.pixelColor(starCenter(i));
        QVERIFY2(alike(middle, accent),
                 qPrintable(QStringLiteral("star %1 is %2, not %3").arg(i).arg(middle.name()).arg(accent.name())));
    }
    for (int i = 3; i < kStars; ++i) {
        QCOMPARE(accentPixels(frame, starBox(i)), 0);
        // The middle of a star that is not on is the page: only its outline is
        // drawn, and the outline does not reach the middle.
        QVERIFY(alike(frame.pixelColor(starCenter(i)), page_));
    }
}

/*! Material's star is a solid, so the only star drawn as a line is the one that
 *  is not filled in - in the ink kept for what is not filled in. */
void tst_MaterialRating::anEmptyStarIsTheOneThatIsDrawnAsALine()
{
    QWidget *page = makePage();
    MaterialRating *rating = makeRating(page);
    rating->setValue(5);

    const QImage filled = rating->grab().toImage();
    const QColor page_ = rating->palette().color(QPalette::Window);
    const int solid = inkedPixels(filled, starBox(4), page_);
    QVERIFY2(solid > 0, qPrintable(QString::number(solid)));

    rating->setValue(0);
    const QImage empty = rating->grab().toImage();
    // The same star as a line: drawn, but mostly not drawn.
    const int line = inkedPixels(empty, starBox(4), page_);
    QVERIFY2(line > 0, qPrintable(QString::number(line)));
    // A one-pixel outline is about half the ink of the star it surrounds, so the
    // two are not merely "one is less than the other": they are two drawings.
    QVERIFY2(line * 3 < solid * 2, qPrintable(QStringLiteral("line %1 vs solid %2").arg(line).arg(solid)));
    QCOMPARE(accentPixels(empty, starBox(4)), 0);
}

/*! What the pointer says replaces what is rated rather than adding to it: a row
 *  that kept four stars while the pointer was on the second would be saying a
 *  click here would give four. */
void tst_MaterialRating::thePointerShowsWhatAClickWouldDo()
{
    QWidget *page = makePage();
    MaterialRating *rating = makeRating(page);
    rating->setValue(4);
    const QColor page_ = rating->palette().color(QPalette::Window);
    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
    const QColor wouldFill = QxControls::QxStateLayer::layered(page_, accent, 0.35);

    pointerAt(rating, starCenter(1));
    const QImage preview = rating->grab().toImage();
    for (int i = 0; i < 2; ++i) {
        const QColor middle = preview.pixelColor(starCenter(i));
        QVERIFY2(alike(middle, wouldFill), qPrintable(QStringLiteral("star %1 is %2").arg(i).arg(middle.name())));
    }
    // The fourth star was rated and is not any more in the picture: the pointer
    // is asking about two, so two is what a click would leave.
    QCOMPARE(accentPixels(preview, starBox(3)), 0);
    QVERIFY(alike(preview.pixelColor(starCenter(3)), page_));

    // The pointer leaving gives the rating back its own reading.
    pointerLeaves(rating);
    const QImage resting = rating->grab().toImage();
    QVERIFY(accentPixels(resting, starBox(3)) > 0);
    QVERIFY(alike(resting.pixelColor(starCenter(3)), accent));
}

/*! A rating is a value in a range, and the keyboard walks it. */
void tst_MaterialRating::theKeyboardWalksTheRating()
{
    QWidget *page = makePage();
    MaterialRating *rating = makeRating(page);
    rating->setFocus();

    QTest::keyClick(rating, Qt::Key_Right);
    QCOMPARE(rating->value(), 1);
    QTest::keyClick(rating, Qt::Key_Up);
    QCOMPARE(rating->value(), 2);
    QTest::keyClick(rating, Qt::Key_Left);
    QCOMPARE(rating->value(), 1);

    QTest::keyClick(rating, Qt::Key_End);
    QCOMPARE(rating->value(), kStars);
    // The end of the range is the end: it does not wrap round.
    QTest::keyClick(rating, Qt::Key_Right);
    QCOMPARE(rating->value(), kStars);

    QTest::keyClick(rating, Qt::Key_Home);
    QCOMPARE(rating->value(), 0);
    QTest::keyClick(rating, Qt::Key_Down);
    QCOMPARE(rating->value(), 0);
}

/*! A score that is only to be read is not pressed, does not preview, and is not
 *  a place in the way of the keyboard - but the application can still set it. */
void tst_MaterialRating::aScoreThatIsOnlyToBeReadIsNotPressed()
{
    QWidget *page = makePage();
    MaterialRating *rating = makeRating(page);
    rating->setValue(2);
    rating->setReadOnly(true);

    QCOMPARE(rating->focusPolicy(), Qt::NoFocus);
    QTest::mouseClick(rating, Qt::LeftButton, Qt::NoModifier, starCenter(4));
    QCOMPARE(rating->value(), 2);

    // The pointer says nothing either: there is no click for it to describe.
    const QImage still = rating->grab().toImage();
    pointerAt(rating, starCenter(4));
    const QImage after = rating->grab().toImage();
    QVERIFY(after == still);

    // The application is not the reader: it may still say what the score is.
    rating->setValue(5);
    QCOMPARE(rating->value(), 5);

    rating->setReadOnly(false);
    QCOMPARE(rating->focusPolicy(), Qt::StrongFocus);
    QTest::mouseClick(rating, Qt::LeftButton, Qt::NoModifier, starCenter(2));
    QCOMPARE(rating->value(), 3);
}

/*! A rating nobody can use answers nothing, and is drawn with less ink. */
void tst_MaterialRating::aRatingNobodyCanUseAnswersNothing()
{
    QWidget *page = makePage();
    MaterialRating *rating = makeRating(page);
    rating->setValue(3);
    const QColor page_ = rating->palette().color(QPalette::Window);
    const int lit = inkedPixels(rating->grab().toImage(), starBox(0), page_);

    rating->setEnabled(false);
    QTest::mouseClick(rating, Qt::LeftButton, Qt::NoModifier, starCenter(4));
    QCOMPARE(rating->value(), 3);

    const int dimmed = inkedPixels(rating->grab().toImage(), starBox(0), page_);
    QVERIFY2(dimmed < lit, qPrintable(QStringLiteral("%1 vs %2").arg(dimmed).arg(lit)));
}

/*! The number of stars is the caller's to say, and a score cannot be more stars
 *  than there are. */
void tst_MaterialRating::moreOrFewerStarsIsAWholeRow()
{
    QWidget *page = makePage();
    MaterialRating *rating = makeRating(page);
    rating->setValue(5);
    QSignalSpy spy(rating, &MaterialRating::valueChanged);

    rating->setCount(10);
    QCOMPARE(rating->count(), 10);
    QCOMPARE(rating->sizeHint(), QSize(10 * kStarSize, kStarSize));
    QCOMPARE(rating->value(), 5);
    QCOMPARE(spy.count(), 0);

    // Four stars cannot hold a score of five; the score comes down with them
    // and the listeners are told, because the value they can read changed.
    rating->setCount(4);
    QCOMPARE(rating->value(), 4);
    QCOMPARE(spy.count(), 1);

    // A row of no stars is not a row.
    rating->setCount(0);
    QCOMPARE(rating->count(), 1);
}

/*! To an assistive tool a rating is a value in a range, and the range is the
 *  number of stars. */
void tst_MaterialRating::theRatingAnswersAsAValue()
{
    QWidget *page = makePage();
    MaterialRating *rating = makeRating(page);
    rating->setValue(3);

    QAccessibleInterface *face = QAccessible::queryAccessibleInterface(rating);
    QVERIFY(face);
    QCOMPARE(face->role(), QAccessible::Slider);

    QAccessibleValueInterface *value = face->valueInterface();
    QVERIFY(value);
    QCOMPARE(value->currentValue().toInt(), 3);
    QCOMPARE(value->maximumValue().toInt(), kStars);
    QCOMPARE(value->minimumValue().toInt(), 0);
    QCOMPARE(value->minimumStepSize().toInt(), 1);

    value->setCurrentValue(2);
    QCOMPARE(rating->value(), 2);
}

TEST_ADD(tst_MaterialRating)

#include "tst_materialrating.moc"
