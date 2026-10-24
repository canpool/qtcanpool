/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentgridview.h"

#include "qxtheme/qxthememanager.h"

#include <QtGui/QPainter>
#include <QtGui/QStandardItem>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QStyleOptionViewItem>
#include <QAccessible>

QX_FLUENT_USE_NAMESPACE

namespace
{
// The tile's own measures, read the way a reader reads them.
constexpr int kTileWidth = 152;
constexpr int kTileHeight = 112;
constexpr int kIconSize = 32;
constexpr int kIconTop = 20;
constexpr int kNameGap = 10;
constexpr int kNameHeight = 32;
constexpr int kNameInset = 8;

/*! How much ink \a room carries: the distance of its pixels from the page, added
 *  up. A name written at a fraction of its weight answers with a fraction of the
 *  sum, so "fainter" is read without naming a colour - which is what makes the
 *  reading hold in either theme. */
qreal inkWeight(const QImage &image, const QRect &room, const QColor &page)
{
    qreal weight = 0.0;
    const QRect room_ = room.intersected(image.rect());
    for (int y = room_.top(); y <= room_.bottom(); ++y) {
        for (int x = room_.left(); x <= room_.right(); ++x) {
            weight += qAbs(qreal(image.pixelColor(x, y).lightness()) - qreal(page.lightness()));
        }
    }
    return weight;
}

/*! How many pixels of \a room are the accent itself, allowance made for the
 *  one or two of anti-aliasing either side of it. */
int accentPixels(const QImage &image, const QRect &room)
{
    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
    int count = 0;
    const QRect room_ = room.intersected(image.rect());
    for (int y = room_.top(); y <= room_.bottom(); ++y) {
        for (int x = room_.left(); x <= room_.right(); ++x) {
            const QColor pixel = image.pixelColor(x, y);
            if (qAbs(pixel.red() - accent.red()) <= 8 && qAbs(pixel.green() - accent.green()) <= 8 &&
                qAbs(pixel.blue() - accent.blue()) <= 8) {
                ++count;
            }
        }
    }
    return count;
}
}   // namespace

class tst_FluentGridView : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void aTileIsAHundredAndFiftyTwoByAHundredAndTwelve();
    void theTilesWrapBeforeTheyStack();
    void thePointerWashesTheTileItIsOver();
    void theChosenTileWearsTheAccentAroundIt();
    void aTileNobodyCanPickIsWrittenFainter();
    void theKeyboardWalksTheTiles();
    void theSizeIsTwoTilesEitherWay();
    void theAccessibleFaceIsAList();
private:
    FluentGridView *makeGrid();
private:
    QWidget *m_page = nullptr;
};

void tst_FluentGridView::init()
{
    qtcGiveFonts();
    m_page = new QWidget;
    m_page->resize(420, 320);
    m_page->show();
}

void tst_FluentGridView::cleanup()
{
    delete m_page;
    m_page = nullptr;
}

FluentGridView *tst_FluentGridView::makeGrid()
{
    auto *grid = new FluentGridView(m_page);
    grid->setGeometry(0, 0, 400, 300);
    grid->addItem(QStringLiteral("Alpha"));
    grid->addItem(QStringLiteral("Beta"));
    grid->addItem(QStringLiteral("Gamma"));
    grid->addItem(QStringLiteral("Delta"));
    grid->show();
    return grid;
}

/*! A tile is the size the grid lays out. */
void tst_FluentGridView::aTileIsAHundredAndFiftyTwoByAHundredAndTwelve()
{
    FluentGridView *grid = makeGrid();
    const QRect tile = grid->visualRect(grid->model()->index(0, 0));
    QCOMPARE(tile.size(), QSize(kTileWidth, kTileHeight));
}

/*! A grid grows sideways before it grows down: the second tile stands to the
 *  right of the first, not under it. */
void tst_FluentGridView::theTilesWrapBeforeTheyStack()
{
    FluentGridView *grid = makeGrid();
    const QRect first = grid->visualRect(grid->model()->index(0, 0));
    const QRect second = grid->visualRect(grid->model()->index(1, 0));
    QCOMPARE(second.top(), first.top());
    QVERIFY(second.left() > first.left());
}

/*! The pointer's wash is what a hovered tile looks like: the delegate paints it
 *  when the tile arrives carrying State_MouseOver, and that is the whole of the
 *  message the view has to hand it.
 *
 *  Read off the delegate rather than off a frame taken while the pointer is
 *  moved over the view, because that cannot be done here: a view hears the
 *  pointer's arrival through HoverEnter, which the platform sends from the real
 *  cursor, and QTest::mouseMove only moves that cursor while its window holds
 *  the activation - in a suite that has just torn down another test's page it
 *  does not, and the frame then reads the same before and after the move.
 */
