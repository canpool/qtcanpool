/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxcupertino/cupertinotokens.h"

#include "qxtheme/qxthememotion.h"

#include <QColor>
#include <QFont>
#include <QSet>

QX_CUPERTINO_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

class tst_CupertinoTokens : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void cornersGrowWithTheScale();
    void theRampIsSizesAndWeights();
    void elevationSpreadsUnderHalfItsSpread();
    void durationsRideOnTheTiers();
    void reducedMotionSilencesEveryDuration();
    void theSemanticColoursComeInPairs();
    void anUnknownStepAnswersNothing();
};

void tst_CupertinoTokens::cleanup()
{
    // The motion switch and the scale are process-wide, and the tier ladder is
    // what every other suite reads through.
    QxTheme::setMotionReduced(false);
    QxTheme::setMotionScale(1.0);
}

/*! A corner scale is read as an order: each step turns more than the one
 *  before, and the round one turns more than any rectangle can. */
void tst_CupertinoTokens::cornersGrowWithTheScale()
{
    using R = CupertinoShape::Radius;

    const qreal none = CupertinoShape::radius(R::None);
    const qreal small = CupertinoShape::radius(R::Small);
    const qreal medium = CupertinoShape::radius(R::Medium);
    const qreal large = CupertinoShape::radius(R::Large);
    const qreal xLarge = CupertinoShape::radius(R::XLarge);

    QCOMPARE(none, 0.0);
    QVERIFY(small > none);
    QVERIFY(medium > small);
    QVERIFY(large > medium);
    QVERIFY(xLarge > large);

    // The scale is a close one: the widest corner before "round" is still a
    // corner, not half a control.
    QVERIFY(xLarge <= 16.0);

    // Fully round is a radius no rectangle reaches, which is the only way to
    // say it in a value a painter takes.
    QVERIFY(CupertinoShape::radius(R::Circular) > 1000.0);
}

/*! The ramp answers sizes and weights: the heavier cut of a step is that step's
 *  own size with more weight, so a control does not have to pick a bigger step
 *  to emphasise a line. */
void tst_CupertinoTokens::theRampIsSizesAndWeights()
{
    using T = CupertinoType::Ramp;

    const QFont caption = CupertinoType::font(T::Caption);
    const QFont body = CupertinoType::font(T::Body);
    const QFont subtitle = CupertinoType::font(T::Subtitle);
    const QFont title = CupertinoType::font(T::Title);
    const QFont display = CupertinoType::font(T::Display);

    QVERIFY(caption.pointSizeF() < body.pointSizeF());
    QVERIFY(body.pointSizeF() < subtitle.pointSizeF());
    QVERIFY(subtitle.pointSizeF() < title.pointSizeF());
    QVERIFY(title.pointSizeF() < display.pointSizeF());

    const QFont strong = CupertinoType::font(T::Body, true);
    QCOMPARE(strong.pointSizeF(), body.pointSizeF());
    QVERIFY(strong.weight() > body.weight());
}

/*! Elevation is one answer to "how far and how dark": the spread grows with the
 *  level, the darkness grows with it, and the offset stays well under half the
 *  spread - which is the proportion that makes this family's lift read as light
 *  rather than as a cut-out edge. */
void tst_CupertinoTokens::elevationSpreadsUnderHalfItsSpread()
{
    using L = CupertinoElevation::Level;

    const QxTheme::QxShadow flat = CupertinoElevation::shadow(L::None);
    QCOMPARE(flat.blurRadius, 0.0);
    QCOMPARE(flat.alpha, 0);

    QxTheme::QxShadow previous = flat;
    for (int i = int(L::Raised); i <= int(L::Modal); ++i) {
        const QxTheme::QxShadow level = CupertinoElevation::shadow(CupertinoElevation::Level(i));
        QVERIFY(level.blurRadius > previous.blurRadius);
        QVERIFY(level.alpha > previous.alpha);
        QVERIFY(level.yOffset > 0.0);
        QVERIFY(level.yOffset < level.blurRadius / 2.0);
        previous = level;
    }
}

/*! The durations are answered in the shared ladder's three tiers; the scale has
 *  four steps, so the last two share the longest tier. What the mapping has to
 *  keep is the order: a longer move never lands on a shorter tier. */
