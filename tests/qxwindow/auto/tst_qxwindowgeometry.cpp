/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxwindow/windowagentwidget.h"
#include "qxwindow/windowgeometry_p.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QPoint>
#include <QSize>
#include <QWidget>
#include <QWindow>

QX_WINDOW_USE_NAMESPACE

/*!
 * The resize border is the only piece of geometry qxwindow has that does not need
 * a window manager to answer: given the size of the window and a point, which
 * edges would a press resize from, and which cursor advertises them. It used to
 * be two file-static helpers buried in the Qt backend, where the only way to
 * reach it was a QWindow with the right visibility - so it was untested.
 *
 * It now lives in windowgeometry_p.h as plain functions over a QSize, and this
 * suite is the first coverage it has had. The cases below pin down the answers
 * themselves, the half-open boundary between "client" and "border", and the
 * invariant that the cursor and the edges always agree.
 *
 * The one thing left that still needs a platform is at the bottom, gated behind
 * an environment variable because a headless run cannot honestly exercise it.
 */
class tst_QxWindowGeometry : public QObject
{
    Q_OBJECT
private slots:
    void edgesInTheMiddle();
    void edgesAlongEachSide();
    void edgesAtEachCorner();
    void edgesBoundaryIsHalfOpen();
    void edgesOnAWindowNarrowerThanTwoBorders();
    void cursorInTheMiddle();
    void cursorAlongEachSide();
    void cursorAtEachCorner();
    void cursorAgreesWithEdges();
    void borderIsAStripOfAKnownSize();
    void windowFollowsTheBorder();

    void noBorderMeansNoEdgesAnywhere();
    void noBorderMeansTheArrowEverywhere();
};

// A window comfortably wider and taller than two borders, so the four strips and
// the middle never overlap.
static const QSize kWindow(200, 100);

static int asInt(Qt::Edges edges)
{
    return static_cast<int>(edges);
}

static int countEdges(Qt::Edges edges)
{
    return (edges & Qt::LeftEdge ? 1 : 0) + (edges & Qt::RightEdge ? 1 : 0) + (edges & Qt::TopEdge ? 1 : 0) +
           (edges & Qt::BottomEdge ? 1 : 0);
}

/*!
 * The middle of the window is the client area: no edges at all, not four edges
 * with nothing set.
 */
void tst_QxWindowGeometry::edgesInTheMiddle()
{
    QCOMPARE(asInt(resizeEdgesFor(kWindow, QPoint(kWindow.width() / 2, kWindow.height() / 2))), asInt(Qt::Edges()));
}

/*!
 * Each side reports exactly its own edge, at the outer pixel and one pixel inside
 * the strip.
 */
void tst_QxWindowGeometry::edgesAlongEachSide()
{
    const int y = kWindow.height() / 2;
    const int x = kWindow.width() / 2;

    QCOMPARE(asInt(resizeEdgesFor(kWindow, QPoint(0, y))), asInt(Qt::LeftEdge));
    QCOMPARE(asInt(resizeEdgesFor(kWindow, QPoint(kResizeBorderThickness - 1, y))), asInt(Qt::LeftEdge));
    QCOMPARE(asInt(resizeEdgesFor(kWindow, QPoint(kWindow.width() - 1, y))), asInt(Qt::RightEdge));
    QCOMPARE(asInt(resizeEdgesFor(kWindow, QPoint(kWindow.width() - kResizeBorderThickness, y))), asInt(Qt::RightEdge));

    QCOMPARE(asInt(resizeEdgesFor(kWindow, QPoint(x, 0))), asInt(Qt::TopEdge));
    QCOMPARE(asInt(resizeEdgesFor(kWindow, QPoint(x, kResizeBorderThickness - 1))), asInt(Qt::TopEdge));
    QCOMPARE(asInt(resizeEdgesFor(kWindow, QPoint(x, kWindow.height() - 1))), asInt(Qt::BottomEdge));
    QCOMPARE(asInt(resizeEdgesFor(kWindow, QPoint(x, kWindow.height() - kResizeBorderThickness))),
             asInt(Qt::BottomEdge));
}

