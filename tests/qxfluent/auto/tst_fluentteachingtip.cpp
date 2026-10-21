/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentteachingtip.h"

#include <QAccessible>
#include <QCloseEvent>
#include <QSignalSpy>
#include <QWidget>

QX_FLUENT_USE_NAMESPACE

namespace
{
// A tip is a three-twenty surface with twelve inside it; the cross is a
// twenty-four square twelve in from the top-right corner.
constexpr int kPadding = 12;
constexpr int kCloseSize = 24;

/*! A window with a target in it, stood up so that global coordinates mean
 *  something: a tip is placed against where its target is on screen. */
struct Scene {
    QWidget window;
    QWidget target;

    Scene(const QSize &size, const QRect &spot)
        : target(&window)
    {
        window.resize(size);
        target.setGeometry(spot);
        window.show();
    }

    QRect targetRect() const
    {
        return QRect(target.mapToGlobal(QPoint(0, 0)), target.size());
    }
};
}   // namespace

class tst_FluentTeachingTip : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void itPointsAtWhatItIsAbout();
    void theOtherSideWhenThereIsNoRoom();
    void theSideItWasAskedFor();
    void theCrossDismissesIt();
    void aTipWithNothingToPointAtIsNothing();
    void whatItSaysIsWhatItIs();
    void whatAnAssistiveToolReads();
};

void tst_FluentTeachingTip::cleanup()
{
    QAccessible::setActive(false);
}

/*! A tip is not a tool tip: it is opened on a thing, and it points at that
 *  thing - the tail is what says which of the things on screen the sentence
 *  is about. */
void tst_FluentTeachingTip::itPointsAtWhatItIsAbout()
{
    Scene scene(QSize(600, 600), QRect(250, 200, 40, 20));
    FluentTeachingTip tip;
    tip.setTarget(&scene.target);

    tip.setOpen(true);
    QVERIFY(tip.isOpen());
    QCOMPARE(tip.side(), FluentTeachingTip::Side::Bottom);

    const QRect spot = scene.targetRect();
    QVERIFY(tip.geometry().top() >= spot.bottom());
    // The tail stands under the middle of what it is about - to the pixel a
    // rectangle's own middle can be given, which for an even width is one off.
    QVERIFY(qAbs(tip.geometry().center().x() - spot.center().x()) <= 1);
}

/*! Which side it takes is not a preference but a measurement: below when the
 *  room is there, above when it is not. */
void tst_FluentTeachingTip::theOtherSideWhenThereIsNoRoom()
{
    Scene scene(QSize(600, 320), QRect(250, 280, 40, 20));
    FluentTeachingTip tip;
    tip.setTarget(&scene.target);

    tip.setOpen(true);
    QCOMPARE(tip.side(), FluentTeachingTip::Side::Top);
    QVERIFY(tip.geometry().bottom() <= scene.targetRect().top());

    tip.setOpen(false);
    QVERIFY(!tip.isOpen());
}

/*! A side can be asked for, and what is asked for is what it takes - the
 *  measurement is only for the ones that did not say. */
void tst_FluentTeachingTip::theSideItWasAskedFor()
{
    Scene scene(QSize(600, 600), QRect(450, 200, 40, 20));
    FluentTeachingTip tip;
    tip.setTarget(&scene.target);
    tip.setPreferredSide(FluentTeachingTip::Side::Left);

    tip.setOpen(true);
    QCOMPARE(tip.side(), FluentTeachingTip::Side::Left);
    QVERIFY(tip.geometry().right() <= scene.targetRect().left());
}

/*! A tip stays until it is dismissed, and the cross is the way the user
 *  dismisses it - it says it went, because the application opened it. */
void tst_FluentTeachingTip::theCrossDismissesIt()
{
    Scene scene(QSize(600, 600), QRect(250, 200, 40, 20));
    FluentTeachingTip tip;
    tip.setTarget(&scene.target);
    tip.setOpen(true);

    QSignalSpy closed(&tip, &FluentTeachingTip::closed);
    QTest::mouseClick(&tip, Qt::LeftButton, Qt::NoModifier,
                      QPoint(tip.width() - kPadding - kCloseSize / 2, kPadding + kCloseSize / 2));

    QVERIFY(!tip.isOpen());
    QCOMPARE(closed.count(), 1);
}

/*! A tip with nothing to point at is not a tip: it is a sentence floating over
 *  the window, and it does not open. */
void tst_FluentTeachingTip::aTipWithNothingToPointAtIsNothing()
{
    FluentTeachingTip tip;
    tip.setOpen(true);
    QVERIFY(!tip.isOpen());
    QVERIFY(!tip.isVisible());
}

/*! A tip is a title and a sentence, and it is as tall as the sentence needs
 *  once it has been wrapped. */
void tst_FluentTeachingTip::whatItSaysIsWhatItIs()
{
    FluentTeachingTip tip;
    tip.setTitle(QStringLiteral("Try this"));
    QCOMPARE(tip.title(), QStringLiteral("Try this"));
    const int titled = tip.sizeHint().height();

    tip.setMessage(QStringLiteral("Short."));
    QCOMPARE(tip.message(), QStringLiteral("Short."));
    QVERIFY(tip.sizeHint().height() > titled);

    const int oneLine = tip.sizeHint().height();
    tip.setMessage(QStringLiteral("This is a much longer sentence, the kind a teaching tip exists "
                                  "to carry, and it wraps onto more than one line because the "
                                  "surface it sits on is only three hundred and twenty wide."));
    QVERIFY(tip.sizeHint().height() > oneLine);
}

/*! What an assistive tool is told: a flyout that explains something on screen,
 *  which is a balloon - not a tool tip, which the toolkit owns. */
void tst_FluentTeachingTip::whatAnAssistiveToolReads()
{
    FluentTeachingTip tip;
    QAccessible::setActive(true);
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&tip);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::HelpBalloon);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

TEST_ADD(tst_FluentTeachingTip)

#include "tst_fluentteachingtip.moc"
