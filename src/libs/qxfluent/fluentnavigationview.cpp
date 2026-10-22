/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentnavigationview.h"

#include "fluenttokens.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QWidget>
// QAction is in Widgets on Qt 5 and in Gui on Qt 6, so it is asked for without
// naming a module - the same way the rest of the libraries ask.
#include <QAction>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// Fluent's navigation measures: a pane of three hundred and twenty with its
// words, forty-eight without them, a menu button and destinations forty tall,
// an icon of sixteen, and a bar three wide marking the one in force.
constexpr qreal kOpenPaneWidth = 320.0;
constexpr qreal kCompactPaneWidth = 48.0;
constexpr qreal kMenuButtonHeight = 40.0;
constexpr qreal kItemHeight = 40.0;
constexpr qreal kItemPadding = 16.0;
constexpr qreal kIconSize = 16.0;
constexpr qreal kIconGap = 12.0;
constexpr qreal kSelectionWidth = 3.0;
constexpr qreal kSelectionHeight = 16.0;
constexpr qreal kHamburgerGlyph = 16.0;
constexpr qreal kHamburgerStep = 5.0;

// The mark of the destination in force is the accent, both as the bar and as a
// wash under the whole row; a destination nobody can use is the same drawing
// thinned. Neither moves the geometry.
constexpr qreal kSelectedOpacity = 0.08;
constexpr qreal kDisabledOpacity = 0.4;
constexpr qreal kDividerOpacity = 0.10;

// What itemAt() answers for the menu button, which is a place on the pane no
// destination owns.
constexpr int kMenuIndex = -2;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}

/*! The three bars that fold and unfold the pane: drawn rather than typed,
 *  because a glyph in a font is a glyph the theme cannot colour and an
 *  assistive tool would read. */
void paintHamburger(QPainter *painter, const QRectF &rect, const QColor &color)
{
    const qreal left = rect.center().x() - kHamburgerGlyph / 2.0;
    const qreal middle = rect.center().y();
    painter->setPen(QPen(color, 1.5, Qt::SolidLine, Qt::RoundCap));
    for (int step = -1; step <= 1; ++step) {
        const qreal y = middle + step * kHamburgerStep;
        painter->drawLine(QLineF(left, y, left + kHamburgerGlyph, y));
    }
}
}   // namespace

struct NavigationItem {
    QAction *action = Q_NULLPTR;
    QRectF rect;
};

class FluentNavigationViewPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentNavigationView)

    QList<NavigationItem> items;
    int hovered = -1;
    int armed = -1;
    int current = -1;
    int focused = -1;
    bool paneOpen = true;
    bool menuHovered = false;
    QWidget *content = Q_NULLPTR;

    qreal paneWidth() const;
    QRectF menuButtonRect() const;
    void relayout();
    int itemAt(const QPointF &pos) const;
    void activate(int index);
};

qreal FluentNavigationViewPrivate::paneWidth() const
{
    return paneOpen ? kOpenPaneWidth : kCompactPaneWidth;
}

QRectF FluentNavigationViewPrivate::menuButtonRect() const
{
    return QRectF(0.0, 0.0, paneWidth(), kMenuButtonHeight);
}

void FluentNavigationViewPrivate::relayout()
{
    Q_Q(FluentNavigationView);
    const qreal width = paneWidth();
    qreal top = kMenuButtonHeight;
    for (NavigationItem &item : items) {
        item.rect = QRectF(0.0, top, width, kItemHeight);
        top += kItemHeight;
    }
    if (content) {
        const int pane = int(width);
        content->setGeometry(pane, 0, qMax(0, q->width() - pane), q->height());
    }
}

int FluentNavigationViewPrivate::itemAt(const QPointF &pos) const
{
    // The pane may have been resized since it was last asked; where its items
    // stand is answered now, because a hit test against stale geometry is a
    // lie - and a view that has never been shown is never told it was resized.
    const_cast<FluentNavigationViewPrivate *>(this)->relayout();
    for (int i = 0; i < items.count(); ++i) {
        if (items.at(i).rect.contains(pos)) {
            return i;
        }
    }
    return -1;
}

