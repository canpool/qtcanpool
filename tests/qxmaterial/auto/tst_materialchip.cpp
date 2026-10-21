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
 * surface, not the accent itself, so what is counted is a leaning towards it. */
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
    QCOMPARE(tintedPixels(&chip), 0);

    QSignalSpy flipped(&chip, &MaterialChip::toggled);
    QTest::mouseClick(&chip, Qt::LeftButton);
    QVERIFY(chip.isChecked());
    QCOMPARE(flipped.count(), 1);
    QCOMPARE(flipped.at(0).at(0).toBool(), true);
    QVERIFY(tintedPixels(&chip) > 0);

    // Pressing it again takes the filter back.
    QTest::mouseClick(&chip, Qt::LeftButton);
    QVERIFY(!chip.isChecked());
    QCOMPARE(flipped.count(), 2);
    QCOMPARE(tintedPixels(&chip), 0);
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

TEST_ADD(tst_MaterialChip)

#include "tst_materialchip.moc"
