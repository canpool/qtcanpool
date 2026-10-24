/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialtable.h"

#include "qxcontrols/qxstatelayer.h"

#include "qxtheme/qxthememanager.h"

#include <QtGui/QPainter>
#include <QtGui/QStandardItemModel>
#include <QAccessible>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QStyleOptionViewItem>

QX_MATERIAL_USE_NAMESPACE

namespace
{
// The table's own measures, read the way a reader reads them.
constexpr int kHeaderHeight = 56;
constexpr int kRowHeight = 52;

/*! The darkest pixel in \a room: what the ink of a cell comes to where the
 *  glyphs are thickest. Counting pixels under a threshold instead would answer
 *  for the anti-aliasing as much as for the ink - a fainter text spreads more
 *  of its pixels just under the page and can count higher than a solid one. */
int darkestPixel(const QImage &image, const QRect &room)
{
    int darkest = 255;
    const QRect room_ = room.intersected(image.rect());
    for (int y = room_.top(); y <= room_.bottom(); ++y) {
        for (int x = room_.left(); x <= room_.right(); ++x) {
            darkest = qMin(darkest, image.pixelColor(x, y).lightness());
        }
    }
    return darkest;
}

/*! Whether \a pixel is \a wanted, allowance made for compositing and rounding. */
bool near(const QColor &pixel, const QColor &wanted, int tolerance = 2)
{
    return qAbs(pixel.red() - wanted.red()) <= tolerance && qAbs(pixel.green() - wanted.green()) <= tolerance &&
           qAbs(pixel.blue() - wanted.blue()) <= tolerance;
}
}   // namespace

class tst_MaterialTable : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void theNamesAndTheRowsAreMeasuredApart();
    void whatTheTableHoldsIsWhatItSays();
    void theNamesCarryTheHairlineUnderThem();
    void thePointerWashesTheCellItIsOver();
    void theChosenRowWearsTheTint();
    void aDisabledCellIsFainter();
    void theKeyboardWalksTheRows();
    void theSizeIsTheRowsItShows();
    void theAccessibleFaceIsATable();
private:
    MaterialTable *makeTable();
private:
    QWidget *m_page = nullptr;
};

void tst_MaterialTable::init()
{
    qtcGiveFonts();
    m_page = new QWidget;
    m_page->resize(520, 360);
    m_page->show();
}

void tst_MaterialTable::cleanup()
{
    delete m_page;
    m_page = nullptr;
}

MaterialTable *tst_MaterialTable::makeTable()
{
    auto *table = new MaterialTable(m_page);
    table->setGeometry(0, 0, 480, 300);
    table->setHeaders(QStringList() << QStringLiteral("Name") << QStringLiteral("Status"));
    // Two columns that fill the width, so a probe at the right edge is over a
    // cell rather than the viewport's bare floor.
    table->setColumnWidth(0, 240);
    table->setColumnWidth(1, 240);
    table->addRow(QStringList() << QStringLiteral("Alpha") << QStringLiteral("Open"));
    table->addRow(QStringList() << QStringLiteral("Beta") << QStringLiteral("Closed"));
    table->addRow(QStringList() << QStringLiteral("Gamma") << QStringLiteral("Open"));
    table->show();
    return table;
}

/*! The names are read once and the rows are read along, so they are measured
 *  apart: fifty-six over fifty-two. */
void tst_MaterialTable::theNamesAndTheRowsAreMeasuredApart()
{
    MaterialTable *table = makeTable();
    QCOMPARE(table->horizontalHeader()->height(), kHeaderHeight);
    QCOMPARE(table->visualRect(table->model()->index(0, 0)).height(), kRowHeight);
    QCOMPARE(table->visualRect(table->model()->index(1, 0)).top(), kRowHeight);
    QCOMPARE(table->visualRect(table->model()->index(2, 0)).top(), 2 * kRowHeight);
}

/*! The rows added by hand are the rows the table says it has, and a cell that
 *  is not there says nothing rather than something wrong. */
void tst_MaterialTable::whatTheTableHoldsIsWhatItSays()
{
    MaterialTable *table = makeTable();
    QCOMPARE(table->rowCount(), 3);
    QCOMPARE(table->cellText(1, 1), QStringLiteral("Closed"));
    QVERIFY(table->cellText(3, 0).isEmpty());
    QVERIFY(table->cellText(0, 2).isEmpty());
    QVERIFY(table->cellText(-1, 0).isEmpty());

    table->setRows(QList<QStringList>() << (QStringList() << QStringLiteral("One") << QStringLiteral("A")));
    QCOMPARE(table->rowCount(), 1);
    QCOMPARE(table->cellText(0, 0), QStringLiteral("One"));
    // The names are the table's, not the rows': replacing the rows keeps them.
    QCOMPARE(table->model()->headerData(1, Qt::Horizontal).toString(), QStringLiteral("Status"));
}

/*! Under the names runs the hairline that says where the table starts. */
void tst_MaterialTable::theNamesCarryTheHairlineUnderThem()
{
    MaterialTable *table = makeTable();
    const QImage header = table->horizontalHeader()->grab().toImage();
    const QColor page = table->palette().color(QPalette::Window);
    const QColor line = QxControls::QxStateLayer::layered(page, table->palette().color(QPalette::WindowText), 0.12);

    const QColor pixel = header.pixelColor(40, header.height() - 1);
    QVERIFY2(near(pixel, line, 3), qPrintable(QStringLiteral("bottom %1,%2,%3 vs line %4,%5,%6")
                                                  .arg(pixel.red())
                                                  .arg(pixel.green())
                                                  .arg(pixel.blue())
                                                  .arg(line.red())
                                                  .arg(line.green())
                                                  .arg(line.blue())));
}

