/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialtogglebuttongroup.h"

#include "qxcontrols/qxstatelayer.h"

#include "qxtheme/qxthememanager.h"

#include <QtGui/QPainter>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtGui/QEnterEvent>
#endif
#include <QAccessible>

QX_MATERIAL_USE_NAMESPACE

namespace
{
// The row's own measures, read the way a reader reads them.
constexpr int kRowHeight = 40;
constexpr int kLeastWidth = 48;

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

/*! The darkest pixel of \a room: a label nobody can use is a label drawn with
 *  less ink, and how many pixels are dark is not what says which is which -
 *  anti-aliasing counts as ink. */
QColor darkestPixel(const QImage &image, const QRect &room)
{
    QColor darkest(Qt::white);
    const QRect room_ = room.intersected(image.rect());
    for (int y = room_.top(); y <= room_.bottom(); ++y) {
        for (int x = room_.left(); x <= room_.right(); ++x) {
            const QColor pixel = image.pixelColor(x, y);
            if (pixel.lightness() < darkest.lightness()) {
                darkest = pixel;
            }
        }
    }
    return darkest;
}

/*! How far apart two colours are, as the distance between them in the cube. */
int distance(const QColor &a, const QColor &b)
{
    return qAbs(a.red() - b.red()) + qAbs(a.green() - b.green()) + qAbs(a.blue() - b.blue());
}

/*! Tell \a widget the pointer has arrived, the way the platform would. */
void pointerArrives(QWidget *widget)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QPointF here(5.0, 5.0);
    QEnterEvent enter(here, here, here);
    QApplication::sendEvent(widget, &enter);
#else
    QEvent enter(QEvent::Enter);
    QApplication::sendEvent(widget, &enter);
#endif
}
}   // namespace

class tst_MaterialToggleButtonGroup : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void theRowIsAsWideAsItsButtonsAsk();
    void nothingHasToBeOn();
    void oneButtonAtATimeUnlessToldOtherwise();
    void aButtonThatIsOnWearsTheAccent();
    void theWashFollowsThePointerAndThePress();
    void aButtonNobodyCanUseIsDimmer();
    void theKeyboardThrowsASwitch();
    void theArrowsPassTheChoiceAlong();
    void anExclusiveRowIsOneTabStop();
    void theFocusShowsWhereTheKeyboardIs();
    void theButtonsAnswerAsSwitches();
    void takingAButtonOutLeavesTheRowWhole();
};

namespace
{
/*! A row of three buttons of plainly different widths: two letters that cannot
 *  grow past the floor a button refuses to go under, and one word that can. */
MaterialToggleButtonGroup *makeRow(QWidget *page)
{
    auto *row = new MaterialToggleButtonGroup(page);
    row->addButton(QStringLiteral("B"));
    row->addButton(QStringLiteral("I"));
    row->addButton(QStringLiteral("Underline"));
    row->resize(row->sizeHint());
    // Away from where the pointer of the offscreen platform parks itself: the
    // hover wash is driven by the real cursor, and a row built under it would
    // be washed before the test said anything about the pointer.
    row->move(0, 60);
    row->show();
    QTest::qWaitForWindowExposed(row);
    return row;
}

QWidget *makePage()
{
    auto *page = new QWidget;
    page->resize(420, 200);
    page->show();
    QTest::qWaitForWindowExposed(page);
    return page;
}

/*! The button that stands where \a index says. The row lays its buttons out
 *  itself, so the only honest way for a test to reach one is its rectangle. */
QWidget *buttonAt(MaterialToggleButtonGroup *row, int index)
{
    return row->childAt(row->buttonRect(index).center());
}
}   // namespace

void tst_MaterialToggleButtonGroup::initTestCase()
{
    // The labels are drawn with whatever font the machine has, and the tests
    // read pixels, so there has to be one.
    qtcGiveFonts();
}

/*! The row is as tall as the buttons it belongs among, and each button is as
 *  wide as its own label asks - not an equal share of the row. */
