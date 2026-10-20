/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxwindow/windowhitregion_p.h"

#include <QList>
#include <QPoint>
#include <QRect>
#include <QStringList>

QX_WINDOW_USE_NAMESPACE

/*!
 * Every hit test in qxwindow passes through two questions: is this point inside
 * one of the system buttons, and is it inside the title bar's draggable area.
 * windowcontext_win.cpp asks them on every WM_NCHITTEST, so they decide whether
 * a press resizes the window, drags it, or reaches the widget underneath.
 *
 * They used to live in windowcontext.cpp, which put them out of reach of a test
 * for two reasons at once: the file needs a native window to drive, and its
 * symbols are not exported from a Windows DLL. windowhitregion_p.h is the
 * geometry pulled out of that file - plain functions over rectangles - which is
 * what makes these cases runnable on any platform.
 *
 * The layout every case is written against, from windowcontext.cpp's own order:
 *
 *   (0,0)+--------------------------------------------------+
 *        |[icon][min][max][close]      title bar body       |
 *        |  0..120                   120..400              |
 *        |          buttons are 30x20, title bar is y=0..20  |
 *        |                                                    |
 *        |                   client area                      |
 *        +----------------------------------------------------+
 *
 * with a 400x300 window.
 */
class tst_QxWindowHitRegion : public QObject
{
    Q_OBJECT
private:
    Q_SLOT void systemButtonAtFindsTheButtonUnderThePoint();
    Q_SLOT void systemButtonAtReportsTheIndex();
    Q_SLOT void systemButtonAtPrefersTheEarlierButtonWhenOverlaps();
    Q_SLOT void systemButtonAtSkipsHiddenEntries();
    Q_SLOT void systemButtonAtRejectsAPointOutsideEveryRectangle();
    Q_SLOT void systemButtonAtReportsMinusOneWhenNothingIsFound();
    Q_SLOT void systemButtonAtIgnoresTrailingInvisibleEntries();

    Q_SLOT void draggableAreaIsTheTitleBarMinusItsButtons();
    Q_SLOT void draggableAreaIsFalseWhenTheTitleBarIsOutsideTheWindow();
    Q_SLOT void draggableAreaIsFalseForAPointOutsideTheTitleBar();
    Q_SLOT void draggableAreaExcludesHitTestVisibleItems();
    Q_SLOT void draggableAreaSkipsInvisibleHitTestItems();
    Q_SLOT void draggableAreaExcludesNonCaptionChildren();
    Q_SLOT void draggableAreaAcceptsNominatedCaptionChildren();

    Q_SLOT void captionClassNamesMatchExactly();
    Q_SLOT void captionClassNamesAreFalseForAnEmptyList();

    Q_SLOT void edgeEdgesOfAColourlessWindow();
    Q_SLOT void edgeCornersWinOverEdges();
    Q_SLOT void edgeLeftAndRightStripsAreWidened();
    Q_SLOT void edgeTitleBarIsCaptionOnlyAwayFromTheBorders();
    Q_SLOT void edgeFixedSizeReportsBorderInsteadOfAnEdge();
    Q_SLOT void edgeFixedSizeStillFindsTheTitleBar();

    Q_SLOT void borderWithoutTheCursorIsTheAreaItSitsIn();
    Q_SLOT void borderWithoutTheCursorLeavesTheTitleBarAlone();
    Q_SLOT void borderWithoutTheCursorIsNotAFlagOnFixedSizeOnly();
};

namespace
{
constexpr int kButtonWidth = 30;
constexpr int kButtonHeight = 20;
constexpr int kStripHeight = 20;
constexpr int kHostWidth = 400;
constexpr int kHostHeight = 300;

/*!
 * The four buttons across the top-left of the strip, and the title bar that spans
 * the whole width. Returned by value because each case mutates one of them and
 * they must not share state.
 */
struct Layout {
    QRect windowRect = QRect(0, 0, kHostWidth, kHostHeight);
    QRect titleBarRect = QRect(0, 0, kHostWidth, kStripHeight);
    QList<QRect> buttons;
    QList<bool> visible;

    Layout()
    {
        for (int i = 0; i < 4; ++i) {
            buttons.append(QRect(i * kButtonWidth, 0, kButtonWidth, kButtonHeight));
            visible.append(true);
        }
    }

    /*! The centre of button \a index, which is where a press would land. */
    static QPoint centreOfButton(int index)
    {
        return QRect(index * kButtonWidth, 0, kButtonWidth, kButtonHeight).center();
    }

    /*! A point in the title bar body, to the right of every button. */
    static QPoint inTitleBarBody()
    {
        return QPoint(200, 10);
    }