/*! The pointer's wash is what a hovered cell looks like: the delegate paints it
 *  when the cell arrives carrying State_MouseOver. Read off the delegate, for
 *  the reason the list view gives - a view hears the pointer through
 *  HoverEnter, which a test cannot move. */
void tst_MaterialTable::thePointerWashesTheCellItIsOver()
{
    MaterialTable *table = makeTable();
    const QModelIndex index = table->model()->index(1, 1);
    const QRect cell = table->visualRect(index);
    const QColor page = table->palette().color(QPalette::Window);

    QStyleOptionViewItem option;
    // The frame is the cell's own room, so the cell stands at its origin: an
    // option rect carried over at the cell's place in the table would paint the
    // whole cell outside the image.
    option.rect = QRect(QPoint(0, 0), cell.size());
    option.palette = table->palette();
    option.state = QStyle::State_Enabled;

    QImage resting(cell.size(), QImage::Format_ARGB32);
    resting.fill(page);
    QPainter painter(&resting);
    table->itemDelegate()->paint(&painter, option, index);
    painter.end();

    option.state |= QStyle::State_MouseOver;
    QImage hovered(cell.size(), QImage::Format_ARGB32);
    hovered.fill(page);
    QPainter hoverPainter(&hovered);
    table->itemDelegate()->paint(&hoverPainter, option, index);
    hoverPainter.end();

    // Above the words and away from the hairline under them.
    const QPoint away(cell.width() - 6, 10);
    QCOMPARE(resting.pixelColor(away), page);
    QVERIFY(hovered.pixelColor(away) != page);
}

/*! The chosen row is tinted with the accent across all of its cells. */
void tst_MaterialTable::theChosenRowWearsTheTint()
{
    MaterialTable *table = makeTable();
    table->setCurrentIndex(table->model()->index(1, 0));

    // The viewport, not the table: the frame's first row is the first row of
    // rows, not the row of column names standing over them.
    const QImage frame = table->viewport()->grab().toImage();
    const QColor page = table->palette().color(QPalette::Window);
    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
    const QColor tint = QxControls::QxStateLayer::layered(page, accent, 0.08);

    // Near the right edge of the second column, above the hairline: the tint,
    // and nothing else.
    const QPoint chosen(frame.width() - 20, kRowHeight + 10);
    const QPoint other(frame.width() - 20, 10);
    QVERIFY2(near(frame.pixelColor(chosen), tint, 3), qPrintable(QStringLiteral("chosen %1,%2,%3 vs tint %4,%5,%6")
                                                                     .arg(frame.pixelColor(chosen).red())
                                                                     .arg(frame.pixelColor(chosen).green())
                                                                     .arg(frame.pixelColor(chosen).blue())
                                                                     .arg(tint.red())
                                                                     .arg(tint.green())
                                                                     .arg(tint.blue())));
    QVERIFY(near(frame.pixelColor(other), page, 2));
}

/*! A cell that cannot be chosen is written fainter than one that can. */
void tst_MaterialTable::aDisabledCellIsFainter()
{
    MaterialTable *table = makeTable();
    auto *model_ = qobject_cast<QStandardItemModel *>(table->model());
    QVERIFY(model_);
    model_->item(1, 0)->setEnabled(false);

    const QImage frame = table->viewport()->grab().toImage();
    const int able = darkestPixel(frame, QRect(16, 8, 120, kRowHeight - 16));
    const int unable = darkestPixel(frame, QRect(16, kRowHeight + 8, 120, kRowHeight - 16));
    QVERIFY(able < 100);
    QVERIFY2(unable > able, qPrintable(QStringLiteral("enabled %1, disabled %2").arg(able).arg(unable)));
}

/*! The keyboard walks the rows the pointer does. */
void tst_MaterialTable::theKeyboardWalksTheRows()
{
    MaterialTable *table = makeTable();
    table->setFocus();
    table->setCurrentIndex(table->model()->index(0, 0));

    QTest::keyClick(table, Qt::Key_Down);
    QCOMPARE(table->currentIndex().row(), 1);
    QTest::keyClick(table, Qt::Key_Down);
    QCOMPARE(table->currentIndex().row(), 2);
    QTest::keyClick(table, Qt::Key_Up);
    QCOMPARE(table->currentIndex().row(), 1);
}

/*! The table asks for room for five rows under its names, and says how little
 *  it can get by with. */
void tst_MaterialTable::theSizeIsTheRowsItShows()
{
    MaterialTable table;
    QCOMPARE(table.sizeHint(), QSize(480, kHeaderHeight + 5 * kRowHeight));
    QCOMPARE(table.minimumSizeHint(), QSize(160, kHeaderHeight + kRowHeight));
}

/*! To an assistive tool a table is a table, with the cells as its children -
 *  the framework's answer, kept rather than replaced. */
void tst_MaterialTable::theAccessibleFaceIsATable()
{
    MaterialTable *table = makeTable();
    QAccessibleInterface *face = QAccessible::queryAccessibleInterface(table);
    QVERIFY(face);
    QCOMPARE(face->role(), QAccessible::Table);
    QVERIFY(face->childCount() >= 1);
}

TEST_ADD(tst_MaterialTable)

#include "tst_materialtable.moc"
