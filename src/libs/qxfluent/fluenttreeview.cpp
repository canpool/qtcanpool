/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluenttreeview.h"

#include "fluenttokens.h"

#include "qxcontrols/qxstatelayer.h"

#include "qxtheme/qxthememanager.h"

#include <QtCore/QStringList>
#include <QtCore/QVector>
#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include <QtGui/QPolygonF>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QStyleOptionViewItem>
#include <QtWidgets/QStyledItemDelegate>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// A node: denser than a list row because a tree is followed rather than
// scanned, and a level further in adds sixteen to its left.
constexpr int kRowHeight = 32;
constexpr int kIndent = 16;
constexpr int kPadding = 8;
constexpr int kChevronSize = 12;
constexpr int kChevronGap = 8;

constexpr qreal kSelectionOpacity = 0.10;
constexpr qreal kBarWidth = 3.0;
constexpr qreal kBarHeight = 16.0;
constexpr qreal kBarInset = 2.0;

constexpr int kHintWidth = 280;
constexpr int kShownRows = 6;
constexpr int kLeastWidth = 120;
constexpr int kLeastRows = 2;

/*! The chevron at a node's left: a corner turned to point right when the node
 *  is closed and down when it is open - which is the whole of what an open node
 *  looks like from the outside. */
void drawChevron(QPainter *painter, const QRectF &room, bool open, const QColor &color)
{
    const qreal half = room.width() / 2.0;
    const QPointF middle = room.center();
    QPolygonF corner;
    if (open) {
        corner << QPointF(middle.x() - half, middle.y() - half / 2.0)
               << QPointF(middle.x() + half, middle.y() - half / 2.0) << QPointF(middle.x(), middle.y() + half / 2.0);
    } else {
        corner << QPointF(middle.x() - half / 2.0, middle.y() - half) << QPointF(middle.x() + half / 2.0, middle.y())
               << QPointF(middle.x() - half / 2.0, middle.y() + half);
    }
    painter->setPen(Qt::NoPen);
    painter->setBrush(color);
    painter->drawPolygon(corner);
}
}   // namespace

/*! The node: the chevron that says there is more, the words, and the states the
 *  list already taught. */
class FluentTreeDelegate : public QStyledItemDelegate
{
public:
    explicit FluentTreeDelegate(QObject *parent = Q_NULLPTR)
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
        const bool more = option.state & QStyle::State_Children;
        const qreal radius = FluentShape::radius(FluentShape::Radius::Medium);

        painter->save();
        painter->setRenderHint(QPainter::Antialiasing, true);

        if (chosen) {
            painter->setPen(Qt::NoPen);
            painter->setBrush(QxControls::QxStateLayer::layered(page, accent, kSelectionOpacity));
            painter->drawRoundedRect(row.adjusted(1.0, 1.0, -1.0, -1.0), radius, radius);
            painter->setBrush(accent);
            painter->drawRoundedRect(
                QRectF(row.left() + kBarInset, row.center().y() - kBarHeight / 2.0, kBarWidth, kBarHeight),
                kBarWidth / 2.0, kBarWidth / 2.0);
        } else if (option.state & QStyle::State_MouseOver) {
            painter->setPen(Qt::NoPen);
            painter->setBrush(QxControls::QxStateLayer::layered(page, ink, QxControls::QxStateLayer::hoverOpacity));
            painter->drawRoundedRect(row.adjusted(1.0, 1.0, -1.0, -1.0), radius, radius);
        }

        if (option.state & QStyle::State_HasFocus) {
            painter->setBrush(Qt::NoBrush);
            painter->setPen(QPen(accent, 1.0));
            painter->drawRoundedRect(row.adjusted(1.5, 0.5, -1.5, -0.5), radius, radius);
        }

        // The gutter the chevron stands in is kept whether or not there is a
        // mark in it: a name that jumped twenty pixels sideways the moment its
        // node gained a child would make a tree unreadable sideways.
        QRectF content = row.adjusted(kPadding, 0.0, -kPadding, 0.0);
        if (more) {
            QColor chevron = chosen ? accent : ink;
            if (!enabled) {
                chevron.setAlphaF(0.38);
            }
            drawChevron(painter,
                        QRectF(content.left(), row.center().y() - kChevronSize / 2.0, kChevronSize, kChevronSize),
                        option.state & QStyle::State_Open, chevron);
        }
        content.setLeft(content.left() + kChevronSize + kChevronGap);

        QColor words = chosen ? accent : ink;
        if (!enabled) {
            words.setAlphaF(0.38);
        }
        painter->setPen(words);
        painter->setFont(FluentType::font(FluentType::Ramp::Body));
        const QString text = index.data(Qt::DisplayRole).toString();
        const QString elided = QFontMetrics(painter->font()).elidedText(text, Qt::ElideRight, int(content.width()));
        painter->drawText(content, Qt::AlignLeft | Qt::AlignVCenter, elided);
        painter->restore();
    }
};

class FluentTreeViewPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentTreeView)

    QStandardItemModel *own = Q_NULLPTR;
};

FluentTreeView::FluentTreeView(QWidget *parent)
    : QTreeView(parent)
    , d_ptr(new FluentTreeViewPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    d_ptr->own = new QStandardItemModel(this);
    setModel(d_ptr->own);

    setItemDelegate(new FluentTreeDelegate(this));
    // The branch is drawn by the delegate, so the view's own decoration - the
    // arrows in the gutter - would be a second one beside it.
    setRootIsDecorated(false);
    setIndentation(kIndent);
    setUniformRowHeights(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setHeaderHidden(true);
    setFrameShape(QFrame::NoFrame);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setExpandsOnDoubleClick(true);
    setMouseTracking(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Base, pal.color(QPalette::Window));
    setPalette(pal);
}

FluentTreeView::~FluentTreeView() = default;

QStandardItem *FluentTreeView::addNode(const QString &text)
{
    if (model() != d_ptr->own) {
        return Q_NULLPTR;
    }
    auto *node = new QStandardItem(text);
    node->setEditable(false);
    d_ptr->own->appendRow(node);
    return node;
}

QStandardItem *FluentTreeView::addChild(QStandardItem *parent, const QString &text)
{
    if (model() != d_ptr->own || !parent) {
        return Q_NULLPTR;
    }
    auto *node = new QStandardItem(text);
    node->setEditable(false);
    parent->appendRow(node);
    return node;
}

int FluentTreeView::nodeCount() const
{
    if (model() != d_ptr->own) {
        return 0;
    }
    // Counted by walking, because a tree's items are not a flat list: every
    // level has its own row count.
    int count = 0;
    QVector<QStandardItem *> queue;
    for (int row = 0; row < d_ptr->own->rowCount(); ++row) {
        queue.append(d_ptr->own->item(row));
    }
    while (!queue.isEmpty()) {
        QStandardItem *item = queue.takeLast();
        ++count;
        for (int row = 0; row < item->rowCount(); ++row) {
            queue.append(item->child(row));
        }
    }
    return count;
}

QSize FluentTreeView::sizeHint() const
{
    return QSize(kHintWidth, kShownRows * kRowHeight);
}

QSize FluentTreeView::minimumSizeHint() const
{
    return QSize(kLeastWidth, kLeastRows * kRowHeight);
}

QX_FLUENT_END_NAMESPACE