void FluentNavigationViewPrivate::activate(int index)
{
    Q_Q(FluentNavigationView);
    if (index < 0 || index >= items.count()) {
        return;
    }
    const NavigationItem &item = items.at(index);
    if (!item.action->isEnabled()) {
        return;
    }
    q->setCurrentIndex(index);
    item.action->trigger();
}

FluentNavigationView::FluentNavigationView(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentNavigationViewPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // Which destination the pointer is on is part of what the pane says, so
    // moves are wanted without a button - the same reason a rating wants them.
    setMouseTracking(true);
}

FluentNavigationView::~FluentNavigationView(){QX_FINI_PRIVATE()}

QAction *FluentNavigationView::addItem(const QString &text, const QIcon &icon)
{
    QAction *action = new QAction(icon, text, this);
    NavigationItem item;
    item.action = action;
    d_ptr->items.append(item);
    if (d_ptr->focused < 0) {
        d_ptr->focused = 0;
    }
    d_ptr->relayout();
    updateGeometry();
    update();
    return action;
}

void FluentNavigationView::removeItem(QAction *action)
{
    for (int i = 0; i < d_ptr->items.count(); ++i) {
        if (d_ptr->items.at(i).action == action) {
            d_ptr->items.removeAt(i);
            // The destination in force may be the one leaving, and an index is
            // a position in a list that just got shorter: what was said of the
            // one that left is said of its neighbour now, or of nothing.
            if (d_ptr->current >= d_ptr->items.count()) {
                d_ptr->current = d_ptr->items.isEmpty() ? -1 : d_ptr->items.count() - 1;
            }
            if (d_ptr->focused >= d_ptr->items.count()) {
                d_ptr->focused = d_ptr->items.isEmpty() ? -1 : d_ptr->items.count() - 1;
            }
            d_ptr->hovered = -1;
            delete action;
            d_ptr->relayout();
            updateGeometry();
            update();
            return;
        }
    }
}

void FluentNavigationView::clear()
{
    if (d_ptr->items.isEmpty()) {
        return;
    }
    const QList<NavigationItem> items = d_ptr->items;
    d_ptr->items.clear();
    d_ptr->current = -1;
    d_ptr->focused = -1;
    d_ptr->hovered = -1;
    for (const NavigationItem &item : items) {
        delete item.action;
    }
    d_ptr->relayout();
    updateGeometry();
    update();
}

QList<QAction *> FluentNavigationView::items() const
{
    QList<QAction *> actions;
    actions.reserve(d_ptr->items.count());
    for (const NavigationItem &item : d_ptr->items) {
        actions.append(item.action);
    }
    return actions;
}

QAction *FluentNavigationView::currentItem() const
{
    if (d_ptr->current < 0 || d_ptr->current >= d_ptr->items.count()) {
        return Q_NULLPTR;
    }
    return d_ptr->items.at(d_ptr->current).action;
}

int FluentNavigationView::currentIndex() const
{
    return d_ptr->current;
}

void FluentNavigationView::setContent(QWidget *widget)
{
    if (d_ptr->content == widget) {
        return;
    }
    if (d_ptr->content) {
        d_ptr->content->setParent(Q_NULLPTR);
    }
    d_ptr->content = widget;
    if (widget) {
        widget->setParent(this);
    }
    d_ptr->relayout();
    update();
}

QWidget *FluentNavigationView::contentWidget() const
{
    return d_ptr->content;
}

bool FluentNavigationView::isPaneOpen() const
{
    return d_ptr->paneOpen;
}

void FluentNavigationView::setPaneOpen(bool open)
{
    if (d_ptr->paneOpen == open) {
        return;
    }
    d_ptr->paneOpen = open;
    d_ptr->relayout();
    updateGeometry();
    update();
    Q_EMIT paneOpenChanged(open);
}