/*!
 * A corner is two edges, and which two tells you where it is.
 */
void tst_QxWindowGeometry::edgesAtEachCorner()
{
    const int w = kWindow.width();
    const int h = kWindow.height();

    QCOMPARE(asInt(resizeEdgesFor(kWindow, QPoint(0, 0))), asInt(Qt::LeftEdge | Qt::TopEdge));
    QCOMPARE(asInt(resizeEdgesFor(kWindow, QPoint(w - 1, 0))), asInt(Qt::RightEdge | Qt::TopEdge));
    QCOMPARE(asInt(resizeEdgesFor(kWindow, QPoint(0, h - 1))), asInt(Qt::LeftEdge | Qt::BottomEdge));
    QCOMPARE(asInt(resizeEdgesFor(kWindow, QPoint(w - 1, h - 1))), asInt(Qt::RightEdge | Qt::BottomEdge));
}

/*!
 * The strip is half-open, but not the same half on both sides: the left and top
 * borders are \c [0, thickness) while the right and bottom ones are
 * \c [size - thickness, size). So the far border is one pixel wider, and a point
 * exactly \c thickness in from the left is client area while the mirror-image
 * point from the right is not. This is the original behaviour; it is asserted
 * rather than "fixed" so that any change to it has to be deliberate.
 */
void tst_QxWindowGeometry::edgesBoundaryIsHalfOpen()
{
    const int y = kWindow.height() / 2;
    const int x = kWindow.width() / 2;

    // Near borders: the inner boundary belongs to the client area.
    QCOMPARE(asInt(resizeEdgesFor(kWindow, QPoint(kResizeBorderThickness, y))), asInt(Qt::Edges()));
    QCOMPARE(asInt(resizeEdgesFor(kWindow, QPoint(x, kResizeBorderThickness))), asInt(Qt::Edges()));

    // Far borders: it belongs to the border.
    QCOMPARE(asInt(resizeEdgesFor(kWindow, QPoint(kWindow.width() - kResizeBorderThickness - 1, y))),
             asInt(Qt::Edges()));
    QCOMPARE(asInt(resizeEdgesFor(kWindow, QPoint(x, kWindow.height() - kResizeBorderThickness - 1))),
             asInt(Qt::Edges()));
}

/*!
 * A window narrower than two borders has no middle: every column is inside both
 * the left and the right strip at once, so the edges are reported together. That
 * is degenerate rather than useful, but it is what the arithmetic does and the
 * cursor path above it has to keep agreeing with it.
 */
void tst_QxWindowGeometry::edgesOnAWindowNarrowerThanTwoBorders()
{
    const QSize narrow(kResizeBorderThickness * 2 - 6, 100);

    QCOMPARE(asInt(resizeEdgesFor(narrow, QPoint(5, 50))), asInt(Qt::LeftEdge | Qt::RightEdge));
    QCOMPARE(countEdges(resizeEdgesFor(narrow, QPoint(5, 50))), 2);
}

/*!
 * The middle is the arrow cursor, which is what "not resizable from here" means.
 */
void tst_QxWindowGeometry::cursorInTheMiddle()
{
    QCOMPARE(resizeCursorFor(kWindow, QPoint(kWindow.width() / 2, kWindow.height() / 2)), Qt::ArrowCursor);
}

/*!
 * A single edge gets the straight cursor for its axis, whichever side it is.
 */
void tst_QxWindowGeometry::cursorAlongEachSide()
{
    const int y = kWindow.height() / 2;
    const int x = kWindow.width() / 2;

    QCOMPARE(resizeCursorFor(kWindow, QPoint(0, y)), Qt::SizeHorCursor);
    QCOMPARE(resizeCursorFor(kWindow, QPoint(kWindow.width() - 1, y)), Qt::SizeHorCursor);
    QCOMPARE(resizeCursorFor(kWindow, QPoint(x, 0)), Qt::SizeVerCursor);
    QCOMPARE(resizeCursorFor(kWindow, QPoint(x, kWindow.height() - 1)), Qt::SizeVerCursor);
}

