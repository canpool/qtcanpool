/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialimagelist.h"
#include "qxmaterial/materialtokens.h"

#include "qxcontrols/qxcontrol.h"
#include "qxcontrols/qxstatelayer.h"

#include "qxtheme/qxthememanager.h"

#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QAccessible>

QX_MATERIAL_USE_NAMESPACE

namespace
{
// The grid the tests build: three columns of a hundred, eight between them, in
// five hundred points of room - so a cell is a hundred and sixty-one wide and
// the leftover goes to the column at the edge.
constexpr int kColumns = 3;
constexpr int kRowHeight = 100;
constexpr int kGap = 8;
constexpr int kWidth = 500;
constexpr int kCell = (kWidth - (kColumns - 1) * kGap) / kColumns;

/*! A picture of one colour: what it is standing for is read from the pixels, so
 *  a colour is enough and a photograph is not. */
QPixmap picture(const QColor &color, int width = 40, int height = 10)
{
    QPixmap pixmap(width, height);
    pixmap.fill(color);
    return pixmap;
}

/*! Whether \a pixel is \a expected, allowing for the anti-aliasing of whatever
 *  was drawn over it. */
bool alike(const QColor &pixel, const QColor &expected)
{
    return qAbs(pixel.red() - expected.red()) <= 3 && qAbs(pixel.green() - expected.green()) <= 3 &&
           qAbs(pixel.blue() - expected.blue()) <= 3;
}

/*! How many pixels of \a room are the white the names are drawn in. */
int whitePixels(const QImage &image, const QRect &room)
{
    int count = 0;
    const QRect room_ = room.intersected(image.rect());
    for (int y = room_.top(); y <= room_.bottom(); ++y) {
        for (int x = room_.left(); x <= room_.right(); ++x) {
            const QColor pixel = image.pixelColor(x, y);
            if (pixel.red() > 245 && pixel.green() > 245 && pixel.blue() > 245) {
                ++count;
            }
        }
    }
    return count;
}

/*! The tile standing at \a index: the list's child under the middle of the tile -
 *  the middle, because the bar's own widget stands at one end of it. */
QWidget *tileAt(MaterialImageList *grid, int index)
{
    return grid->childAt(grid->itemRect(index).center());
}

/*! The pointer arriving at \a widget, which is a state a test can only hand
 *  over: the offscreen platform never moves one. */
void pointerArrives(QWidget *widget)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QEnterEvent event(QPointF(1.0, 1.0), QPointF(1.0, 1.0), QPointF(1.0, 1.0));
#else
    QEvent event(QEvent::Enter);
#endif
    QApplication::sendEvent(widget, &event);
}

void pointerLeaves(QWidget *widget)
{
    QEvent event(QEvent::Leave);
    QApplication::sendEvent(widget, &event);
}

/*! A grid of \a count pictures, each of one colour, in the shape the tests read:
 *  moved off the page's own corner, where the offscreen platform keeps its
 *  pointer - a grid built at (0, 0) has its first tile hovered before any test
 *  asks for it. */
MaterialImageList *makeGrid(QWidget *page, int count, const QColor &color = QColor(200, 60, 60))
{
    auto *grid = new MaterialImageList(page);
    grid->setColumns(kColumns);
    grid->setRowHeight(kRowHeight);
    grid->setGap(kGap);
    grid->resize(kWidth, 300);
    for (int i = 0; i < count; ++i) {
        grid->addItem(picture(color), QStringLiteral("Picture %1").arg(i));
    }
    grid->move(0, 60);
    grid->show();
    QTest::qWaitForWindowExposed(grid);
    return grid;
}

QWidget *makePage()
{
    auto *page = new QWidget;
    page->resize(520, 400);
    page->show();
    QTest::qWaitForWindowExposed(page);
    return page;
}
}   // namespace

class tst_MaterialImageList : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();

    void theGridDividesTheRoomItIsGiven();
    void aPictureMayTakeMoreThanOneCell();
    void thePicturesFillTheirTiles();
    void theNamesStandOnThePicture();
    void theBarGoesWhereItWasTold();
    void aPressTakesThePicture();
    void theKeyboardWalksTheCells();
    void aSpanWiderThanTheGridIsTheGrid();
    void takingAPictureOutClosesTheHole();
    void theGridAnswersForItsShape();
    void aPictureIsAnnouncedByName();
    void theWashLandsOnThePicture();
    void aPictureNobodyCanUseIsNotTaken();
};

