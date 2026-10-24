/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentlistview.h"

#include "qxtheme/qxthememanager.h"

#include <QtGui/QPainter>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QStyleOptionViewItem>
#include <QAccessible>

QX_FLUENT_USE_NAMESPACE

namespace
{
// The row's own measures, read the way a reader reads them.
constexpr int kRowHeight = 40;
constexpr int kBarInset = 4;
constexpr int kBarWidth = 3;

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

/*! How many pixels of \a room two frames disagree about. A one-pixel stroke
 *  laid between two pixel rows is answered by the anti-aliasing as two half
 *  pixels, so "the ring is there" is read as "these frames differ here". */
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

/*! How many pixels of \a room are darker than the page - a row's ink, however
 *  the anti-aliasing got there. */
int inkPixels(const QImage &image, const QRect &room)
{
    const QColor page = image.pixelColor(room.center());
    Q_UNUSED(page)
    int count = 0;
    const QRect room_ = room.intersected(image.rect());
    for (int y = room_.top(); y <= room_.bottom(); ++y) {
        for (int x = room_.left(); x <= room_.right(); ++x) {
            const QColor pixel = image.pixelColor(x, y);
            if (pixel.lightness() < 200) {
                ++count;
            }
        }
    }
    return count;
}
}   // namespace

class tst_FluentListView : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void theRowsStandFortyTall();
    void whatTheListHoldsIsWhatItSays();
    void thePointerWashesTheRowItIsOver();
    void theFocusRingStandsInsideTheRow();
    void theChosenRowWearsTheAccent();
    void aDisabledRowIsFainter();
    void theKeyboardWalksTheRows();
    void theSizeIsTheRowsItShows();
    void theAccessibleFaceIsAList();
private:
    FluentListView *makeList();
private:
    QWidget *m_page = nullptr;
};

void tst_FluentListView::init()
{
    qtcGiveFonts();
    m_page = new QWidget;
    m_page->resize(240, 240);
    m_page->show();
}

void tst_FluentListView::cleanup()
{
    delete m_page;
    m_page = nullptr;
}

FluentListView *tst_FluentListView::makeList()
{
    auto *list = new FluentListView(m_page);
    list->setGeometry(0, 0, 200, 200);
    list->addItem(QStringLiteral("Alpha"));
    list->addItem(QStringLiteral("Beta"));
    list->addItem(QStringLiteral("Gamma"));
    list->show();
    return list;
}

/*! A row is forty tall, and the rows stack from the top of the view. */
void tst_FluentListView::theRowsStandFortyTall()
{
    FluentListView *list = makeList();
    QCOMPARE(list->visualRect(list->model()->index(0, 0)).height(), kRowHeight);
    QCOMPARE(list->visualRect(list->model()->index(1, 0)).top(), kRowHeight);
    QCOMPARE(list->visualRect(list->model()->index(2, 0)).top(), 2 * kRowHeight);
}

/*! The rows added by hand are the rows the list says it has, and a row that is
 *  not there says nothing rather than something wrong. */
void tst_FluentListView::whatTheListHoldsIsWhatItSays()
{
    FluentListView *list = makeList();
    QCOMPARE(list->count(), 3);
    QCOMPARE(list->itemText(1), QStringLiteral("Beta"));
    QVERIFY(list->itemText(-1).isEmpty());
    QVERIFY(list->itemText(3).isEmpty());

    list->setItems(QStringList() << QStringLiteral("One") << QStringLiteral("Two"));
    QCOMPARE(list->count(), 2);
    QCOMPARE(list->itemText(1), QStringLiteral("Two"));
}

/*! The pointer's wash is what a hovered row looks like: the delegate paints it
 *  when the row arrives carrying State_MouseOver, and that is the whole of the
 *  message the view has to hand it.
 *
 *  Read off the delegate rather than off a frame taken while the pointer is
 *  moved over the view, because that cannot be done here: a view hears the
 *  pointer's arrival through HoverEnter, which the platform sends from the real
 *  cursor, and QTest::mouseMove only moves that cursor while its window holds
 *  the activation - in a suite that has just torn down another test's page it
 *  does not, and the frame then reads the same before and after the move.
 */
void tst_FluentListView::thePointerWashesTheRowItIsOver()
{
    FluentListView *list = makeList();
    const QModelIndex index = list->model()->index(1, 0);
    const QRect row = list->visualRect(index);
    const QColor page = list->palette().color(QPalette::Window);

    QStyleOptionViewItem option;
    // The frame is the row's own room, so the row stands at its origin: an
    // option rect carried over at the row's place in the view would paint the
    // whole row outside the image.
    option.rect = QRect(QPoint(0, 0), row.size());
    option.palette = list->palette();
    option.state = QStyle::State_Enabled;

    QImage resting(row.size(), QImage::Format_ARGB32);
    resting.fill(page);
    QPainter painter(&resting);
    list->itemDelegate()->paint(&painter, option, index);
    painter.end();

    option.state |= QStyle::State_MouseOver;
    QImage hovered(row.size(), QImage::Format_ARGB32);
    hovered.fill(page);
    QPainter hoverPainter(&hovered);
    list->itemDelegate()->paint(&hoverPainter, option, index);
    hoverPainter.end();

    // A point well right of the text, where only the wash can reach.
    const QPoint away(180, row.height() / 2);
    QVERIFY(resting.pixelColor(away) == page);
    QVERIFY(hovered.pixelColor(away) != page);
}

