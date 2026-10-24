/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialtooltip.h"

#include <QEvent>
#include <QPalette>
#include <QWidget>

QX_MATERIAL_USE_NAMESPACE

namespace
{
constexpr int kGap = 8;

void arrive(QWidget *anchor)
{
    QEvent enter(QEvent::Enter);
    QApplication::sendEvent(anchor, &enter);
}

void focusArrives(QWidget *anchor)
{
    QEvent focusIn(QEvent::FocusIn);
    QApplication::sendEvent(anchor, &focusIn);
}

void focusLeaves(QWidget *anchor)
{
    QEvent focusOut(QEvent::FocusOut);
    QApplication::sendEvent(anchor, &focusOut);
}
}   // namespace

class tst_MaterialTooltip : public QObject
{
    Q_OBJECT
private slots:
    void aTransientHintStaysOutOfTheFocusPath();
    void theChipStandsWhereItIsTold();
    void theChipIsTheInversePair();
    void theKeyboardOpensItTooAndLeavingClosesIt();
};

/*! Four sides, one answer each: the chip stands a gap off the side it was
 *  told, centred along it. */
void tst_MaterialTooltip::theChipStandsWhereItIsTold()
{
    QWidget anchor;
    anchor.move(40, 40);
    anchor.resize(100, 40);
    anchor.show();

    MaterialTooltip tip;
    tip.setAnchor(&anchor);
    tip.setText("Copied");
    QVERIFY(tip.anchor() == &anchor);

    tip.setPlacement(MaterialTooltip::Placement::Top);
    tip.popup();
    QVERIFY(tip.isVisible());
    QCOMPARE(tip.geometry().center().x(), anchor.geometry().center().x());
    QCOMPARE(tip.geometry().bottom(), anchor.geometry().top() - kGap - 1);

    tip.setPlacement(MaterialTooltip::Placement::Bottom);
    QCOMPARE(tip.geometry().center().x(), anchor.geometry().center().x());
    QCOMPARE(tip.geometry().top(), anchor.geometry().bottom() + kGap + 1);

    tip.setPlacement(MaterialTooltip::Placement::Left);
    QCOMPARE(tip.geometry().right(), anchor.geometry().left() - kGap - 1);
    QCOMPARE(tip.geometry().center().y(), anchor.geometry().center().y());

    tip.setPlacement(MaterialTooltip::Placement::Right);
    QCOMPARE(tip.geometry().left(), anchor.geometry().right() + kGap + 1);
    QCOMPARE(tip.geometry().center().y(), anchor.geometry().center().y());
}

/*! The chip is the inverse pair - the ink the page writes with as the
 *  surface, the page's colour as the text - and no arrow drawn anywhere. */
void tst_MaterialTooltip::theChipIsTheInversePair()
{
    MaterialTooltip tip;
    tip.setText("Copied");

    QPalette pal = tip.palette();
    pal.setColor(QPalette::Window, Qt::white);
    pal.setColor(QPalette::WindowText, Qt::red);
    tip.setPalette(pal);
    tip.resize(tip.sizeHint());

    const QImage image = tip.grab().toImage();
    // The surface is the ink: the chip's face left of the words is surface
    // red. And no arrow pokes out of the chip anywhere: both straight edges
    // are surface from one corner to the other, never the page's colour.
    QCOMPARE(image.pixelColor(4, image.height() / 2), QColor(Qt::red));
    for (int x = 4; x < image.width() - 4; ++x) {
        QCOMPARE(image.pixelColor(x, 1), QColor(Qt::red));
        QCOMPARE(image.pixelColor(x, image.height() - 2), QColor(Qt::red));
    }
}

/*! The keyboard arrives the way the pointer does, and leaving is leaving. */
void tst_MaterialTooltip::theKeyboardOpensItTooAndLeavingClosesIt()
{
    QWidget anchor;
    anchor.resize(100, 40);
    anchor.show();

    MaterialTooltip tip;
    tip.setAnchor(&anchor);
    tip.setText("Copied");

    focusArrives(&anchor);
    QTest::qWait(500);
    QVERIFY(tip.isVisible());

    focusLeaves(&anchor);
    QVERIFY(!tip.isVisible());
}

void tst_MaterialTooltip::aTransientHintStaysOutOfTheFocusPath()
{
    MaterialTooltip widget;
    widget.show();
    QApplication::processEvents();

    // A tooltip is a transient hint; it must never capture keyboard focus.
    widget.setFocus();
    QApplication::processEvents();
    QVERIFY(!widget.hasFocus());
    QVERIFY(QApplication::focusWidget() != &widget);
}

TEST_ADD(tst_MaterialTooltip)

#include "tst_materialtooltip.moc"