void tst_MaterialToggleButtonGroup::theRowIsAsWideAsItsButtonsAsk()
{
    QWidget *page = makePage();
    MaterialToggleButtonGroup *row = makeRow(page);

    QCOMPARE(row->height(), kRowHeight);
    QCOMPARE(row->minimumSizeHint(), QSize(3 * kLeastWidth, kRowHeight));

    // The two one-letter buttons are at the floor and equal; the word is wider.
    QCOMPARE(row->buttonRect(0).width(), kLeastWidth);
    QCOMPARE(row->buttonRect(1).width(), kLeastWidth);
    QVERIFY2(row->buttonRect(2).width() > row->buttonRect(0).width(),
             qPrintable(QStringLiteral("%1 vs %2").arg(row->buttonRect(2).width()).arg(row->buttonRect(0).width())));

    // One row, not three buttons with gaps: each cell begins where the one
    // before it ended.
    QCOMPARE(row->buttonRect(1).left(), row->buttonRect(0).right() + 1);
    QCOMPARE(row->buttonRect(2).left(), row->buttonRect(1).right() + 1);
    QCOMPARE(row->buttonRect(2).right(), row->width() - 1);

    // Given more room, the extra is shared out evenly, so the buttons keep the
    // difference their labels asked for.
    const int narrow = row->buttonRect(0).width();
    const int wide = row->buttonRect(2).width();
    row->resize(row->sizeHint().width() + 30, kRowHeight);
    QCOMPARE(row->buttonRect(0).width(), narrow + 10);
    QCOMPARE(row->buttonRect(2).width(), wide + 10);
    QCOMPARE(row->buttonRect(2).right(), row->width() - 1);
}

/*! The first of the three places this parts company with a segmented button:
 *  a row of switches with nothing switched on is a row that is fine. */
