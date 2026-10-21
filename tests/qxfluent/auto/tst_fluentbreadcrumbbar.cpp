/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentbreadcrumbbar.h"
#include "qxfluent/fluenttokens.h"

#include <QApplication>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QSignalSpy>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QEnterEvent>
#endif

QX_FLUENT_USE_NAMESPACE

namespace
{
// The same layout the bar draws with: a step is its text with eight either
// side, and the separator between two steps is twelve. The test needs to aim
// at a step, and aiming means knowing where one is.
constexpr int kStepPadding = 8;
constexpr int kSeparatorWidth = 12;

QPoint stepCenter(const FluentBreadcrumbBar *bar, int index)
{
    const QFontMetrics metrics(FluentType::font(FluentType::Ramp::Body));
    int left = 0;
    for (int i = 0; i < index; ++i) {
        left += metrics.horizontalAdvance(bar->items().at(i)) + 2 * kStepPadding + kSeparatorWidth;
    }
    const int width = metrics.horizontalAdvance(bar->items().at(index)) + 2 * kStepPadding;
    return QPoint(left + width / 2, bar->height() / 2);
}

void sendEnter(QWidget *widget, const QPoint &pos)
{
    // Hovering is two events in reality - the pointer arrives, then it moves -
    // and a move alone would not put the bar in its hover state.
    const QPoint global = widget->mapToGlobal(pos);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QEnterEvent event(QPointF{pos}, QPointF{pos}, QPointF{global});
#else
    QEvent event(QEvent::Enter);
    Q_UNUSED(global)
#endif
    QApplication::sendEvent(widget, &event);
}

void sendHover(QWidget *widget, const QPoint &pos)
{
    const QPoint global = widget->mapToGlobal(pos);
    QMouseEvent event(QEvent::MouseMove, pos, global, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(widget, &event);
}
}   // namespace

class tst_FluentBreadcrumbBar : public QObject
{
    Q_OBJECT
private slots:
    void thePathIsWhatItSays();
    void theLastStepIsNotADestination();
    void aStepBeforeTheLastIsAWayBack();
    void theHoverMarksTheStepUnderThePointer();
    void theKeyboardWalksThePath();
    void anEmptyPathIsNothing();
};

/*! What an assistive tool reads is the path in one sentence, not a row of
 *  buttons: the same thing a sighted user reads. */
void tst_FluentBreadcrumbBar::thePathIsWhatItSays()
{
    FluentBreadcrumbBar bar;
    QVERIFY(bar.items().isEmpty());

    const QStringList path{QStringLiteral("Home"), QStringLiteral("Library"), QStringLiteral("This file")};
    bar.setItems(path);
    QCOMPARE(bar.items(), path);
    QCOMPARE(bar.accessibleName(), QStringLiteral("Home / Library / This file"));
}

/*! The last step is where the user already is, so it answers nothing - not to
 *  a click and not to the keyboard. The bar still grows to hold it. */
void tst_FluentBreadcrumbBar::theLastStepIsNotADestination()
{
    FluentBreadcrumbBar bar;
    bar.setItems({QStringLiteral("Home"), QStringLiteral("Library")});
    const int two = bar.sizeHint().width();

    bar.setItems({QStringLiteral("Home"), QStringLiteral("Library"), QStringLiteral("This file")});
    QVERIFY(bar.sizeHint().width() > two);
    bar.resize(bar.sizeHint());

    QSignalSpy clicked(&bar, &FluentBreadcrumbBar::itemClicked);
    QTest::mousePress(&bar, Qt::LeftButton, Qt::NoModifier, stepCenter(&bar, 2));
    QCOMPARE(clicked.count(), 0);
}

/*! Every step before the last is a way back, and what it answers with is the
 *  step itself - which one, and what it says. */
void tst_FluentBreadcrumbBar::aStepBeforeTheLastIsAWayBack()
{
    FluentBreadcrumbBar bar;
    bar.setItems({QStringLiteral("Home"), QStringLiteral("Library"), QStringLiteral("This file")});
    bar.resize(bar.sizeHint());

    QSignalSpy clicked(&bar, &FluentBreadcrumbBar::itemClicked);
    QTest::mousePress(&bar, Qt::LeftButton, Qt::NoModifier, stepCenter(&bar, 0));
    QCOMPARE(clicked.count(), 1);
    QCOMPARE(clicked.at(0).at(0).toInt(), 0);
    QCOMPARE(clicked.at(0).at(1).toString(), QStringLiteral("Home"));

    QTest::mousePress(&bar, Qt::LeftButton, Qt::NoModifier, stepCenter(&bar, 1));
    QCOMPARE(clicked.count(), 2);
    QCOMPARE(clicked.at(1).at(0).toInt(), 1);
}

/*! Which step the pointer is on is part of what the bar says, and the wash
 *  goes on that step alone - not on the whole row. */
void tst_FluentBreadcrumbBar::theHoverMarksTheStepUnderThePointer()
{
    FluentBreadcrumbBar bar;
    bar.setItems({QStringLiteral("Home"), QStringLiteral("Library"), QStringLiteral("This file")});
    bar.resize(bar.sizeHint());
    const QImage resting = bar.grab().toImage();

    sendEnter(&bar, stepCenter(&bar, 0));
    sendHover(&bar, stepCenter(&bar, 0));
    const QImage onFirst = bar.grab().toImage();
    QVERIFY(onFirst != resting);

    sendHover(&bar, stepCenter(&bar, 1));
    QVERIFY(bar.grab().toImage() != onFirst);
}

/*! The keyboard walks the path and stops at both ends; the last step is where
 *  the user is, so walking onto it and pressing does nothing. */
void tst_FluentBreadcrumbBar::theKeyboardWalksThePath()
{
    FluentBreadcrumbBar bar;
    bar.setItems({QStringLiteral("Home"), QStringLiteral("Library"), QStringLiteral("This file")});
    bar.resize(bar.sizeHint());
    QSignalSpy clicked(&bar, &FluentBreadcrumbBar::itemClicked);

    QTest::keyClick(&bar, Qt::Key_Right);
    QTest::keyClick(&bar, Qt::Key_Right);
    QCOMPARE(clicked.count(), 0);

    QTest::keyClick(&bar, Qt::Key_Enter);
    QCOMPARE(clicked.count(), 1);
    QCOMPARE(clicked.at(0).at(0).toInt(), 1);
    QCOMPARE(clicked.at(0).at(1).toString(), QStringLiteral("Library"));

    // Walk onto the current step and press: it is not a destination.
    QTest::keyClick(&bar, Qt::Key_Right);
    QTest::keyClick(&bar, Qt::Key_Enter);
    QCOMPARE(clicked.count(), 1);
}

/*! With nothing to show the bar is nothing: no path to read, and nothing that
 *  could answer a click. */
void tst_FluentBreadcrumbBar::anEmptyPathIsNothing()
{
    FluentBreadcrumbBar bar;
    QCOMPARE(bar.sizeHint().width(), 0);
    QCOMPARE(bar.minimumSizeHint().width(), 0);

    QSignalSpy clicked(&bar, &FluentBreadcrumbBar::itemClicked);
    QTest::mousePress(&bar, Qt::LeftButton, Qt::NoModifier, QPoint(1, 1));
    QCOMPARE(clicked.count(), 0);
}

TEST_ADD(tst_FluentBreadcrumbBar)

#include "tst_fluentbreadcrumbbar.moc"
