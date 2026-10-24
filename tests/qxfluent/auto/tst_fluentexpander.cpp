/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentexpander.h"

#include "qxtheme/qxthememanager.h"
#include "qxtheme/qxthememotion.h"

#include <QAccessible>
#include <QImage>
#include <QLabel>

QX_FLUENT_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
constexpr int kSettleWait = 500;

// The header is forty tall.
constexpr int kHeaderHeight = 40;
}   // namespace

class tst_FluentExpander : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void theHeaderIsWhatThePointerCanReach();
    void openingGivesTheBodyRoom();
    void reduceMotionOpensItAtOnce();
    void itIsAButtonThatOpensSomething();
    void aHeaderNobodyCanOpenIsDrawnFaded();
    void theKeyboardLandsOnIt();
private:
    bool m_wasReduced = false;
    bool m_wasActive = false;
};

void tst_FluentExpander::cleanup()
{
    QxTheme::setMotionReduced(m_wasReduced);
    QAccessible::setActive(m_wasActive);
}

/*! A click below the header is not a request to open: the header is the
 *  control, and what it keeps is not. */
void tst_FluentExpander::theHeaderIsWhatThePointerCanReach()
{
    FluentExpander expander;
    expander.resize(200, 120);

    QTest::mousePress(&expander, Qt::LeftButton, Qt::NoModifier, QPoint(100, kHeaderHeight + 20));
    QTest::mouseRelease(&expander, Qt::LeftButton, Qt::NoModifier, QPoint(100, kHeaderHeight + 20));
    QVERIFY(!expander.isExpanded());

    QTest::mousePress(&expander, Qt::LeftButton, Qt::NoModifier, QPoint(100, 20));
    QTest::mouseRelease(&expander, Qt::LeftButton, Qt::NoModifier, QPoint(100, 20));
    QVERIFY(expander.isExpanded());
}

/*! Open is not a flag: it is room, and the room is the body's. */
void tst_FluentExpander::openingGivesTheBodyRoom()
{
    FluentExpander expander;
    QLabel body(QStringLiteral("Body"));
    expander.setContent(&body);
    const int bodyHeight = body.sizeHint().height();

    QCOMPARE(expander.sizeHint().height(), kHeaderHeight);

    expander.setExpanded(true);
    QTest::qWait(kSettleWait);

    QCOMPARE(expander.sizeHint().height(), kHeaderHeight + bodyHeight);
    // isVisible() answers for the whole ancestry, and nothing here is shown;
    // isHidden() is the question about the body itself.
    QVERIFY(!body.isHidden());
}

/*! Asked for less motion, an expander is simply open - the movement was never
 *  the point. */
void tst_FluentExpander::reduceMotionOpensItAtOnce()
{
    m_wasReduced = QxTheme::isMotionReduced();
    QxTheme::setMotionReduced(true);

    FluentExpander expander;
    QLabel body(QStringLiteral("Body"));
    expander.setContent(&body);

    expander.setExpanded(true);
    // No wait: under reduce-motion the room is there at once.
    QCOMPARE(expander.sizeHint().height(), kHeaderHeight + body.sizeHint().height());
}

/*! What it is, and what is true of it: a button that opens something, and
 *  whether what it keeps is out. */
void tst_FluentExpander::itIsAButtonThatOpensSomething()
{
    m_wasActive = QAccessible::isActive();
    QAccessible::setActive(true);

    FluentExpander expander;
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&expander);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::Button);
    QVERIFY(iface->state().expandable);
    QVERIFY(!iface->state().expanded);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));

    expander.setExpanded(true);
    QAccessibleInterface *openIface = QAccessible::queryAccessibleInterface(&expander);
    QVERIFY(openIface);
    QVERIFY(openIface->state().expanded);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(openIface));
}

/*! A header nobody can open is drawn faded, and does not open either: the
 *  chevron still points at a body it will not show, which is the part that
 *  makes the promise. */
void tst_FluentExpander::aHeaderNobodyCanOpenIsDrawnFaded()
{
    FluentExpander ready;
    ready.resize(200, 60);
    ready.setTitle(QStringLiteral("Advanced"));
    const QImage live = ready.grab().toImage();

    FluentExpander faded;
    faded.resize(200, 60);
    faded.setTitle(QStringLiteral("Advanced"));
    // Pinned, so only the family's own ink can make the frames differ.
    qtcPinDisabledColours(&faded);
    faded.setEnabled(false);

    QTest::mousePress(&faded, Qt::LeftButton, Qt::NoModifier, QPoint(100, 20));
    QTest::mouseRelease(&faded, Qt::LeftButton, Qt::NoModifier, QPoint(100, 20));

    QVERIFY(!faded.isExpanded());
    QVERIFY(faded.grab().toImage() != live);
}

/*! The keyboard finds it, and the ring says where the keyboard is: a control
 *  the keyboard cannot reach is a control half the users cannot use, and one it
 *  reaches without a mark is a control they cannot find.
 */
void tst_FluentExpander::theKeyboardLandsOnIt()
{
    FluentExpander widget;
    widget.setTitle(QStringLiteral("More"));
    widget.resize(240, widget.sizeHint().height());
    widget.show();
    QApplication::processEvents();

    widget.setFocus();
    QVERIFY(widget.hasFocus());
    QCOMPARE(QApplication::focusWidget(), &widget);
}

TEST_ADD(tst_FluentExpander)

#include "tst_fluentexpander.moc"
