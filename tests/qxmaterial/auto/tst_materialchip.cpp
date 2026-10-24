/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialchip.h"

#include "qxtheme/qxthememanager.h"

#include <QAccessible>
#include <QImage>
#include <QSignalSpy>

QX_MATERIAL_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
const QColor kAccent(0, 0, 255);

/* The fill of a chip whose filter is in force is the accent tinted into the
 * surface, not the accent itself, so what is counted is a leaning towards it -
 * which a glyph's coloured fringe also has, hence the counts are only ever
 * read as a difference between two pictures of the same chip. */
int tintedPixels(QWidget *widget)
{
    const QImage image = widget->grab().toImage();
    int count = 0;
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            const QColor pixel = image.pixelColor(x, y);
            if (pixel.blue() > pixel.red() + 16 && pixel.blue() > pixel.green() + 16) {
                ++count;
            }
        }
    }
    return count;
}
}   // namespace

class tst_MaterialChip : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void anAssistChipIsPressedAndHoldsNothing();
    void aFilterChipHoldsWhatItStandsFor();
    void theMarkTakesRoomWhenAFilterIsInForce();
    void theKeyboardPressesIt();
    void whatAnAssistiveToolReads();
    void aChipNobodyCanPressIsDrawnFainter();
private:
    bool m_hadOverride = false;
    bool m_wasActive = false;
};

void tst_MaterialChip::init()
{
    m_hadOverride = QxThemeManager::instance()->hasAccentOverride();
    QxThemeManager::instance()->setAccentColor(kAccent);
    m_wasActive = QAccessible::isActive();
}

void tst_MaterialChip::cleanup()
{
    if (!m_hadOverride) {
        QxThemeManager::instance()->setAccentColor(QColor());
    }
    QAccessible::setActive(m_wasActive);
}

/*! An assist chip is a button in a chip's shape: it does something, and it
 *  holds nothing - so there is no state to read back or to set. */
void tst_MaterialChip::anAssistChipIsPressedAndHoldsNothing()
{
    MaterialChip chip(MaterialChip::Kind::Assist);
    chip.setText(QStringLiteral("Add to calendar"));
    chip.resize(chip.sizeHint());

    QSignalSpy pressed(&chip, &MaterialChip::clicked);
    QSignalSpy flipped(&chip, &MaterialChip::toggled);
    QTest::mouseClick(&chip, Qt::LeftButton);
    QCOMPARE(pressed.count(), 1);
    QCOMPARE(flipped.count(), 0);
    QVERIFY(!chip.isChecked());

    // Asking it to hold a state does nothing, because it has none.
    chip.setChecked(true);
    QVERIFY(!chip.isChecked());
    QCOMPARE(flipped.count(), 0);
}

/*! A filter chip holds a state: pressing it puts the filter in force and takes
 *  it back, and while it is in force the chip is filled. */
void tst_MaterialChip::aFilterChipHoldsWhatItStandsFor()
{
    MaterialChip chip(MaterialChip::Kind::Filter);
    chip.setText(QStringLiteral("Unread"));
    chip.resize(chip.sizeHint());
    QVERIFY(!chip.isChecked());
    // What being in force looks like is the fill, and the fill is the accent
    // leaning into the surface across the whole of the chip. It cannot be read
    // off one picture: the sentence is drawn with the platform's own text
    // antialiasing, and on the Linux legs that is subpixel - every glyph
    // carries a coloured fringe of its own, 182 pixels of it in this chip,
    // which is neither the fill nor separable from one by colour alone. Hence
    // the difference between two pictures of the same chip: the fill is most
    // of it, a fringe is a twentieth.
    const int resting = tintedPixels(&chip);

    QSignalSpy flipped(&chip, &MaterialChip::toggled);
    QTest::mouseClick(&chip, Qt::LeftButton);
    QVERIFY(chip.isChecked());
    QCOMPARE(flipped.count(), 1);
    QCOMPARE(flipped.at(0).at(0).toBool(), true);
    const int filled = tintedPixels(&chip);
    QVERIFY(filled - resting > chip.width() * chip.height() / 2);

    // Pressing it again takes the filter back, and the fill goes with it. Not
    // to the same number: the sentence starts after the mark while the filter
    // is in force, so it is drawn a few pixels along and its fringes with it -
    // a count that lands within a handful of the one it started from, beside
    // which the fill was thousands.
    QTest::mouseClick(&chip, Qt::LeftButton);
    QVERIFY(!chip.isChecked());
    QCOMPARE(flipped.count(), 2);
    QVERIFY(qAbs(tintedPixels(&chip) - resting) < (filled - resting) / 8);
}