/*!
 * The corners get the diagonal cursors, and the two diagonals are not symmetric:
 * top-left and bottom-right are the "forward" one, the other two are the
 * "backward" one. Easy to get backwards, so it is pinned here.
 */
void tst_QxWindowGeometry::cursorAtEachCorner()
{
    const int w = kWindow.width();
    const int h = kWindow.height();

    QCOMPARE(resizeCursorFor(kWindow, QPoint(0, 0)), Qt::SizeFDiagCursor);
    QCOMPARE(resizeCursorFor(kWindow, QPoint(w - 1, h - 1)), Qt::SizeFDiagCursor);
    QCOMPARE(resizeCursorFor(kWindow, QPoint(w - 1, 0)), Qt::SizeBDiagCursor);
    QCOMPARE(resizeCursorFor(kWindow, QPoint(0, h - 1)), Qt::SizeBDiagCursor);
}

/*!
 * The point of splitting the two functions out rather than testing one of them:
 * they have to agree everywhere, or the cursor would advertise a resize the press
 * would not start. Sweeping the whole window costs nothing and catches any future
 * edit that touches one of the two comparison chains but not the other.
 */
void tst_QxWindowGeometry::cursorAgreesWithEdges()
{
    for (int y = 0; y < kWindow.height(); ++y) {
        for (int x = 0; x < kWindow.width(); ++x) {
            const QPoint pos(x, y);
            const Qt::Edges edges = resizeEdgesFor(kWindow, pos);
            const Qt::CursorShape shape = resizeCursorFor(kWindow, pos);
            const int count = countEdges(edges);

            if (count == 0) {
                QCOMPARE(shape, Qt::ArrowCursor);
                continue;
            }
            QVERIFY2(shape != Qt::ArrowCursor, qPrintable(QStringLiteral("no cursor at %1,%2").arg(x).arg(y)));

            switch (count) {
            case 1:
                if (edges & (Qt::LeftEdge | Qt::RightEdge)) {
                    QCOMPARE(shape, Qt::SizeHorCursor);
                } else {
                    QCOMPARE(shape, Qt::SizeVerCursor);
                }
                break;
            case 2:
                if (edges == (Qt::LeftEdge | Qt::TopEdge) || edges == (Qt::RightEdge | Qt::BottomEdge)) {
                    QCOMPARE(shape, Qt::SizeFDiagCursor);
                } else {
                    QCOMPARE(shape, Qt::SizeBDiagCursor);
                }
                break;
            default:
                QFAIL("a window wider than two borders has no four-edge point");
            }
        }
    }
}

/*!
 * The area of the border strip, so that the four comparisons are shown to carve
 * out the shape they are supposed to and not something that merely looks right at
 * the sample points above. The middle is a rectangle of
 * \c (width - 2 * thickness) by \c (height - 2 * thickness).
 */
void tst_QxWindowGeometry::borderIsAStripOfAKnownSize()
{
    int onTheBorder = 0;
    for (int y = 0; y < kWindow.height(); ++y) {
        for (int x = 0; x < kWindow.width(); ++x) {
            if (resizeEdgesFor(kWindow, QPoint(x, y)) != Qt::Edges()) {
                ++onTheBorder;
            }
        }
    }

    const int middle = (kWindow.width() - 2 * kResizeBorderThickness) * (kWindow.height() - 2 * kResizeBorderThickness);
    QCOMPARE(onTheBorder, kWindow.width() * kWindow.height() - middle);
}

/*!
 * With the border out of the hit test the same sweep has to come up empty: not
 * fewer edges, none. Sweeping rather than sampling is what makes this worth
 * having - a flag that only reached some of the four comparisons would still
 * answer correctly at a handful of hand-picked points.
 */
void tst_QxWindowGeometry::noBorderMeansNoEdgesAnywhere()
{
    for (int y = 0; y < kWindow.height(); ++y) {
        for (int x = 0; x < kWindow.width(); ++x) {
            QCOMPARE(asInt(resizeEdgesFor(kWindow, QPoint(x, y), false)), asInt(Qt::Edges()));
        }
    }
}

