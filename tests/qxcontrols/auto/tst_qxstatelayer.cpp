/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxcontrols/qxstatelayer.h"

#include <QImage>
#include <QPainter>

QX_CONTROLS_USE_NAMESPACE

class tst_QxStateLayer : public QObject
{
    Q_OBJECT
private slots:
    void opacityClamps();
    void layeredComposites();
    void paintMatchesLayered();
    void restDrawsNothing();
};

/*! An opacity that overreaches clamps instead of painting a state the design
 *  never drew; the resting zero stays zero. */
void tst_QxStateLayer::opacityClamps()
{
    QCOMPARE(QxStateLayer::clampedOpacity(-0.5), 0.0);
    QCOMPARE(QxStateLayer::clampedOpacity(0.0), 0.0);
    QCOMPARE(QxStateLayer::clampedOpacity(0.08), 0.08);
    QCOMPARE(QxStateLayer::clampedOpacity(1.0), 1.0);
    QCOMPARE(QxStateLayer::clampedOpacity(4.0), 1.0);
}

/*! The value compositing gives: the layer fraction over the base, per channel,
 *  with the default opacities in their order - a hover is the faintest, a
 *  drag the strongest. */
void tst_QxStateLayer::layeredComposites()
{
    const QColor base(0x20, 0x40, 0x80);
    const QColor layer(0xff, 0xff, 0xff);

    QCOMPARE(QxStateLayer::layered(base, layer, 0.0), base);
    QCOMPARE(QxStateLayer::layered(base, layer, 1.0), layer);
    // Half way between the two, with one rounding step of slack: the colour
    // stores its channels as floats, and 143.5 may land on either neighbour.
    const QColor halfway = QxStateLayer::layered(base, layer, 0.5);
    QVERIFY(qAbs(halfway.red() - 0x90) <= 1);
    QVERIFY(qAbs(halfway.green() - 0xa0) <= 1);
    QVERIFY(qAbs(halfway.blue() - 0xc0) <= 1);

    // An invalid layer is no layer at all.
    QCOMPARE(QxStateLayer::layered(base, QColor(), 0.5), base);

    // And the opacities keep their order, faintest to strongest.
    QVERIFY(QxStateLayer::hoverOpacity < QxStateLayer::focusOpacity);
    QVERIFY(QxStateLayer::focusOpacity <= QxStateLayer::pressedOpacity);
    QVERIFY(QxStateLayer::pressedOpacity < QxStateLayer::draggedOpacity);
}

/*! What paint() puts in a pixel is what layered() predicted - one answer,
 *  two ways to read it. */
void tst_QxStateLayer::paintMatchesLayered()
{
    QImage image(8, 8, QImage::Format_ARGB32_Premultiplied);
    const QColor base(0x20, 0x40, 0x80);
    image.fill(base);

    QPainter painter(&image);
    QxStateLayer::paint(&painter, QRectF(0, 0, 8, 8), QColor(0xff, 0xff, 0xff), 0.5);
    painter.end();

    const QColor painted = image.pixelColor(4, 4);
    const QColor expected = QxStateLayer::layered(base, QColor(0xff, 0xff, 0xff), 0.5);
    // One rounding step is the difference between the two arithmetic paths.
    QVERIFY(qAbs(painted.red() - expected.red()) <= 1);
    QVERIFY(qAbs(painted.green() - expected.green()) <= 1);
    QVERIFY(qAbs(painted.blue() - expected.blue()) <= 1);
    QCOMPARE(painted.alpha(), expected.alpha());
}

/*! The resting state draws nothing: no layer, or a wash of nothing, leaves
 *  the pixels as they were. */
void tst_QxStateLayer::restDrawsNothing()
{
    QImage image(8, 8, QImage::Format_ARGB32_Premultiplied);
    const QColor base(0x20, 0x40, 0x80);
    image.fill(base);

    QPainter painter(&image);
    QxStateLayer::paint(&painter, QRectF(0, 0, 8, 8), QColor(), 0.5);
    QxStateLayer::paint(&painter, QRectF(0, 0, 8, 8), QColor(0xff, 0xff, 0xff), 0.0);
    painter.end();

    QCOMPARE(image.pixelColor(4, 4), base);
}

TEST_ADD(tst_QxStateLayer)

#include "tst_qxstatelayer.moc"
