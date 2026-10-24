/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentlistview.h"

#include "fluenttokens.h"

#include "qxcontrols/qxstatelayer.h"

#include "qxtheme/qxthememanager.h"

#include <QtCore/QStringList>
#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QStyleOptionViewItem>
#include <QtGui/QStandardItemModel>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// A row is forty tall, which is what an icon, a line of text and the pointer's
// own room come to; the text keeps a dozen from each side and the icon, when
// there is one, stands in that room at the left.
constexpr int kRowHeight = 40;
constexpr int kPadding = 12;
constexpr int kIconSize = 16;
constexpr int kIconGap = 12;

// The chosen row: a tint of the accent behind it, and the bar at its left that
// is what actually says "this one". The bar is short - it marks the row rather
// than outlining it.
constexpr qreal kSelectionOpacity = 0.10;
constexpr qreal kBarWidth = 3.0;
constexpr qreal kBarHeight = 16.0;
constexpr qreal kBarInset = 4.0;

// The rows a list shows before it starts scrolling, and how narrow it may get.
constexpr int kShownRows = 5;
constexpr int kLeastRows = 2;
constexpr int kHintWidth = 280;
constexpr int kLeastWidth = 120;

/*! The ink of a row: the accent when it is the one in force, the page's ink
 *  otherwise, and either of them thinned when the row cannot be chosen. */
QColor rowInk(const QStyleOptionViewItem &option, bool chosen, bool enabled)
{
    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
    QColor ink = chosen ? accent : option.palette.color(QPalette::WindowText);
    if (!enabled) {
        ink.setAlphaF(0.38);
    }
    return ink;
}
}   // namespace

/*!
 * The row: what a list decides. Everything the view does - scrolling,
 * selection, the keyboard, what is in a model - is Qt's, and this answers only
 * how a row looks.
 */
class FluentListDelegate : public QStyledItemDelegate
{
public:
    explicit FluentListDelegate(QObject *parent = Q_NULLPTR)
        : QStyledItemDelegate(parent)
    {
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        Q_UNUSED(option)
        Q_UNUSED(index)
        return QSize(0, kRowHeight);
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        const QRectF row(option.rect);
        const QColor page = option.palette.color(QPalette::Window);
        const QColor ink = option.palette.color(QPalette::WindowText);
        const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
        const bool enabled = option.state & QStyle::State_Enabled;
        const bool chosen = option.state & QStyle::State_Selected;
        const qreal radius = FluentShape::radius(FluentShape::Radius::Medium);

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);

        // The wash first: the pointer's is the ink's, thinned; the chosen row's
        // is the accent's. A chosen row keeps its tint while the pointer is
        // elsewhere - that is what makes it the one in force.
        if (chosen) {
            painter->setPen(Qt::NoPen);
            painter->setBrush(QxControls::QxStateLayer::layered(page, accent, kSelectionOpacity));
            painter->drawRoundedRect(row.adjusted(2.0, 1.0, -2.0, -1.0), radius, radius);

            painter->setBrush(accent);
            painter->drawRoundedRect(
                QRectF(row.left() + kBarInset, row.center().y() - kBarHeight / 2.0, kBarWidth, kBarHeight),
                kBarWidth / 2.0, kBarWidth / 2.0);
        } else if (option.state & QStyle::State_MouseOver) {
            painter->setPen(Qt::NoPen);
            painter->setBrush(QxControls::QxStateLayer::layered(page, ink, QxControls::QxStateLayer::hoverOpacity));
            painter->drawRoundedRect(row.adjusted(2.0, 1.0, -2.0, -1.0), radius, radius);
        }

        // The focus ring is drawn inside the row, so the ring and the tint do
        // not fight over the same edge.
        if (option.state & QStyle::State_HasFocus) {
            painter->setBrush(Qt::NoBrush);
            painter->setPen(QPen(accent, 1.0));
            painter->drawRoundedRect(row.adjusted(1.5, 0.5, -1.5, -0.5), radius, radius);
        }

        QRectF content = row.adjusted(kPadding, 0.0, -kPadding, 0.0);
        const QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
        if (!icon.isNull()) {
            const QRectF room(content.left(), row.center().y() - kIconSize / 2.0, kIconSize, kIconSize);
            icon.paint(painter, room.toRect(), Qt::AlignCenter, enabled ? QIcon::Normal : QIcon::Disabled);
            content.setLeft(content.left() + kIconSize + kIconGap);
        }

        painter->setPen(rowInk(option, chosen, enabled));
        painter->setFont(FluentType::font(FluentType::Ramp::Body));
        const QString text = index.data(Qt::DisplayRole).toString();
        // Elided by the font the row is written in, not the one the option
        // carries: a row that cuts its text to a stranger's measure leaves a
        // gap the reader cannot account for.
        const QString elided = QFontMetrics(painter->font()).elidedText(text, Qt::ElideRight, int(content.width()));
        painter->drawText(content, Qt::AlignLeft | Qt::AlignVCenter, elided);
        painter->restore();
    }
};

class FluentListViewPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentListView)

    QStandardItemModel *own = Q_NULLPTR;
};

FluentListView::FluentListView(QWidget *parent)
    : QListView(parent)
    , d_ptr(new FluentListViewPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    d_ptr->own = new QStandardItemModel(this);
    setModel(d_ptr->own);

    setItemDelegate(new FluentListDelegate(this));
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
    setUniformItemSizes(true);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    // The pointer's arrival is a state of the row, so the view has to hear
    // about it without a button held down; and what a row says is drawn by the
    // delegate, so the view itself keeps the page's own face rather than a
    // list's white.
    setMouseTracking(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Base, pal.color(QPalette::Window));
    setPalette(pal);
}

FluentListView::~FluentListView() = default;

void FluentListView::addItem(const QString &text, const QIcon &icon)
{
    if (model() != d_ptr->own) {
        return;
    }
    auto *row = new QStandardItem(text);
    row->setEditable(false);
    if (!icon.isNull()) {
        row->setData(icon, Qt::DecorationRole);
    }
    d_ptr->own->appendRow(row);
}

void FluentListView::setItems(const QStringList &texts)
{
    if (model() != d_ptr->own) {
        return;
    }
    d_ptr->own->clear();
    for (const QString &text : texts) {
        addItem(text);
    }
}

int FluentListView::count() const
{
    return model() == d_ptr->own ? d_ptr->own->rowCount() : 0;
}

QString FluentListView::itemText(int row) const
{
    if (model() != d_ptr->own || row < 0 || row >= d_ptr->own->rowCount()) {
        return QString();
    }
    return d_ptr->own->item(row)->text();
}

QSize FluentListView::sizeHint() const
{
    return QSize(kHintWidth, kShownRows * kRowHeight);
}

QSize FluentListView::minimumSizeHint() const
{
    return QSize(kLeastWidth, kLeastRows * kRowHeight);
}

QX_FLUENT_END_NAMESPACE