void tst_FluentGridView::thePointerWashesTheTileItIsOver()
{
    FluentGridView *grid = makeGrid();
    const QModelIndex index = grid->model()->index(0, 0);
    const QRect tile = grid->visualRect(index);
    const QColor page = grid->palette().color(QPalette::Window);

    QStyleOptionViewItem option;
    // The frame is the tile's own room, so the tile stands at its origin: an
    // option rect carried over at the tile's place in the view would paint the
    // whole tile outside the image.
    option.rect = QRect(QPoint(0, 0), tile.size());
    option.palette = grid->palette();
    option.state = QStyle::State_Enabled;

    QImage resting(tile.size(), QImage::Format_ARGB32);
    resting.fill(page);
    QPainter painter(&resting);
    grid->itemDelegate()->paint(&painter, option, index);
    painter.end();

    option.state |= QStyle::State_MouseOver;
    QImage hovered(tile.size(), QImage::Format_ARGB32);
    hovered.fill(page);
    QPainter hoverPainter(&hovered);
    grid->itemDelegate()->paint(&hoverPainter, option, index);
    hoverPainter.end();

    // A point near the tile's own corner, where only the wash can reach.
    const QPoint corner(6, 6);
    QVERIFY(resting.pixelColor(corner) != hovered.pixelColor(corner));
    QCOMPARE(resting.pixelColor(corner), page);
}

/*! The chosen tile is the one with the accent around it, and behind it; the
 *  tiles beside it wear neither. */
void tst_FluentGridView::theChosenTileWearsTheAccentAroundIt()
{
    FluentGridView *grid = makeGrid();
    grid->setCurrentIndex(grid->model()->index(0, 0));

    const QImage frame = grid->grab().toImage();
    const QRect tile = grid->visualRect(grid->model()->index(0, 0));
    const QRect other = grid->visualRect(grid->model()->index(1, 0));

    // Along the top edge of the chosen tile, inside the stroke.
    QVERIFY(accentPixels(frame, QRect(tile.left() + 20, tile.top(), tile.width() - 40, 2)) > 0);
    QVERIFY(accentPixels(frame, QRect(other.left() + 20, other.top(), other.width() - 40, 2)) == 0);
    // And its name takes the accent too.
    QVERIFY(accentPixels(frame, QRect(tile.left(), tile.top() + 60, tile.width(), 28)) > 0);
}

/*! A tile nobody can pick is written fainter than one that can: the delegate
 *  keeps a disabled name at thirty-eight parts in a hundred. Read as the tile
 *  before and after it is turned off, so the same name is measured twice - two
 *  tiles side by side would differ by their letters as much as by their state.
 */
void tst_FluentGridView::aTileNobodyCanPickIsWrittenFainter()
{
    FluentGridView *grid = makeGrid();
    auto *model_ = qobject_cast<QStandardItemModel *>(grid->model());
    QVERIFY(model_);
    // A turned-off tile would be drawn from the palette's disabled group as
    // well, and that alone is already fainter. Held to the active group, the
    // only thing left that can fade the name is the delegate's own hand.
    qtcPinDisabledColours(grid);

    const QRect tile = grid->visualRect(grid->model()->index(1, 0));
    const QRect name(tile.left() + kNameInset, tile.top() + kIconTop + kIconSize + kNameGap,
                     tile.width() - 2 * kNameInset, kNameHeight);
    const QRect beside = grid->visualRect(grid->model()->index(0, 0));
    const QRect otherName(beside.left() + kNameInset, beside.top() + kIconTop + kIconSize + kNameGap,
                          beside.width() - 2 * kNameInset, kNameHeight);
    const QColor page = grid->palette().color(QPalette::Window);

    const QImage before = grid->grab().toImage();
    const qreal whole = inkWeight(before, name, page);
    QVERIFY(whole > 0.0);

    model_->item(1)->setEnabled(false);
    const QImage after = grid->grab().toImage();
    const qreal faded = inkWeight(after, name, page);

    // Written still - a tile that kept its name is not a tile that lost it -
    // but at a fraction of its weight.
    QVERIFY(faded > 0.0);
    QVERIFY2(faded < whole * 0.7, qPrintable(QStringLiteral("whole %1, faded %2").arg(whole).arg(faded)));
    // And only the tile that was turned off: its neighbour is untouched.
    QCOMPARE(inkWeight(after, otherName, page), inkWeight(before, otherName, page));
}

/*! The keyboard walks the tiles: Right for the next one across, Down for the
 *  next row of them. */
void tst_FluentGridView::theKeyboardWalksTheTiles()
{
    FluentGridView *grid = makeGrid();
    grid->setFocus();
    grid->setCurrentIndex(grid->model()->index(0, 0));

    QTest::keyClick(grid, Qt::Key_Right);
    QCOMPARE(grid->currentIndex().row(), 1);
    QTest::keyClick(grid, Qt::Key_Down);
    QCOMPARE(grid->currentIndex().row(), 3);
}

/*! The grid asks for two tiles either way, and for one tile when it is given
 *  as little as it will take. */
void tst_FluentGridView::theSizeIsTwoTilesEitherWay()
{
    FluentGridView grid;
    QCOMPARE(grid.sizeHint(), QSize(2 * kTileWidth + 3 * 8, 2 * kTileHeight + 3 * 8));
    QCOMPARE(grid.minimumSizeHint(), QSize(kTileWidth + 2 * 8, kTileHeight + 2 * 8));
}

/*! To an assistive tool a grid of tiles is still a list of children: the
 *  framework has no grid role, and inventing one would be a lie told in a
 *  vocabulary nothing reads. */
void tst_FluentGridView::theAccessibleFaceIsAList()
{
    FluentGridView *grid = makeGrid();
    QAccessibleInterface *face = QAccessible::queryAccessibleInterface(grid);
    QVERIFY(face);
    QCOMPARE(face->role(), QAccessible::List);
    QVERIFY(face->childCount() >= 1);
}

TEST_ADD(tst_FluentGridView)

#include "tst_fluentgridview.moc"
