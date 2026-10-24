/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluenttreeview.h"

#include "qxtheme/qxthememanager.h"

#include <QAccessible>
#include <QtGui/QPainter>
#include <QtGui/QStandardItem>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QStyleOptionViewItem>

QX_FLUENT_USE_NAMESPACE

namespace
{
// The node's own measures, read the way a reader reads them.
constexpr int kRowHeight = 32;
constexpr int kIndent = 16;
constexpr int kChevronSize = 12;
constexpr int kPadding = 8;
constexpr int kChevronGap = 8;
constexpr int kWordsLeft = kPadding + kChevronSize + kChevronGap;

/*! How much ink \a room carries: the distance of its pixels from the page, added
 *  up - the reading a faded name answers with a fraction of. */
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

/*! How many pixels of \a room two frames disagree about. */
int differingPixels(const QImage &left, const QImage &right, const QRect &room)
{
    int count = 0;
    const QRect room_ = room.intersected(left.rect());
    for (int y = room_.top(); y <= room_.bottom(); ++y) {
        for (int x = room_.left(); x <= room_.right(); ++x) {
            if (left.pixelColor(x, y) != right.pixelColor(x, y)) {
                ++count;
            }
        }
    }
    return count;
}
}   // namespace

class tst_FluentTreeView : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void aNodeStandsThirtyTwoTall();
    void aLevelInIsSixteenFurtherIn();
    void theChevronSaysThereIsMore();
    void thePointerWashesTheNodeItIsOver();
    void theChosenNodeWearsTheAccent();
    void aNodeNobodyCanPickIsWrittenFainter();
    void theKeyboardWalksAndOpens();
    void whatIsUnderANodeIsItsOwn();
    void theSizeIsTheRowsItShows();
    void theAccessibleFaceIsATree();
private:
    FluentTreeView *makeTree();
private:
    QWidget *m_page = nullptr;
};

void tst_FluentTreeView::init()
{
    qtcGiveFonts();
    m_page = new QWidget;
    m_page->resize(280, 240);
    m_page->show();
}

void tst_FluentTreeView::cleanup()
{
    delete m_page;
    m_page = nullptr;
}

FluentTreeView *tst_FluentTreeView::makeTree()
{
    auto *tree = new FluentTreeView(m_page);
    tree->setGeometry(0, 0, 240, 200);
    QStandardItem *documents = tree->addNode(QStringLiteral("Documents"));
    tree->addChild(documents, QStringLiteral("Notes"));
    tree->addChild(documents, QStringLiteral("Letters"));
    tree->addNode(QStringLiteral("Pictures"));
    tree->show();
    return tree;
}

/*! A node is thirty-two tall - denser than a list row, because a tree is
 *  followed rather than scanned. */
void tst_FluentTreeView::aNodeStandsThirtyTwoTall()
{
    FluentTreeView *tree = makeTree();
    QCOMPARE(tree->visualRect(tree->model()->index(0, 0)).height(), kRowHeight);
    QCOMPARE(tree->visualRect(tree->model()->index(1, 0)).top(), kRowHeight);
}

/*! Every level stands sixteen further in, which is what makes a tree a tree to
 *  the eye rather than a list with names that happen to differ. */
void tst_FluentTreeView::aLevelInIsSixteenFurtherIn()
{
    FluentTreeView *tree = makeTree();
    const int rootLeft = tree->visualRect(tree->model()->index(0, 0)).left();
    tree->expandAll();
    const QModelIndex documents = tree->model()->index(0, 0);
    const QModelIndex child = tree->model()->index(0, 0, documents);
    QVERIFY(child.isValid());
    const int childLeft = tree->visualRect(child).left();
    QCOMPARE(childLeft - rootLeft, kIndent);
}

/*! The node with more under it carries the chevron, the one without does not,
 *  and the chevron turns when the node opens. */
