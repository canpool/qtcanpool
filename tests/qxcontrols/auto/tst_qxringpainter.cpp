/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxcontrols/qxringpainter.h"

#include <QImage>
#include <QPainter>

QX_CONTROLS_USE_NAMESPACE

namespace
{
QImage whiteImage(int size)
{
    QImage image(size, size, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::white);
    return image;
}

int grayAt(const QImage &image, int x, int y)
{
    return qGray(image.pixelColor(x, y).rgb());
}
}

class tst_QxRingPainter : public QObject
{
    Q_OBJECT
private slots:
    void nothingDrawsWithoutARing();
    void bandSitsWhereItSays();
    void negativeInsetHangsOver();
    void cornersFollowTheControl();
};

/*! No colour or no width is a control that shows no focus, not an error. */
void tst_QxRingPainter::nothingDrawsWithoutARing()
{
    QImage image = whiteImage(40);
    QPainter painter(&image);
    const QRectF rect(0, 0, 40, 40);

    QxRing invisible;
    invisible.color = QColor();
    invisible.width = 2.0;
    invisible.inset = 0.0;
    QxRingPainter::paint(&painter, rect, 0.0, invisible);
    QxRingPainter::paint(&painter, rect, 0.0, QxRing{Qt::black, 0.0, 0.0});
    painter.end();

    QCOMPARE(image.pixelColor(20, 1), QColor(Qt::white));
}

/*! The band occupies the strip it names: inset to inset + width inside the
 *  rectangle, nothing more. */
void tst_QxRingPainter::bandSitsWhereItSays()
{
    QImage image = whiteImage(40);
    QPainter painter(&image);
    QxRingPainter::paint(&painter, QRectF(0, 0, 40, 40), 0.0, QxRing{Qt::black, 2.0, 0.0});
    painter.end();

    // Inside the band on all four sides, inside the hole in the middle.
    QVERIFY(grayAt(image, 20, 1) < 50);
    QVERIFY(grayAt(image, 20, 38) < 50);
    QVERIFY(grayAt(image, 1, 20) < 50);
    QVERIFY(grayAt(image, 38, 20) < 50);
    QVERIFY(grayAt(image, 20, 10) > 200);
    QVERIFY(grayAt(image, 20, 20) > 200);
}

/*! A negative inset widens the rectangle: the ring hangs over the edge, which
 *  is how the languages that reach past the control draw it. */
void tst_QxRingPainter::negativeInsetHangsOver()
{
    QImage image = whiteImage(50);
    QPainter painter(&image);
    // The control occupies 10..40, so a band at [-2, 0] sits outside it.
    QxRingPainter::paint(&painter, QRectF(10, 10, 30, 30), 0.0, QxRing{Qt::black, 2.0, -2.0});
    painter.end();

    QVERIFY(grayAt(image, 25, 9) < 50);
    QVERIFY(grayAt(image, 25, 12) > 200);
    QVERIFY(grayAt(image, 25, 1) > 200);
}

/*! The ring's corners turn with the control's, so a ring that hugs a rounded
 *  corner stays off the square one. */
void tst_QxRingPainter::cornersFollowTheControl()
{
    QImage square = whiteImage(40);
    QPainter p(&square);
    QxRingPainter::paint(&p, QRectF(0, 0, 40, 40), 0.0, QxRing{Qt::black, 2.0, 0.0});
    p.end();
    QVERIFY(grayAt(square, 1, 1) < 50);

    QImage rounded = whiteImage(40);
    QPainter p2(&rounded);
    QxRingPainter::paint(&p2, QRectF(0, 0, 40, 40), 15.0, QxRing{Qt::black, 2.0, 0.0});
    p2.end();
    QVERIFY(grayAt(rounded, 1, 1) > 200);
    // The middle of an edge does not care about the radius.
    QVERIFY(grayAt(rounded, 20, 1) < 50);
}

TEST_ADD(tst_QxRingPainter)

#include "tst_qxringpainter.moc"