    /*! A point below the strip: client area. */
    static QPoint inClientArea()
    {
        return QPoint(200, 100);
    }
};
}   // namespace

void tst_QxWindowHitRegion::systemButtonAtFindsTheButtonUnderThePoint()
{
    Layout layout;
    const int index = systemButtonAt(Layout::centreOfButton(3), layout.buttons, layout.visible);
    QVERIFY2(index >= 0, "a point in the middle of a registered button should be inside it");
}

void tst_QxWindowHitRegion::systemButtonAtReportsTheIndex()
{
    Layout layout;
    // Button 1 sits at x=30..60, so its centre at x=45 is inside it and well clear
    // of button 3 at x=90..120. This pins that the right index comes back, not
    // merely that some rectangle was found.
    QCOMPARE(systemButtonAt(Layout::centreOfButton(1), layout.buttons, layout.visible), 1);
    QCOMPARE(systemButtonAt(Layout::centreOfButton(3), layout.buttons, layout.visible), 3);
}

void tst_QxWindowHitRegion::systemButtonAtPrefersTheEarlierButtonWhenOverlaps()
{
    Layout layout;
    // Two buttons over the same point: windowcontext.cpp returns the first one it
    // finds while walking the system buttons in order, so the earlier index wins.
    layout.buttons[0] = QRect(0, 0, kButtonWidth, kButtonHeight);
    layout.buttons[1] = QRect(0, 0, kButtonWidth, kButtonHeight);
    QCOMPARE(systemButtonAt(QPoint(15, 10), layout.buttons, layout.visible), 0);
}

void tst_QxWindowHitRegion::systemButtonAtSkipsHiddenEntries()
{
    Layout layout;
    layout.visible[3] = false;

    // A hidden button must not claim the point, or a click meant for whatever sits
    // underneath it would be swallowed by a button nobody can see.
    QVERIFY2(systemButtonAt(Layout::centreOfButton(3), layout.buttons, layout.visible) < 0,
             "a hidden button must not report itself as hit");
    // The others still work, so the check is per-entry and not global.
    QCOMPARE(systemButtonAt(Layout::centreOfButton(2), layout.buttons, layout.visible), 2);
}

void tst_QxWindowHitRegion::systemButtonAtRejectsAPointOutsideEveryRectangle()
{
    Layout layout;
    // Inside the title bar but to the right of every button.
    QVERIFY(systemButtonAt(Layout::inTitleBarBody(), layout.buttons, layout.visible) < 0);
}

void tst_QxWindowHitRegion::systemButtonAtReportsMinusOneWhenNothingIsFound()
{
    Layout layout;
    QCOMPARE(systemButtonAt(QPoint(-1, -1), layout.buttons, layout.visible), -1);
}

void tst_QxWindowHitRegion::systemButtonAtIgnoresTrailingInvisibleEntries()
{
    // The visibility list is allowed to be shorter than the geometry list, which
    // happens for the buttons a backend never populated. Reading past the end of
    // the short list would be a crash rather than a missed hit.
    Layout layout;
    QList<bool> shortVisibility = {true, true};
    QCOMPARE(systemButtonAt(Layout::centreOfButton(1), layout.buttons, shortVisibility), 1);
    QVERIFY(systemButtonAt(Layout::centreOfButton(3), layout.buttons, shortVisibility) < 0);
}

void tst_QxWindowHitRegion::draggableAreaIsTheTitleBarMinusItsButtons()
{
    Layout layout;
    const QList<QRect> noItems;
    const QList<bool> nothing;

    QVERIFY2(isInTitleBarDraggableArea(Layout::inTitleBarBody(), layout.windowRect, layout.titleBarRect, layout.buttons,
                                       layout.visible, noItems, nothing, {}, {}),
             "the empty part of the title bar should be draggable");

    // On a system button: the button outranks the title bar, otherwise a press
    // there would start a window move instead of a click.
    QVERIFY2(!isInTitleBarDraggableArea(Layout::centreOfButton(3), layout.windowRect, layout.titleBarRect,
                                        layout.buttons, layout.visible, noItems, nothing, {}, {}),
             "a point on a system button must not be draggable");

    // Below the strip is client area.
    QVERIFY(!isInTitleBarDraggableArea(Layout::inClientArea(), layout.windowRect, layout.titleBarRect, layout.buttons,
                                       layout.visible, noItems, nothing, {}, {}));
}