void tst_MaterialImageList::initTestCase()
{
    qtcGiveFonts();
    QxTheme::QxThemeManager::instance();
}

/*! Three columns in five hundred points: a hundred and sixty-one each, eight
 *  between them, and the leftover belongs to the column at the edge - a grid
 *  that ended two points short of its room would look like a mistake. */
void tst_MaterialImageList::theGridDividesTheRoomItIsGiven()
{
    QWidget *page = makePage();
    MaterialImageList *grid = makeGrid(page, 4);

    QCOMPARE(grid->count(), 4);
    QCOMPARE(grid->columns(), kColumns);
    QCOMPARE(grid->rowHeight(), kRowHeight);
    QCOMPARE(grid->gap(), kGap);

    QCOMPARE(grid->itemRect(0), QRect(0, 0, kCell, kRowHeight));
    QCOMPARE(grid->itemRect(1), QRect(kCell + kGap, 0, kCell, kRowHeight));
    QCOMPARE(grid->itemRect(2).right(), kWidth - 1);
    QVERIFY(grid->itemRect(2).width() > kCell);
    // The fourth picture is on the second row, one row and one gap down.
    QCOMPARE(grid->itemRect(3).top(), kRowHeight + kGap);
    QCOMPARE(grid->itemRect(4), QRect());
    QCOMPARE(grid->itemRect(-1), QRect());
}

/*! A picture that takes two cells both ways stands in the first place its span
 *  fits, and what comes after it is packed beside it rather than wrapped past
 *  it: the cells under a two-row picture are not left empty. */
void tst_MaterialImageList::aPictureMayTakeMoreThanOneCell()
{
    QWidget *page = makePage();
    MaterialImageList *grid = makeGrid(page, 6);
    grid->setItemSpan(0, 2, 2);

    QCOMPARE(grid->itemColumnSpan(0), 2);
    QCOMPARE(grid->itemRowSpan(0), 2);
    // The subject is two cells across and two down.
    QCOMPARE(grid->itemRect(0), QRect(0, 0, 2 * kCell + kGap, 2 * kRowHeight + kGap));

    // Beside it, not below it: the second picture takes the third column of the
    // first row, and the third picture the third column of the second row.
    QCOMPARE(grid->itemRect(1).topLeft(), QPoint(2 * (kCell + kGap), 0));
    QCOMPARE(grid->itemRect(2).top(), kRowHeight + kGap);
    // The fourth picture starts the third row: the cells under the subject are
    // taken, so it cannot stand there.
    QCOMPARE(grid->itemRect(3).topLeft(), QPoint(0, 2 * (kRowHeight + kGap)));

    // And nothing overlaps: every pair of tiles either is apart or touches.
    for (int i = 0; i < grid->count(); ++i) {
        for (int j = i + 1; j < grid->count(); ++j) {
            const QRect a = grid->itemRect(i);
            const QRect b = grid->itemRect(j);
            QVERIFY2(!a.intersects(b), qPrintable(QStringLiteral("%1 and %2 overlap").arg(i).arg(j)));
        }
    }
}

/*! A picture fills its tile: it is scaled until it covers the tile and the
 *  middle of it is what is shown, so a tile is never half a photograph with the
 *  page showing beside it. */
void tst_MaterialImageList::thePicturesFillTheirTiles()
{
    QWidget *page = makePage();
    const QColor red(200, 60, 60);
    MaterialImageList *grid = makeGrid(page, 3, red);

    const QImage frame = grid->grab().toImage();
    const QRect room = grid->itemRect(1);
    // Well inside the tile, which is where the picture has to be.
    // Above the bar, which covers the foot of every tile that has a name.
    const QList<QPoint> inside = {QPoint(8, 8), QPoint(room.width() / 2, 20),
                                  QPoint(room.width() - 9, room.height() - 56)};
    for (const QPoint &point : inside) {
        const QColor pixel = frame.pixelColor(room.topLeft() + point);
        QVERIFY2(alike(pixel, red), qPrintable(pixel.name()));
    }
    // And outside it is the page, which is what the gap between two tiles is.
    QVERIFY(alike(frame.pixelColor(room.right() + 2, room.center().y()), page->palette().color(QPalette::Window)));
}

/*! The names are drawn on the picture, in white, over a scrim: what is behind
 *  them is a photograph the theme has never seen, so the contrast is the tile's
 *  to make and not the palette's to promise. */