void tst_FluentTreeView::theChevronSaysThereIsMore()
{
    FluentTreeView *tree = makeTree();
    const QModelIndex documents = tree->model()->index(0, 0);
    const QRect room(0, 0, 120, kRowHeight);

    auto painted = [tree, room](const QModelIndex &index, QStyle::State extra) {
        QStyleOptionViewItem option;
        option.rect = room;
        option.palette = tree->palette();
        option.state = QStyle::State_Enabled | extra;
        QImage frame(room.size(), QImage::Format_ARGB32);
        frame.fill(tree->palette().color(QPalette::Window));
        QPainter painter(&frame);
        tree->itemDelegate()->paint(&painter, option, index);
        painter.end();
        return frame;
    };

    // The same node with and without a mark in its gutter: the gutter is kept
    // either way, so whatever differs in it is the mark and not a name that
    // moved.
    const QImage withMore = painted(documents, QStyle::State_Children);
    const QImage without = painted(documents, QStyle::State_None);
    const QRect chevronRoom(8, kRowHeight / 2 - kChevronSize / 2, kChevronSize, kChevronSize);
    QVERIFY(differingPixels(withMore, without, chevronRoom) > 0);

    // Opened, the chevron turns: same node, different mark.
    const QImage opened = painted(documents, QStyle::State_Children | QStyle::State_Open);
    QVERIFY(differingPixels(withMore, opened, chevronRoom) > 0);
}

/*! The pointer's wash is what a hovered node looks like, read off the delegate
 *  for the reason the list view gives. */
void tst_FluentTreeView::thePointerWashesTheNodeItIsOver()
{
    FluentTreeView *tree = makeTree();
    const QModelIndex index = tree->model()->index(1, 0);
    const QRect node(0, 0, 200, kRowHeight);
    const QColor page = tree->palette().color(QPalette::Window);

    QStyleOptionViewItem option;
    // The frame is the node's own room, so the node stands at its origin.
    option.rect = node;
    option.palette = tree->palette();
    option.state = QStyle::State_Enabled;

    QImage resting(node.size(), QImage::Format_ARGB32);
    resting.fill(page);
    QPainter painter(&resting);
    tree->itemDelegate()->paint(&painter, option, index);
    painter.end();

    option.state |= QStyle::State_MouseOver;
    QImage hovered(node.size(), QImage::Format_ARGB32);
    hovered.fill(page);
    QPainter hoverPainter(&hovered);
    tree->itemDelegate()->paint(&hoverPainter, option, index);
    hoverPainter.end();

    const QPoint away(180, node.height() / 2);
    QCOMPARE(resting.pixelColor(away), page);
    QVERIFY(hovered.pixelColor(away) != page);
}

/*! The chosen node wears the accent: the bar at its left and a tint behind it,
 *  which is what the list does and what a tree, being a list that branches,
 *  does too. */
void tst_FluentTreeView::theChosenNodeWearsTheAccent()
{
    FluentTreeView *tree = makeTree();
    tree->setCurrentIndex(tree->model()->index(1, 0));

    const QImage frame = tree->viewport()->grab().toImage();
    const QRect chosenBar(2, kRowHeight + kRowHeight / 2 - 6, 3, 12);
    const QRect otherBar(2, kRowHeight / 2 - 6, 3, 12);
    QVERIFY(accentPixels(frame, chosenBar) > 0);
    QCOMPARE(accentPixels(frame, otherBar), 0);
}

/*! A node nobody can pick is written fainter - the words and the chevron alike,
 *  both kept at thirty-eight parts in a hundred. Measured before and after the
 *  node is turned off, so what changes is the state and not the letters.
 */
