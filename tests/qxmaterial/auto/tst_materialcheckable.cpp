/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialcheckbox.h"
#include "qxmaterial/materialradio.h"

#include "qxtheme/qxthememanager.h"
#include "qxtheme/qxthememotion.h"

#include <QAccessible>
#include <QSignalSpy>
#include <QtWidgets/QApplication>
#include <QtTest/QTest>

QX_MATERIAL_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
const QColor kAccent(0, 0, 255);
}   // namespace

class tst_MaterialCheckable : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void theThreeAnswersWearThreeFaces();
    void theRowIsWhatAnswers();
    void oneChoiceOfSeveralStandsAlone();
    void theRippleAnswersFromTheBoxNotTheRow();
    void whatAnAssistiveToolReads();
    void aChoiceNobodyCanMakeIsDrawnFainter();
private:
    bool m_hadOverride = false;
    bool m_wasReduced = false;
};

void tst_MaterialCheckable::init()
{
    m_hadOverride = QxTheme::QxThemeManager::instance()->hasAccentOverride();
    QxTheme::QxThemeManager::instance()->setAccentColor(kAccent);
    m_wasReduced = QxTheme::isMotionReduced();
    QxTheme::setMotionReduced(false);
}

void tst_MaterialCheckable::cleanup()
{
    if (!m_hadOverride) {
        QxTheme::QxThemeManager::instance()->setAccentColor(QColor());
    }
    QxTheme::setMotionReduced(m_wasReduced);
}

/*! The empty box is the page under a border, a box with an answer wears the
 *  primary, and the partial answer is the primary with the bar in it. */
void tst_MaterialCheckable::theThreeAnswersWearThreeFaces()
{
    MaterialCheckBox box;
    box.resize(box.sizeHint());

    // The box is the eighteen-wide square at the row's left, and the answer
    // is read where it is drawn rather than at a point guessed from the
    // row's height: the height follows the font the platform brought, the
    // square does not.
    const auto accentPixels = [](const QImage &image, int *outside) {
        int count = 0;
        *outside = 0;
        for (int y = 0; y < image.height(); ++y) {
            for (int x = 0; x < image.width(); ++x) {
                if (image.pixelColor(x, y) == kAccent) {
                    ++count;
                    if (x >= 18) {
                        ++*outside;
                    }
                }
            }
        }
        return count;
    };
    int outside = 0;

    box.setCheckState(Qt::Unchecked);
    // An empty box keeps the page: it asks for no accent at all.
    QCOMPARE(accentPixels(box.grab().toImage(), &outside), 0);
    box.setCheckState(Qt::Checked);
    // The answer is the primary, and it is the box that wears it.
    QVERIFY(accentPixels(box.grab().toImage(), &outside) > 0);
    QCOMPARE(outside, 0);

    box.setCheckState(Qt::PartiallyChecked);
    // The partial answer is the primary too - the bar is drawn in it.
    QVERIFY(accentPixels(box.grab().toImage(), &outside) > 0);
    QCOMPARE(outside, 0);
}

/*! The label is half of the same answer: a press on the words turns the box
 *  over, and the keyboard asks the same question. */
void tst_MaterialCheckable::theRowIsWhatAnswers()
{
    MaterialCheckBox box;
    box.setText(QStringLiteral("Include archives"));
    box.resize(box.sizeHint());
    QSignalSpy changed(&box, &MaterialCheckBox::stateChanged);

    QTest::mouseClick(&box, Qt::LeftButton, Qt::NoModifier, QPoint(box.width() - 6, box.height() / 2));
    QCOMPARE(box.checkState(), Qt::Checked);
    QCOMPARE(changed.count(), 1);

    QTest::keyClick(&box, Qt::Key_Space);
    QCOMPARE(box.checkState(), Qt::Unchecked);
    QCOMPARE(changed.count(), 2);
}

/*! The choice is one of several: the brothers that share its parent cannot
 *  be on while one of them is. */