/*!
 * The cursor half of the same statement, and it has to sweep too: a resize cursor
 * left behind on one strip would advertise a resize the press would not start.
 */
void tst_QxWindowGeometry::noBorderMeansTheArrowEverywhere()
{
    for (int y = 0; y < kWindow.height(); ++y) {
        for (int x = 0; x < kWindow.width(); ++x) {
            QCOMPARE(resizeCursorFor(kWindow, QPoint(x, y), false), Qt::ArrowCursor);
        }
    }
}

/*!
 * The geometry above is what the Qt backend feeds into the resize path, and this
 * is the one case that goes through the whole chain: a real agent, a real window,
 * a synthetic mouse move, and the cursor the agent ends up setting on the widget.
 *
 * It skips rather than fails when the environment cannot support it, because two
 * things have to hold and neither is the caller's business:
 *
 *  - The Qt backend has to be the one in charge. Windows and macOS default to a
 *    native backend that does its hit testing in a window proc, so the shared
 *    event filter this case drives is not installed there at all. The platform
 *    name does not answer this - CI runs every leg under the offscreen platform -
 *    so the answer is taken from a difference the two backends already have: the
 *    Qt backend asks Qt for a frameless window (WindowContextQt::winIdChanged()),
 *    the native ones reach around Qt and never touch the window flags.
 *  - The window has to come up, which a headless platform may refuse.
 *
 * Where the Qt backend is in charge and the window is mapped - the ubuntu CI legs,
 * for instance - the case runs for real. And where it cannot be excused, it fails
 * rather than skips: see the comment on the mapping check below.
 */
void tst_QxWindowGeometry::windowFollowsTheBorder()
{
    QWidget window;
    window.resize(200, 100);

    WindowAgentWidget agent(&window);
    QVERIFY(agent.setup(&window));

    // setup() asks for a native window, so the handle exists before the window is
    // shown - and before the backend has had a chance to set the flag below.
    QVERIFY(window.windowHandle());

    // The only acceptable reason to skip the rest is that this platform may put a
    // native backend in charge, which is a property of the platform, not of the
    // run. On a platform that has only the Qt backend, a skip would silently take
    // away the only coverage this case has, so there the checks below are failures.
#if defined(Q_OS_WINDOWS) || defined(Q_OS_MACOS)
    const bool theBackendMayBeNative = true;
#else
    const bool theBackendMayBeNative = false;
#endif

    if (theBackendMayBeNative && !(window.windowFlags() & Qt::FramelessWindowHint)) {
        QSKIP("the native backend is in charge here, so the shared filter this case drives is not installed");
    }

#ifdef Q_OS_MACOS
    QSKIP("the macOS build of the Qt backend answers ArrowCursor by design");
#endif

    // The geometry only applies once the window is in the windowed state.
    window.show();
    if (!QTest::qWaitForWindowExposed(&window) || window.windowHandle()->visibility() != QWindow::Windowed) {
        QVERIFY2(theBackendMayBeNative, "the window did not come up although the Qt backend is the only one here");
        QSKIP("the window could not be mapped here");
    }

    auto moveTo = [&window](const QPoint &pos) {
        const QPointF scenePos(pos);
        // The third position is what getMouseEventScenePos() reads; the global one
        // is only used by the context menu path, which this does not touch.
        QMouseEvent event(QEvent::MouseMove, scenePos, scenePos, scenePos, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
        QCoreApplication::sendEvent(window.windowHandle(), &event);
    };

    moveTo(QPoint(2, 2));
    QCOMPARE(window.cursor().shape(), Qt::SizeFDiagCursor);

    moveTo(QPoint(2, kWindow.height() / 2));
    QCOMPARE(window.cursor().shape(), Qt::SizeHorCursor);

    moveTo(QPoint(kWindow.width() / 2, kWindow.height() / 2));
    QCOMPARE(window.cursor().shape(), Qt::ArrowCursor);
}

TEST_ADD(tst_QxWindowGeometry)

#include "tst_qxwindowgeometry.moc"
