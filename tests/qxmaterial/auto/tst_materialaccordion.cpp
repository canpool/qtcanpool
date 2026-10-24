/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialaccordion.h"
#include "qxmaterial/materialtokens.h"

#include "qxtheme/qxthememanager.h"
#include "qxtheme/qxthememotion.h"

#include <QLabel>
#include <QSignalSpy>

QX_MATERIAL_USE_NAMESPACE

namespace
{
// A header is forty-eight tall, the height of a list row.
constexpr int kHeaderHeight = 48;

/*! How long a body takes to come out, from the family's own answer. */
int travelDuration()
{
    return MaterialMotion::forDuration(MaterialMotion::Duration::Medium).duration;
}

/*! A body of a known height, so that the room a header gives it can be read
 *  off the column. */
QLabel *body(int height = 120)
{
    auto *label = new QLabel(QStringLiteral("Body"));
    label->setFixedHeight(height);
    return label;
}
}   // namespace

class tst_MaterialAccordion : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void aSectionIsAHeaderAndABody();
    void openingGivesTheBodyRoom();
    void theBodyComesOutOverTime();
    void oneAtATime();
    void moreThanOneWhenAsked();
    void noMotionOpensItAtOnce();
    void aClickOnTheHeaderOpensIt();
    void theKeyboardOpensIt();
    void aSectionNobodyCanOpenIsDrawnFainter();
private:
    bool m_wasReduced = false;
};

void tst_MaterialAccordion::init()
{
    qtcGiveFonts();
    m_wasReduced = QxTheme::isMotionReduced();
    QxTheme::setMotionReduced(false);
}

void tst_MaterialAccordion::cleanup()
{
    QxTheme::setMotionReduced(m_wasReduced);
}

/*! A header keeps the body it was given, and says what it is called. */
void tst_MaterialAccordion::aSectionIsAHeaderAndABody()
{
    MaterialAccordion accordion;
    QLabel *shipping = body();
    const int index = accordion.addSection(QStringLiteral("Shipping"), shipping);
    accordion.addSection(QStringLiteral("Payment"));

    QCOMPARE(index, 0);
    QCOMPARE(accordion.sectionCount(), 2);
    QCOMPARE(accordion.sectionTitle(0), QStringLiteral("Shipping"));
    QCOMPARE(accordion.sectionTitle(2), QString());
    QCOMPARE(accordion.sectionContent(0), shipping);
    QVERIFY(!accordion.isSectionExpanded(0));
    // Two headers and no body out: the column is two headers tall.
    QCOMPARE(accordion.sizeHint().height(), 2 * kHeaderHeight);
}

/*! Opening a header gives its body room, and the room is the body's. */
void tst_MaterialAccordion::openingGivesTheBodyRoom()
{
    MaterialAccordion accordion;
    QLabel *shipping = body();
    accordion.addSection(QStringLiteral("Shipping"), shipping);
    const int bodyHeight = shipping->sizeHint().height();

    accordion.setSectionExpanded(0, true);
    QTest::qWait(travelDuration() + 80);

    QCOMPARE(accordion.sizeHint().height(), kHeaderHeight + bodyHeight);
    // isVisible() answers for the whole ancestry; the body itself is out.
    QVERIFY(!shipping->isHidden());
    QCOMPARE(shipping->height(), bodyHeight);
}

/*! The room is given over time, not at once: a moment after the header was
 *  opened the body is out part of the way, and after that it is all the way. */
void tst_MaterialAccordion::theBodyComesOutOverTime()
{
    MaterialAccordion accordion;
    QLabel *shipping = body();
    accordion.addSection(QStringLiteral("Shipping"), shipping);
    const int bodyHeight = shipping->sizeHint().height();

    accordion.setSectionExpanded(0, true);
    // A moment in: the body is out part of the way, not all of it.
    QTest::qWait(travelDuration() / 3);
    const int onItsWay = accordion.sizeHint().height();
    QVERIFY(onItsWay > kHeaderHeight);
    QVERIFY(onItsWay < kHeaderHeight + bodyHeight);

    QTest::qWait(travelDuration() + 80);
    QCOMPARE(accordion.sizeHint().height(), kHeaderHeight + bodyHeight);
}

/*! One at a time is the column's default: opening a header closes the one
 *  that was open, and the column says so about both. */
