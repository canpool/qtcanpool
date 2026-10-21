/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxcontrols/qxshadowpainter.h"

#include <QImage>
#include <QPainter>

QX_CONTROLS_USE_NAMESPACE

namespace
{
const QxTheme::QxShadow kShadow{4.0, 1.0, 45};
}

class tst_QxShadowPainter : public QObject
{
    Q_OBJECT
private slots:
    void nothingDrawsWithoutAShadow();
    void tileRendersTheFalloff();
    void paintDrawsAroundTheRect();
};

/*! The flat tier spelled as "do not draw one": no alpha, no blur or no colour
 *  is no shadow, not an error. */
void tst_QxShadowPainter::nothingDrawsWithoutAShadow()
{
    QVERIFY(QxShadowPainter::tile(4.0, QColor(), kShadow).isNull());
    QVERIFY(QxShadowPainter::tile(4.0, Qt::black, QxTheme::QxShadow{4.0, 1.0, 0}).isNull());
    QVERIFY(QxShadowPainter::tile(4.0, Qt::black, QxTheme::QxShadow{0.0, 1.0, 45}).isNull());

    QImage image(60, 60, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::white);
    QPainter painter(&image);
    QxShadowPainter::paint(&painter, QRectF(10, 10, 40, 40), 4.0, Qt::black,
                           QxTheme::QxShadow{4.0, 1.0, 0});
    painter.end();
    QCOMPARE(image.pixelColor(30, 8), QColor(Qt::white));
}

/*! The tile falls off from the tier's alpha at the edge towards nothing at
 *  its outer margin, and the centre is the darkest point. */
void tst_QxShadowPainter::tileRendersTheFalloff()
{
    const QPixmap tile = QxShadowPainter::tile(4.0, Qt::black, kShadow);
    QVERIFY(!tile.isNull());
    const int margin = 5;   // ceil(4) + 1
    QCOMPARE(tile.width(), 2 * 4 + 16 + 2 * margin);

    const QImage image = tile.toImage();
    const int mid = tile.width() / 2;
    // On the control's edge the shadow reads at the tier's alpha.
    QCOMPARE(image.pixelColor(mid, margin).alpha(), kShadow.alpha);
    // Half a blur out it has fallen off measurably, and the outer margin is
    // reached by nothing.
    const int justOutside = image.pixelColor(mid, margin - 2).alpha();
    const int farOut = image.pixelColor(mid, 0).alpha();
    QVERIFY(justOutside > 0);
    QVERIFY(justOutside < kShadow.alpha);
    QCOMPARE(farOut, 0);
}

/*! The drawn shadow surrounds the rectangle, follows the tier's y offset, and
 *  stops at its own reach. */
void tst_QxShadowPainter::paintDrawsAroundTheRect()
{
    QImage image(60, 60, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::white);
    QPainter painter(&image);
    QxShadowPainter::paint(&painter, QRectF(10, 10, 40, 40), 4.0, Qt::black, kShadow);
    painter.end();

    // Under the control and just outside it the shadow is there; beyond the
    // blur's reach it is not. The y offset of one drops the reach by one.
    QVERIFY(image.pixelColor(30, 30).alpha() > 0);
    QVERIFY(image.pixelColor(30, 8).alpha() > 0);
    QCOMPARE(image.pixelColor(30, 3), QColor(Qt::white));
}

TEST_ADD(tst_QxShadowPainter)

#include "tst_qxshadowpainter.moc"
