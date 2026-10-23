/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxcupertino/cupertinoactivityindicator.h"
#include "qxcupertino/cupertinotokens.h"

#include "qxtheme/qxthememanager.h"

#include <QtCore/QtMath>
#include <QtGui/QImage>

QX_CUPERTINO_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
/*! Puts the theme's reduce-motion switch back how it was found, whichever way
 *  the case leaves - a probe that turns a global switch off and then fails
 *  before turning it back would change every case after it. */
class ReducedMotionGuard
{
public:
    explicit ReducedMotionGuard(bool on)
        : m_wasOn(QxThemeManager::instance()->reduceMotion())
    {
        QxThemeManager::instance()->setReduceMotion(on);
    }
    ~ReducedMotionGuard()
    {
        QxThemeManager::instance()->setReduceMotion(m_wasOn);
    }
private:
    bool m_wasOn;
};

/*! The ground every frame is rendered onto. The control paints no surface of
 *  its own - an indicator is strokes on whatever is behind it - so a frame has
 *  to say what is behind it before there is anything to measure. */
const QColor kGround(255, 255, 255);

/*! How far a pixel has to be from the ground before it counts as ink. The
 *  faintest of the eight spokes is drawn at about a fifth of the grey, which
 *  is still tens of units from white - this is well under that and well over
 *  the noise of an antialiased edge. */
constexpr int kInkDistance = 24;

QImage frameOf(QWidget *widget)
{
    QImage image(widget->size(), QImage::Format_ARGB32_Premultiplied);
    image.fill(kGround);
    widget->render(&image);
    return image;
}

int distanceFromGround(const QColor &colour, const QColor &ground)
{
    return qAbs(colour.red() - ground.red()) + qAbs(colour.green() - ground.green()) +
           qAbs(colour.blue() - ground.blue());
}

/*!
 * Where the ink is, in a frame.
 *
 * The ground is read out of the frame itself - its top left corner - rather
 * than assumed to be the colour the render started from, because the widget
 * still paints the page's own background over that corner and the page's
 * colour is the application's to change. What is being compared is where the
 * spokes are, and that answer does not change with the page.
 */
QVector<QPoint> inkPoints(const QImage &image)
{
    QVector<QPoint> points;
    const QColor ground = image.pixelColor(0, 0);
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            if (distanceFromGround(image.pixelColor(x, y), ground) >= kInkDistance) {
                points.append(QPoint(x, y));
            }
        }
    }
    return points;
}

/*!
 * How much ink lies along each of the eight directions the spokes are drawn in.
 *
 * The wheel is read by direction rather than by sector because the spokes are
 * what is being counted: a pixel is counted for the spoke whose direction it is
 * within fourteen degrees of, and the radius is bounded so that the centre -
 * where every spoke meets - does not count for every one of them at once.
 */
QVector<int> inkAlongSpokes(const QImage &image)
{
    QVector<int> counts(8, 0);
    const QColor ground = image.pixelColor(0, 0);
    const qreal centreX = image.width() / 2.0;
    const qreal centreY = image.height() / 2.0;
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            if (distanceFromGround(image.pixelColor(x, y), ground) < kInkDistance) {
                continue;
            }
            const qreal dx = x + 0.5 - centreX;
            const qreal dy = y + 0.5 - centreY;
            if (qSqrt(dx * dx + dy * dy) < 3.0) {
                continue;
            }
            const qreal degrees = qRadiansToDegrees(qAtan2(dy, dx));
            for (int spoke = 0; spoke < 8; ++spoke) {
                // Spoke zero points up, which is minus ninety degrees in the
                // image's own coordinates, and the rest follow every 45.
                qreal apart = qAbs(degrees - (-90.0 + 45.0 * spoke));
                while (apart > 180.0) {
                    apart = 360.0 - apart;
                }
                if (apart <= 14.0) {
                    ++counts[spoke];
                    break;
                }
            }
        }
    }
    return counts;
}

int totalInk(const QVector<int> &counts)
{
    int total = 0;
    for (int each : counts) {
        total += each;
    }
    return total;
}
}   // namespace

class tst_CupertinoActivityIndicator : public QObject
{
    Q_OBJECT
private slots:
    void theEightSpokesSitWhereTheyShould();
    void aStoppedWheelIsStillDrawn();
    void theWheelTurns();
    void reduceMotionStopsTheTurnAndKeepsTheSpokes();
};

/*! Eight spokes, one every forty-five degrees, on a wheel that is not turning -
 *  so this is the shape of the mark and not its motion. */
void tst_CupertinoActivityIndicator::theEightSpokesSitWhereTheyShould()
{
    CupertinoActivityIndicator indicator;
    indicator.resize(indicator.sizeHint());
    indicator.setAnimating(false);

    const QVector<int> counts = inkAlongSpokes(frameOf(&indicator));
    QCOMPARE(counts.size(), 8);
    for (int spoke = 0; spoke < counts.size(); ++spoke) {
        QVERIFY2(counts.at(spoke) >= 3,
                 qPrintable(QStringLiteral("spoke %1 carries %2 pixels").arg(spoke).arg(counts.at(spoke))));
    }
    // And nothing else is drawn: the eight spokes are the whole control.
    QVERIFY(totalInk(counts) >= 8 * 3);
}

/*! A stopped wheel is a still mark, not an empty widget: stopping is what a
 *  page that has finished waiting does with the timer, not with the drawing. */
void tst_CupertinoActivityIndicator::aStoppedWheelIsStillDrawn()
{
    CupertinoActivityIndicator indicator;
    indicator.resize(indicator.sizeHint());
    QVERIFY(indicator.isAnimating());

    indicator.setAnimating(false);
    QVERIFY(!indicator.isAnimating());
    QVERIFY(totalInk(inkAlongSpokes(frameOf(&indicator))) >= 8 * 3);
}

/*! Turning: two frames taken a step apart are two different pictures. The step
 *  is the family's quickest duration, so a wait of a little over one of them is
 *  enough for the wheel to have moved on. */
void tst_CupertinoActivityIndicator::theWheelTurns()
{
    if (QxThemeManager::instance()->reduceMotion()) {
        QSKIP("the theme in force has reduce motion on, so nothing is meant to turn");
    }

    CupertinoActivityIndicator indicator;
    indicator.resize(indicator.sizeHint());
    indicator.show();

    const QImage first = frameOf(&indicator);
    QTest::qWait(160);
    const QImage second = frameOf(&indicator);
    QVERIFY2(inkPoints(second) != inkPoints(first), "the wheel did not move");
}

/*! Reduce motion is a still frame and not an absent one: the spokes stay where
 *  they are, and two frames a while apart are the same picture. */
void tst_CupertinoActivityIndicator::reduceMotionStopsTheTurnAndKeepsTheSpokes()
{
    const ReducedMotionGuard guard(true);

    CupertinoActivityIndicator indicator;
    indicator.resize(indicator.sizeHint());
    indicator.show();

    const QImage first = frameOf(&indicator);
    QTest::qWait(220);
    const QImage second = frameOf(&indicator);
    QCOMPARE(inkPoints(second), inkPoints(first));
    QVERIFY(totalInk(inkAlongSpokes(first)) >= 8 * 3);
}

TEST_ADD(tst_CupertinoActivityIndicator)

#include "tst_cupertinoactivityindicator.moc"
