/*!
 * \file
 *
 * DockSplitter is QSplitter plus the three questions the docking layout asks of
 * it: is there anything visible in here, which widget is at either end, and does
 * this splitter hold the central widget.
 *
 * hasVisibleContent() is the one with a rule behind it - it walks the children
 * and stops at the first visible one, and it is asked while the layout climbs
 * the splitter tree to hide the parents that emptied out. The distinction it
 * draws is "explicitly hidden" (isHidden()), not "currently on screen", which is
 * why these tests hide and show a child instead of showing the splitter: that
 * keeps them independent of whether a window is actually mapped.
 *
 * Whether a freshly added child starts out visible is Qt's business, not this
 * class's, so the test drives the child through hide() and show() and only
 * asserts that the answer follows - it does not assert where it starts.
 */
#include "tst_global.h"

#include "qxdock/docksplitter.h"

QX_DOCK_USE_NAMESPACE

class tst_DockSplitter : public QObject
{
    Q_OBJECT

private slots:
    void emptySplitter();
    void firstAndLastWidget();
    void visibleContentFollowsTheChildren();
};

void tst_DockSplitter::emptySplitter()
{
    DockSplitter splitter;

    QVERIFY(!splitter.hasVisibleContent());
    QVERIFY(!splitter.firstWidget());
    QVERIFY(!splitter.lastWidget());

    // No DockPanel child, so no central widget area to resize with.
    QVERIFY(!splitter.isResizingWithContainer());
}

void tst_DockSplitter::firstAndLastWidget()
{
    DockSplitter splitter;

    QWidget *first = new QWidget;
    QWidget *second = new QWidget;
    splitter.addWidget(first);
    splitter.addWidget(second);

    QCOMPARE(splitter.firstWidget(), first);
    QCOMPARE(splitter.lastWidget(), second);

    // One widget is both ends at once.
    DockSplitter single;
    QWidget *only = new QWidget;
    single.addWidget(only);
    QCOMPARE(single.firstWidget(), only);
    QCOMPARE(single.lastWidget(), only);
}

void tst_DockSplitter::visibleContentFollowsTheChildren()
{
    DockSplitter splitter;
    QWidget *child = new QWidget;
    splitter.addWidget(child);

    child->hide();
    QVERIFY(!splitter.hasVisibleContent());

    child->show();
    QVERIFY(splitter.hasVisibleContent());

    // One visible child is enough, even with hidden ones in front of it.
    QWidget *hidden = new QWidget;
    splitter.insertWidget(0, hidden);
    hidden->hide();
    QVERIFY(splitter.hasVisibleContent());
}

TEST_ADD(tst_DockSplitter)

#include "tst_docksplitter.moc"
