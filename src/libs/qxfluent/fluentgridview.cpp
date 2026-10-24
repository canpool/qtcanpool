/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentgridview.h"

#include "fluenttokens.h"

#include "qxcontrols/qxstatelayer.h"

#include "qxtheme/qxthememanager.h"

#include <QtCore/QStringList>
#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QStyleOptionViewItem>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// A tile: the icon over the name, both centred, in a room that fits two lines
// of a caption's worth of name. The gap is what keeps two tiles from reading
// as one card.
constexpr int kTileWidth = 152;
constexpr int kTileHeight = 112;
constexpr int kTilesApart = 8;
constexpr int kIconSize = 32;
constexpr int kIconTop = 20;
constexpr int kNameGap = 10;
constexpr int kNameHeight = 32;

constexpr qreal kSelectionOpacity = 0.10;
constexpr qreal kTileRadius = 4.0;

constexpr int kHintWidth = 2 * kTileWidth + 3 * kTilesApart;
constexpr int kHintHeight = 2 * kTileHeight + 3 * kTilesApart;
}   // namespace

/*!
 * The tile: what a grid decides. The view keeps the model, the selection and
 * the reflow; this answers what a tile is made of.
 */
class FluentGridDelegate : public QStyledItemDelegate
{
public:
    explicit FluentGridDelegate(QObject *parent = Q_NULLPTR)
        : QStyledItemDelegate(parent)
    {
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        Q_UNUSED(option)
        Q_UNUSED(index)
        return QSize(kTileWidth, kTileHeight);
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        const QRectF tile(option.rect);
        const QColor page = option.palette.color(QPalette::Window);
        const QColor ink = option.palette.color(QPalette::WindowText);
        const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
        const bool enabled = option.state & QStyle::State_Enabled;
        const bool chosen = option.state & QStyle::State_Selected;

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);

        if (chosen) {
            // A tile is a card that gets picked up: the tint stands inside the
            // stroke rather than under it, so the stroke keeps its weight.
            painter->setPen(Qt::NoPen);
            painter->setBrush(QxControls::QxStateLayer::layered(page, accent, kSelectionOpacity));
            painter->drawRoundedRect(tile.adjusted(1.0, 1.0, -1.0, -1.0), kTileRadius, kTileRadius);
            painter->setBrush(Qt::NoBrush);
            painter->setPen(QPen(accent, 1.0));
            painter->drawRoundedRect(tile.adjusted(0.5, 0.5, -0.5, -0.5), kTileRadius, kTileRadius);
        } else if (option.state & QStyle::State_MouseOver) {
            painter->setPen(Qt::NoPen);
            painter->setBrush(QxControls::QxStateLayer::layered(page, ink, QxControls::QxStateLayer::hoverOpacity));
            painter->drawRoundedRect(tile.adjusted(1.0, 1.0, -1.0, -1.0), kTileRadius, kTileRadius);
        }

        if (option.state & QStyle::State_HasFocus) {
            painter->setBrush(Qt::NoBrush);
            painter->setPen(QPen(accent, 1.0));
            painter->drawRoundedRect(tile.adjusted(3.5, 3.5, -3.5, -3.5), kTileRadius, kTileRadius);
        }

        const QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
        if (!icon.isNull()) {
            const QRectF room(tile.center().x() - kIconSize / 2.0, tile.top() + kIconTop, kIconSize, kIconSize);
            icon.paint(painter, room.toRect(), Qt::AlignCenter, enabled ? QIcon::Normal : QIcon::Disabled);
        }

        QColor name = chosen ? accent : ink;
        if (!enabled) {
            name.setAlphaF(0.38);
        }
        painter->setPen(name);
        painter->setFont(FluentType::font(FluentType::Ramp::Caption));
        const QRectF room(tile.left() + 8.0, tile.top() + kIconTop + kIconSize + kNameGap, tile.width() - 16.0,
                          kNameHeight);
        const QString text = index.data(Qt::DisplayRole).toString();
        const QString elided = QFontMetrics(painter->font()).elidedText(text, Qt::ElideRight, int(room.width()));
        painter->drawText(room, Qt::AlignHCenter | Qt::AlignTop, elided);
        painter->restore();
    }
};

class FluentGridViewPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentGridView)

    QStandardItemModel *own = Q_NULLPTR;
};

FluentGridView::FluentGridView(QWidget *parent)
    : QListView(parent)
    , d_ptr(new FluentGridViewPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    d_ptr->own = new QStandardItemModel(this);
    setModel(d_ptr->own);

    setViewMode(QListView::IconMode);
    setMovement(QListView::Static);
    setResizeMode(QListView::Adjust);
    setWordWrap(false);
    setItemDelegate(new FluentGridDelegate(this));
    setSelectionMode(QAbstractItemView::SingleSelection);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
    setUniformItemSizes(true);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSpacing(kTilesApart);
    setGridSize(QSize(kTileWidth, kTileHeight));
    setMouseTracking(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Base, pal.color(QPalette::Window));
    setPalette(pal);
}

FluentGridView::~FluentGridView() = default;

void FluentGridView::addItem(const QString &text, const QIcon &icon)
{
    if (model() != d_ptr->own) {
        return;
    }
    auto *tile = new QStandardItem(text);
    tile->setEditable(false);
    if (!icon.isNull()) {
        tile->setData(icon, Qt::DecorationRole);
    }
    d_ptr->own->appendRow(tile);
}

void FluentGridView::setItems(const QStringList &texts)
{
    if (model() != d_ptr->own) {
        return;
    }
    d_ptr->own->clear();
    for (const QString &text : texts) {
        addItem(text);
    }
}

int FluentGridView::count() const
{
    return model() == d_ptr->own ? d_ptr->own->rowCount() : 0;
}

QString FluentGridView::itemText(int row) const
{
    if (model() != d_ptr->own || row < 0 || row >= d_ptr->own->rowCount()) {
        return QString();
    }
    return d_ptr->own->item(row)->text();
}

QSize FluentGridView::sizeHint() const
{
    return QSize(kHintWidth, kHintHeight);
}

QSize FluentGridView::minimumSizeHint() const
{
    return QSize(kTileWidth + 2 * kTilesApart, kTileHeight + 2 * kTilesApart);
}

QX_FLUENT_END_NAMESPACE
