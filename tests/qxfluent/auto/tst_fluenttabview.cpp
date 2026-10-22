/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluenttabview.h"
#include "qxfluent/fluenttokens.h"

#include <QAccessible>
#include <QAction>
#include <QLabel>
#include <QSignalSpy>
#include <QWidget>

QX_FLUENT_USE_NAMESPACE

namespace
{
using QxFluent::FluentTabView;

// The measures the row lays out with: twelve either side of a tab's name, two
// between tabs, and - when a tab can be closed - a mark sixteen wide at its
// end. Aiming at a tab, or at its close mark, means knowing where it stands.
constexpr int kPadding = 12;
constexpr int kSpacing = 2;
constexpr int kCloseGap = 4;
constexpr int kCloseSize = 16;
constexpr int kBarHeight = 40;

int tabWidth(const QString &text, bool closable)
{
    const QFontMetrics metrics(FluentType::font(FluentType::Ramp::Body));
    qreal width = qreal(kPadding) + qreal(metrics.horizontalAdvance(text)) + qreal(kPadding);
    if (closable) {
        width += qreal(kCloseGap + kCloseSize);
    }
    return int(qMax<qreal>(width, 32.0));
}

int tabLeft(const QStringList &texts, bool closable, int index)
{
    int left = 0;
    for (int i = 0; i < index; ++i) {
        left += tabWidth(texts.at(i), closable) + kSpacing;
    }
    return left;
}

QPoint tabPoint(const QStringList &texts, bool closable, int index)
{
    return QPoint(tabLeft(texts, closable, index) + kPadding / 2, kBarHeight / 2);
}

QPoint closePoint(const QStringList &texts, bool closable, int index)
{
    const int left = tabLeft(texts, closable, index);
    const int width = tabWidth(texts.at(index), closable);
    return QPoint(left + width - kPadding / 2 - kCloseSize / 2, kBarHeight / 2);
}

FluentTabView *viewWith(const QStringList &texts, bool closable)
{
    FluentTabView *view = new FluentTabView;
    view->setClosable(closable);
    for (const QString &text : texts) {
        view->addTab(new QLabel(text), text);
    }
    view->resize(view->sizeHint());
    return view;
}
}   // namespace

class tst_FluentTabView : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void aTabIsSomethingOpen();
    void aClickLooksAtTheTabUnderIt();
    void onlyTheTabBeingLookedAtIsShowing();
    void theContentFollowsTheRoomItIsGiven();
    void closingIsAskedAboutNotDone();
    void theRowIsWalkedSideways();
    void whatAnAssistiveToolReads();
};

void tst_FluentTabView::cleanup()
{
    QAccessible::setActive(false);
}

/*! A tab is not a destination: it is something open, and it keeps what was
 *  being done in it when another one is looked at. */
void tst_FluentTabView::aTabIsSomethingOpen()
{
    std::unique_ptr<FluentTabView> view(viewWith({QStringLiteral("One"), QStringLiteral("Two")}, false));
    QCOMPARE(view->tabs().count(), 2);
    QCOMPARE(view->currentIndex(), 0);

    QAction *two = view->tabs().at(1);
    QCOMPARE(view->contentOf(two)->objectName(), QString());
    QCOMPARE(qobject_cast<QLabel *>(view->contentOf(two))->text(), QStringLiteral("Two"));

    view->removeTab(two);
    QCOMPARE(view->tabs().count(), 1);
    QVERIFY(!view->contentOf(two));
}

void tst_FluentTabView::aClickLooksAtTheTabUnderIt()
{
    std::unique_ptr<FluentTabView> view(viewWith({QStringLiteral("One"), QStringLiteral("Two")}, false));
    QSignalSpy changed(view.get(), &FluentTabView::currentIndexChanged);

    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier,
                      tabPoint({QStringLiteral("One"), QStringLiteral("Two")}, false, 1));

    QCOMPARE(view->currentIndex(), 1);
    QCOMPARE(view->currentTab(), view->tabs().at(1));
    QCOMPARE(changed.count(), 1);
}

/*! The content is the tab's, so what shows is the one being looked at and the
 *  others are still there - hidden, not gone. */