void tst_QxWindowHitRegion::draggableAreaIsFalseWhenTheTitleBarIsOutsideTheWindow()
{
    Layout layout;
    const QList<QRect> noItems;
    const QList<bool> nothing;
    // Park the title bar well away from the window. windowcontext.cpp checks for
    // this explicitly and treats it as no title bar at all.
    const QRect outside(5000, 5000, 100, kStripHeight);

    QVERIFY2(!isInTitleBarDraggableArea(QPoint(5010, 5010), layout.windowRect, outside, layout.buttons, layout.visible,
                                        noItems, nothing, {}, {}),
             "a title bar entirely outside the window should behave as if there were none");
}

void tst_QxWindowHitRegion::draggableAreaIsFalseForAPointOutsideTheTitleBar()
{
    Layout layout;
    const QList<QRect> noItems;
    const QList<bool> nothing;
    // A point inside the window but below the strip.
    QVERIFY(!isInTitleBarDraggableArea(Layout::inClientArea(), layout.windowRect, layout.titleBarRect, layout.buttons,
                                       layout.visible, noItems, nothing, {}, {}));
}

void tst_QxWindowHitRegion::draggableAreaExcludesHitTestVisibleItems()
{
    Layout layout;
    const QList<bool> nothing;
    // A control the caller marked hit-testable: a point over it belongs to that
    // control, not to the title bar, so it must not start a window drag.
    const QRect control(200, 0, 60, kStripHeight);
    const QList<QRect> items = {control};

    QVERIFY2(!isInTitleBarDraggableArea(QPoint(220, 10), layout.windowRect, layout.titleBarRect, layout.buttons,
                                        layout.visible, items, {true}, {}, {}),
             "a point over a hit-test-visible item must not be draggable");
    // A point in the title bar but clear of the control is still draggable.
    QVERIFY(isInTitleBarDraggableArea(QPoint(320, 10), layout.windowRect, layout.titleBarRect, layout.buttons,
                                      layout.visible, items, {true}, {}, {}));
}

void tst_QxWindowHitRegion::draggableAreaSkipsInvisibleHitTestItems()
{
    Layout layout;
    const QList<QRect> items = {QRect(200, 0, 60, kStripHeight)};

    QVERIFY2(isInTitleBarDraggableArea(QPoint(220, 10), layout.windowRect, layout.titleBarRect, layout.buttons,
                                       layout.visible, items, {false}, {}, {}),
             "an invisible hit-test item must not take the point away from the drag area");
}

void tst_QxWindowHitRegion::draggableAreaExcludesNonCaptionChildren()
{
    Layout layout;
    const QList<QRect> noItems;
    const QList<bool> nothing;
    const QStringList names = {QStringLiteral("QLabel")};
    // A child of the title bar that was not nominated claims the point, or it
    // would be impossible to press at all - the title bar would start a drag.
    QVERIFY2(!isInTitleBarDraggableArea(Layout::inTitleBarBody(), layout.windowRect, layout.titleBarRect,
                                        layout.buttons, layout.visible, noItems, nothing, QStringLiteral("QFrame"),
                                        names),
             "a non-nominated child of the title bar should claim the point");
}

void tst_QxWindowHitRegion::draggableAreaAcceptsNominatedCaptionChildren()
{
    Layout layout;
    const QList<QRect> noItems;
    const QList<bool> nothing;
    const QStringList names = {QStringLiteral("QLabel")};
    // Nominating the child's class hands its points back to the drag area, which
    // is what lets a ribbon title row stay draggable.
    QVERIFY2(isInTitleBarDraggableArea(Layout::inTitleBarBody(), layout.windowRect, layout.titleBarRect, layout.buttons,
                                       layout.visible, noItems, nothing, QStringLiteral("QLabel"), names),
             "a nominated caption child should not take the point away from the drag area");

    // With no nomination at all the same point is draggable regardless of the
    // child's class - an empty list means "no rule".
    QVERIFY(isInTitleBarDraggableArea(Layout::inTitleBarBody(), layout.windowRect, layout.titleBarRect, layout.buttons,
                                      layout.visible, noItems, nothing, QStringLiteral("QFrame"), QStringList()));
}

void tst_QxWindowHitRegion::captionClassNamesMatchExactly()
{
    const QStringList names = {QStringLiteral("QLabel"), QStringLiteral("QFrame")};

    QVERIFY(isCaptionClassName("QLabel", names));
    QVERIFY(isCaptionClassName("QFrame", names));

    // Exact match only: a name that merely starts the same must not match, or
    // nominating "QLabel" would silently capture every QLabel subclass too.
    QVERIFY2(!isCaptionClassName("QLabelExtra", names), "a name that only shares a prefix must not match");
    QVERIFY(!isCaptionClassName("qlabel", names));
    QVERIFY(!isCaptionClassName("QLabel2", names));
}