void tst_MaterialImageList::theNamesStandOnThePicture()
{
    QWidget *page = makePage();
    const QColor red(200, 60, 60);
    MaterialImageList *grid = makeGrid(page, 3, red);
    grid->setItemTitle(1, QStringLiteral("A photograph"));

    const QImage frame = grid->grab().toImage();
    const QRect room = grid->itemRect(1);
    const QRect bar(room.left(), room.bottom() - 47, room.width(), 48);
    QVERIFY2(whitePixels(frame, bar) > 0, "the name is not drawn");

    // The bar is at the foot unless it was told otherwise, so the head of the
    // tile is the photograph itself, undimmed by any scrim.
    const QPoint head(room.center().x(), room.top() + 8);
    QVERIFY2(alike(frame.pixelColor(head), red), qPrintable(frame.pixelColor(head).name()));
    // Under the bar the picture is dimmed: the scrim took light away from it.
    const QPoint under(room.left() + 8, room.bottom() - 6);
    QVERIFY(frame.pixelColor(under).lightness() < red.lightness());

    // A picture with no name has no bar: the foot of it is the picture.
    const int nameless = grid->addItem(picture(red));
    const QRect bare = grid->itemRect(nameless);
    const QPoint foot(bare.left() + 8, bare.bottom() - 6);
    const QColor besided = grid->grab().toImage().pixelColor(foot);
    QVERIFY2(alike(besided, red), qPrintable(besided.name()));
}

/*! Which edge the bar sits on is the picture's to say - a picture whose subject
 *  is low down wants its name high up. */
void tst_MaterialImageList::theBarGoesWhereItWasTold()
{
    QWidget *page = makePage();
    const QColor red(200, 60, 60);
    MaterialImageList *grid = makeGrid(page, 3, red);
    grid->setItemTitle(1, QStringLiteral("A photograph"));
    grid->setItemSubtitle(1, QStringLiteral("Somebody's"));
    grid->setItemBarPosition(1, MaterialImageList::BarPosition::Top);
    QCOMPARE(grid->itemBarPosition(1), MaterialImageList::BarPosition::Top);
    QCOMPARE(grid->itemBarPosition(0), MaterialImageList::BarPosition::Bottom);

    const QImage frame = grid->grab().toImage();
    const QRect room = grid->itemRect(1);
    // Both lines are on it: a name, and the line under the name.
    QVERIFY(whitePixels(frame, QRect(room.left(), room.top(), room.width(), 68)) > 20);
    const QPoint head(room.left() + 8, room.top() + 6);
    QVERIFY(frame.pixelColor(head).lightness() < red.lightness());
    // The foot is the photograph, not a bar.
    const QPoint foot(room.left() + 8, room.bottom() - 6);
    QVERIFY2(alike(frame.pixelColor(foot), red), qPrintable(frame.pixelColor(foot).name()));
}

/*! A press takes the picture. A press that lands on the widget in its bar is
 *  that widget's: the star is not the photograph. */
void tst_MaterialImageList::aPressTakesThePicture()
{
    QWidget *page = makePage();
    MaterialImageList *grid = makeGrid(page, 3);

    QSignalSpy taken(grid, &MaterialImageList::itemClicked);
    QTest::mouseClick(tileAt(grid, 2), Qt::LeftButton);
    QCOMPARE(taken.count(), 1);
    QCOMPARE(taken.at(0).at(0).toInt(), 2);

    // The star in the bar answers for itself, so the picture never hears the
    // press at all. It becomes the tile's child when it is given a place there.
    // It is a control, which is the point: a widget that ignores a press lets it
    // through to what stands under it, and a star that does that is a star on a
    // photograph that takes the photograph's clicks.
    auto *star = new QxControls::QxControl;
    star->setFixedSize(24, 24);
    QSignalSpy starred(star, &QxControls::QxControl::clicked);
    grid->setItemTrailingWidget(1, star);
    QCOMPARE(grid->itemTrailingWidget(1), star);
    QCOMPARE(star->parentWidget(), tileAt(grid, 1));
    QVERIFY(star->isVisible());

    // It stands at the far end of the bar, centred in it: the tile's own
    // coordinates, moved to the grid's, are where the far edge is.
    const QRect room = grid->itemRect(1);
    QCOMPARE(room.right() - (star->geometry().right() + room.left()), 16);
    QVERIFY(qAbs(star->geometry().center().y() - (room.bottom() - 23)) <= 1);

    QTest::mouseClick(star, Qt::LeftButton);
    QCOMPARE(starred.count(), 1);
    QCOMPARE(taken.count(), 1);
}