void tst_FluentTabView::onlyTheTabBeingLookedAtIsShowing()
{
    std::unique_ptr<FluentTabView> view(viewWith({QStringLiteral("One"), QStringLiteral("Two")}, false));
    QWidget *one = view->contentOf(view->tabs().at(0));
    QWidget *two = view->contentOf(view->tabs().at(1));

    // isHidden(), not isVisible(): nothing here has been shown, and a widget
    // that has never been shown answers false to everything that asks whether
    // it can be seen. What is being asked is what the view did.
    QVERIFY(!one->isHidden());
    QVERIFY(two->isHidden());

    view->setCurrentIndex(1);
    QVERIFY(one->isHidden());
    QVERIFY(!two->isHidden());
}

/*! The room a tab's content gets is the room under the row, and it is worked
 *  out when it is asked for rather than when the room changed: a view that has
 *  never been shown is never told it was resized, so a layout made before that
 *  would go on giving the content the room the view used to have. */
void tst_FluentTabView::theContentFollowsTheRoomItIsGiven()
{
    std::unique_ptr<FluentTabView> view(viewWith({QStringLiteral("One")}, false));
    QWidget *one = view->contentOf(view->tabs().at(0));
    QVERIFY(one->height() != 260);

    view->resize(600, 300);
    // What makes the view work out where things stand again is being asked
    // where they are: a press asks.
    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier, tabPoint({QStringLiteral("One")}, false, 0));

    QCOMPARE(one->width(), 600);
    QCOMPARE(one->height(), 300 - kBarHeight);
}

/*! The cross is a question, not an answer: the view says which tab's cross was
 *  clicked and closes nothing, because a tab with work in it is the
 *  application's to close. */
void tst_FluentTabView::closingIsAskedAboutNotDone()
{
    std::unique_ptr<FluentTabView> view(viewWith({QStringLiteral("One"), QStringLiteral("Two")}, true));
    QVERIFY(view->isClosable());
    QAction *one = view->tabs().at(0);
    // Caught with a connection rather than a spy: what a spy hands back is a
    // QVariant, and reading a QAction out of one is a question Qt 5 and Qt 6
    // answer differently - which is nothing to do with what is being tested.
    int closeCount = 0;
    QAction *closed = Q_NULLPTR;
    connect(view.get(), &FluentTabView::closeRequested, [&closeCount, &closed](QAction *action) {
        ++closeCount;
        closed = action;
    });

    QTest::mouseClick(view.get(), Qt::LeftButton, Qt::NoModifier,
                      closePoint({QStringLiteral("One"), QStringLiteral("Two")}, true, 0));

    QCOMPARE(closeCount, 1);
    QCOMPARE(closed, one);
    QCOMPARE(view->tabs().count(), 2);
    QCOMPARE(view->currentIndex(), 0);
}

void tst_FluentTabView::theRowIsWalkedSideways()
{
    std::unique_ptr<FluentTabView> view(
        viewWith({QStringLiteral("One"), QStringLiteral("Two"), QStringLiteral("Three")}, false));

    QTest::keyClick(view.get(), Qt::Key_Right);
    QCOMPARE(view->currentIndex(), 1);
    QTest::keyClick(view.get(), Qt::Key_Right);
    QCOMPARE(view->currentIndex(), 2);
    // The end is a wall: walking on from the last tab stays there.
    QTest::keyClick(view.get(), Qt::Key_Right);
    QCOMPARE(view->currentIndex(), 2);
    QTest::keyClick(view.get(), Qt::Key_Left);
    QCOMPARE(view->currentIndex(), 1);
}

void tst_FluentTabView::whatAnAssistiveToolReads()
{
    std::unique_ptr<FluentTabView> view(viewWith({QStringLiteral("One")}, false));
    QAccessible::setActive(true);
    QAccessibleInterface *face = QAccessible::queryAccessibleInterface(view.get());
    QVERIFY(face);
    QCOMPARE(face->role(), QAccessible::PageTabList);
}

TEST_ADD(tst_FluentTabView)

#include "tst_fluenttabview.moc"
