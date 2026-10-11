/*!
 * \file
 *
 * Tests for the conversion and comparison helpers in windowkit_win_p.h.
 *
 * The whole header is Windows-only - it includes <dwmapi.h> and
 * <shellscalingapi.h> - so every case here is behind Q_OS_WIN. The rest of
 * tst_qxwindow runs everywhere unchanged; CI's windows-latest leg is what
 * actually executes this file.
 *
 * What is worth testing here is not the arithmetic but the shape. These
 * helpers sit between Win32 structs that count edges differently from the Qt
 * equivalents, and between structs whose fields are declared in different
 * orders. Nothing about either is visible to the compiler, and every caller
 * that gets it backwards produces a plausible wrong answer rather than a
 * warning. See doc/design/4.3-TASKS.md, A6.
 */
#include "tst_global.h"

#if defined(Q_OS_WIN)
#include "qxwindow/windowkit_win_p.h"

#include <QtCore/QMargins>
#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QtCore/QSize>
#endif

class tst_QxWindowWinKit : public QObject
{
    Q_OBJECT

private slots:
    void windowsOnlyHelpers();

#if defined(Q_OS_WIN)
    // POINT
    void pointEqualityComparesBothAxes();
    void pointEqualityIsExact();

    // SIZE
    void sizeEqualityComparesBothAxes();
    void sizeOrderingComparesAreaWhileEqualityComparesFields();

    // RECT
    void rectEqualityComparesAllFourEdges();

    // conversions
    void pointConversionRoundTrips();
    void sizeConversionRoundTrips();
    void rectConversionCountsEdgesTheWayEachSideDoes();
    void rectRoundTripThroughBothConversionsLosesAPixel();
    void marginsConversionReordersRatherThanCopies();
    void marginsRoundTripPreservesEverySide();
    void negativeAndLargeValuesSurviveConversion();

    void hwndStringPadsToEightDigits();
    void hwndStringUppercasesHexDigits();
    void nullHwndStringifiesToAllZeros();
#endif
};

/*!
 * One case is always registered, so the suite has the same shape everywhere.
 * Everywhere but Windows there is nothing to say about a Windows header, and
 * saying so is more useful than the target vanishing from ctest output.
 */
void tst_QxWindowWinKit::windowsOnlyHelpers()
{
#if !defined(Q_OS_WIN)
    QSKIP("windowkit_win_p.h is Windows-only");
#endif
}

#if defined(Q_OS_WIN)

void tst_QxWindowWinKit::pointEqualityComparesBothAxes()
{
    QVERIFY(QxWindow::operator==(POINT{3, 4}, POINT{3, 4}));
    QVERIFY(QxWindow::operator!=(POINT{3, 4}, POINT{3, 5}));
    QVERIFY(QxWindow::operator!=(POINT{3, 4}, POINT{4, 4}));
}

void tst_QxWindowWinKit::pointEqualityIsExact()
{
    // Not a tolerance comparison: one unit of difference in either axis is a
    // difference. Sizes here are pixels, and a fractional drift is the sort of
    // thing that would otherwise accumulate along a resize.
    QVERIFY(!QxWindow::operator==(POINT{10, 10}, POINT{10, 11}));
    QVERIFY(!QxWindow::operator==(POINT{-1, 0}, POINT{0, 0}));
    QVERIFY(QxWindow::operator!=(POINT{10, 10}, POINT{10, 11}));
}

void tst_QxWindowWinKit::sizeEqualityComparesBothAxes()
{
    QVERIFY(QxWindow::operator==(SIZE{4, 5}, SIZE{4, 5}));
    QVERIFY(QxWindow::operator!=(SIZE{4, 5}, SIZE{4, 6}));
    QVERIFY(QxWindow::operator!=(SIZE{4, 5}, SIZE{5, 5}));
}