/*! The keyboard walks the cells, not the numbers: the picture below the one in
 *  force is the one standing under it. */
void tst_MaterialImageList::theKeyboardWalksTheCells()
{
    QWidget *page = makePage();
    MaterialImageList *grid = makeGrid(page, 6);
    // Six pictures, the first taking the two cells in the corner: the third
    // picture stands at the end of the second row, and the sixth under it.
    grid->setItemSpan(0, 2, 2);
    QWidget *first = tileAt(grid, 0);
    QWidget *second = tileAt(grid, 1);
    QWidget *third = tileAt(grid, 2);
    QWidget *fourth = tileAt(grid, 3);
    QWidget *sixth = tileAt(grid, 5);

    third->setFocus();
    QVERIFY(third->hasFocus());

    // Right and left walk the order.
    QTest::keyClick(third, Qt::Key_Right);
    QVERIFY(fourth->hasFocus());
    QTest::keyClick(fourth, Qt::Key_Left);
    QVERIFY(third->hasFocus());

    // Down from the third lands on the sixth, which stands under it.
    QTest::keyClick(third, Qt::Key_Down);
    QVERIFY(sixth->hasFocus());
    // Up from the sixth comes back to the third.
    QTest::keyClick(sixth, Qt::Key_Up);
    QVERIFY(third->hasFocus());
    // Up from the third comes back to the second, which stands on the cell above.
    QTest::keyClick(third, Qt::Key_Up);
    QVERIFY(second->hasFocus());

    // Left at the first picture is not a walk off the grid.
    first->setFocus();
    QTest::keyClick(first, Qt::Key_Left);
    QVERIFY(first->hasFocus());

    // Space takes what the keyboard is on, the way a press does.
    QSignalSpy taken(grid, &MaterialImageList::itemClicked);
    QTest::keyClick(first, Qt::Key_Space);
    QCOMPARE(taken.count(), 1);
    QCOMPARE(taken.at(0).at(0).toInt(), 0);
}

/*! A span wider than the grid is read as the whole grid while the grid is that
 *  narrow - and the picture gets its cells back when the grid widens again. */
void tst_MaterialImageList::aSpanWiderThanTheGridIsTheGrid()
{
    QWidget *page = makePage();
    MaterialImageList *grid = makeGrid(page, 3);
    grid->setItemSpan(0, 5, 1);

    QCOMPARE(grid->itemColumnSpan(0), 5);
    QCOMPARE(grid->itemRect(0), QRect(0, 0, kWidth, kRowHeight));
    // The rest of the pictures start on the row below, since the first one took
    // the whole of this one.
    QCOMPARE(grid->itemRect(1).top(), kRowHeight + kGap);

    // Six columns leave a cell free at the end of the row, so the picture takes
    // its five and the first neighbour stands beside it again.
    grid->setColumns(6);
    QCOMPARE(grid->columns(), 6);
    QVERIFY(grid->itemRect(0).width() < kWidth);
    QCOMPARE(grid->itemRect(1).top(), 0);
}

/*! Taking a picture out closes the hole it left: the ones after it are packed
 *  again and answer to their new numbers. */
void tst_MaterialImageList::takingAPictureOutClosesTheHole()
{
    QWidget *page = makePage();
    MaterialImageList *grid = makeGrid(page, 4);
    QCOMPARE(grid->itemTitle(1), QStringLiteral("Picture 1"));

    grid->removeItem(0);
    QCOMPARE(grid->count(), 3);
    QCOMPARE(grid->itemTitle(0), QStringLiteral("Picture 1"));
    QCOMPARE(grid->itemRect(0), QRect(0, 0, kCell, kRowHeight));

    QSignalSpy taken(grid, &MaterialImageList::itemClicked);
    QTest::mouseClick(tileAt(grid, 0), Qt::LeftButton);
    QCOMPARE(taken.count(), 1);
    QCOMPARE(taken.at(0).at(0).toInt(), 0);

    grid->clear();
    QCOMPARE(grid->count(), 0);
    QCOMPARE(grid->itemRect(0), QRect());
    QCOMPARE(grid->sizeHint().height(), 0);
}

/*! The grid answers for the shape it was given: a cell is as tall as the row it
 *  was told, the grid is as tall as the rows the spans came to, and that height
 *  never follows the width - which is why this widget keeps its own. */
