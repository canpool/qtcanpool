/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialtable.h"

#include "materialtokens.h"

#include "qxcontrols/qxstatelayer.h"

#include "qxtheme/qxthememanager.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QStyleOptionViewItem>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// A table is measured in the two rows it has: the names of the columns, which
// are read once and kept, and the rows themselves, which are read along.
constexpr int kHeaderHeight = 56;
constexpr int kRowHeight = 52;
constexpr int kPadding = 16;

// The hairline under the names and under every row: the table's own lines, and
// the ones that make it a table rather than a list.
constexpr qreal kDividerOpacity = 0.12;
// The pointer's wash is lighter here than on a list: a table is dense, and a
// wash at the list's strength would read as a chosen row.
constexpr qreal kHoverOpacity = 0.04;
constexpr qreal kSelectedOpacity = 0.08;

// The ink a column name is written in: the page's ink at six tenths.
constexpr qreal kHeaderInkOpacity = 0.6;

// What the table asks for, and how little of it it can get by with.
constexpr int kHintWidth = 480;
constexpr int kShownRows = 5;
constexpr int kLeastWidth = 160;
}   // namespace

/*!
 * The cell: the page behind it, the pointer's or the chosen row's state over
 * it, the words and the hairline under them. A cell draws the line rather than
 * the table painting a grid, because a grid drawn over cells would sit on top
 * of their washes.
 */
class MaterialTableCellDelegate : public QStyledItemDelegate
{
public:
    explicit MaterialTableCellDelegate(QObject *parent = Q_NULLPTR)
        : QStyledItemDelegate(parent)
    {
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        const QRectF cell(option.rect);
        const QColor page = option.palette.color(QPalette::Window);
        const QColor ink = option.palette.color(QPalette::WindowText);
        const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
        const bool enabled = option.state & QStyle::State_Enabled;
        const bool chosen = option.state & QStyle::State_Selected;

        painter->save();
        if (chosen) {
            painter->fillRect(cell, QxControls::QxStateLayer::layered(page, accent, kSelectedOpacity));
        } else if (option.state & QStyle::State_MouseOver) {
            painter->fillRect(cell, QxControls::QxStateLayer::layered(page, ink, kHoverOpacity));
        }

        if (option.state & QStyle::State_HasFocus) {
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->setBrush(Qt::NoBrush);
            painter->setPen(QPen(accent, 1.0));
            painter->drawRect(cell.adjusted(1.5, 1.5, -1.5, -1.5));
            painter->setRenderHint(QPainter::Antialiasing, false);
        }

        QColor words = ink;
        if (!enabled) {
            words.setAlphaF(0.38);
        }
        painter->setPen(words);
        painter->setFont(MaterialType::font(MaterialType::Scale::Body));
        const QString text = index.data(Qt::DisplayRole).toString();
        const QRectF room = cell.adjusted(kPadding, 0.0, -kPadding, -1.0);
        const QString elided = QFontMetrics(painter->font()).elidedText(text, Qt::ElideRight, int(room.width()));
        painter->drawText(room, Qt::AlignLeft | Qt::AlignVCenter, elided);

        // The line under the row, drawn by the cell that stands on it.
        painter->setPen(QPen(QxControls::QxStateLayer::layered(page, ink, kDividerOpacity), 1.0));
        painter->drawLine(QPointF(cell.left(), cell.bottom()), QPointF(cell.right(), cell.bottom()));
        painter->restore();
    }
};

/*!
 * The column name: the family's small print in its heavier cut, the page's ink
 * at six tenths, and the hairline under it. A header view is not an item view -
 * it paints its sections itself, not through a delegate - so this is the one
 * place in the table where a subclass is the way in.
 */