void tst_QxWindowWinKit::sizeOrderingComparesAreaWhileEqualityComparesFields()
{
    // This is the one that reads wrong until you look at it. The ordering
    // operators multiply the two sides rather than comparing width and height,
    // while operator== compares field by field. The two do not agree.
    QVERIFY(QxWindow::operator>(SIZE{10, 1}, SIZE{3, 3}));    // 10 > 9
    QVERIFY(QxWindow::operator>(SIZE{1, 10}, SIZE{3, 3}));    // 10 > 9
    QVERIFY(QxWindow::operator>(SIZE{4, 4}, SIZE{3, 3}));     // 16 > 9
    QVERIFY(!QxWindow::operator>(SIZE{3, 3}, SIZE{10, 1}));   // 9 < 10
    QVERIFY(QxWindow::operator<(SIZE{3, 3}, SIZE{10, 1}));

    // Same area, different shape. Not equal (the fields differ) and not
    // greater (the areas are equal) - which leaves "<" true in both
    // directions and ">=" false in both directions. That is the behaviour the
    // implementation has, and it is pinned here rather than smoothed over: the
    // four operators are not a consistent total order for equal areas.
    QVERIFY(QxWindow::operator!=(SIZE{10, 1}, SIZE{5, 2}));
    QVERIFY(!QxWindow::operator>(SIZE{10, 1}, SIZE{5, 2}));
    QVERIFY(!QxWindow::operator>=(SIZE{10, 1}, SIZE{5, 2}));

    QVERIFY(QxWindow::operator<(SIZE{10, 1}, SIZE{5, 2}));
    QVERIFY(QxWindow::operator<=(SIZE{10, 1}, SIZE{5, 2}));
    QVERIFY(QxWindow::operator<(SIZE{5, 2}, SIZE{10, 1}));
    QVERIFY(QxWindow::operator<=(SIZE{5, 2}, SIZE{10, 1}));

    // Comparing an area against itself, where fields and area agree, is the
    // only combination where the whole set behaves.
    QVERIFY(QxWindow::operator==(SIZE{4, 5}, SIZE{4, 5}));
    QVERIFY(QxWindow::operator>=(SIZE{4, 5}, SIZE{4, 5}));
    QVERIFY(QxWindow::operator<=(SIZE{4, 5}, SIZE{4, 5}));
    QVERIFY(!QxWindow::operator>(SIZE{4, 5}, SIZE{4, 5}));
    QVERIFY(!QxWindow::operator<(SIZE{4, 5}, SIZE{4, 5}));
}

void tst_QxWindowWinKit::rectEqualityComparesAllFourEdges()
{
    QVERIFY(QxWindow::operator==(RECT{1, 2, 3, 4}, RECT{1, 2, 3, 4}));

    // Every edge is load-bearing: a rectangle is not identified by its size.
    QVERIFY(QxWindow::operator!=(RECT{1, 2, 3, 4}, RECT{0, 2, 3, 4}));
    QVERIFY(QxWindow::operator!=(RECT{1, 2, 3, 4}, RECT{1, 0, 3, 4}));
    QVERIFY(QxWindow::operator!=(RECT{1, 2, 3, 4}, RECT{1, 2, 4, 4}));
    QVERIFY(QxWindow::operator!=(RECT{1, 2, 3, 4}, RECT{1, 2, 3, 5}));
}

void tst_QxWindowWinKit::pointConversionRoundTrips()
{
    // Compared field by field rather than with QCOMPARE: the helper's own
    // operator== lives in the QxWindow namespace, which argument-dependent
    // lookup does not reach from a template, so QCOMPARE cannot use it.
    QCOMPARE(QxWindow::point2qpoint(POINT{7, -3}), QPoint(7, -3));

    const POINT back = QxWindow::qpoint2point(QPoint(7, -3));
    QCOMPARE(back.x, LONG(7));
    QCOMPARE(back.y, LONG(-3));

    QCOMPARE(QxWindow::point2qpoint(QxWindow::qpoint2point(QPoint(11, 12))), QPoint(11, 12));
    QCOMPARE(QxWindow::qpoint2point(QxWindow::point2qpoint(POINT{13, 14})).x, LONG(13));
}

void tst_QxWindowWinKit::sizeConversionRoundTrips()
{
    QCOMPARE(QxWindow::size2qsize(SIZE{320, 240}), QSize(320, 240));

    const SIZE back = QxWindow::qsize2size(QSize(320, 240));
    QCOMPARE(back.cx, LONG(320));
    QCOMPARE(back.cy, LONG(240));

    QCOMPARE(QxWindow::size2qsize(QxWindow::qsize2size(QSize(1, 2))), QSize(1, 2));
    QCOMPARE(QxWindow::qsize2size(QxWindow::size2qsize(SIZE{3, 4})).cy, LONG(4));
}

