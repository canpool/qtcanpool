/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialripple.h"

#include "qxtheme/qxthememotion.h"

#include <QImage>
#include <QPainter>

QX_MATERIAL_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
// The ripple is drawn on the surface it belongs to, so a test that wants to see
// it paints it onto a page of its own.
const QColor kPage(255, 255, 255);
const QColor kInk(0, 0, 0);
}   // namespace

class tst_MaterialRipple : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void aPressStartsOneWhereThePointerLanded();
    void itSpreadsAndGoesAway();
    void itAnswersInsideTheShapeAndNotOutsideIt();
    void lessMotionMeansNoAnswerAtAll();
private:
    bool m_wasReduced = false;
};

void tst_MaterialRipple::init()
{
    m_wasReduced = QxTheme::isMotionReduced();
    QxTheme::setMotionReduced(false);
}

void tst_MaterialRipple::cleanup()
{
    QxTheme::setMotionReduced(m_wasReduced);
}

/*! It starts where the pointer landed, which is the whole of what it says: the
 *  surface answers from the place the user touched it. */
void tst_MaterialRipple::aPressStartsOneWhereThePointerLanded()
{
    QWidget host;
    host.resize(80, 80);
    MaterialRipple ripple(&host);
    QCOMPARE(ripple.widget(), &host);
    QVERIFY(!ripple.isSpreading());

    QTest::mousePress(&host, Qt::LeftButton, Qt::NoModifier, QPoint(20, 30));
    QVERIFY(ripple.isSpreading());
    QCOMPARE(ripple.origin(), QPointF(20, 30));
}

/*! It is over before the pointer has moved: it spreads, it fades, and then
 *  there is nothing left to paint. */
void tst_MaterialRipple::itSpreadsAndGoesAway()
{
    QWidget host;
    host.resize(80, 80);
    MaterialRipple ripple(&host);

    QTest::mousePress(&host, Qt::LeftButton, Qt::NoModifier, QPoint(40, 40));
    QVERIFY(ripple.isSpreading());
    QTest::qWait(120);
    const qreal spreading = ripple.progress();
    QVERIFY(spreading > 0.0);

    // Longer than the longest answer Material asks for.
    QTest::qWait(500);
    QVERIFY(!ripple.isSpreading());
}

/*! Clipped to the corners of what it is on: a ripple that ran out over the
 *  edge would be a ripple off the surface, which is not a surface answering. */
void tst_MaterialRipple::itAnswersInsideTheShapeAndNotOutsideIt()
{
    QWidget host;
    host.resize(80, 80);
    MaterialRipple ripple(&host);
    ripple.setColor(kInk);

    QTest::mousePress(&host, Qt::LeftButton, Qt::NoModifier, QPoint(40, 40));
    QTest::qWait(120);

    QImage page(80, 80, QImage::Format_ARGB32);
    page.fill(kPage);
    QPainter painter(&page);
    ripple.paint(&painter, QRectF(0.0, 0.0, 80.0, 80.0), 8.0);
    painter.end();

    QVERIFY(page.pixelColor(40, 40) != kPage);
    // The corner is outside the corners: the shape is a rounded one.
    QCOMPARE(page.pixelColor(0, 0), kPage);
}

/*! A ripple is the movement, so when the user asked for less motion there is
 *  nothing to draw - the press is still answered by the state layer. */
void tst_MaterialRipple::lessMotionMeansNoAnswerAtAll()
{
    QxTheme::setMotionReduced(true);

    QWidget host;
    host.resize(80, 80);
    MaterialRipple ripple(&host);

    QTest::mousePress(&host, Qt::LeftButton, Qt::NoModifier, QPoint(40, 40));
    QVERIFY(!ripple.isSpreading());
}

TEST_ADD(tst_MaterialRipple)

#include "tst_materialripple.moc"
