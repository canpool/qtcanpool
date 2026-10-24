/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialsegmentedbutton.h"

#include "qxmaterial/materialtokens.h"

#include <QAccessible>
#include <QFontMetricsF>
#include <QSignalSpy>
#include <QWidget>

QX_MATERIAL_USE_NAMESPACE

namespace
{
QList<QWidget *> segmentsOf(QWidget *widget)
{
    return widget->findChildren<QWidget *>();
}
}   // namespace

class tst_MaterialSegmentedButton : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void theFirstGoesInForce();
    void aClickPutsASegmentInForce();
    void theArrowsMoveWhatIsInForce();
    void theRowIsOneStopOnTheKeyboard();
    void aSegmentIsWideEnoughForItsOwnWord();
    void whatAnAssistiveToolReads();
private:
    bool m_wasActive = false;
};

void tst_MaterialSegmentedButton::init()
{
    m_wasActive = QAccessible::isActive();
}

void tst_MaterialSegmentedButton::cleanup()
{
    QAccessible::setActive(m_wasActive);
}

/*! A row of choices with nothing in force answers nothing, so making the row
 *  puts the first one in force. */
void tst_MaterialSegmentedButton::theFirstGoesInForce()
{
    MaterialSegmentedButton button;
    QCOMPARE(button.currentIndex(), -1);
    QCOMPARE(button.currentText(), QString());

    QSignalSpy changed(&button, &MaterialSegmentedButton::currentIndexChanged);
    button.setItems(QStringList() << QStringLiteral("Day") << QStringLiteral("Week") << QStringLiteral("Month"));
    QCOMPARE(button.count(), 3);
    QCOMPARE(button.currentIndex(), 0);
    QCOMPARE(button.currentText(), QStringLiteral("Day"));
    QCOMPARE(changed.count(), 1);
}

/*! A press on a segment puts it in force: the segment in force is what the row
 *  answers, and the answer changes at once. */
void tst_MaterialSegmentedButton::aClickPutsASegmentInForce()
{
    MaterialSegmentedButton button;
    button.setItems(QStringList() << QStringLiteral("Day") << QStringLiteral("Week"));
    button.resize(button.sizeHint());
    button.show();
    QCOMPARE(segmentsOf(&button).count(), 2);

    QSignalSpy changed(&button, &MaterialSegmentedButton::currentIndexChanged);
    QTest::mouseClick(segmentsOf(&button).at(1), Qt::LeftButton);
    QCOMPARE(button.currentIndex(), 1);
    QCOMPARE(button.currentText(), QStringLiteral("Week"));
    QCOMPARE(changed.count(), 1);

    // Pressing the one already in force answers nothing, because nothing
    // changed.
    QTest::mouseClick(segmentsOf(&button).at(1), Qt::LeftButton);
    QCOMPARE(changed.count(), 1);
}

/*! The arrows move what is in force, the way they do in a group of radio
 *  buttons, and they come round at the ends. */
void tst_MaterialSegmentedButton::theArrowsMoveWhatIsInForce()
{
    MaterialSegmentedButton button;
    button.setItems(QStringList() << QStringLiteral("Day") << QStringLiteral("Week") << QStringLiteral("Month"));
    button.resize(button.sizeHint());

    const QList<QWidget *> segments = segmentsOf(&button);
    QTest::keyClick(segments.at(0), Qt::Key_Right);
    QCOMPARE(button.currentIndex(), 1);
    QTest::keyClick(segments.at(1), Qt::Key_Left);
    QCOMPARE(button.currentIndex(), 0);
    QTest::keyClick(segments.at(0), Qt::Key_Left);
    QCOMPARE(button.currentIndex(), 2);
    QTest::keyClick(segments.at(2), Qt::Key_Right);
    QCOMPARE(button.currentIndex(), 0);
}

/*! The row is one stop on the way through the keyboard, not one per segment:
 *  only the segment in force can be reached with Tab, and it moves with what
 *  is in force. */
void tst_MaterialSegmentedButton::theRowIsOneStopOnTheKeyboard()
{
    MaterialSegmentedButton button;
    button.setItems(QStringList() << QStringLiteral("Day") << QStringLiteral("Week"));

    const QList<QWidget *> segments = segmentsOf(&button);
    QCOMPARE(segments.at(0)->focusPolicy(), Qt::StrongFocus);
    QCOMPARE(segments.at(1)->focusPolicy(), Qt::NoFocus);

    button.setCurrentIndex(1);
    QCOMPARE(segments.at(0)->focusPolicy(), Qt::NoFocus);
    QCOMPARE(segments.at(1)->focusPolicy(), Qt::StrongFocus);
}

/*! What an assistive tool is told: a row of radio buttons, one of which is
 *  checked - which is what a choice out of a fixed number is. */
/*! A segment is as wide as its own word plus the room on both sides of it, and
 *  the room the row has to spare is shared out after that. Dividing the row
 *  equally instead gives "Day" and "Month" the same width, and the segment that
 *  came up short draws its word clipped - which is how the demo's third choice
 *  of three read "Mont". */
void tst_MaterialSegmentedButton::aSegmentIsWideEnoughForItsOwnWord()
{
    MaterialSegmentedButton row;
    const QStringList words = QStringList{QStringLiteral("Day"), QStringLiteral("Week"), QStringLiteral("Month")};
    row.setItems(words);
    row.resize(row.sizeHint());
    row.show();
    QVERIFY(QTest::qWaitForWindowExposed(&row));

    constexpr qreal kPadding = 12.0;
    const QFontMetricsF metrics(MaterialType::font(MaterialType::Scale::Body));
    for (int i = 0; i < words.count(); ++i) {
        const qreal needed = metrics.horizontalAdvance(words.at(i)) + 2.0 * kPadding;
        QVERIFY2(row.segmentRect(i).width() >= needed, qPrintable(QStringLiteral("%1 asks for %2 and has %3")
                                                                      .arg(words.at(i))
                                                                      .arg(needed)
                                                                      .arg(row.segmentRect(i).width())));
    }

    // And they still stand side by side without a gap or an overlap.
    for (int i = 1; i < words.count(); ++i) {
        QCOMPARE(row.segmentRect(i).left(), row.segmentRect(i - 1).right() + 1);
    }
}

void tst_MaterialSegmentedButton::whatAnAssistiveToolReads()
{
    MaterialSegmentedButton button;
    button.resize(button.sizeHint());
    button.setItems(QStringList() << QStringLiteral("Day") << QStringLiteral("Week"));

    QAccessible::setActive(true);
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(segmentsOf(&button).at(0));
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::RadioButton);
    QCOMPARE(iface->text(QAccessible::Name), QStringLiteral("Day"));
    QVERIFY(iface->state().checkable);
    QVERIFY(iface->state().checked);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));

    iface = QAccessible::queryAccessibleInterface(segmentsOf(&button).at(1));
    QVERIFY(iface);
    QVERIFY(iface->state().checkable);
    QVERIFY(!iface->state().checked);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

TEST_ADD(tst_MaterialSegmentedButton)

#include "tst_materialsegmentedbutton.moc"