void tst_MaterialAccordion::oneAtATime()
{
    MaterialAccordion accordion;
    accordion.addSection(QStringLiteral("Shipping"), body());
    accordion.addSection(QStringLiteral("Payment"), body());
    QCOMPARE(accordion.mode(), MaterialAccordion::Mode::Single);

    QSignalSpy changedSpy(&accordion, &MaterialAccordion::sectionExpandedChanged);
    accordion.setSectionExpanded(0, true);
    QCOMPARE(changedSpy.count(), 1);
    QCOMPARE(changedSpy.at(0).at(0).toInt(), 0);
    QCOMPARE(changedSpy.at(0).at(1).toBool(), true);

    accordion.setSectionExpanded(1, true);
    // Two answers: the first header closed, the second opened.
    QCOMPARE(changedSpy.count(), 3);
    QVERIFY(!accordion.isSectionExpanded(0));
    QVERIFY(accordion.isSectionExpanded(1));
    QCOMPARE(changedSpy.at(1).at(0).toInt(), 0);
    QCOMPARE(changedSpy.at(1).at(1).toBool(), false);
    QCOMPARE(changedSpy.at(2).at(0).toInt(), 1);
    QCOMPARE(changedSpy.at(2).at(1).toBool(), true);

    // Saying what is already true says nothing.
    accordion.setSectionExpanded(1, true);
    QCOMPARE(changedSpy.count(), 3);
}

/*! Asked for more than one, every header answers for itself. */
void tst_MaterialAccordion::moreThanOneWhenAsked()
{
    MaterialAccordion accordion;
    accordion.addSection(QStringLiteral("Shipping"), body());
    accordion.addSection(QStringLiteral("Payment"), body());

    QSignalSpy modeSpy(&accordion, &MaterialAccordion::modeChanged);
    accordion.setMode(MaterialAccordion::Mode::Multiple);
    QCOMPARE(accordion.mode(), MaterialAccordion::Mode::Multiple);
    QCOMPARE(modeSpy.count(), 1);

    accordion.setSectionExpanded(0, true);
    accordion.setSectionExpanded(1, true);
    QVERIFY(accordion.isSectionExpanded(0));
    QVERIFY(accordion.isSectionExpanded(1));
}

/*! Asked for less motion, a header is simply open: the movement was never the
 *  point. */
void tst_MaterialAccordion::noMotionOpensItAtOnce()
{
    QxTheme::setMotionReduced(true);

    MaterialAccordion accordion;
    QLabel *shipping = body();
    accordion.addSection(QStringLiteral("Shipping"), shipping);

    accordion.setSectionExpanded(0, true);
    // No wait: the room is there at once.
    QCOMPARE(accordion.sizeHint().height(), kHeaderHeight + shipping->sizeHint().height());
}

/*! A click on the header opens it; a click on the body is the body's, and a
 *  second click on the header closes it again. */
void tst_MaterialAccordion::aClickOnTheHeaderOpensIt()
{
    MaterialAccordion accordion;
    QLabel *shipping = body();
    accordion.addSection(QStringLiteral("Shipping"), shipping);
    accordion.resize(320, kHeaderHeight);
    accordion.show();
    QWidget *header = shipping->parentWidget();

    QTest::mouseClick(header, Qt::LeftButton, Qt::NoModifier, QPoint(100, 20));
    QVERIFY(accordion.isSectionExpanded(0));

    // A click on the body is the body's, not the header's.
    QTest::qWait(travelDuration() + 80);
    QTest::mouseClick(header, Qt::LeftButton, Qt::NoModifier, QPoint(100, kHeaderHeight + 40));
    QVERIFY(accordion.isSectionExpanded(0));

    // A second click on the header closes it again.
    QTest::mouseClick(header, Qt::LeftButton, Qt::NoModifier, QPoint(100, 20));
    QVERIFY(!accordion.isSectionExpanded(0));
}

/*! The keyboard asks the same question the pointer does. */
void tst_MaterialAccordion::theKeyboardOpensIt()
{
    MaterialAccordion accordion;
    accordion.addSection(QStringLiteral("Shipping"), body());
    accordion.resize(320, kHeaderHeight);
    accordion.show();

    QWidget *header = accordion.sectionContent(0)->parentWidget();
    header->setFocus();
    QTest::keyClick(header, Qt::Key_Space);
    QVERIFY(accordion.isSectionExpanded(0));
    QTest::keyClick(header, Qt::Key_Return);
    QVERIFY(!accordion.isSectionExpanded(0));
}

/*! A section nobody can open is written a step down - both headers, since the
 *  whole accordion is what was turned off. Pinned first, leaving only its own
 *  hand between the two frames.
 */
void tst_MaterialAccordion::aSectionNobodyCanOpenIsDrawnFainter()
{
    MaterialAccordion accordion;
    accordion.addSection(QStringLiteral("Shipping"), body());
    accordion.addSection(QStringLiteral("Payment"));
    accordion.resize(320, accordion.sizeHint().height());
    accordion.show();
    qtcPinDisabledColours(&accordion);

    const QImage live = accordion.grab().toImage();
    accordion.setEnabled(false);
    const QImage faded = accordion.grab().toImage();
    QVERIFY(faded != live);
}

TEST_ADD(tst_MaterialAccordion)

#include "tst_materialaccordion.moc"
