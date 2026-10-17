/**
 * Copyright (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxribbon/ribbonpage.h"
#include "qxribbon/ribbonbar.h"
#include "qxribbon/ribbongroup.h"
#include "qxribbon/ribbonpagecontext.h"

#include <QAccessible>
#include <QAction>
#include <QCoreApplication>
#include <QToolButton>
#include <QWheelEvent>

QX_RIBBON_USE_NAMESPACE

class tst_RibbonPage : public QObject
{
    Q_OBJECT
private slots:
    void page();
    void group();
    void scroll();
    void wheelAndButtonsShareOneRange();
    void clampAfterWidening();
    void groupShrink();
    void clear();
    void accessibleRoles();
};

void tst_RibbonPage::page()
{
    RibbonBar rb;

    RibbonPage *page = rb.addPage(tr("page"));
    QCOMPARE(tr("page"), page->pageName());
    page->setPageName(tr("egap"));
    QCOMPARE(tr("egap"), page->pageName());

    // Accessibility (B2): the page title is mirrored into the accessible name, with the
    // '&' mnemonic stripped.
    QCOMPARE(page->accessibleName(), QStringLiteral("egap"));
    page->setPageName(tr("&Home"));
    QCOMPARE(page->accessibleName(), QStringLiteral("Home"));

    // Accessibility (B2): the ribbon container exposes a stable accessible name.
    QCOMPARE(rb.accessibleName(), QStringLiteral("Ribbon"));

    QCOMPARE(true, page->isCanCustomize());
    page->setCanCustomize(false);
    QCOMPARE(false, page->isCanCustomize());

    QCOMPARE(false, page->isPageContext());
    RibbonPageContext *pc = rb.addPageContext(tr("context"));
    RibbonPage *page2 = pc->addPage(tr("page2"));
    QCOMPARE(true, page2->isPageContext());
}

void tst_RibbonPage::group()
{
    RibbonBar rb;

    RibbonPage *page = rb.addPage(tr("page"));
    RibbonGroup *g1 = page->addGroup(tr("group1"));
    QCOMPARE(0, page->groupIndex(g1));
    RibbonGroup *g2 = page->addGroup(tr("group2"));
    QCOMPARE(1, page->groupIndex(g2));
    RibbonGroup *g3 = new RibbonGroup(tr("group3"), page);
    page->addGroup(g3);
    QCOMPARE(2, page->groupIndex(g3));
    RibbonGroup *g4 = new RibbonGroup(page);
    page->addGroup(g4);
    QCOMPARE(3, page->groupIndex(g4));
    RibbonGroup *g5 = page->insertGroup(2, tr("group5"));
    QCOMPARE(2, page->groupIndex(g5));
    QCOMPARE(3, page->groupIndex(g3));

    QCOMPARE(g2, page->group(1));
    QCOMPARE(nullptr, page->group(-1));
    QCOMPARE(nullptr, page->group(10));

    QCOMPARE(g1, page->groupByName(tr("group1")));
    QCOMPARE(nullptr, page->groupByName(tr("xxx")));
    QCOMPARE(g4, page->groupByName(tr("")));   // g4 name is empty and is first

    QCOMPARE(g1, page->groupByObjectName(tr("group1")));
    QCOMPARE(nullptr, page->groupByObjectName(tr("xxx")));
    QCOMPARE(g3, page->groupByObjectName(tr("")));   // g3 and g4 object name are empty, but g3 is first

    QCOMPARE(5, page->groupCount());
    QCOMPARE(page->groupCount(), page->groupList().count());

    QString titles = "";
    for (RibbonGroup *g : page->groupList()) {
        titles += g->groupName() + ",";
    }
    QCOMPARE("group1,group2,group5,group3,,", titles);

    // move
    page->moveGroup(2, 3);
    QCOMPARE(2, page->groupIndex(g3));
    QCOMPARE(3, page->groupIndex(g5));
    titles = "";
    for (RibbonGroup *g : page->groupList()) {
        titles += g->groupName() + ",";
    }
    QCOMPARE("group1,group2,group3,group5,,", titles);

    page->moveGroup(3, 2);
    QCOMPARE(2, page->groupIndex(g5));
    QCOMPARE(3, page->groupIndex(g3));
    titles = "";
    for (RibbonGroup *g : page->groupList()) {
        titles += g->groupName() + ",";
    }
    QCOMPARE("group1,group2,group5,group3,,", titles);

    // take
    QCOMPARE(false, page->takeGroup(nullptr));
    QCOMPARE(true, page->takeGroup(g5));
    QCOMPARE(tr("group5"), g5->groupName());   // not deleted
    QCOMPARE(-1, page->groupIndex(g5));
    QCOMPARE(2, page->groupIndex(g3));
    QCOMPARE(4, page->groupCount());
    titles = "";
    for (RibbonGroup *g : page->groupList()) {
        titles += g->groupName() + ",";
    }
    QCOMPARE("group1,group2,group3,,", titles);

    delete g5;

    // remove
    QCOMPARE(true, page->removeGroup(g4));   // deleted
    g4 = nullptr;
    QCOMPARE(3, page->groupCount());
    titles = "";
    for (RibbonGroup *g : page->groupList()) {
        titles += g->groupName() + ",";
    }
    QCOMPARE("group1,group2,group3,", titles);

    QCOMPARE(true, page->removeGroup(0));   // g1 deleted
    g1 = nullptr;
    QCOMPARE(0, page->groupIndex(g2));
    QCOMPARE(2, page->groupCount());
    titles = "";
    for (RibbonGroup *g : page->groupList()) {
        titles += g->groupName() + ",";
    }
    QCOMPARE("group2,group3,", titles);

    QCOMPARE(false, page->removeGroup(-1));
    QCOMPARE(false, page->removeGroup(10));
}

void tst_RibbonPage::scroll()
{
    RibbonBar rb;

    RibbonPage *page = rb.addPage(tr("page"));
    // Content has to overflow the page by more than one page, otherwise a single
    // step cannot be told apart from the clamp at either end.
    for (int i = 0; i < 16; ++i) {
        RibbonGroup *group = page->addGroup(QStringLiteral("group%1").arg(i));
        group->addLargeAction(new QAction(QStringLiteral("action%1").arg(i), &rb));
    }
    page->resize(120, 80);

    // The two scroll buttons belong to RibbonPagePrivate, whose class is not
    // exported, so they are collected as plain QToolButtons and told apart by
    // the arrow only they set.
    QToolButton *left = nullptr;
    QToolButton *right = nullptr;
    int leftCount = 0;
    int rightCount = 0;
    for (QToolButton *button : page->findChildren<QToolButton *>()) {
        if (button->arrowType() == Qt::LeftArrow) {
            left = button;
            ++leftCount;
        } else if (button->arrowType() == Qt::RightArrow) {
            right = button;
            ++rightCount;
        }
    }
    QCOMPARE(leftCount, 1);
    QCOMPARE(rightCount, 1);

    RibbonGroup *first = page->group(0);
    RibbonGroup *last = page->group(page->groupCount() - 1);

    // The content has to overflow by more than one page, otherwise a single
    // step cannot be told apart from the clamp at either end.
    QVERIFY2(
        last->geometry().right() > 3 * page->width(),
        qPrintable(
            QStringLiteral("content spans %1 in a %2 wide page").arg(last->geometry().right()).arg(page->width())));

    // At the left edge only the button that scrolls away from it is offered.
    QVERIFY(!right->isHidden());
    QVERIFY(left->isHidden());

    const int origin = first->x();

    // One right click moves the content left by exactly one page: the step is
    // read off the behaviour rather than hard-coded, because its pixel size
    // depends on the font.
    right->click();
    const int step = origin - first->x();
    QVERIFY(step > 0);
    right->click();
    QCOMPARE(first->x(), origin - 2 * step);

    // The right clamp holds: the left button appears and further clicks stop
    // moving the content.
    for (int i = 0; i < 40; ++i) {
        right->click();
    }
    QVERIFY(!left->isHidden());
    const int rightmost = first->x();
    QVERIFY(rightmost <= origin - 2 * step);
    right->click();
    QCOMPARE(first->x(), rightmost);

    // The left clamp holds too, and lands back on the margin rather than
    // overshooting it by a page.
    for (int i = 0; i < 40; ++i) {
        left->click();
    }
    QCOMPARE(first->x(), origin);
    QVERIFY(left->isHidden());
    left->click();
    QCOMPARE(first->x(), origin);
}

/*!
 * E2: the wheel and the scroll buttons go through one range, so the same
 * journey lands on the same offsets whichever way it was made.
 *
 * They used to be two spellings of the range - a hand-rolled pair of if-chains
 * in the wheel path, a qBound() in the button path - which agreed only because
 * someone had read them against each other. Here they are driven to both ends
 * and compared pixel for pixel.
 */