void tst_MaterialToggleButtonGroup::nothingHasToBeOn()
{
    QWidget *page = makePage();
    MaterialToggleButtonGroup *row = makeRow(page);

    QVERIFY(row->isExclusive());
    QVERIFY(row->checkedButtons().isEmpty());
    QCOMPARE(row->isChecked(0), false);

    QTest::mouseClick(buttonAt(row, 0), Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QCOMPARE(row->isChecked(0), true);

    // Pressing the one that is on is how it is turned off.
    QTest::mouseClick(buttonAt(row, 0), Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QCOMPARE(row->isChecked(0), false);
    QVERIFY(row->checkedButtons().isEmpty());
}

/*! At most one is on unless told otherwise - and at most one, never exactly
 *  one: an exclusive row may be emptied. */
void tst_MaterialToggleButtonGroup::oneButtonAtATimeUnlessToldOtherwise()
{
    QWidget *page = makePage();
    MaterialToggleButtonGroup *row = makeRow(page);
    QSignalSpy spy(row, &MaterialToggleButtonGroup::buttonToggled);

    row->setChecked(0, true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 0);
    QCOMPARE(spy.at(0).at(1).toBool(), true);

    // Passing the choice along is two changes, and the button going off is
    // reported before the one coming on.
    row->setChecked(1, true);
    QCOMPARE(spy.count(), 3);
    QCOMPARE(spy.at(1).at(0).toInt(), 0);
    QCOMPARE(spy.at(1).at(1).toBool(), false);
    QCOMPARE(spy.at(2).at(0).toInt(), 1);
    QCOMPARE(spy.at(2).at(1).toBool(), true);
    QCOMPARE(row->checkedButtons().count(), 1);
    QCOMPARE(row->checkedButtons().value(0), 1);

    // Saying the same thing twice is not a change.
    row->setChecked(1, true);
    QCOMPARE(spy.count(), 3);

    // Told it may have several, it does.
    row->setExclusive(false);
    row->setChecked(0, true);
    QCOMPARE(row->checkedButtons().count(), 2);
    QCOMPARE(row->checkedButtons().value(0), 0);
    QCOMPARE(row->checkedButtons().value(1), 1);

    // Told it may have one again, the first stays and the rest go off.
    row->setExclusive(true);
    QCOMPARE(row->checkedButtons().count(), 1);
    QCOMPARE(row->checkedButtons().value(0), 0);

    row->clearChecked();
    QVERIFY(row->checkedButtons().isEmpty());
}

/*! Being on is said by the fill under the label and by the label's own colour -
 *  the outline it shares with its neighbour is not repainted for it. */
void tst_MaterialToggleButtonGroup::aButtonThatIsOnWearsTheAccent()
{
    QWidget *page = makePage();
    MaterialToggleButtonGroup *row = makeRow(page);
    row->setChecked(1, true);

    const QImage frame = row->grab().toImage();
    const QRect on = row->buttonRect(1);
    const QRect off = row->buttonRect(0);
    const QColor page_ = row->palette().color(QPalette::Window);
    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
    const QColor fill = QxControls::QxStateLayer::layered(page_, accent, 0.12);

    // Inside the padding, clear of the label and of the outline.
    const QPoint probe(on.left() + 6, on.center().y());
    const QColor inForce = frame.pixelColor(probe);
    QVERIFY2(alike(inForce, fill), qPrintable(QStringLiteral("fill %1 vs %2").arg(inForce.name()).arg(fill.name())));
    QVERIFY2(alike(frame.pixelColor(QPoint(off.left() + 6, off.center().y())), page_),
             qPrintable(QStringLiteral("resting %1 vs %2 at %3,%4 in %5,%6")
                            .arg(frame.pixelColor(QPoint(off.left() + 6, off.center().y())).name())
                            .arg(page_.name())
                            .arg(off.left() + 6)
                            .arg(off.center().y())
                            .arg(off.width())
                            .arg(off.height())));

    // And its label takes the accent, while its neighbour's stays the page's ink.
    // Read as "which of the two the darkest pixel of the label stands nearer
    // to", not as "a pixel of exactly the accent's colour": how solid a glyph
    // comes out is the platform's font's business, and a thin one at fourteen
    // points can be given no fully covered pixel at all.
    // Both labels are drawn by the same font at the same size, so the two are
    // compared with each other rather than with a colour: one leans toward the
    // accent and the other away from it, whatever the platform's glyphs are.
    const QColor ink = row->palette().color(QPalette::WindowText);
    const QColor onInk = darkestPixel(frame, on);
    const QColor offInk = darkestPixel(frame, off);
    QVERIFY2(distance(onInk, accent) < distance(offInk, accent),
             qPrintable(QStringLiteral("%1 is not nearer the accent than %2").arg(onInk.name(), offInk.name())));
    QVERIFY2(distance(offInk, ink) < distance(onInk, ink),
             qPrintable(QStringLiteral("%1 is not nearer the ink than %2").arg(offInk.name(), onInk.name())));
}

/*! The pointer arriving and the button being pushed are two different washes. */
void tst_MaterialToggleButtonGroup::theWashFollowsThePointerAndThePress()
{
    QWidget *page = makePage();
    MaterialToggleButtonGroup *row = makeRow(page);
    QWidget *middle = buttonAt(row, 1);
    const QPoint probe = QPoint(row->buttonRect(1).left() + 6, row->buttonRect(1).center().y());
    const QColor page_ = row->palette().color(QPalette::Window);
    const QColor ink = row->palette().color(QPalette::WindowText);

    pointerArrives(middle);
    const QColor hovering = row->grab().toImage().pixelColor(probe);
    QVERIFY2(alike(hovering, QxControls::QxStateLayer::layered(page_, ink, QxControls::QxStateLayer::hoverOpacity)),
             qPrintable(hovering.name()));

    QTest::mousePress(middle, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    const QColor pressing = row->grab().toImage().pixelColor(probe);
    QVERIFY2(alike(pressing, QxControls::QxStateLayer::layered(page_, ink, QxControls::QxStateLayer::pressedOpacity)),
             qPrintable(pressing.name()));

    // Letting go over the button is a click, which throws its switch.
    QTest::mouseRelease(middle, Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QVERIFY(row->isChecked(1));
}

/*! A button nobody can use is drawn with less ink, and answers nothing. */
void tst_MaterialToggleButtonGroup::aButtonNobodyCanUseIsDimmer()
{
    QWidget *page = makePage();
    MaterialToggleButtonGroup *row = makeRow(page);
    buttonAt(row, 1)->setEnabled(false);

    const QImage frame = row->grab().toImage();
    const QColor usable = darkestPixel(frame, row->buttonRect(0).adjusted(2, 2, -2, -2));
    const QColor shut = darkestPixel(frame, row->buttonRect(1).adjusted(2, 2, -2, -2));
    QVERIFY2(usable.lightness() < shut.lightness(),
             qPrintable(QStringLiteral("%1 vs %2").arg(usable.lightness()).arg(shut.lightness())));

    QTest::mouseClick(buttonAt(row, 1), Qt::LeftButton, Qt::NoModifier, QPoint(5, 5));
    QCOMPARE(row->isChecked(1), false);
}

/*! Where every button is its own switch, the keyboard throws each one: the
 *  arrows are not the row's business. */
void tst_MaterialToggleButtonGroup::theKeyboardThrowsASwitch()
{
    QWidget *page = makePage();
    MaterialToggleButtonGroup *row = makeRow(page);
    row->setExclusive(false);
    QWidget *first = buttonAt(row, 0);

    QTest::keyClick(first, Qt::Key_Space);
    QCOMPARE(row->isChecked(0), true);
    QTest::keyClick(first, Qt::Key_Return);
    QCOMPARE(row->isChecked(0), false);

    QTest::keyClick(first, Qt::Key_Right);
    QVERIFY(row->checkedButtons().isEmpty());
}

/*! Where at most one may be on, the arrows pass the choice along and carry the
 *  keyboard with it - which is the one thing the two widgets do the same way. */
void tst_MaterialToggleButtonGroup::theArrowsPassTheChoiceAlong()
{
    QWidget *page = makePage();
    MaterialToggleButtonGroup *row = makeRow(page);
    row->setChecked(0, true);

    QTest::keyClick(buttonAt(row, 0), Qt::Key_Right);
    QCOMPARE(row->isChecked(0), false);
    QCOMPARE(row->isChecked(1), true);
    QCOMPARE(row->checkedButtons().count(), 1);

    QTest::keyClick(buttonAt(row, 1), Qt::Key_Down);
    QCOMPARE(row->isChecked(2), true);
    QCOMPARE(buttonAt(row, 2)->hasFocus(), true);

    // The end of the row is not the end of the walk.
    QTest::keyClick(buttonAt(row, 2), Qt::Key_Right);
    QCOMPARE(row->isChecked(0), true);
    QCOMPARE(row->checkedButtons().count(), 1);
}

/*! One stop on the way through the keyboard where at most one may be on, and
 *  one stop per switch where they are independent. */
void tst_MaterialToggleButtonGroup::anExclusiveRowIsOneTabStop()
{
    QWidget *page = makePage();
    MaterialToggleButtonGroup *row = makeRow(page);

    // Nothing is on, so the stop is the first button.
    QCOMPARE(buttonAt(row, 0)->focusPolicy(), Qt::StrongFocus);
    QCOMPARE(buttonAt(row, 1)->focusPolicy(), Qt::NoFocus);

    // The stop follows the choice.
    row->setChecked(1, true);
    QCOMPARE(buttonAt(row, 0)->focusPolicy(), Qt::NoFocus);
    QCOMPARE(buttonAt(row, 1)->focusPolicy(), Qt::StrongFocus);

    row->setExclusive(false);
    QCOMPARE(buttonAt(row, 0)->focusPolicy(), Qt::StrongFocus);
    QCOMPARE(buttonAt(row, 2)->focusPolicy(), Qt::StrongFocus);
}

/*! Where the keyboard is shows on the button it is on, and only there. */
void tst_MaterialToggleButtonGroup::theFocusShowsWhereTheKeyboardIs()
{
    QWidget *page = makePage();
    MaterialToggleButtonGroup *row = makeRow(page);
    QWidget *last = buttonAt(row, 2);
    last->setFocus();
    QVERIFY(last->hasFocus());

    const QImage focused = row->grab().toImage();
    QVERIFY(accentPixels(focused, row->buttonRect(2)) > 0);
    QCOMPARE(accentPixels(focused, row->buttonRect(0)), 0);

    last->clearFocus();
    const QImage resting = row->grab().toImage();
    QCOMPARE(accentPixels(resting, row->buttonRect(2)), 0);
}

/*! To an assistive tool each button is a switch, and the row is nothing of its
 *  own: what is on is a fact about a button, not about the row. */
void tst_MaterialToggleButtonGroup::theButtonsAnswerAsSwitches()
{
    QWidget *page = makePage();
    MaterialToggleButtonGroup *row = makeRow(page);
    row->setChecked(0, true);

    QAccessibleInterface *face = QAccessible::queryAccessibleInterface(buttonAt(row, 0));
    QVERIFY(face);
    QCOMPARE(face->role(), QAccessible::Button);
    QCOMPARE(face->text(QAccessible::Name), QStringLiteral("B"));
    QVERIFY(face->state().checkable);
    QVERIFY(face->state().checked);

    QAccessibleInterface *other = QAccessible::queryAccessibleInterface(buttonAt(row, 1));
    QVERIFY(other);
    QVERIFY(other->state().checkable);
    QCOMPARE(other->state().checked, false);
}

/*! Taking a button out of the row leaves a row, not a hole: the rest close up,
 *  and the buttons after it answer to their new places. */
void tst_MaterialToggleButtonGroup::takingAButtonOutLeavesTheRowWhole()
{
    QWidget *page = makePage();
    MaterialToggleButtonGroup *row = makeRow(page);
    const int wide = row->buttonRect(2).width();
    row->setChecked(2, true);

    row->removeButton(1);
    QCOMPARE(row->count(), 2);
    QCOMPARE(row->buttonText(0), QStringLiteral("B"));
    QCOMPARE(row->buttonText(1), QStringLiteral("Underline"));

    // What was on is still on, and is now the second button.
    QCOMPARE(row->isChecked(1), true);
    QCOMPARE(row->checkedButtons().count(), 1);

    row->resize(row->sizeHint());
    QCOMPARE(row->buttonRect(1).width(), wide);
    QCOMPARE(row->buttonRect(1).right(), row->width() - 1);
    QCOMPARE(row->buttonRect(-1), QRect());
    QCOMPARE(row->buttonRect(2), QRect());

    row->clear();
    QCOMPARE(row->count(), 0);
    QCOMPARE(row->sizeHint().width(), 0);
    QCOMPARE(row->buttonText(0), QString());
}

TEST_ADD(tst_MaterialToggleButtonGroup)

#include "tst_materialtogglebuttongroup.moc"