void tst_MaterialCheckable::oneChoiceOfSeveralStandsAlone()
{
    QWidget row;
    MaterialRadio *first = new MaterialRadio(&row);
    first->setText(QStringLiteral("Daily"));
    MaterialRadio *second = new MaterialRadio(&row);
    second->setText(QStringLiteral("Weekly"));

    first->setChecked(true);
    QVERIFY(first->isChecked());
    QVERIFY(!second->isChecked());

    second->setChecked(true);
    QVERIFY(second->isChecked());
    QVERIFY(!first->isChecked());

    // A button already in force stays so.
    second->setChecked(true);
    QVERIFY(second->isChecked());
}

/*! The ripple answers from the box, not from where the pointer landed: a
 *  press on the words sends the ink to the box's own centre. At a moment
 *  when the centre is carrying ink, the corner its radius has not reached is
 *  still the page's own colour - a ripple anchored at the press would have
 *  covered the whole box from the label's side by then. */
void tst_MaterialCheckable::theRippleAnswersFromTheBoxNotTheRow()
{
    MaterialCheckBox box;
    box.setText(QStringLiteral("Include archives"));
    box.resize(box.sizeHint());

    QTest::mousePress(&box, Qt::LeftButton, Qt::NoModifier, QPoint(box.width() - 6, box.height() / 2));
    QTest::qWait(80);
    const QImage pressed = box.grab().toImage();
    // The wash under the held press is flat and still, so what the centre
    // carries beyond the corner is the ripple, arriving from the box's own
    // centre. A ripple anchored where the press landed would have covered
    // the whole box from the label's side by now, and the two would read the
    // same.
    QVERIFY(pressed.pixelColor(9, 9) != pressed.pixelColor(2, 2));
    QTest::mouseRelease(&box, Qt::LeftButton, Qt::NoModifier, QPoint(box.width() - 6, box.height() / 2));
    QTest::qWait(400);
}

/*! What it is, is a check box or a radio button, and what is true of it is
 *  that it is checkable and when it is on, checked. */
void tst_MaterialCheckable::whatAnAssistiveToolReads()
{
    MaterialCheckBox box;
    QAccessibleInterface *boxFace = QAccessible::queryAccessibleInterface(&box);
    QVERIFY(boxFace);
    QCOMPARE(boxFace->role(), QAccessible::CheckBox);
    QVERIFY(boxFace->state().checkable);
    box.setCheckState(Qt::Checked);
    QVERIFY(QAccessible::queryAccessibleInterface(&box)->state().checked);

    MaterialRadio radio;
    QAccessibleInterface *radioFace = QAccessible::queryAccessibleInterface(&radio);
    QVERIFY(radioFace);
    QCOMPARE(radioFace->role(), QAccessible::RadioButton);
    QVERIFY(radioFace->state().checkable);
    radio.setChecked(true);
    QVERIFY(QAccessible::queryAccessibleInterface(&radio)->state().checked);
}

/*! A choice nobody can make is written a step down - the box and the radio both,
 *  since the two faces of one answer share one hand. Pinned first, so what
 *  differs is the family's drawing rather than the palette's disabled group.
 */
void tst_MaterialCheckable::aChoiceNobodyCanMakeIsDrawnFainter()
{
    MaterialCheckBox box;
    box.setText(QStringLiteral("Remember me"));
    box.resize(box.sizeHint());
    box.show();
    qtcPinDisabledColours(&box);

    const QImage boxLive = box.grab().toImage();
    box.setEnabled(false);
    QVERIFY(box.grab().toImage() != boxLive);

    MaterialRadio radio;
    radio.setText(QStringLiteral("Monthly"));
    radio.resize(radio.sizeHint());
    radio.show();
    qtcPinDisabledColours(&radio);

    const QImage radioLive = radio.grab().toImage();
    radio.setEnabled(false);
    QVERIFY(radio.grab().toImage() != radioLive);
}

TEST_ADD(tst_MaterialCheckable)

#include "tst_materialcheckable.moc"