void tst_QxWindowWinKit::rectConversionCountsEdgesTheWayEachSideDoes()
{
    // RECT counts inclusively: {0, 0, 7, 8} covers 8 columns and 9 rows, since
    // both edges are pixels inside the rectangle. rect2qrect builds the QRect
    // from RECT_WIDTH/RECT_HEIGHT - right-left and bottom-top - so the QRect
    // is 7 by 8. That is the correct result for the conversion rather than an
    // off-by-one: the two structures count their far edge differently.
    const QRect q = QxWindow::rect2qrect(RECT{0, 0, 7, 8});
    QCOMPARE(q.topLeft(), QPoint(0, 0));
    QCOMPARE(q.size(), QSize(7, 8));
    QCOMPARE(q.right(), 6);

    // Going the other way uses QRect::right() and QRect::bottom(), which are
    // the last pixel inside, so a 7-wide QRect comes back as right = 6.
    const RECT r = QxWindow::qrect2rect(QRect(0, 0, 7, 8));
    QCOMPARE(r.left, LONG(0));
    QCOMPARE(r.top, LONG(0));
    QCOMPARE(r.right, LONG(6));
    QCOMPARE(r.bottom, LONG(7));

    // An offset origin survives the position part of both.
    QCOMPARE(QxWindow::rect2qrect(RECT{10, 20, 30, 40}).topLeft(), QPoint(10, 20));
    QCOMPARE(QxWindow::qrect2rect(QRect(10, 20, 30, 40)).left, LONG(10));
    QCOMPARE(QxWindow::qrect2rect(QRect(10, 20, 30, 40)).top, LONG(20));
}

void tst_QxWindowWinKit::rectRoundTripThroughBothConversionsLosesAPixel()
{
    // Each single conversion is faithful about the geometry it is asked about,
    // but composing them is lossy: the width is shortened once on the way to Qt
    // and never restored, because QRect's far edge is inclusive and RECT's is
    // not. A caller that round-trips a RECT therefore ends up with a rectangle
    // one pixel narrower and shorter than it started.
    //
    // This is pinned rather than fixed: rect2qrect is used for frameless
    // window geometry, where the exclusive count is what Win32 reports, and
    // changing it would move real pixels. A caller that needs the far edge
    // back has to add the one itself.
    const RECT original{0, 0, 100, 50};
    const RECT roundTripped = QxWindow::qrect2rect(QxWindow::rect2qrect(original));

    QCOMPARE(roundTripped.left, original.left);
    QCOMPARE(roundTripped.top, original.top);
    QCOMPARE(roundTripped.right, original.right - 1);
    QCOMPARE(roundTripped.bottom, original.bottom - 1);

    // Single conversions, by contrast, are exact about position and about the
    // span they are defined to carry.
    QCOMPARE(QxWindow::rect2qrect(original).topLeft(), QPoint(0, 0));
    QCOMPARE(QxWindow::rect2qrect(original).size(), QSize(100, 50));
    QCOMPARE(QxWindow::qrect2rect(QRect(0, 0, 100, 50)).right, LONG(99));
}

void tst_QxWindowWinKit::marginsConversionReordersRatherThanCopies()
{
    // MARGINS declares its fields left, right, top, bottom. QMargins declares
    // them left, top, right, bottom. The conversion has to move the third
    // value into the second slot; copying positionally would silently swap the
    // right and top margins.
    //
    // 1, 2, 3, 4 in MARGINS order is left=1 right=2 top=3 bottom=4.
    QCOMPARE(QxWindow::margins2qmargins(MARGINS{1, 2, 3, 4}), QMargins(1, 3, 2, 4));

    QCOMPARE(QxWindow::margins2qmargins(MARGINS{5, 6, 7, 8}), QMargins(5, 7, 6, 8));
    QCOMPARE(QxWindow::margins2qmargins(MARGINS{0, 0, 0, 0}), QMargins(0, 0, 0, 0));

    // A distinctive value per slot makes a positional copy impossible to
    // mistake for a correct one.
    QCOMPARE(QxWindow::margins2qmargins(MARGINS{11, 22, 33, 44}), QMargins(11, 33, 22, 44));
}

