/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxribbon/ribboncolorbutton.h"

#include <QSignalSpy>

QX_RIBBON_USE_NAMESPACE

/*
 * RibbonColorButton is a tool button with one piece of state - the colour it is showing -
 * and two ways to report a change: colorChanged() when someone assigns a new colour, and
 * colorClicked() when the user presses the button, which carries whatever colour it is
 * currently showing. The two are independent on purpose: assigning a colour is not a
 * click, and clicking does not change the colour.
 */
class tst_RibbonColorButton : public QObject
{
    Q_OBJECT
private slots:
    void startsWithoutAColor();
    void setColorRoundTrips();
    void setColorToTheSameValueIsSilent();
    void clickingReportsTheColorItIsShowing();
};

void tst_RibbonColorButton::startsWithoutAColor()
{
    RibbonColorButton button;

    // An invalid QColor is what the button paints the "no colour" diagonal for, so
    // "not valid" is the documented starting state rather than an accident.
    QVERIFY(!button.color().isValid());
}

void tst_RibbonColorButton::setColorRoundTrips()
{
    RibbonColorButton button;
    QSignalSpy changed(&button, &RibbonColorButton::colorChanged);

    button.setColor(QColor(Qt::red));

    QCOMPARE(button.color(), QColor(Qt::red));
    QCOMPARE(changed.count(), 1);
    QCOMPARE(qvariant_cast<QColor>(changed.at(0).at(0)), QColor(Qt::red));
}

void tst_RibbonColorButton::setColorToTheSameValueIsSilent()
{
    RibbonColorButton button;
    button.setColor(QColor(Qt::red));

    QSignalSpy changed(&button, &RibbonColorButton::colorChanged);
    button.setColor(QColor(Qt::red));

    QCOMPARE(button.color(), QColor(Qt::red));
    // Assigning the colour it already has must not keep telling the owner about it.
    QCOMPARE(changed.count(), 0);
}

void tst_RibbonColorButton::clickingReportsTheColorItIsShowing()
{
    RibbonColorButton button;
    button.setColor(QColor(Qt::blue));

    QSignalSpy clicked(&button, &RibbonColorButton::colorClicked);
    QSignalSpy changed(&button, &RibbonColorButton::colorChanged);

    button.click();

    QCOMPARE(clicked.count(), 1);
    QCOMPARE(qvariant_cast<QColor>(clicked.at(0).at(0)), QColor(Qt::blue));
    // A click reports the colour, it does not assign one.
    QCOMPARE(changed.count(), 0);
    QCOMPARE(button.color(), QColor(Qt::blue));
}

TEST_ADD(tst_RibbonColorButton)

#include "tst_ribboncolorbutton.moc"
