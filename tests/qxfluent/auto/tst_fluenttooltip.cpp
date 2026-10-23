/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluenttooltip.h"

#include <QAccessible>
#include <QEvent>
#include <QTimer>
#include <QWidget>

QX_FLUENT_USE_NAMESPACE

namespace
{
constexpr int kGap = 8;

/*! A pointer arrival is the anchor's Enter event; the tooltip watches, it
 *  does not wait to be told. */
void arrive(QWidget *anchor)
{
    QEvent enter(QEvent::Enter);
    QApplication::sendEvent(anchor, &enter);
}

void depart(QWidget *anchor)
{
    QEvent leave(QEvent::Leave);
    QApplication::sendEvent(anchor, &leave);
}

/*! A keyboard arrival is the anchor's focus events; the tooltip watches
 *  those the same way, so the walk over is what it answers. */
void focusArrives(QWidget *anchor)
{
    QEvent focusIn(QEvent::FocusIn);
    QApplication::sendEvent(anchor, &focusIn);
}

void focusLeaves(QWidget *anchor)
{
    QEvent focusOut(QEvent::FocusOut);
    QApplication::sendEvent(anchor, &focusOut);
}
}   // namespace

class tst_FluentToolTip : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void theNoteStandsWhereItIsTold();
    void anArrivalThatDoesNotStaySaysNothing();
    void anArrivalThatStaysOpens();
    void theKeyboardOpensItTooAndLeavingClosesIt();
    void whatItIsForAnAssistiveTool();
private:
    bool m_wasActive = false;
};

void tst_FluentToolTip::init()
{
    m_wasActive = QAccessible::isActive();
}

void tst_FluentToolTip::cleanup()
{
    QAccessible::setActive(m_wasActive);
}

/*! Four sides, one answer each: the note stands a gap off the side it was
 *  told, centred along it. */
void tst_FluentToolTip::theNoteStandsWhereItIsTold()
{
    QWidget anchor;
    anchor.move(40, 40);
    anchor.resize(100, 40);
    anchor.show();

    FluentToolTip tip;
    tip.setAnchor(&anchor);
    tip.setText("Saved to the cloud");
    QVERIFY(tip.anchor() == &anchor);

    tip.setPlacement(FluentToolTip::Placement::Top);
    tip.popup();
    QVERIFY(tip.isVisible());
    QCOMPARE(tip.geometry().center().x(), anchor.geometry().center().x());
    QCOMPARE(tip.geometry().bottom(), anchor.geometry().top() - kGap - 1);

    tip.setPlacement(FluentToolTip::Placement::Bottom);
    QCOMPARE(tip.geometry().center().x(), anchor.geometry().center().x());
    QCOMPARE(tip.geometry().top(), anchor.geometry().bottom() + kGap + 1);

    tip.setPlacement(FluentToolTip::Placement::Left);
    QCOMPARE(tip.geometry().right(), anchor.geometry().left() - kGap - 1);
    QCOMPARE(tip.geometry().center().y(), anchor.geometry().center().y());

    tip.setPlacement(FluentToolTip::Placement::Right);
    QCOMPARE(tip.geometry().left(), anchor.geometry().right() + kGap + 1);
    QCOMPARE(tip.geometry().center().y(), anchor.geometry().center().y());
}

/*! The delay is where a passing arrival is found out: gone before it is up,
 *  the note never shows at all. */
void tst_FluentToolTip::anArrivalThatDoesNotStaySaysNothing()
{
    QWidget anchor;
    anchor.resize(100, 40);
    anchor.show();

    FluentToolTip tip;
    tip.setAnchor(&anchor);
    tip.setText("Saved to the cloud");

    arrive(&anchor);
    QTest::qWait(100);
    QVERIFY(!tip.isVisible());

    depart(&anchor);
    QTest::qWait(400);
    QVERIFY(!tip.isVisible());
}

/*! An arrival that stays has waited out the delay, and the note opens. */
void tst_FluentToolTip::anArrivalThatStaysOpens()
{
    QWidget anchor;
    anchor.resize(100, 40);
    anchor.show();

    FluentToolTip tip;
    tip.setAnchor(&anchor);
    tip.setText("Saved to the cloud");

    arrive(&anchor);
    QTest::qWait(500);
    QVERIFY(tip.isVisible());
}

/*! The keyboard arrives the way the pointer does, and leaving is leaving:
 *  the note goes the moment the focus does. */
void tst_FluentToolTip::theKeyboardOpensItTooAndLeavingClosesIt()
{
    QWidget anchor;
    anchor.setFocusPolicy(Qt::StrongFocus);
    anchor.resize(100, 40);
    anchor.show();

    FluentToolTip tip;
    tip.setAnchor(&anchor);
    tip.setText("Saved to the cloud");

    focusArrives(&anchor);
    QTest::qWait(500);
    QVERIFY(tip.isVisible());

    focusLeaves(&anchor);
    QVERIFY(!tip.isVisible());
}

/*! A note is a note: an assistive tool reads it as the role that says so. */
void tst_FluentToolTip::whatItIsForAnAssistiveTool()
{
    QAccessible::setActive(true);
    QWidget anchor;
    anchor.show();

    FluentToolTip tip;
    tip.setAnchor(&anchor);
    tip.setText("Saved to the cloud");
    tip.popup();

    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&tip);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::ToolTip);

    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

TEST_ADD(tst_FluentToolTip)

#include "tst_fluenttooltip.moc"