void tst_QxWindowWinKit::marginsRoundTripPreservesEverySide()
{
    const MARGINS original{1, 2, 3, 4};
    const QMargins asQt = QxWindow::margins2qmargins(original);
    const MARGINS back = QxWindow::qmargins2margins(asQt);

    QCOMPARE(back.cxLeftWidth, LONG(1));
    QCOMPARE(back.cxRightWidth, LONG(2));
    QCOMPARE(back.cyTopHeight, LONG(3));
    QCOMPARE(back.cyBottomHeight, LONG(4));

    // The QMargins reading of the same numbers, to make the reordering
    // explicit rather than something the round trip happens to hide.
    QCOMPARE(asQt.left(), 1);
    QCOMPARE(asQt.top(), 3);
    QCOMPARE(asQt.right(), 2);
    QCOMPARE(asQt.bottom(), 4);

    QCOMPARE(QxWindow::qmargins2margins(QMargins(9, 8, 7, 6)).cyTopHeight, LONG(8));
}

void tst_QxWindowWinKit::negativeAndLargeValuesSurviveConversion()
{
    // Multi-monitor frameless geometry goes negative routinely, so the
    // conversions are signed on both sides and must stay that way.
    QCOMPARE(QxWindow::rect2qrect(RECT{-1920, -100, -100, 0}).topLeft(), QPoint(-1920, -100));
    QCOMPARE(QxWindow::qrect2rect(QRect(-1920, -100, -100, 0)).left, LONG(-1920));
    QCOMPARE(QxWindow::point2qpoint(POINT{-1, -1}), QPoint(-1, -1));
    QCOMPARE(QxWindow::qpoint2point(QPoint(-1, -1)).x, LONG(-1));
    QCOMPARE(QxWindow::size2qsize(SIZE{-1, -1}), QSize(-1, -1));
    QCOMPARE(QxWindow::margins2qmargins(MARGINS{-1, -2, -3, -4}), QMargins(-1, -3, -2, -4));

    // The largest a Win32 edge coordinate reaches in practice, well inside int.
    QCOMPARE(QxWindow::point2qpoint(POINT{32767, -32768}), QPoint(32767, -32768));
    QCOMPARE(QxWindow::qpoint2point(QPoint(32767, -32768)).y, LONG(-32768));
}

void tst_QxWindowWinKit::hwndStringPadsToEightDigits()
{
    // The format is 0x plus eight hex digits, so handles of different widths
    // line up when they are logged next to each other.
    QCOMPARE(QxWindow::hwnd2str(WId(0x1234)), QStringLiteral("0x00001234"));
    QCOMPARE(QxWindow::hwnd2str(WId(0x1)), QStringLiteral("0x00000001"));
    QCOMPARE(QxWindow::hwnd2str(WId(0xdeadbeef)), QStringLiteral("0xDEADBEEF"));

    // Already eight digits: padding must not truncate or widen.
    QCOMPARE(QxWindow::hwnd2str(WId(0x12345678)).size(), 10);
}

void tst_QxWindowWinKit::hwndStringUppercasesHexDigits()
{
    // Padding to eight digits happens before the digits are read, so a shorter
    // value gains leading zeros and keeps its own width: 0xabcdef is six digits
    // and becomes 0x00abcdef with the letters uppercased.
    QCOMPARE(QxWindow::hwnd2str(WId(0xabcdef)), QStringLiteral("0x00ABCDEF"));
    QCOMPARE(QxWindow::hwnd2str(WId(0xaAbBcC)), QStringLiteral("0x00AABBCC"));
    QCOMPARE(QxWindow::hwnd2str(WId(0x12345678)), QStringLiteral("0x12345678"));
}

void tst_QxWindowWinKit::nullHwndStringifiesToAllZeros()
{
    // A null handle is allowed through rather than rejected, and it reads as a
    // handle of the right width rather than an empty string.
    QCOMPARE(QxWindow::hwnd2str(WId(0)), QStringLiteral("0x00000000"));
    QCOMPARE(QxWindow::hwnd2str(WId(0)).size(), 10);
}

#endif   // Q_OS_WIN

TEST_ADD(tst_QxWindowWinKit)

#include "tst_qxwindowwinkit.moc"