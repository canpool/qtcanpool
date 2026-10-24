/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialstepper.h"

#include "qxtheme/qxthememanager.h"

#include <QSignalSpy>

QX_MATERIAL_USE_NAMESPACE

namespace
{
// The row's own measures, read the way a reader reads them off the frame: a
// circle of twenty-four three from the top, its name eight under it, a column
// never narrower than ninety-six, and sixteen at each side of the row.
constexpr qreal kPadding = 16.0;
constexpr qreal kCircle = 24.0;
constexpr qreal kRingRoom = 3.0;

qreal columnWidth(int columns, int width = 361)
{
    return (qreal(width) - 2.0 * kPadding) / qreal(columns);
}

int accentCount(const QImage &image, const QRect &room, const QColor &accent)
{
    int count = 0;
    for (int y = room.top(); y <= room.bottom(); ++y) {
        for (int x = room.left(); x <= room.right(); ++x) {
            if (image.pixelColor(x, y) == accent) {
                ++count;
            }
        }
    }
    return count;
}

QPointF circleCentre(int index, int columns, int width = 361)
{
    const qreal x = kPadding + qreal(index) * columnWidth(columns, width) + columnWidth(columns, width) / 2.0;
    return QPointF(x, kRingRoom + kCircle / 2.0);
}
}   // namespace

class tst_MaterialStepper : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void threeStepsThreeStates();
    void theStatesWearThreeFaces();
    void aStepBehindCanBeTakenAgain();
    void sayingTheSameStepTwiceSaysNothing();
    void captionsMakeTheRowTaller();
    void theKeyboardWalksBack();
private:
    bool m_hadOverride = false;
    MaterialStepper *m_stepper = nullptr;
};

void tst_MaterialStepper::init()
{
    qtcGiveFonts();
    m_hadOverride = QxTheme::QxThemeManager::instance()->hasAccentOverride();
    QxTheme::QxThemeManager::instance()->setAccentColor(QColor(0, 0, 255));
    m_stepper = new MaterialStepper;
    m_stepper->resize(361, m_stepper->sizeHint().height());
    m_stepper->show();
}

void tst_MaterialStepper::cleanup()
{
    delete m_stepper;
    m_stepper = nullptr;
    if (!m_hadOverride) {
        QxTheme::QxThemeManager::instance()->setAccentColor(QColor());
    }
}

/*! Where the work stands is what makes a step: the ones behind it are
 *  completed, the one it is at is current, the rest are upcoming. */
void tst_MaterialStepper::threeStepsThreeStates()
{
    m_stepper->addStep(QStringLiteral("Cart"));
    m_stepper->addStep(QStringLiteral("Ship"));
    m_stepper->addStep(QStringLiteral("Done"));
    QCOMPARE(m_stepper->count(), 3);

    // The work starts at the first step, and nothing is behind it yet.
    QCOMPARE(m_stepper->currentStep(), 0);
    QCOMPARE(m_stepper->stepState(0), MaterialStepper::State::Current);
    QCOMPARE(m_stepper->stepState(1), MaterialStepper::State::Upcoming);
    QCOMPARE(m_stepper->stepState(2), MaterialStepper::State::Upcoming);

    m_stepper->setCurrentStep(1);
    QCOMPARE(m_stepper->stepState(0), MaterialStepper::State::Completed);
    QCOMPARE(m_stepper->stepState(1), MaterialStepper::State::Current);
    QCOMPARE(m_stepper->stepState(2), MaterialStepper::State::Upcoming);

    m_stepper->setCurrentStep(2);
    QCOMPARE(m_stepper->stepState(0), MaterialStepper::State::Completed);
    QCOMPARE(m_stepper->stepState(1), MaterialStepper::State::Completed);
    QCOMPARE(m_stepper->stepState(2), MaterialStepper::State::Current);

    // A step that is not there has no name to say.
    QCOMPARE(m_stepper->stepLabel(3), QString());
    QCOMPARE(m_stepper->stepCaption(3), QString());
}

/*! The three states are three faces: the walked one is filled with the accent
 *  and ticked, the one in force is the accent's ring around the page, and the
 *  one ahead is a quiet outline around the page. */