void tst_QxWindowHitRegion::captionClassNamesAreFalseForAnEmptyList()
{
    // An empty list is "no rule", and a name is never a caption by accident.
    QVERIFY(!isCaptionClassName("QLabel", {}));
}

/*!
 * The edge arithmetic, which is what windowcontext_win.cpp is mostly for.
 *
 * The frame size in every case is 8 - getResizeBorderThickness() reports that on
 * a standard Windows setup - against a 400x300 client area. The top and bottom
 * strips are 8px, the left and right 16px: the sideways strips are doubled so
 * that the corners are easy to grab, and the comment in windowcontext_win.cpp says
 * exactly that. It is the edges a corner is found from that get wider, not the
 * corner test itself.
 */
namespace
{
constexpr int kFrame = 8;
const QSize kClient(400, 300);

HitTestRegion regionAt(const QPoint &pos, bool titleBar = false, bool fixed = false, int frame = kFrame,
                       bool borderOwnsCursor = true)
{
    return hitTestRegionFor(kClient, pos, frame, titleBar, fixed, borderOwnsCursor);
}
}   // namespace

void tst_QxWindowHitRegion::edgeEdgesOfAColourlessWindow()
{
    QCOMPARE(regionAt(QPoint(200, 150)), HitTestRegion::Client);
    QCOMPARE(regionAt(QPoint(0, 150)), HitTestRegion::Left);
    QCOMPARE(regionAt(QPoint(399, 150)), HitTestRegion::Right);
    QCOMPARE(regionAt(QPoint(200, 0)), HitTestRegion::Top);
    QCOMPARE(regionAt(QPoint(200, 299)), HitTestRegion::Bottom);

    // The left strip is doubled (16px) while the top is not (8px), so y=100 is far
    // from any horizontal edge and only the left strip can match there.
    QCOMPARE(regionAt(QPoint(3, 100)), HitTestRegion::Left);
    QCOMPARE(regionAt(QPoint(15, 100)), HitTestRegion::Left);
    QCOMPARE(regionAt(QPoint(16, 100)), HitTestRegion::Client);
}

void tst_QxWindowHitRegion::edgeCornersWinOverEdges()
{
    QCOMPARE(regionAt(QPoint(0, 0)), HitTestRegion::TopLeft);
    QCOMPARE(regionAt(QPoint(399, 0)), HitTestRegion::TopRight);
    QCOMPARE(regionAt(QPoint(0, 299)), HitTestRegion::BottomLeft);
    QCOMPARE(regionAt(QPoint(399, 299)), HitTestRegion::BottomRight);

    // A corner must report a corner, not whichever edge happened to be tested
    // first - this is the order the original chain checks them in.
    QCOMPARE(regionAt(QPoint(2, 2)), HitTestRegion::TopLeft);
}

void tst_QxWindowHitRegion::edgeLeftAndRightStripsAreWidened()
{
    // The top strip is the plain frame size, 8px: y=7 is on the top edge and y=8
    // is not.
    QCOMPARE(regionAt(QPoint(200, 7)), HitTestRegion::Top);
    QCOMPARE(regionAt(QPoint(200, 8)), HitTestRegion::Client);

    // The left and right strips are twice that, 16px, which is what makes the
    // corners easy to grab - so x=15 is still on the left edge.
    QCOMPARE(regionAt(QPoint(15, 100)), HitTestRegion::Left);
    QCOMPARE(regionAt(QPoint(16, 100)), HitTestRegion::Client);

    // The bottom strip is plain too, starting at 300-8 = 292.
    QCOMPARE(regionAt(QPoint(200, 291)), HitTestRegion::Client);
    QCOMPARE(regionAt(QPoint(200, 292)), HitTestRegion::Bottom);
}

void tst_QxWindowHitRegion::edgeTitleBarIsCaptionOnlyAwayFromTheBorders()
{
    // In the middle of a title bar: Caption, which is what makes a press drag the
    // window rather than reach whatever is underneath.
    QCOMPARE(regionAt(QPoint(200, 10), true), HitTestRegion::Caption);
    // Not on the title bar: Client.
    QCOMPARE(regionAt(QPoint(200, 10), false), HitTestRegion::Client);
    // A border outranks the title bar even where both apply - otherwise a press on
    // the corner would drag instead of resizing.
    QCOMPARE(regionAt(QPoint(0, 0), true), HitTestRegion::TopLeft);
}