void tst_CupertinoTokens::durationsRideOnTheTiers()
{
    using D = CupertinoMotion::Duration;

    const int first = CupertinoMotion::forDuration(D::Quick).duration;
    QVERIFY(first > 0);

    int last = first;
    for (int i = int(D::Normal); i <= int(D::Entrance); ++i) {
        const int duration = CupertinoMotion::forDuration(CupertinoMotion::Duration(i)).duration;
        QVERIFY(duration >= last);
        last = duration;
    }
    // And the longest step is the ladder's longest tier, not a value the family
    // invented on the way through.
    QCOMPARE(last, QxTheme::motionFor(2).duration);

    // An entrance settles harder than a movement within a surface, and both are
    // symmetric curves - the property this family's motion is built on.
    const QxTheme::QxMotion standard = CupertinoMotion::forDuration(D::Normal, CupertinoMotion::Easing::Standard);
    const QxTheme::QxMotion emphasized = CupertinoMotion::forDuration(D::Entrance, CupertinoMotion::Easing::Emphasized);
    QVERIFY(standard.curve != emphasized.curve);
}

/*! What the mapping buys: a duration read through the ladder is silenced by the
 *  reduce-motion switch for free, which is the compliance a control that asked
 *  here gets without checking anything itself. */
void tst_CupertinoTokens::reducedMotionSilencesEveryDuration()
{
    using D = CupertinoMotion::Duration;

    QxTheme::setMotionReduced(true);
    for (int i = int(D::Quick); i <= int(D::Entrance); ++i) {
        const QxTheme::QxMotion motion = CupertinoMotion::forDuration(CupertinoMotion::Duration(i));
        QCOMPARE(motion.duration, 0);
        QCOMPARE(motion.curve, QEasingCurve::Linear);
    }
}

/*! The semantic colours are a pair per role, not one value: green on white and
 *  green on black are two different greens, and a control that picked one for
 *  both would be wrong on one of them. */
void tst_CupertinoTokens::theSemanticColoursComeInPairs()
{
    using R = CupertinoColor::Role;

    QSet<QRgb> lightCuts;
    for (int i = int(R::Accent); i <= int(R::Gray); ++i) {
        const QColor light = CupertinoColor::color(CupertinoColor::Role(i));
        const QColor dark = CupertinoColor::color(CupertinoColor::Role(i), true);
        QVERIFY2(light.isValid(), "a role has no light cut");
        QVERIFY2(dark.isValid(), "a role has no dark cut");
        QVERIFY2(light != dark, "the two cuts are the same colour");
        lightCuts.insert(light.rgb());
    }
    // Five roles, five answers: a table that collapsed two of them would make
    // "careful" and "no" the same thing.
    QCOMPARE(lightCuts.size(), 5);

    // The two roles a control asks by meaning answer with that meaning, not with
    // whichever accent the theme installed.
    const QColor green = CupertinoColor::color(R::Green);
    const QColor red = CupertinoColor::color(R::Red);
    QVERIFY(green.green() > green.red() && green.green() > green.blue());
    QVERIFY(red.red() > red.green() && red.red() > red.blue());
}

/*! A step that is not in the table is a value no caller should have, and the
 *  answer is an empty one - not a crash, and not a guess at the nearest step. */
void tst_CupertinoTokens::anUnknownStepAnswersNothing()
{
    const int beyond = 99;

    QCOMPARE(CupertinoShape::radius(CupertinoShape::Radius(beyond)), 0.0);
    QCOMPARE(CupertinoType::font(CupertinoType::Ramp(beyond)).pointSizeF(), QFont().pointSizeF());
    QCOMPARE(CupertinoElevation::shadow(CupertinoElevation::Level(beyond)).alpha, 0);
    QVERIFY(!CupertinoColor::color(CupertinoColor::Role(beyond)).isValid());

    // A negative step reads as the first one everywhere, which is the ladder's
    // own convention rather than a hole each caller has to guard.
    QCOMPARE(CupertinoType::font(CupertinoType::Ramp(-1)).pointSizeF(), QFont().pointSizeF());
    QCOMPARE(CupertinoMotion::forDuration(CupertinoMotion::Duration(-1)).duration, QxTheme::motionFor(0).duration);
}

TEST_ADD(tst_CupertinoTokens)

#include "tst_cupertinotokens.moc"