/*! The mark a filter chip carries while it is in force takes room, so the chip
 *  grows - which is the reading, not the drawing, of it being there. */
void tst_MaterialChip::theMarkTakesRoomWhenAFilterIsInForce()
{
    MaterialChip chip(MaterialChip::Kind::Filter);
    chip.setText(QStringLiteral("Unread"));
    const int resting = chip.sizeHint().width();

    chip.setChecked(true);
    QVERIFY(chip.sizeHint().width() > resting);

    // An assist chip never carries the mark, having no state to carry it for.
    MaterialChip assist(MaterialChip::Kind::Assist);
    assist.setText(QStringLiteral("Unread"));
    const int assistWidth = assist.sizeHint().width();
    assist.setChecked(true);
    QCOMPARE(assist.sizeHint().width(), assistWidth);
}

/*! Enter answers on the press and Space on the release, so holding a key down
 *  does not press the chip over and over. */
void tst_MaterialChip::theKeyboardPressesIt()
{
    MaterialChip assist(MaterialChip::Kind::Assist);
    QSignalSpy pressed(&assist, &MaterialChip::clicked);
    QTest::keyClick(&assist, Qt::Key_Enter);
    QCOMPARE(pressed.count(), 1);
    QTest::keyClick(&assist, Qt::Key_Space);
    QCOMPARE(pressed.count(), 2);

    MaterialChip filter(MaterialChip::Kind::Filter);
    QTest::keyClick(&filter, Qt::Key_Space);
    QVERIFY(filter.isChecked());
    QTest::keyClick(&filter, Qt::Key_Enter);
    QVERIFY(!filter.isChecked());
}

/*! What an assistive tool is told: one of these is a button, the other is a
 *  thing that is on or off - and which of the two it is. */
void tst_MaterialChip::whatAnAssistiveToolReads()
{
    MaterialChip assist(MaterialChip::Kind::Assist);
    assist.setText(QStringLiteral("Add to calendar"));
    MaterialChip filter(MaterialChip::Kind::Filter);
    filter.setText(QStringLiteral("Unread"));
    filter.setChecked(true);

    QAccessible::setActive(true);
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&assist);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::PushButton);
    QCOMPARE(iface->text(QAccessible::Name), QStringLiteral("Add to calendar"));
    QVERIFY(!iface->state().checkable);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));

    iface = QAccessible::queryAccessibleInterface(&filter);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::CheckBox);
    QVERIFY(iface->state().checkable);
    QVERIFY(iface->state().checked);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

/*! A chip nobody can press is written a step down: its word and its mark,
 *  together. Pinned first, so the difference measured is the chip's own.
 */
void tst_MaterialChip::aChipNobodyCanPressIsDrawnFainter()
{
    MaterialChip chip(MaterialChip::Kind::Assist);
    chip.setText(QStringLiteral("Add to calendar"));
    chip.resize(chip.sizeHint());
    chip.show();
    qtcPinDisabledColours(&chip);

    const QImage live = chip.grab().toImage();
    chip.setEnabled(false);
    const QImage faded = chip.grab().toImage();
    QVERIFY(faded != live);

    // And the press is not taken: fading is not a promise that anything changed.
    QSignalSpy pressed(&chip, &MaterialChip::clicked);
    QTest::mouseClick(&chip, Qt::LeftButton);
    QCOMPARE(pressed.count(), 0);
}

TEST_ADD(tst_MaterialChip)

#include "tst_materialchip.moc"
