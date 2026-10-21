/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialtokens.h"

#include <QFont>

QX_MATERIAL_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

class tst_MaterialTokens : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void cornersGrowWithTheScale();
    void theScaleIsRolesAndWeights();
    void elevationSpreadsAndDarkens();
    void durationsRideOnTheTiers();
    void theCurveIsAskedForWithTheDuration();
    void reducedMotionSilencesEveryDuration();
};

void tst_MaterialTokens::cleanup()
{
    // The motion switch and the scale are process-wide, and the tier ladder is
    // what every other suite reads through.
    QxTheme::setMotionReduced(false);
    QxTheme::setMotionScale(1.0);
}

/*! A corner scale is read as an order: each step turns more than the one
 *  before, and the round one turns more than any rectangle can.
 *
 *  Material arrives with seven steps and this scale carries five, because two
 *  of the seven differ by four pixels; what has to survive the collapse is the
 *  order, not the count. */
void tst_MaterialTokens::cornersGrowWithTheScale()
{
    using R = MaterialShape::Radius;

    const qreal none = MaterialShape::radius(R::None);
    const qreal small = MaterialShape::radius(R::Small);
    const qreal medium = MaterialShape::radius(R::Medium);
    const qreal large = MaterialShape::radius(R::Large);
    const qreal xLarge = MaterialShape::radius(R::XLarge);

    QCOMPARE(none, 0.0);
    QVERIFY(small > none);
    QVERIFY(medium > small);
    QVERIFY(large > medium);
    QVERIFY(xLarge > large);

    // Fully round is a radius no rectangle reaches, which is the only way to
    // say it in a value a painter takes.
    QVERIFY(MaterialShape::radius(R::Circular) > 1000.0);
}

/*! A role is not a size: the heavier cut of a role is that role's own size
 *  with more weight, so a control does not have to reach for a bigger role to
 *  emphasise a line. */
void tst_MaterialTokens::theScaleIsRolesAndWeights()
{
    using S = MaterialType::Scale;

    const QFont label = MaterialType::font(S::Label);
    const QFont body = MaterialType::font(S::Body);
    const QFont title = MaterialType::font(S::Title);
    const QFont headline = MaterialType::font(S::Headline);
    const QFont display = MaterialType::font(S::Display);

    QVERIFY(label.pointSizeF() < body.pointSizeF());
    QVERIFY(body.pointSizeF() < title.pointSizeF());
    QVERIFY(title.pointSizeF() < headline.pointSizeF());
    QVERIFY(headline.pointSizeF() < display.pointSizeF());

    const QFont strong = MaterialType::font(S::Body, true);
    QCOMPARE(strong.pointSizeF(), body.pointSizeF());
    QVERIFY(strong.weight() > body.weight());
}

/*! Elevation is one answer to "how far and how dark": the spread grows with
 *  the level, the darkness grows with it, and a surface that lifts further
 *  also sits further below the shadow it casts. */
void tst_MaterialTokens::elevationSpreadsAndDarkens()
{
    using L = MaterialElevation::Level;

    const QxTheme::QxShadow flat = MaterialElevation::shadow(L::None);
    QCOMPARE(flat.blurRadius, 0.0);
    QCOMPARE(flat.alpha, 0);

    QxTheme::QxShadow previous = flat;
    for (int i = int(L::Level1); i <= int(L::Level5); ++i) {
        const QxTheme::QxShadow level = MaterialElevation::shadow(MaterialElevation::Level(i));
        QVERIFY(level.blurRadius > previous.blurRadius);
        QVERIFY(level.alpha > previous.alpha);
        QVERIFY(level.yOffset > previous.yOffset);
        previous = level;
    }
}

/*! Material spells its durations in milliseconds; the shared ladder answers in
 *  three tiers. What the mapping has to keep is the order: a longer move never
 *  lands on a shorter tier. */
void tst_MaterialTokens::durationsRideOnTheTiers()
{
    using D = MaterialMotion::Duration;

    int last = MaterialMotion::forDuration(D::Short).duration;
    QVERIFY(last > 0);
    for (int i = int(D::Medium); i <= int(D::Long); ++i) {
        const int duration = MaterialMotion::forDuration(MaterialMotion::Duration(i)).duration;
        QVERIFY(duration > last);
        last = duration;
    }
    // And the longest step is the ladder's longest tier, not a value Material
    // invented on the way through.
    QCOMPARE(last, QxTheme::motionFor(2).duration);
}

/*! A movement is a duration and a curve, and Material does not treat the
 *  curve as a detail: the same duration rides differently on the emphasized
 *  one, which is why the two are asked for together. */
void tst_MaterialTokens::theCurveIsAskedForWithTheDuration()
{
    using D = MaterialMotion::Duration;
    using E = MaterialMotion::Easing;

    const QxTheme::QxMotion standard = MaterialMotion::forDuration(D::Medium, E::Standard);
    const QxTheme::QxMotion emphasized = MaterialMotion::forDuration(D::Medium, E::Emphasized);

    QCOMPARE(standard.duration, emphasized.duration);
    QVERIFY(standard.curve != emphasized.curve);

    // The default is the standard one: a control that asks only for a duration
    // gets the curve most movements use, not the one that settles.
    QCOMPARE(MaterialMotion::forDuration(D::Medium).curve, standard.curve);
}

/*! What the mapping buys: a duration read through the tier ladder is silenced
 *  by the reduce-motion switch for free, which is the compliance a control
 *  that asked here gets without checking anything itself. */
void tst_MaterialTokens::reducedMotionSilencesEveryDuration()
{
    using D = MaterialMotion::Duration;

    QxTheme::setMotionReduced(true);
    for (int i = int(D::Short); i <= int(D::Long); ++i) {
        const QxTheme::QxMotion motion = MaterialMotion::forDuration(MaterialMotion::Duration(i));
        QCOMPARE(motion.duration, 0);
        QCOMPARE(motion.curve, QEasingCurve::Linear);
    }
}

TEST_ADD(tst_MaterialTokens)

#include "tst_materialtokens.moc"