void tst_QxWindowHitRegion::edgeFixedSizeReportsBorderInsteadOfAnEdge()
{
    // A fixed-size window keeps its edges, but they are Border: the backend
    // answers HTBORDER there, so dragging the edge cannot resize it.
    QCOMPARE(regionAt(QPoint(0, 150), false, true), HitTestRegion::Border);
    QCOMPARE(regionAt(QPoint(200, 0), false, true), HitTestRegion::Border);
    QCOMPARE(regionAt(QPoint(399, 299), false, true), HitTestRegion::Border);

    // Corners collapse into Border too, since the window never resizes.
    QCOMPARE(regionAt(QPoint(0, 0), false, true), HitTestRegion::Border);
}

void tst_QxWindowHitRegion::edgeFixedSizeStillFindsTheTitleBar()
{
    // The title bar is not a border, so a fixed-size window still reports Caption
    // there - a fixed-size window can be dragged by its title bar.
    QCOMPARE(regionAt(QPoint(200, 10), true, true), HitTestRegion::Caption);
    QCOMPARE(regionAt(QPoint(200, 10), false, true), HitTestRegion::Client);
}

/*!
 * With the border out of the hit test, every point on it answers as the area it
 * sits in rather than as an edge or a corner: the press reaches whatever is drawn
 * there instead of starting a resize. The four corners matter most - they are the
 * points where a border wins twice over, from the widened side strip and from the
 * top strip, and both of those have to stop answering.
 */
void tst_QxWindowHitRegion::borderWithoutTheCursorIsTheAreaItSitsIn()
{
    QCOMPARE(regionAt(QPoint(0, 150), false, false, kFrame, false), HitTestRegion::Client);
    QCOMPARE(regionAt(QPoint(399, 150), false, false, kFrame, false), HitTestRegion::Client);
    QCOMPARE(regionAt(QPoint(200, 0), false, false, kFrame, false), HitTestRegion::Client);
    QCOMPARE(regionAt(QPoint(200, 299), false, false, kFrame, false), HitTestRegion::Client);

    // Corners, which are the whole reason the side strips are widened.
    QCOMPARE(regionAt(QPoint(0, 0), false, false, kFrame, false), HitTestRegion::Client);
    QCOMPARE(regionAt(QPoint(399, 0), false, false, kFrame, false), HitTestRegion::Client);
    QCOMPARE(regionAt(QPoint(0, 299), false, false, kFrame, false), HitTestRegion::Client);
    QCOMPARE(regionAt(QPoint(399, 299), false, false, kFrame, false), HitTestRegion::Client);

    // The middle never was a border, so it says the same thing either way.
    QCOMPARE(regionAt(QPoint(200, 150), false, false, kFrame, false), HitTestRegion::Client);
}

/*!
 * Turning the border off must not take the title bar with it: dragging by the
 * title bar is the one way such a window is meant to move, and it is not the
 * border being switched off.
 */
void tst_QxWindowHitRegion::borderWithoutTheCursorLeavesTheTitleBarAlone()
{
    // Well inside the title bar and away from any border.
    QCOMPARE(regionAt(QPoint(200, 10), true, false, kFrame, false), HitTestRegion::Caption);
    QCOMPARE(regionAt(QPoint(200, 10), false, false, kFrame, false), HitTestRegion::Client);

    // Where the title bar and a border overlap the border used to win; now the
    // title bar's own answer is what is left.
    QCOMPARE(regionAt(QPoint(0, 0), true, false, kFrame, false), HitTestRegion::Caption);
    QCOMPARE(regionAt(QPoint(399, 299), true, false, kFrame, false), HitTestRegion::Caption);
}

/*!
 * The flag has to hold for a fixed-size window too. Its border answers Border,
 * which is not a resize code but still keeps the press away from what is drawn
 * underneath - so a flag honoured only in the resizable branch would leave the
 * window swallowing presses along its edges all the same.
 */
void tst_QxWindowHitRegion::borderWithoutTheCursorIsNotAFlagOnFixedSizeOnly()
{
    QCOMPARE(regionAt(QPoint(0, 150), false, true, kFrame, false), HitTestRegion::Client);
    QCOMPARE(regionAt(QPoint(200, 0), false, true, kFrame, false), HitTestRegion::Client);
    QCOMPARE(regionAt(QPoint(0, 0), false, true, kFrame, false), HitTestRegion::Client);

    // And the title bar survives there as well.
    QCOMPARE(regionAt(QPoint(200, 10), true, true, kFrame, false), HitTestRegion::Caption);
}

TEST_ADD(tst_QxWindowHitRegion)

#include "tst_qxwindowhittest.moc"