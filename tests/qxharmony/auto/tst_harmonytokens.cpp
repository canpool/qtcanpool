/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxharmony/harmonytokens.h"

#include "qxtheme/qxthememotion.h"

#include <QColor>
#include <QFont>
#include <QSet>

QX_HARMONY_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

class tst_HarmonyTokens : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void cornersGrowWithTheScale();
    void theRampCarriesWeightTwice();
    void elevationStandsUp();
    void durationsRideOnTheTiers();
    void theSpringyCurveIsNotASlide();
    void reducedMotionSilencesEveryDuration();
    void theSemanticColoursComeInPairs();
    void anUnknownStepAnswersNothing();
};

void tst_HarmonyTokens::cleanup()
{
    // The motion switch and the scale are process-wide, and the tier ladder is
    // what every other suite reads through.
    QxTheme::setMotionReduced(false);
    QxTheme::setMotionScale(1.0);
}

/*! A corner scale is read as an order: each step turns more than the one
 *  before, and the round one turns more than any rectangle can. */
void tst_HarmonyTokens::cornersGrowWithTheScale()
{
    using R = HarmonyShape::Radius;

    const qreal none = HarmonyShape::radius(R::None);
    const qreal small = HarmonyShape::radius(R::Small);
    const qreal medium = HarmonyShape::radius(R::Medium);
    const qreal large = HarmonyShape::radius(R::Large);
    const qreal xLarge = HarmonyShape::radius(R::XLarge);

    QCOMPARE(none, 0.0);
    QVERIFY(small > none);
    QVERIFY(medium > small);
    QVERIFY(large > medium);
    QVERIFY(xLarge > large);

    // The top corner is large enough to be the shape rather than a softened
    // edge, which is what "a sheet laid on the page" means as a value.
    QVERIFY(xLarge >= 24.0);

    QVERIFY(HarmonyShape::radius(R::Circular) > 1000.0);
}

/*! The ramp carries weight twice: every step sits above normal weight, and the
 *  strong cut goes one step further without changing the size. */
void tst_HarmonyTokens::theRampCarriesWeightTwice()
{
    using T = HarmonyType::Ramp;

    const QFont caption = HarmonyType::font(T::Caption);
    const QFont body = HarmonyType::font(T::Body);
    const QFont subtitle = HarmonyType::font(T::Subtitle);
    const QFont title = HarmonyType::font(T::Title);
    const QFont display = HarmonyType::font(T::Display);

    QVERIFY(caption.pointSizeF() < body.pointSizeF());
    QVERIFY(body.pointSizeF() < subtitle.pointSizeF());
    QVERIFY(subtitle.pointSizeF() < title.pointSizeF());
    QVERIFY(title.pointSizeF() < display.pointSizeF());

    // Text leans forward here: the plain cut is already heavier than the
    // book weight a family that sits back would use.
    QVERIFY(body.weight() > QFont::Normal);
    QCOMPARE(caption.weight(), body.weight());

    const QFont strong = HarmonyType::font(T::Body, true);
    QCOMPARE(strong.pointSizeF(), body.pointSizeF());
    QVERIFY(strong.weight() > body.weight());
}

/*! A surface stands up rather than being lit: the spread and the darkness grow
 *  with the level, and the offset never shrinks below half the spread - which is
 *  the proportion "an edge under the sheet" written as a value. */
void tst_HarmonyTokens::elevationStandsUp()
{
    using L = HarmonyElevation::Level;

    const QxTheme::QxShadow flat = HarmonyElevation::shadow(L::None);
    QCOMPARE(flat.blurRadius, 0.0);
    QCOMPARE(flat.alpha, 0);

    QxTheme::QxShadow previous = flat;
    for (int i = int(L::Raised); i <= int(L::Modal); ++i) {
        const QxTheme::QxShadow level = HarmonyElevation::shadow(HarmonyElevation::Level(i));
        QVERIFY(level.blurRadius > previous.blurRadius);
        QVERIFY(level.alpha > previous.alpha);
        QVERIFY(level.yOffset > 0.0);
        QVERIFY(level.yOffset >= level.blurRadius / 2.0);
        previous = level;
    }
}

/*! Three steps and three tiers, so the two scales agree: the longest move is
 *  the ladder's longest tier, and no longer step lands on a shorter tier. */