/*! The keyboard's arrival is drawn too: a row with the focus carries the accent
 *  ring inside its own edge, so the ring and the tint do not fight over it. */
void tst_FluentListView::theFocusRingStandsInsideTheRow()
{
    FluentListView *list = makeList();
    const QModelIndex index = list->model()->index(1, 0);
    const QRect row = list->visualRect(index);
    const QColor page = list->palette().color(QPalette::Window);

    QStyleOptionViewItem option;
    option.rect = QRect(QPoint(0, 0), row.size());
    option.palette = list->palette();
    option.state = QStyle::State_Enabled;

    QImage plain(row.size(), QImage::Format_ARGB32);
    plain.fill(page);
    QPainter painter(&plain);
    list->itemDelegate()->paint(&painter, option, index);
    painter.end();

    option.state |= QStyle::State_HasFocus;
    QImage focused(row.size(), QImage::Format_ARGB32);
    focused.fill(page);
    QPainter focusPainter(&focused);
    list->itemDelegate()->paint(&focusPainter, option, index);
    focusPainter.end();

    // The top edge, where the ring runs: the two frames part there, and only
    // there along that edge.
    const QRect strip(20, 0, row.width() - 40, 2);
    QVERIFY(differingPixels(plain, focused, strip) > 0);
    // And the words are untouched by it: the focus ring is a ring, not a
    // repaint of what the row says.
    const QRect words(12, 10, row.width() - 24, row.height() - 20);
    QVERIFY(differingPixels(plain, focused, words) == 0);
}

/*! The chosen row is the one wearing the accent: the bar at its left edge and
 *  a tint of the accent behind it, and no other row wears either. */
void tst_FluentListView::theChosenRowWearsTheAccent()
{
    FluentListView *list = makeList();
    list->setCurrentIndex(list->model()->index(1, 0));

    const QImage frame = list->grab().toImage();
    const QRect chosenBar(kBarInset, kRowHeight + kRowHeight / 2 - 6, kBarWidth, 12);
    const QRect otherBar(kBarInset, kRowHeight / 2 - 6, kBarWidth, 12);

    QVERIFY(accentPixels(frame, chosenBar) > 0);
    QCOMPARE(accentPixels(frame, otherBar), 0);
    // And the row's words take the accent too, while the ones beside it do not.
    QVERIFY(accentPixels(frame, QRect(12, kRowHeight, 80, kRowHeight)) > 0);
    QCOMPARE(accentPixels(frame, QRect(12, 0, 80, kRowHeight)), 0);
}

/*! A row that cannot be chosen is written fainter than one that can. */
void tst_FluentListView::aDisabledRowIsFainter()
{
    FluentListView *list = makeList();
    auto *model_ = qobject_cast<QStandardItemModel *>(list->model());
    QVERIFY(model_);
    model_->item(1)->setEnabled(false);

    const QImage frame = list->grab().toImage();
    const int able = inkPixels(frame, QRect(12, 0, 80, kRowHeight));
    const int unable = inkPixels(frame, QRect(12, kRowHeight, 80, kRowHeight));
    QVERIFY(able > 0);
    QVERIFY2(unable < able, qPrintable(QStringLiteral("enabled %1, disabled %2").arg(able).arg(unable)));
}

/*! The keyboard walks the same rows the pointer does. */
void tst_FluentListView::theKeyboardWalksTheRows()
{
    FluentListView *list = makeList();
    list->setFocus();
    list->setCurrentIndex(list->model()->index(0, 0));

    QTest::keyClick(list, Qt::Key_Down);
    QCOMPARE(list->currentIndex().row(), 1);
    QTest::keyClick(list, Qt::Key_Down);
    QCOMPARE(list->currentIndex().row(), 2);
    QTest::keyClick(list, Qt::Key_Up);
    QCOMPARE(list->currentIndex().row(), 1);
}

/*! The list asks for the room its rows need, and says how little it can get by
 *  with. */
void tst_FluentListView::theSizeIsTheRowsItShows()
{
    FluentListView list;
    QCOMPARE(list.sizeHint(), QSize(280, 5 * kRowHeight));
    QCOMPARE(list.minimumSizeHint(), QSize(120, 2 * kRowHeight));
    QVERIFY(list.sizeHint().height() >= list.minimumSizeHint().height());
}

/*! To an assistive tool a list of rows is a list, and the rows are its
 *  children - which is the framework's answer, kept rather than replaced. */
void tst_FluentListView::theAccessibleFaceIsAList()
{
    FluentListView *list = makeList();
    QAccessibleInterface *face = QAccessible::queryAccessibleInterface(list);
    QVERIFY(face);
    QCOMPARE(face->role(), QAccessible::List);
    QVERIFY(face->childCount() >= 1);
}

TEST_ADD(tst_FluentListView)

#include "tst_fluentlistview.moc"