void tst_RibbonPage::wheelAndButtonsShareOneRange()
{
    RibbonBar rb;

    RibbonPage *page = rb.addPage(tr("page"));
    // More than one page of overflow, so an end is an end rather than a step.
    for (int i = 0; i < 16; ++i) {
        RibbonGroup *group = page->addGroup(QStringLiteral("group%1").arg(i));
        group->addLargeAction(new QAction(QStringLiteral("action%1").arg(i), &rb));
    }
    page->resize(120, 80);
    page->updateItemGeometry();

    QToolButton *left = nullptr;
    QToolButton *right = nullptr;
    for (QToolButton *button : page->findChildren<QToolButton *>()) {
        if (button->arrowType() == Qt::LeftArrow) {
            left = button;
        } else if (button->arrowType() == Qt::RightArrow) {
            right = button;
        }
    }
    QVERIFY(left);
    QVERIFY(right);

    RibbonGroup *first = page->group(0);

    // One notch is 120 eighths of a degree, which is 15 pixels of content: the
    // conversion lives in doWheelEvent(). Enough notches therefore reach either
    // end whatever the font makes the content.
    const auto wheelBy = [page](int notches) {
        QWheelEvent event(QPointF(10, 10), page->mapToGlobal(QPoint(10, 10)), QPoint(), QPoint(0, notches * 120),
                          Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
        QCoreApplication::sendEvent(page, &event);
    };

    // The buttons define the two ends first: the right button walks the content
    // left, and the left button walks it back.
    for (int i = 0; i < 40; ++i) {
        right->click();
    }
    const int farEnd = first->x();
    for (int i = 0; i < 40; ++i) {
        left->click();
    }
    const int homeEnd = first->x();
    QVERIFY(farEnd < homeEnd);

    // The wheel has to agree with both of them, end for end - and the clamps
    // hold, so asking for more does not walk past them. The wheel's sign is the
    // opposite of the direction the content moves in: an upward wheel walks the
    // content left, exactly as the right button does.
    wheelBy(400);
    QCOMPARE(first->x(), farEnd);
    wheelBy(400);
    QCOMPARE(first->x(), farEnd);

    wheelBy(-400);
    QCOMPARE(first->x(), homeEnd);
    wheelBy(-400);
    QCOMPARE(first->x(), homeEnd);

    // A page whose content fits has nothing to scroll to: the range collapses
    // to 0, so the wheel leaves the content exactly where it was.
    RibbonPage *wide = rb.addPage(tr("wide"));
    RibbonGroup *only = wide->addGroup(QStringLiteral("only"));
    only->addLargeAction(new QAction(QStringLiteral("only"), &rb));
    wide->resize(2000, 80);
    wide->updateItemGeometry();

    const int sittingStill = only->x();
    QWheelEvent event(QPointF(10, 10), wide->mapToGlobal(QPoint(10, 10)), QPoint(), QPoint(0, -120), Qt::NoButton,
                      Qt::NoModifier, Qt::NoScrollPhase, false);
    QCoreApplication::sendEvent(wide, &event);
    QCOMPARE(only->x(), sittingStill);
}

void tst_RibbonPage::clampAfterWidening()
{
    RibbonBar rb;

    RibbonPage *page = rb.addPage(tr("page"));
    // Content has to overflow the page by more than one page, otherwise a
    // single step cannot be told apart from the clamp at either end.
    for (int i = 0; i < 16; ++i) {
        RibbonGroup *group = page->addGroup(QStringLiteral("group%1").arg(i));
        group->addLargeAction(new QAction(QStringLiteral("action%1").arg(i), &rb));
    }
    page->resize(120, 80);
    // The page is not shown, so the resize is not what lays the items out -
    // the refresh is asked for explicitly, exactly as a resize event does it.
    page->updateItemGeometry();

    RibbonGroup *last = page->group(page->groupCount() - 1);
    QVERIFY2(
        last->geometry().right() > 3 * page->width(),
        qPrintable(
            QStringLiteral("content spans %1 in a %2 wide page").arg(last->geometry().right()).arg(page->width())));

    QToolButton *right = nullptr;
    for (QToolButton *button : page->findChildren<QToolButton *>()) {
        if (button->arrowType() == Qt::RightArrow) {
            right = button;
        }
    }
    QVERIFY(right);

    // Scroll to the right end: the offset now sits on the lower bound of its
    // legal range, which is derived from the page width at this size. The
    // content then ends inside the page, and what is left of the page to the
    // left of the first group shows that the page still overflows.
    for (int i = 0; i < 40; ++i) {
        right->click();
    }
    const int atTheEnd = last->geometry().right();
    QVERIFY2(atTheEnd < page->width(),
             qPrintable(QStringLiteral("content ends at %1 in a %2 wide page").arg(atTheEnd).arg(page->width())));
    QVERIFY2(
        page->group(0)->x() < 0,
        qPrintable(
            QStringLiteral("first group starts at %1 in a %2 wide page").arg(page->group(0)->x()).arg(page->width())));

    // Widening the page raises the lower bound above the offset that is set.
    // The page used to keep the stale offset, which left the content short of
    // the right edge by exactly the width of the resize, while the buttons
    // still reported the right end as reached - clicking one healed it, so the
    // defect was a transient gap rather than broken state.
    const int widenBy = 80;
    page->resize(page->width() + widenBy, 80);
    page->updateItemGeometry();
    const int widened = last->geometry().right();
    QVERIFY2(widened == atTheEnd + widenBy,
             qPrintable(QStringLiteral("content ends at %1, %2 short of where the resize put the page edge")
                            .arg(widened)
                            .arg(atTheEnd + widenBy - widened)));
}

/*!
 * B11: a page narrows its groups before it falls back to its scroll buttons.
 *
 * A group has two presentations. The natural one gives a large button a column of its
 * own; the reduced one packs those buttons as small ones instead, which costs rows but
 * saves columns and so makes the group narrower. A page that runs out of room reduces
 * groups from the right - the side a shrinking window eats into first - until they fit,
 * and only when the narrowest form of every group still overflows do its scroll buttons
 * appear.
 */
void tst_RibbonPage::groupShrink()
{
    RibbonBar rb;

    RibbonPage *page = rb.addPage(tr("page"));
    // Three large actions per group: laid out naturally they own a column each, while
    // the reduced presentation packs them into a single column of small ones.
    for (int i = 0; i < 6; ++i) {
        RibbonGroup *group = page->addGroup(QStringLiteral("group%1").arg(i));
        for (int j = 0; j < 3; ++j) {
            group->addLargeAction(new QAction(QStringLiteral("action%1%2").arg(i).arg(j), &rb));
        }
    }

    // The two scroll buttons belong to RibbonPagePrivate, whose class is not exported,
    // so they are collected as plain QToolButtons and told apart by the arrow only they
    // set.
    RibbonGroup *first = page->group(0);
    RibbonGroup *last = page->group(page->groupCount() - 1);
    QToolButton *left = nullptr;
    QToolButton *right = nullptr;
    for (QToolButton *button : page->findChildren<QToolButton *>()) {
        if (button->arrowType() == Qt::LeftArrow) {
            left = button;
        } else if (button->arrowType() == Qt::RightArrow) {
            right = button;
        }
    }
    QVERIFY(left);
    QVERIFY(right);

    // A page with room to spare: every group keeps its natural presentation, and the
    // page has no reason to offer its scroll buttons.
    page->resize(4000, 120);
    page->updateItemGeometry();
    for (RibbonGroup *group : page->groupList()) {
        QVERIFY(!group->isReduced());
    }
    QVERIFY(left->isHidden());
    QVERIFY(right->isHidden());
    const int naturalEnd = last->geometry().right();
    QVERIFY(naturalEnd < page->width());

    // The width one group needs in each of its presentations, read back through the
    // group rather than assumed, because a button's width depends on the font.
    const int naturalGroup = first->sizeHint().width();
    first->setReduced(true);
    const int reducedGroup = first->sizeHint().width();
    first->setReduced(false);
    QVERIFY2(reducedGroup < naturalGroup,
             qPrintable(QStringLiteral("the reduced presentation is %1 wide, the natural one %2")
                            .arg(reducedGroup)
                            .arg(naturalGroup)));
    const int reducedEnd = naturalEnd - (naturalGroup - reducedGroup) * page->groupCount();
    QVERIFY(reducedEnd < naturalEnd);

    // Walk the page down from a width that leaves room to one below the narrowest form
    // of the groups. Somewhere in between it has to shed width by narrowing some groups
    // and leaving the others alone.
    int narrowed = 0;
    for (int w = naturalEnd + 200; w > reducedEnd / 2; --w) {
        page->resize(w, 120);
        page->updateItemGeometry();
        narrowed = 0;
        for (RibbonGroup *group : page->groupList()) {
            if (group->isReduced()) {
                ++narrowed;
            }
        }
        if ((narrowed > 0) && (narrowed < page->groupCount())) {
            break;
        }
    }
    QVERIFY2((narrowed > 0) && (narrowed < page->groupCount()),
             qPrintable(QStringLiteral("%1 of %2 groups were narrowed, and no width in the range did less")
                            .arg(narrowed)
                            .arg(page->groupCount())));
    // The width comes off the right-hand groups ...
    QVERIFY(!first->isReduced());
    QVERIFY(last->isReduced());
    // ... and it is enough, so the content fits and no scroll button is offered.
    QVERIFY2(
        last->geometry().right() <= page->width(),
        qPrintable(
            QStringLiteral("content ends at %1 in a %2 wide page").arg(last->geometry().right()).arg(page->width())));
    QVERIFY(left->isHidden());
    QVERIFY(right->isHidden());

    // Narrower than the reduced groups: there is nothing left to give, so the page falls
    // back to its scroll buttons, with every group narrowed as far as it goes.
    page->resize(reducedEnd / 3, 120);
    page->updateItemGeometry();
    for (RibbonGroup *group : page->groupList()) {
        QVERIFY(group->isReduced());
    }
    QVERIFY2(
        last->geometry().right() > page->width(),
        qPrintable(
            QStringLiteral("content spans %1 in a %2 wide page").arg(last->geometry().right()).arg(page->width())));
    QVERIFY(!right->isHidden());
}

void tst_RibbonPage::clear()
{
    RibbonBar rb;

    RibbonPage *p1 = rb.addPage(tr("page1"));
    RibbonGroup *g1 = p1->addGroup(tr("group1"));
    RibbonGroup *g2 = p1->addGroup(tr("group2"));

    RibbonPage *p2 = rb.addPage(tr("page2"));
    RibbonGroup *g3 = new RibbonGroup(tr("group3"), p2);
    p2->addGroup(g3);
    RibbonGroup *g4 = new RibbonGroup(p2);
    p2->addGroup(g4);

    p2->clearGroups();

    QCOMPARE(g3->groupName(), tr("group3"));   // g3 deleteLater
    QCOMPARE(g4->groupName(), QString());

    rb.clearPages();

    // QCOMPARE(g1->groupName(), tr("group1")); // g1 is deleted when p1 is deleted
    // QCOMPARE(g3->groupName(), tr("group3")); // g3 is deleted when p2 is deleted

    Q_UNUSED(g1);
    Q_UNUSED(g2);

    p1 = nullptr;
    g1 = nullptr;
    g2 = nullptr;
    p2 = nullptr;
    g3 = nullptr;
    g4 = nullptr;
}

void tst_RibbonPage::accessibleRoles()
{
    RibbonBar rb;
    RibbonPage *page = rb.addPage(tr("&Home"));
    RibbonGroup *group = page->addGroup(tr("&Clipboard"));

    // Accessibility (B2): page and group are painted widgets with no text of their
    // own, so the ribbon registers a QAccessible factory that gives them a
    // meaningful role and mirrors each one's name.
    QAccessibleInterface *pageIf = QAccessible::queryAccessibleInterface(page);
    QVERIFY(pageIf);
    QCOMPARE(pageIf->role(), QAccessible::Pane);
    QCOMPARE(pageIf->text(QAccessible::Name), QStringLiteral("Home"));

    QAccessibleInterface *groupIf = QAccessible::queryAccessibleInterface(group);
    QVERIFY(groupIf);
    QCOMPARE(groupIf->role(), QAccessible::Grouping);
    QCOMPARE(groupIf->text(QAccessible::Name), QStringLiteral("Clipboard"));
}

TEST_ADD(tst_RibbonPage)

#include "tst_ribbonpage.moc"
