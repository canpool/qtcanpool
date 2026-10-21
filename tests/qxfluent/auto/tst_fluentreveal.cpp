/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentcard.h"
#include "qxfluent/fluentreveal.h"

#include <QApplication>
#include <QImage>
#include <QMouseEvent>
#include <QPainter>

QX_FLUENT_USE_NAMESPACE

namespace
{
const QColor kInk(0, 0, 0);
const QColor kPage(255, 255, 255);

void sendHover(QWidget *widget, const QPoint &pos)
{
    const QPoint global = widget->mapToGlobal(pos);
    QMouseEvent event(QEvent::MouseMove, pos, global, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(widget, &event);
}

void sendLeave(QWidget *widget)
{
    QEvent event(QEvent::Leave);
    QApplication::sendEvent(widget, &event);
}

/*! How much ink is on a pixel: a wash is the only thing on this image, so the
 *  further it is from the page the more wash there is. */
int darkness(const QColor &color)
{
    return 255 - qMin(color.red(), qMin(color.green(), color.blue()));
}
}   // namespace

class tst_FluentReveal : public QObject
{
    Q_OBJECT
private slots:
    void itIsLitFromWhereThePointerIs();
    void itGoesWhenThePointerDoes();
    void aCardIsLitFromWhereThePointerIs();
};

/*! A hover that switches on says the pointer is somewhere on the surface;
 *  Fluent's says where. The wash is strongest under the pointer, weaker away
 *  from it, and gone beyond the surface's own reach. */
void tst_FluentReveal::itIsLitFromWhereThePointerIs()
{
    QWidget surface;
    surface.resize(100, 100);
    FluentReveal reveal(&surface);
    QVERIFY(!reveal.isActive());

    sendHover(&surface, QPoint(10, 10));
    QVERIFY(reveal.isActive());
    QCOMPARE(reveal.pointer(), QPointF(10, 10));

    QImage image(100, 100, QImage::Format_ARGB32);
    image.fill(kPage);
    QPainter painter(&image);
    reveal.paint(&painter, QRectF(0, 0, 100, 100), 4.0, kInk);
    painter.end();

    const int under = darkness(image.pixelColor(10, 10));
    const int away = darkness(image.pixelColor(60, 60));
    const int beyond = darkness(image.pixelColor(95, 95));
    QVERIFY(under > away);
    QVERIFY(away > 0);
    // Past the surface's own reach there is nothing left of it - a wash that
    // reached further would be a flat tint with extra steps.
    QCOMPARE(beyond, 0);
}

/*! A wash that outlived the pointer would be a surface lit from somewhere the
 *  user is not. */
void tst_FluentReveal::itGoesWhenThePointerDoes()
{
    QWidget surface;
    surface.resize(100, 100);
    FluentReveal reveal(&surface);

    sendHover(&surface, QPoint(10, 10));
    QVERIFY(reveal.isActive());
    sendLeave(&surface);
    QVERIFY(!reveal.isActive());

    QImage image(100, 100, QImage::Format_ARGB32);
    image.fill(kPage);
    QPainter painter(&image);
    reveal.paint(&painter, QRectF(0, 0, 100, 100), 4.0, kInk);
    painter.end();
    QCOMPARE(image.pixelColor(10, 10), kPage);
}

/*! On a card the reveal is the hover: the card draws no flat wash of its own,
 *  and moving the pointer moves the lit place. */
void tst_FluentReveal::aCardIsLitFromWhereThePointerIs()
{
    FluentCard card;
    card.resize(200, 100);
    FluentReveal *reveal = card.findChild<FluentReveal *>();
    QVERIFY(reveal);

    const QImage resting = card.grab().toImage();
    sendHover(&card, QPoint(20, 50));
    QVERIFY(reveal->isActive());
    const QImage litLeft = card.grab().toImage();
    QVERIFY(litLeft != resting);

    sendHover(&card, QPoint(180, 50));
    const QImage litRight = card.grab().toImage();
    // The same card, the same pointer distance from an edge: what moved is the
    // light, which is the whole claim a reveal makes.
    QVERIFY(litRight != litLeft);
}

TEST_ADD(tst_FluentReveal)

#include "tst_fluentreveal.moc"
