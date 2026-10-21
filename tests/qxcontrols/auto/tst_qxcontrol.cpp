/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxcontrols/qxcontrol.h"

#include <QApplication>
#include <QImage>
#include <QMouseEvent>
#include <QPainter>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtGui/QEnterEvent>
#endif

QX_CONTROLS_USE_NAMESPACE

namespace
{
/*! A control with no look of its own: a white surface, no content, and the
 *  base's state machine on top of it. */
class ProbeControl : public QxControl
{
public:
    using QxControl::QxControl;

    int surfacePaints = 0;
    int contentPaints = 0;
protected:
    void paintSurface(QPainter *painter) override
    {
        ++surfacePaints;
        painter->fillRect(rect(), Qt::white);
    }
    void paintContent(QPainter *painter) override
    {
        ++contentPaints;
        Q_UNUSED(painter)
    }
};

void sendEnter(QWidget *widget)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QEnterEvent enter(QPointF(1, 1), QPointF(1, 1), QPointF(1, 1));
    QApplication::sendEvent(widget, &enter);
#else
    QEvent enter(QEvent::Enter);
    QApplication::sendEvent(widget, &enter);
#endif
}

void sendLeave(QWidget *widget)
{
    QEvent leave(QEvent::Leave);
    QApplication::sendEvent(widget, &leave);
}

/*! A press or a release, sent straight at the widget. QTest's own helpers
 *  warn when a disabled widget does not take the event, which is exactly the
 *  answer this suite wants to assert rather than be warned about. */
void sendButton(QWidget *widget, QEvent::Type type, const QPoint &pos)
{
    const QPoint global = widget->mapToGlobal(pos);
    QMouseEvent event(type, pos, global, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(widget, &event);
}
}

class tst_QxControl : public QObject
{
    Q_OBJECT
private slots:
    void pressAndReleaseClicks();
    void releaseOutsideIsNotAClick();
    void hoverTracksThePointer();
    void washFollowsTheState();
    void disabledControlDoesNotClick();
};

/*! A press and its release inside is a click, and the state walks back down. */
void tst_QxControl::pressAndReleaseClicks()
{
    ProbeControl control;
    control.resize(40, 24);
    QSignalSpy clicks(&control, &QxControl::clicked);

    QTest::mousePress(&control, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10));
    QCOMPARE(control.state(), QxControl::State::Pressed);
    QTest::mouseRelease(&control, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10));

    QCOMPARE(clicks.count(), 1);
    QCOMPARE(control.state(), QxControl::State::Hover);
}

/*! The press began on the control and ended somewhere else: no click, and the
 *  control is left at rest. */
void tst_QxControl::releaseOutsideIsNotAClick()
{
    ProbeControl control;
    control.resize(40, 24);
    QSignalSpy clicks(&control, &QxControl::clicked);

    QTest::mousePress(&control, Qt::LeftButton, Qt::NoModifier, QPoint(10, 10));
    QTest::mouseRelease(&control, Qt::LeftButton, Qt::NoModifier, QPoint(-5, -5));

    QCOMPARE(clicks.count(), 0);
    QCOMPARE(control.state(), QxControl::State::Rest);
}

/*! Hover is a state the base tracks, not a flag every subclass keeps. */
void tst_QxControl::hoverTracksThePointer()
{
    ProbeControl control;
    control.resize(40, 24);
    QCOMPARE(control.state(), QxControl::State::Rest);

    sendEnter(&control);
    QCOMPARE(control.state(), QxControl::State::Hover);

    sendLeave(&control);
    QCOMPARE(control.state(), QxControl::State::Rest);
}

/*! The wash lands between the surface and the content: the surface stays the
 *  colour the subclass painted, the middle of the control picks up the wash,
 *  and at rest nothing is tinted. */
void tst_QxControl::washFollowsTheState()
{
    ProbeControl control;
    control.resize(40, 24);

    const QImage rest = control.grab().toImage();
    QCOMPARE(rest.pixelColor(20, 12), QColor(Qt::white));

    sendEnter(&control);
    const QImage hovered = control.grab().toImage();
    const QColor washed = hovered.pixelColor(20, 12);
    QVERIFY(washed.red() < 255);
    QCOMPARE(washed.red(), washed.green());

    // And the subclass's hooks are what drew it: both ran, once per frame.
    QVERIFY(control.surfacePaints >= 2);
    QVERIFY(control.contentPaints >= 2);
}

/*! A disabled control does not react to the pointer at all. */
void tst_QxControl::disabledControlDoesNotClick()
{
    ProbeControl control;
    control.resize(40, 24);
    control.setEnabled(false);
    QSignalSpy clicks(&control, &QxControl::clicked);

    sendButton(&control, QEvent::MouseButtonPress, QPoint(10, 10));
    sendButton(&control, QEvent::MouseButtonRelease, QPoint(10, 10));

    QCOMPARE(clicks.count(), 0);
    QCOMPARE(control.state(), QxControl::State::Rest);
}

TEST_ADD(tst_QxControl)

#include "tst_qxcontrol.moc"