class MaterialTableHeader : public QHeaderView
{
public:
    explicit MaterialTableHeader(QWidget *parent = Q_NULLPTR)
        : QHeaderView(Qt::Horizontal, parent)
    {
    }
protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override
    {
        if (!rect.isValid()) {
            return;
        }
        const QColor page = palette().color(QPalette::Window);
        const QColor ink = palette().color(QPalette::WindowText);
        QColor words = ink;
        words.setAlphaF(kHeaderInkOpacity);

        painter->save();
        painter->fillRect(rect, page);
        painter->setPen(words);
        painter->setFont(MaterialType::font(MaterialType::Scale::Label, true));
        const QRectF room = QRectF(rect).adjusted(kPadding, 0.0, -kPadding, -1.0);
        const QString text =
            model() ? model()->headerData(logicalIndex, Qt::Horizontal, Qt::DisplayRole).toString() : QString();
        const QString elided = QFontMetrics(painter->font()).elidedText(text, Qt::ElideRight, int(room.width()));
        painter->drawText(room, Qt::AlignLeft | Qt::AlignVCenter, elided);
        painter->setPen(QPen(QxControls::QxStateLayer::layered(page, ink, kDividerOpacity), 1.0));
        painter->drawLine(QPointF(rect.left(), rect.bottom()), QPointF(rect.right(), rect.bottom()));
        painter->restore();
    }
};

class MaterialTablePrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialTable)

    QStandardItemModel *own = Q_NULLPTR;
};

MaterialTable::MaterialTable(QWidget *parent)
    : QTableView(parent)
    , d_ptr(new MaterialTablePrivate)
{
    QX_SET_PRIVATE(d_ptr)
    d_ptr->own = new QStandardItemModel(this);
    setModel(d_ptr->own);

    setHorizontalHeader(new MaterialTableHeader(this));
    horizontalHeader()->setFixedHeight(kHeaderHeight);
    horizontalHeader()->setSectionsClickable(false);
    horizontalHeader()->setHighlightSections(false);
    horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    verticalHeader()->setVisible(false);
    verticalHeader()->setDefaultSectionSize(kRowHeight);
    verticalHeader()->setMinimumSectionSize(kRowHeight);

    setItemDelegate(new MaterialTableCellDelegate(this));
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setShowGrid(false);
    setWordWrap(false);
    setFrameShape(QFrame::NoFrame);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setMouseTracking(true);
    // The cells are drawn by the delegate over the page, so the view keeps the
    // page's own face rather than a table's white.
    QPalette pal = palette();
    pal.setColor(QPalette::Base, pal.color(QPalette::Window));
    setPalette(pal);
}

MaterialTable::~MaterialTable() = default;

void MaterialTable::setHeaders(const QStringList &labels)
{
    if (model() != d_ptr->own) {
        return;
    }
    d_ptr->own->setColumnCount(labels.count());
    d_ptr->own->setHorizontalHeaderLabels(labels);
}

void MaterialTable::addRow(const QStringList &cells)
{
    if (model() != d_ptr->own) {
        return;
    }
    if (cells.count() > d_ptr->own->columnCount()) {
        d_ptr->own->setColumnCount(cells.count());
    }
    QList<QStandardItem *> row;
    row.reserve(cells.count());
    for (const QString &text : cells) {
        auto *cell = new QStandardItem(text);
        cell->setEditable(false);
        row.append(cell);
    }
    d_ptr->own->appendRow(row);
}

void MaterialTable::setRows(const QList<QStringList> &rows)
{
    if (model() != d_ptr->own) {
        return;
    }
    const QStringList labels = [this]() {
        QStringList names;
        for (int column = 0; column < d_ptr->own->columnCount(); ++column) {
            names.append(d_ptr->own->headerData(column, Qt::Horizontal).toString());
        }
        return names;
    }();
    d_ptr->own->clear();
    setHeaders(labels);
    for (const QStringList &row : rows) {
        addRow(row);
    }
}

int MaterialTable::rowCount() const
{
    return model() == d_ptr->own ? d_ptr->own->rowCount() : 0;
}

QString MaterialTable::cellText(int row, int column) const
{
    if (model() != d_ptr->own || row < 0 || column < 0 || row >= d_ptr->own->rowCount() ||
        column >= d_ptr->own->columnCount()) {
        return QString();
    }
    const QStandardItem *cell = d_ptr->own->item(row, column);
    return cell ? cell->text() : QString();
}

QSize MaterialTable::sizeHint() const
{
    return QSize(kHintWidth, kHeaderHeight + kShownRows * kRowHeight);
}

QSize MaterialTable::minimumSizeHint() const
{
    return QSize(kLeastWidth, kHeaderHeight + kRowHeight);
}

QX_MATERIAL_END_NAMESPACE
