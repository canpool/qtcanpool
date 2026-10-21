/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluenttokens.h"

#include <QFont>

QX_FLUENT_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

class tst_FluentTokens : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void cornersGrowWithTheScale();
    void theRampIsSizesAndWeights();
    void elevationSpreadsAndDarkens();
    void durationsRideOnTheTiers();
    void reducedMotionSilencesEveryDuration();
};

void tst_FluentTokens::cleanup()
{
    // The motion switch and the scale are process-wide, and the tier ladder is
    // what every other suite reads through.
    QxTheme::setMotionReduced(false);
    QxTheme::setMotionScale(1.0);
}

/*! A corner scale is read as an order: each step turns more than the one
 *  before, and the round one turns more than any rectangle can. */
void tst_FluentTokens::cornersGrowWithTheScale()
{
    using R = FluentShape::Radius;

    const qreal none = FluentShape::radius(R::None);
    const qreal small = FluentShape::radius(R::Small);
    const qreal medium = FluentShape::radius(R::Medium);
    const qreal large = FluentShape::radius(R::Large);
    const qreal xLarge = FluentShape::radius(R::XLarge);

    QCOMPARE(none, 0.0);
    QVERIFY(small > none);
    QVERIFY(medium > small);
    QVERIFY(large > medium);
    QVERIFY(xLarge > large);

    // Fully round is a radius no rectangle reaches, which is the only way to
    // say it in a value a painter takes.
    QVERIFY(FluentShape::radius(R::Circular) > 1000.0);
}

/*! The ramp is not a list of sizes: the heavier cut of a step is that step's
 *  own size with more weight, so a control does not have to pick a bigger
 *  step to emphasise a line. */
void tst_FluentTokens::theRampIsSizesAndWeights()
{
    using T = FluentType::Ramp;

    const QFont caption = FluentType::font(T::Caption);
    const QFont body = FluentType::font(T::Body);
    const QFont subtitle = FluentType::font(T::Subtitle);
    const QFont title = FluentType::font(T::Title);
    const QFont display = FluentType::font(T::Display);

    QVERIFY(caption.pointSizeF() < body.pointSizeF());
    QVERIFY(body.pointSizeF() < subtitle.pointSizeF());
    QVERIFY(subtitle.pointSizeF() < title.pointSizeF());
    QVERIFY(title.pointSizeF() < display.pointSizeF());

    const QFont strong = FluentType::font(T::Body, true);
    QCOMPARE(strong.pointSizeF(), body.pointSizeF());
    QVERIFY(strong.weight() > body.weight());
}

/*! Elevation is one answer to "how far and how dark": the spread grows with
 *  the level, the darkness grows with it, and the surface sits half its spread
 *  below the shadow it casts. */
void tst_FluentTokens::elevationSpreadsAndDarkens()
{
    using L = FluentElevation::Level;

    const QxTheme::QxShadow flat = FluentElevation::shadow(L::None);
    QCOMPARE(flat.blurRadius, 0.0);
    QCOMPARE(flat.alpha, 0);

    QxTheme::QxShadow previous = flat;
    for (int i = int(L::Depth2); i <= int(L::Depth64); ++i) {
        const QxTheme::QxShadow level = FluentElevation::shadow(FluentElevation::Level(i));
        QVERIFY(level.blurRadius > previous.blurRadius);
        QVERIFY(level.alpha > previous.alpha);
        QCOMPARE(level.yOffset, level.blurRadius / 2.0);
        previous = level;
    }
}

/*! Fluent spells its durations in milliseconds; the shared ladder answers in
 *  three tiers. The mapping is the scale's whole content, and what it has to
 *  keep is the order: a longer move never lands on a shorter tier. */
void tst_FluentTokens::durationsRideOnTheTiers()
{
    using D = FluentMotion::Duration;

    const int previous = FluentMotion::forDuration(D::UltraFast).duration;
    QVERIFY(previous > 0);

    int last = previous;
    for (int i = int(D::Faster); i <= int(D::Slower); ++i) {
        const int duration = FluentMotion::forDuration(FluentMotion::Duration(i)).duration;
        QVERIFY(duration >= last);
        last = duration;
    }
    // And the longest step is the ladder's longest tier, not a value Fluent
    // invented on the way through.
    QCOMPARE(last, QxTheme::motionFor(2).duration);
}

/*! What the mapping buys: a duration read through the tier ladder is silenced
 *  by the reduce-motion switch for free, which is the compliance a control
 *  that asked here gets without checking anything itself. */
void tst_FluentTokens::reducedMotionSilencesEveryDuration()
{
    using D = FluentMotion::Duration;

    QxTheme::setMotionReduced(true);
    for (int i = int(D::UltraFast); i <= int(D::Slower); ++i) {
        const QxTheme::QxMotion motion = FluentMotion::forDuration(FluentMotion::Duration(i));
        QCOMPARE(motion.duration, 0);
        QCOMPARE(motion.curve, QEasingCurve::Linear);
    }
}

TEST_ADD(tst_FluentTokens)

#include "tst_fluenttokens.moc"
