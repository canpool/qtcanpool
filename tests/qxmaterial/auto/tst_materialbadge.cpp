/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialbadge.h"

#include <QAccessible>

QX_MATERIAL_USE_NAMESPACE

class tst_MaterialBadge : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void aCountIsAnumberAndNoCountIsADot();
    void itStopsCountingAtTheMaximum();
    void itIsAMarkNotAStop();
    void whatAnAssistiveToolReads();
};

void tst_MaterialBadge::cleanup()
{
    QAccessible::setActive(false);
}

/*! A badge says how much, or only that there is something - and the two are
 *  different shapes, not one shape with a different number in it. */
void tst_MaterialBadge::aCountIsAnumberAndNoCountIsADot()
{
    MaterialBadge badge;
    QCOMPARE(badge.count(), 0);
    const QSize dot = badge.sizeHint();
    QCOMPARE(dot.width(), dot.height());

    badge.setCount(5);
    QCOMPARE(badge.count(), 5);
    QVERIFY(badge.sizeHint().width() > dot.width());
    QCOMPARE(badge.sizeHint().height(), 16);

    badge.setCount(0);
    QCOMPARE(badge.sizeHint(), dot);
}

/*! Past the maximum a badge stops counting and starts saying so: "127" is not
 *  more useful than "99+" and it is wider. */
void tst_MaterialBadge::itStopsCountingAtTheMaximum()
{
    MaterialBadge badge;
    badge.setCount(150);
    QCOMPARE(badge.maximumCount(), 99);
    const QSize capped = badge.sizeHint();
    QCOMPARE(badge.accessibleName(), QStringLiteral("99+"));

    badge.setCount(1000);
    QCOMPARE(badge.sizeHint(), capped);

    badge.setMaximumCount(9);
    QVERIFY(badge.sizeHint().width() < capped.width());
    QCOMPARE(badge.accessibleName(), QStringLiteral("9+"));
}

/*! A badge is a mark on something else: it is not a stop on the way through
 *  the keyboard and not something the pointer takes hold of. */
void tst_MaterialBadge::itIsAMarkNotAStop()
{
    MaterialBadge badge;
    QCOMPARE(badge.focusPolicy(), Qt::NoFocus);
    QVERIFY(badge.testAttribute(Qt::WA_TransparentForMouseEvents));
}

/*! A badge has nothing to say but its number, and an assistive tool reads the
 *  number rather than the shape. */
void tst_MaterialBadge::whatAnAssistiveToolReads()
{
    MaterialBadge badge;
    badge.setCount(3);

    QAccessible::setActive(true);
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&badge);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::StaticText);
    QCOMPARE(iface->text(QAccessible::Name), QStringLiteral("3"));
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

TEST_ADD(tst_MaterialBadge)

#include "tst_materialbadge.moc"