void tst_MaterialStepper::theStatesWearThreeFaces()
{
    m_stepper->addStep(QStringLiteral("Cart"));
    m_stepper->addStep(QStringLiteral("Ship"));
    m_stepper->addStep(QStringLiteral("Done"));
    m_stepper->setCurrentStep(1);

    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
    const QImage image = m_stepper->grab().toImage();

    const auto circleRoom = [](int index) {
        const QPointF centre = circleCentre(index, 3);
        return QRect(int(centre.x() - kCircle / 2.0), int(centre.y() - kCircle / 2.0), int(kCircle), int(kCircle));
    };

    // A point inside a circle that neither the tick nor the number crosses:
    // eight to the left of its middle.
    const auto inside = [](int index) {
        const QPointF centre = circleCentre(index, 3);
        return QPointF(centre.x() - 8.0, centre.y()).toPoint();
    };

    // Walked: inside is the accent itself - a fill, not a ring.
    QCOMPARE(image.pixelColor(inside(0)), accent);
    QVERIFY(accentCount(image, circleRoom(0), accent) > 100);

    // In force: inside is the page, since the accent is a ring drawn around
    // it - and the ring is there all the same.
    QVERIFY(image.pixelColor(inside(1)) != accent);
    QVERIFY(accentCount(image, circleRoom(1), accent) > 0);

    // Ahead: nothing about its circle is the accent at all.
    QVERIFY(image.pixelColor(inside(2)) != accent);
    QCOMPARE(accentCount(image, circleRoom(2), accent), 0);
}

/*! A step that is behind can be taken again by clicking it; a step ahead
 *  cannot, and the step in force is already taken. */
void tst_MaterialStepper::aStepBehindCanBeTakenAgain()
{
    m_stepper->addStep(QStringLiteral("Cart"));
    m_stepper->addStep(QStringLiteral("Ship"));
    m_stepper->addStep(QStringLiteral("Done"));
    m_stepper->setCurrentStep(1);

    QSignalSpy changedSpy(m_stepper, &MaterialStepper::currentChanged);

    // A step ahead: the row does not walk ahead of the work.
    QTest::mouseClick(m_stepper, Qt::LeftButton, Qt::NoModifier, circleCentre(2, 3).toPoint());
    QCOMPARE(m_stepper->currentStep(), 1);
    QCOMPARE(changedSpy.count(), 0);

    // A step behind: the work goes back to it.
    QTest::mouseClick(m_stepper, Qt::LeftButton, Qt::NoModifier, circleCentre(0, 3).toPoint());
    QCOMPARE(m_stepper->currentStep(), 0);
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(changedSpy.at(0).at(0).toInt(), 0);
}

/*! Saying the step that is already in force says nothing, and a step past the
 *  last one is no answer at all. */
void tst_MaterialStepper::sayingTheSameStepTwiceSaysNothing()
{
    m_stepper->addStep(QStringLiteral("Cart"));
    m_stepper->addStep(QStringLiteral("Ship"));

    QSignalSpy changedSpy(m_stepper, &MaterialStepper::currentChanged);
    m_stepper->setCurrentStep(0);
    QCOMPARE(changedSpy.count(), 0);

    m_stepper->setCurrentStep(1);
    QCOMPARE(changedSpy.count(), 1);

    // Past the end, and before the beginning: neither is a step.
    m_stepper->setCurrentStep(2);
    m_stepper->setCurrentStep(-1);
    QCOMPARE(m_stepper->currentStep(), 1);
    QCOMPARE(changedSpy.count(), 1);
}

/*! A row of names is one line tall; a row that also says a caption under each
 *  name is taller by that line. */
void tst_MaterialStepper::captionsMakeTheRowTaller()
{
    m_stepper->addStep(QStringLiteral("Cart"));
    const int plain = m_stepper->sizeHint().height();

    MaterialStepper withCaption;
    withCaption.addStep(QStringLiteral("Cart"), QStringLiteral("Three items"));
    QVERIFY(withCaption.sizeHint().height() > plain);

    // The caption belongs to its step, and asking for it answers with it.
    QCOMPARE(withCaption.stepCaption(0), QStringLiteral("Three items"));
    QCOMPARE(m_stepper->stepCaption(0), QString());
}

/*! The row takes the keyboard for the one thing it can do: walking back. */
void tst_MaterialStepper::theKeyboardWalksBack()
{
    m_stepper->addStep(QStringLiteral("Cart"));
    m_stepper->addStep(QStringLiteral("Ship"));
    m_stepper->addStep(QStringLiteral("Done"));
    m_stepper->setCurrentStep(2);

    m_stepper->setFocus();
    QTest::keyClick(m_stepper, Qt::Key_Left);
    QCOMPARE(m_stepper->currentStep(), 1);
    QTest::keyClick(m_stepper, Qt::Key_Up);
    QCOMPARE(m_stepper->currentStep(), 0);
    // At the first step there is nowhere back to go.
    QTest::keyClick(m_stepper, Qt::Key_Left);
    QCOMPARE(m_stepper->currentStep(), 0);
}

TEST_ADD(tst_MaterialStepper)

#include "tst_materialstepper.moc"