void FluentNavigationView::setCurrentIndex(int index)
{
    if (d_ptr->current == index) {
        return;
    }
    if (index >= 0 && index >= d_ptr->items.count()) {
        return;
    }
    d_ptr->current = index;
    update();
    Q_EMIT currentIndexChanged(index);
    Q_EMIT currentItemChanged(currentItem());
}

void FluentNavigationView::setCurrentItem(QAction *action)
{
    for (int i = 0; i < d_ptr->items.count(); ++i) {
        if (d_ptr->items.at(i).action == action) {
            setCurrentIndex(i);
            return;
        }
    }
}

QSize FluentNavigationView::sizeHint() const
{
    return QSize(int(kOpenPaneWidth) + 400, int(kMenuButtonHeight) + 8 * int(kItemHeight));
}

QSize FluentNavigationView::minimumSizeHint() const
{
    return QSize(int(kCompactPaneWidth) + 80, int(kMenuButtonHeight) + int(kItemHeight));
}

QRectF FluentNavigationView::chromeRect() const
{
    // Nothing about this view is the control: it is a column of destinations
    // and a room beside them, and a wash round the whole of it would be a wash
    // round the content as well. The destinations draw their own.
    return QRectF();
}

void FluentNavigationView::paintSurface(QPainter *painter)
{
    // The pane is a layer of its own: the destinations stand on it, and it is
    // told from the content beside it by that and by the line between them.
    const qreal pane = d_ptr->paneWidth();
    painter->fillRect(QRectF(0.0, 0.0, pane, qreal(height())), palette().color(QPalette::AlternateBase));
    QColor divider = textColor();
    divider.setAlphaF(kDividerOpacity);
    painter->setPen(divider);
    painter->drawLine(QLineF(pane - 0.5, 0.0, pane - 0.5, qreal(height())));
}

void FluentNavigationView::paintContent(QPainter *painter)
{
    const bool enabled = isEnabled();
    if (d_ptr->menuHovered) {
        QxControls::QxStateLayer::paint(painter, d_ptr->menuButtonRect(), textColor(),
                                        QxControls::QxStateLayer::hoverOpacity);
    }
    painter->setOpacity(enabled ? 1.0 : kDisabledOpacity);
    paintHamburger(painter, d_ptr->menuButtonRect(), textColor());

    const QFont font = FluentType::font(FluentType::Ramp::Body);
    const QFontMetrics metrics(font);
    const bool withWords = d_ptr->paneOpen;
    const QColor accent = accentColor();

    for (int i = 0; i < d_ptr->items.count(); ++i) {
        const NavigationItem &item = d_ptr->items.at(i);
        if (!item.rect.intersects(rect())) {
            continue;
        }
        const bool usable = item.action->isEnabled();
        painter->setOpacity(enabled ? (usable ? 1.0 : kDisabledOpacity) : kDisabledOpacity);

        if (i == d_ptr->current) {
            QxControls::QxStateLayer::paint(painter, item.rect, accent, kSelectedOpacity);
            // The bar at the leading edge: Fluent marks the destination in
            // force here rather than by filling the row, so the mark survives
            // at a glance and does not read as a pressed button.
            const QRectF bar(item.rect.left() + (kItemPadding - kSelectionWidth) / 2.0,
                             item.rect.center().y() - kSelectionHeight / 2.0, kSelectionWidth, kSelectionHeight);
            painter->setPen(Qt::NoPen);
            painter->setBrush(accent);
            painter->drawRoundedRect(bar, kSelectionWidth / 2.0, kSelectionWidth / 2.0);
        }
        if (i == d_ptr->hovered) {
            QxControls::QxStateLayer::paint(painter, item.rect, textColor(), QxControls::QxStateLayer::hoverOpacity);
        }
        if (hasFocus() && i == d_ptr->focused) {
            QxControls::QxStateLayer::paint(painter, item.rect, textColor(), QxControls::QxStateLayer::focusOpacity);
        }

        // Folded, the pane is its icons and nothing else: the icon takes the
        // row's middle, and the words are not drawn at all.
        const qreal iconLeft = withWords ? kItemPadding : (item.rect.width() - kIconSize) / 2.0;
        const QRectF iconRect(item.rect.left() + iconLeft, item.rect.center().y() - kIconSize / 2.0, kIconSize,
                              kIconSize);
        if (!item.action->icon().isNull()) {
            item.action->icon().paint(painter, iconRect.toRect(), Qt::AlignCenter,
                                      usable ? QIcon::Normal : QIcon::Disabled);
        }

        if (withWords) {
            const QString whole = item.action->text();
            const qreal textLeft = iconRect.right() + kIconGap;
            const qreal room = item.rect.right() - textLeft - kItemPadding;
            const QString text = room > 0 ? metrics.elidedText(whole, Qt::ElideRight, int(room)) : QString();
            if (!text.isEmpty()) {
                painter->setFont(font);
                painter->setPen(textColor());
                painter->drawText(QRectF(textLeft, item.rect.top(), room, item.rect.height()),
                                  Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, text);
            }
        }
    }
    painter->setOpacity(1.0);
}

