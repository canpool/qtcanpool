/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

/*!
 * \file
 *
 * Tests for the shared helpers in windowkit_p.h.
 *
 * getMouseEventScenePos() and getMouseEventGlobalPos() exist only because Qt 6
 * renamed what Qt 5 called windowPos()/screenPos(), so each is a version branch
 * where the wrong branch still compiles and still returns a plausible point. A
 * test that passes on Qt 6 says nothing about Qt 5, and the other way round -
 * the only thing that can check both is to build distinct values into the event
 * and read one of them back, which is what these do.
 */
#include "tst_global.h"

#include "qxwindow/windowkit_p.h"

#include <QtCore/QPoint>
#include <QtGui/QMouseEvent>

class tst_QxWindowKit : public QObject
{
    Q_OBJECT

private slots:
    void scenePositionIsReadFromTheEvent();
    void globalPositionIsReadFromTheEvent();
    void sceneAndGlobalAreNotConfused();
    void fractionalPositionsAreRoundedNotTruncated();
    void negativePositionsRoundSymmetricallyAboutZero();
};

/*!
 * Builds a mouse event whose local, window/scene and screen/global positions are
 * all different, so reading the wrong one cannot pass by accident.
 *
 * The three-position constructor exists in both Qt 5 and Qt 6 with the same
 * meaning; only the name of the middle field changed, which is exactly the
 * difference the helpers are hiding.
 */
static QMouseEvent makeEvent(const QPoint &local, const QPoint &middle, const QPoint &global)
{
    return QMouseEvent(QEvent::MouseButtonPress, QPointF(local), QPointF(middle), QPointF(global), Qt::LeftButton,
                       Qt::NoButton, Qt::NoModifier);
}

void tst_QxWindowKit::scenePositionIsReadFromTheEvent()
{
    const QMouseEvent event = makeEvent(QPoint(1, 2), QPoint(30, 40), QPoint(300, 400));
    QCOMPARE(QxWindow::getMouseEventScenePos(&event), QPoint(30, 40));
}

void tst_QxWindowKit::globalPositionIsReadFromTheEvent()
{
    const QMouseEvent event = makeEvent(QPoint(1, 2), QPoint(30, 40), QPoint(300, 400));
    QCOMPARE(QxWindow::getMouseEventGlobalPos(&event), QPoint(300, 400));
}

void tst_QxWindowKit::sceneAndGlobalAreNotConfused()
{
    // The two helpers take their values from different fields, and on Qt 6 those
    // are scenePosition() and globalPosition(); on Qt 5 they are windowPos() and
    // screenPos(). Swapping them compiles on both, so the only thing that
    // catches it is having the two fields disagree.
    const QMouseEvent event = makeEvent(QPoint(7, 8), QPoint(70, 80), QPoint(700, 800));

    QCOMPARE(QxWindow::getMouseEventScenePos(&event), QPoint(70, 80));
    QCOMPARE(QxWindow::getMouseEventGlobalPos(&event), QPoint(700, 800));

    // Not equal, and neither is the local position: three distinct answers
    // from one event, so no substitution goes unnoticed.
    QVERIFY(QxWindow::getMouseEventScenePos(&event) != QxWindow::getMouseEventGlobalPos(&event));
    QVERIFY(QxWindow::getMouseEventScenePos(&event) != QPoint(7, 8));
    QVERIFY(QxWindow::getMouseEventGlobalPos(&event) != QPoint(7, 8));
}

void tst_QxWindowKit::fractionalPositionsAreRoundedNotTruncated()
{
    // toPoint() rounds to the nearest integer on both Qt majors - it does not
    // truncate towards zero, and the negative cases below are what make the
    // difference visible: -9.4 rounds to -9, where truncation would also give
    // -9, but -9.6 rounds to -10 where truncation gives -9.
    //
    // Pinned because the hit test divides by this point: rounding and
    // truncating disagree about every half pixel near a border, so a change
    // here moves where the resize strip ends.
    const QMouseEvent event = makeEvent(QPoint(0, 0), QPoint(70, 80), QPoint(70, 80));
    QCOMPARE(QxWindow::getMouseEventScenePos(&event), QPoint(70, 80));

    // Half-pixel positions arrive through the QPointF overload of the same
    // constructor. Just below the midpoint rounds down, just above rounds up.
    const QMouseEvent below(QEvent::MouseMove, QPointF(0, 0), QPointF(9.4, 19.4), QPointF(99.4, 119.4), Qt::NoButton,
                            Qt::NoButton, Qt::NoModifier);
    QCOMPARE(QxWindow::getMouseEventScenePos(&below), QPoint(9, 19));
    QCOMPARE(QxWindow::getMouseEventGlobalPos(&below), QPoint(99, 119));

    const QMouseEvent above(QEvent::MouseMove, QPointF(0, 0), QPointF(9.6, 19.6), QPointF(99.6, 119.6), Qt::NoButton,
                            Qt::NoButton, Qt::NoModifier);
    QCOMPARE(QxWindow::getMouseEventScenePos(&above), QPoint(10, 20));
    QCOMPARE(QxWindow::getMouseEventGlobalPos(&above), QPoint(100, 120));
}

void tst_QxWindowKit::negativePositionsRoundSymmetricallyAboutZero()
{
    // A window on a secondary monitor to the left or above the origin produces
    // negative coordinates. Rounding is symmetric about zero here, so -9.4
    // becomes -9 and -9.6 becomes -10; the sign does not flip the outcome the
    // way a floor-based rule would.
    const QMouseEvent event = makeEvent(QPoint(-5, -6), QPoint(-70, -80), QPoint(-700, -800));
    QCOMPARE(QxWindow::getMouseEventScenePos(&event), QPoint(-70, -80));
    QCOMPARE(QxWindow::getMouseEventGlobalPos(&event), QPoint(-700, -800));

    const QMouseEvent nearZero(QEvent::MouseMove, QPointF(0, 0), QPointF(-9.4, -19.4), QPointF(-99.4, -119.4),
                               Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QCOMPARE(QxWindow::getMouseEventScenePos(&nearZero), QPoint(-9, -19));
    QCOMPARE(QxWindow::getMouseEventGlobalPos(&nearZero), QPoint(-99, -119));

    const QMouseEvent pastMidpoint(QEvent::MouseMove, QPointF(0, 0), QPointF(-9.6, -19.6), QPointF(-99.6, -119.6),
                                   Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QCOMPARE(QxWindow::getMouseEventScenePos(&pastMidpoint), QPoint(-10, -20));
    QCOMPARE(QxWindow::getMouseEventGlobalPos(&pastMidpoint), QPoint(-100, -120));
}

TEST_ADD(tst_QxWindowKit)

#include "tst_qxwindowkit.moc"