void tst_HarmonyTokens::durationsRideOnTheTiers()
{
    using D = HarmonyMotion::Duration;

    const int first = HarmonyMotion::forDuration(D::Quick).duration;
    QVERIFY(first > 0);

    int last = first;
    for (int i = int(D::Normal); i <= int(D::Entrance); ++i) {
        const int duration = HarmonyMotion::forDuration(HarmonyMotion::Duration(i)).duration;
        QVERIFY(duration >= last);
        last = duration;
    }
    QCOMPARE(last, QxTheme::motionFor(2).duration);
}

/*! The curve is where this scale says something the others do not: the springy
 *  one overshoots and settles back, and it is not the standard curve. */
void tst_HarmonyTokens::theSpringyCurveIsNotASlide()
{
    using D = HarmonyMotion::Duration;
    using E = HarmonyMotion::Easing;

    const QxTheme::QxMotion standard = HarmonyMotion::forDuration(D::Normal, E::Standard);
    const QxTheme::QxMotion springy = HarmonyMotion::forDuration(D::Normal, E::Springy);

    QCOMPARE(standard.curve, QEasingCurve::OutCubic);
    QCOMPARE(springy.curve, QEasingCurve::OutBack);
    // Same duration either way: the curve is what changed, not the length.
    QCOMPARE(springy.duration, standard.duration);
}

/*! What the mapping buys: a duration read through the ladder is silenced by the
 *  reduce-motion switch for free. */
void tst_HarmonyTokens::reducedMotionSilencesEveryDuration()
{
    using D = HarmonyMotion::Duration;

    QxTheme::setMotionReduced(true);
    for (int i = int(D::Quick); i <= int(D::Entrance); ++i) {
        const QxTheme::QxMotion motion = HarmonyMotion::forDuration(HarmonyMotion::Duration(i));
        QCOMPARE(motion.duration, 0);
        QCOMPARE(motion.curve, QEasingCurve::Linear);
    }
}

/*! The semantic colours are a pair per role, not one value, and five roles
 *  answer five different colours. */
void tst_HarmonyTokens::theSemanticColoursComeInPairs()
{
    using R = HarmonyColor::Role;

    QSet<QRgb> lightCuts;
    for (int i = int(R::Accent); i <= int(R::Gray); ++i) {
        const QColor light = HarmonyColor::color(HarmonyColor::Role(i));
        const QColor dark = HarmonyColor::color(HarmonyColor::Role(i), true);
        QVERIFY2(light.isValid(), "a role has no light cut");
        QVERIFY2(dark.isValid(), "a role has no dark cut");
        QVERIFY2(light != dark, "the two cuts are the same colour");
        lightCuts.insert(light.rgb());
    }
    QCOMPARE(lightCuts.size(), 5);

    // The accent is the brand's blue, and the roles a control asks by meaning
    // answer with that meaning.
    const QColor accent = HarmonyColor::color(R::Accent);
    QVERIFY(accent.blue() > accent.red() && accent.blue() > accent.green());
    const QColor green = HarmonyColor::color(R::Green);
    const QColor red = HarmonyColor::color(R::Red);
    QVERIFY(green.green() > green.red() && green.green() > green.blue());
    QVERIFY(red.red() > red.green() && red.red() > red.blue());
}

/*! A step that is not in the table is a value no caller should have, and the
 *  answer is an empty one - not a crash, and not a guess at the nearest step. */
void tst_HarmonyTokens::anUnknownStepAnswersNothing()
{
    const int beyond = 99;

    QCOMPARE(HarmonyShape::radius(HarmonyShape::Radius(beyond)), 0.0);
    QCOMPARE(HarmonyType::font(HarmonyType::Ramp(beyond)).pointSizeF(), QFont().pointSizeF());
    QCOMPARE(HarmonyElevation::shadow(HarmonyElevation::Level(beyond)).alpha, 0);
    QVERIFY(!HarmonyColor::color(HarmonyColor::Role(beyond)).isValid());

    QCOMPARE(HarmonyType::font(HarmonyType::Ramp(-1)).pointSizeF(), QFont().pointSizeF());
    QCOMPARE(HarmonyMotion::forDuration(HarmonyMotion::Duration(-1)).duration, QxTheme::motionFor(0).duration);
}

TEST_ADD(tst_HarmonyTokens)

#include "tst_harmonytokens.moc"