void FluentNavigationView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    d_ptr->relayout();
    QxControls::QxControl::resizeEvent(event);
}

void FluentNavigationView::mousePressEvent(QMouseEvent *event)
{
    const QPointF pos = eventPos(event);
    if (d_ptr->menuButtonRect().contains(pos)) {
        d_ptr->armed = kMenuIndex;
    } else {
        d_ptr->armed = d_ptr->itemAt(pos);
    }
    update();
}

void FluentNavigationView::mouseReleaseEvent(QMouseEvent *event)
{
    const QPointF pos = eventPos(event);
    const int armed = d_ptr->armed;
    d_ptr->armed = -1;
    if (armed == kMenuIndex) {
        if (d_ptr->menuButtonRect().contains(pos)) {
            setPaneOpen(!d_ptr->paneOpen);
        }
    } else if (armed >= 0 && d_ptr->itemAt(pos) == armed) {
        d_ptr->activate(armed);
        d_ptr->focused = armed;
    }
    update();
}

void FluentNavigationView::mouseMoveEvent(QMouseEvent *event)
{
    const QPointF pos = eventPos(event);
    const bool menu = d_ptr->menuButtonRect().contains(pos);
    const int hovered = menu ? -1 : d_ptr->itemAt(pos);
    if (menu != d_ptr->menuHovered || hovered != d_ptr->hovered) {
        d_ptr->menuHovered = menu;
        d_ptr->hovered = hovered;
        update();
    }
}

void FluentNavigationView::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    d_ptr->hovered = -1;
    d_ptr->menuHovered = false;
    update();
}

void FluentNavigationView::keyPressEvent(QKeyEvent *event)
{
    const int count = d_ptr->items.count();
    if (count == 0) {
        QWidget::keyPressEvent(event);
        return;
    }
    if (d_ptr->focused < 0) {
        d_ptr->focused = 0;
    }
    switch (event->key()) {
    case Qt::Key_Down:
        d_ptr->focused = qMin(d_ptr->focused + 1, count - 1);
        update();
        event->accept();
        return;
    case Qt::Key_Up:
        d_ptr->focused = qMax(d_ptr->focused - 1, 0);
        update();
        event->accept();
        return;
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Space:
        d_ptr->activate(d_ptr->focused);
        event->accept();
        return;
    default:
        break;
    }
    QxControls::QxControl::keyPressEvent(event);
}

QAccessible::Role FluentNavigationView::accessibleRole() const
{
    // It is a list of destinations with a room beside them, and what an
    // assistive tool is meant to walk is the list.
    return QAccessible::List;
}

QX_FLUENT_END_NAMESPACE