void tst_MaterialImageList::theGridAnswersForItsShape()
{
    QWidget *page = makePage();
    MaterialImageList *grid = makeGrid(page, 6);

    // Six pictures in three columns are two rows.
    QCOMPARE(grid->sizeHint(), QSize(kColumns * kRowHeight, 2 * kRowHeight + kGap));
    QCOMPARE(grid->minimumSizeHint().height(), grid->sizeHint().height());
    QVERIFY(grid->minimumSizeHint().width() < grid->sizeHint().width());
    QCOMPARE(grid->sizePolicy().verticalPolicy(), QSizePolicy::Fixed);

    // Taller rows: the pictures follow and the grid's own height follows them,
    // while the width it asks for is still the columns of square tiles.
    grid->setRowHeight(120);
    QCOMPARE(grid->itemRect(0).height(), 120);
    QCOMPARE(grid->sizeHint(), QSize(kColumns * 120, 2 * 120 + kGap));

    // No gap: the pictures touch, and the grid is a gap shorter.
    grid->setGap(0);
    QCOMPARE(grid->itemRect(1).left(), grid->itemRect(0).right() + 1);
    QCOMPARE(grid->sizeHint().height(), 2 * 120);

    // Two columns put the same six pictures in three rows.
    grid->setColumns(2);
    QCOMPARE(grid->sizeHint().height(), 3 * 120);
}

/*! To an assistive tool a picture is a button that is called what its name
 *  says - and the name follows the title until the application names the tile
 *  itself. */
void tst_MaterialImageList::aPictureIsAnnouncedByName()
{
    QWidget *page = makePage();
    MaterialImageList *grid = makeGrid(page, 3);

    QWidget *tile = tileAt(grid, 1);
    QAccessibleInterface *face = QAccessible::queryAccessibleInterface(tile);
    QVERIFY(face);
    QCOMPARE(face->role(), QAccessible::Button);
    QCOMPARE(face->text(QAccessible::Name), QStringLiteral("Picture 1"));

    grid->setItemTitle(1, QStringLiteral("A photograph"));
    QCOMPARE(QAccessible::queryAccessibleInterface(tileAt(grid, 1))->text(QAccessible::Name),
             QStringLiteral("A photograph"));
}

/*! The wash a control wears lands on the photograph, because the photograph is
 *  the tile's surface: a wash under an opaque picture would be a wash nobody
 *  sees. */
void tst_MaterialImageList::theWashLandsOnThePicture()
{
    QWidget *page = makePage();
    const QColor red(200, 60, 60);
    MaterialImageList *grid = makeGrid(page, 3, red);
    QWidget *tile = tileAt(grid, 0);

    // Above the bar, which is where the picture is on its own.
    const QPoint inside = grid->itemRect(0).topLeft() + QPoint(30, 30);
    QVERIFY(alike(grid->grab().toImage().pixelColor(inside), red));

    pointerArrives(tile);
    const QColor ink = tile->palette().color(QPalette::WindowText);
    const QColor washed = QxControls::QxStateLayer::layered(red, ink, QxControls::QxStateLayer::hoverOpacity);
    const QColor pixel = grid->grab().toImage().pixelColor(inside);
    QVERIFY2(alike(pixel, washed), qPrintable(QStringLiteral("%1 is not %2").arg(pixel.name(), washed.name())));

    pointerLeaves(tile);
    QVERIFY(alike(grid->grab().toImage().pixelColor(inside), red));
}

/*! A picture nobody can take is not taken, and is drawn with less of itself. */
void tst_MaterialImageList::aPictureNobodyCanUseIsNotTaken()
{
    QWidget *page = makePage();
    const QColor red(200, 60, 60);
    MaterialImageList *grid = makeGrid(page, 3, red);

    QSignalSpy taken(grid, &MaterialImageList::itemClicked);
    grid->setEnabled(false);
    const QPoint inside = grid->itemRect(0).topLeft() + QPoint(30, 30);
    const QColor dimmed = grid->grab().toImage().pixelColor(inside);
    // Thinned toward the page: it is still there, less of it.
    QVERIFY(dimmed.lightness() > red.lightness());
    QVERIFY(dimmed.lightness() < page->palette().color(QPalette::Window).lightness());

    QTest::mouseClick(tileAt(grid, 0), Qt::LeftButton);
    QCOMPARE(taken.count(), 0);
}

TEST_ADD(tst_MaterialImageList)

#include "tst_materialimagelist.moc"