void tst_FluentTreeView::aNodeNobodyCanPickIsWrittenFainter()
{
    FluentTreeView *tree = makeTree();
    auto *model_ = qobject_cast<QStandardItemModel *>(tree->model());
    QVERIFY(model_);
    // The palette's own disabled group would fade a turned-off node by itself.
    // Pinned to the active group, the fade that is measured is the delegate's.
    qtcPinDisabledColours(tree);

    const QRect leaf = tree->visualRect(tree->model()->index(1, 0));
    const QRect words(leaf.left() + kWordsLeft, leaf.top() + 2, leaf.width() - kWordsLeft - kPadding, kRowHeight - 4);
    const QRect branch = tree->visualRect(tree->model()->index(0, 0));
    const QRect chevron(branch.left() + kPadding, branch.center().y() - kChevronSize / 2, kChevronSize, kChevronSize);
    const QColor page = tree->viewport()->palette().color(QPalette::Window);

    const QImage before = tree->viewport()->grab().toImage();
    const qreal wholeWords = inkWeight(before, words, page);
    const qreal wholeChevron = inkWeight(before, chevron, page);
    QVERIFY(wholeWords > 0.0);
    QVERIFY(wholeChevron > 0.0);

    model_->item(0)->setEnabled(false);
    model_->item(1)->setEnabled(false);
    const QImage after = tree->viewport()->grab().toImage();

    // The leaf's name: still written, at a fraction of its weight.
    const qreal fadedWords = inkWeight(after, words, page);
    QVERIFY(fadedWords > 0.0);
    QVERIFY2(fadedWords < wholeWords * 0.7,
             qPrintable(QStringLiteral("words whole %1, faded %2").arg(wholeWords).arg(fadedWords)));
    // The branch's chevron: faded with its words, so the mark and the name stay
    // of one voice.
    const qreal fadedChevron = inkWeight(after, chevron, page);
    QVERIFY(fadedChevron > 0.0);
    QVERIFY2(fadedChevron < wholeChevron * 0.7,
             qPrintable(QStringLiteral("chevron whole %1, faded %2").arg(wholeChevron).arg(fadedChevron)));
}

/*! The keyboard walks the open nodes and opens what it walks onto. */
void tst_FluentTreeView::theKeyboardWalksAndOpens()
{
    FluentTreeView *tree = makeTree();
    tree->setFocus();
    tree->setCurrentIndex(tree->model()->index(0, 0));

    QTest::keyClick(tree, Qt::Key_Down);
    QCOMPARE(tree->currentIndex().row(), 1);
    QTest::keyClick(tree, Qt::Key_Up);
    QCOMPARE(tree->currentIndex().row(), 0);

    // Right opens the node it stands on, and the children are then walked.
    QTest::keyClick(tree, Qt::Key_Right);
    QVERIFY(tree->isExpanded(tree->model()->index(0, 0)));
    QTest::keyClick(tree, Qt::Key_Down);
    QCOMPARE(tree->currentIndex(), tree->model()->index(0, 0, tree->model()->index(0, 0)));
}

/*! What is under a node is the node's own: a child says whose it is, and the
 *  tree counts every node at every level. */
void tst_FluentTreeView::whatIsUnderANodeIsItsOwn()
{
    FluentTreeView *tree = makeTree();
    QCOMPARE(tree->nodeCount(), 4);

    const QModelIndex documents = tree->model()->index(0, 0);
    QCOMPARE(tree->model()->index(1, 0, documents).data(Qt::DisplayRole).toString(), QStringLiteral("Letters"));
    QVERIFY(!tree->isExpanded(documents));
    tree->expand(documents);
    QVERIFY(tree->isExpanded(documents));
    QVERIFY(tree->isRowHidden(0, tree->model()->index(0, 0)) == false);
}

/*! The tree asks for room for six nodes and says how little it can get by
 *  with. */
void tst_FluentTreeView::theSizeIsTheRowsItShows()
{
    FluentTreeView tree;
    QCOMPARE(tree.sizeHint(), QSize(280, 6 * kRowHeight));
    QCOMPARE(tree.minimumSizeHint(), QSize(120, 2 * kRowHeight));
}

/*! To an assistive tool a tree is a tree, with the nodes as its children. */
void tst_FluentTreeView::theAccessibleFaceIsATree()
{
    FluentTreeView *tree = makeTree();
    QAccessibleInterface *face = QAccessible::queryAccessibleInterface(tree);
    QVERIFY(face);
    QCOMPARE(face->role(), QAccessible::Tree);
    QVERIFY(face->childCount() >= 1);
}

TEST_ADD(tst_FluentTreeView)

#include "tst_fluenttreeview.moc"
