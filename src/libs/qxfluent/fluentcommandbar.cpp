/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentcommandbar.h"

#include "fluenttokens.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QResizeEvent>
// QAction is in Widgets on Qt 5 and in Gui on Qt 6, so it is asked for without
// naming a module - the same way the rest of the libraries ask.
#include <QAction>
#include <QMenu>
#include <QWidget>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// Fluent's command bar measures: a forty tall row holding thirty-two tall
// commands, an icon of sixteen, and a chevron of twenty for the two roles
// that carry a menu.
constexpr qreal kBarHeight = 40.0;
constexpr qreal kItemHeight = 32.0;
constexpr qreal kItemPadding = 8.0;
constexpr qreal kIconSize = 16.0;
constexpr qreal kIconGap = 8.0;
constexpr qreal kChevronWidth = 20.0;
constexpr qreal kChevronGlyph = 8.0;
constexpr qreal kItemSpacing = 2.0;
constexpr qreal kOverflowWidth = 32.0;
constexpr qreal kDotRadius = 2.0;
constexpr qreal kDotStep = 6.0;

// A command that is in force is washed in the accent, and a command nobody
// can use is the same drawing thinned: neither moves the geometry.
constexpr qreal kCheckedOpacity = 0.12;
constexpr qreal kDisabledOpacity = 0.4;
constexpr qreal kDividerOpacity = 0.10;
constexpr qreal kDividerInset = 6.0;

// What itemAt() answers for the overflow button, which is a place on the bar
// that no command owns.
constexpr int kOverflowIndex = -2;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}

/*! A command is centred in the bar that holds it, whatever height that bar has
 *  been given. `kBarHeight` is what the bar asks for; a layout is free to give
 *  it more, and one that does is the reason this takes the widget rather than
 *  the constant: the row used to sit against the top edge of a taller bar. */
QRectF centeredVertically(const QWidget *bar, qreal left, qreal width, qreal height)
{
    return QRectF(left, (bar->height() - height) / 2.0, width, height);
}

/*! The chevron that says a menu is here: a point turned down, drawn rather
 *  than typed, because a glyph in a font is a glyph the theme cannot colour
 *  and an assistive tool would read. */
void paintChevron(QPainter *painter, const QRectF &rect, const QColor &color)
{
    const qreal x = rect.center().x() - kChevronGlyph / 2.0;
    const qreal y = rect.center().y() - kChevronGlyph / 4.0;
    QPainterPath chevron;
    chevron.moveTo(x, y);
    chevron.lineTo(x + kChevronGlyph, y);
    chevron.lineTo(x + kChevronGlyph / 2.0, y + kChevronGlyph / 2.0 + kChevronGlyph / 4.0);
    painter->setPen(QPen(color, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawPath(chevron);
}
}   // namespace

struct CommandItem {
    QAction *action = Q_NULLPTR;
    FluentCommandBar::ItemRole role = FluentCommandBar::ItemRole::Button;
    /*! Empty while the command is in the overflow. */
    QRectF rect;
    /*! The dropdown half of a Split; empty for every other role. */
    QRectF chevron;
};

class FluentCommandBarPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentCommandBar)

    QList<CommandItem> items;
    int hovered = -1;
    int focused = -1;
    int armed = -1;
    QMenu *overflow = Q_NULLPTR;
    /*! What fits is a function of the room and of the commands, so it is
     *  answered when it is asked for and not before: a bar that is being
     *  resized is asked many times and a bar nobody has drawn yet has never
     *  been given a room at all. */
    int laidOutWidth = -1;
    /*! Centring depends on the height as much as on the width: a bar given more
     *  room is laid out again, and without this its row would keep the offset
     *  it was given when it was shorter. */
    int laidOutHeight = -1;
    bool dirty = true;

    int itemWidth(const CommandItem &item) const;
    void relayout();
    QRectF overflowRect() const;
    int itemAt(const QPointF &pos) const;
    void activate(int index, const QPointF &pos);
    void openMenu(const CommandItem &item);
};

int FluentCommandBarPrivate::itemWidth(const CommandItem &item) const
{
    const QFontMetrics metrics(FluentType::font(FluentType::Ramp::Body));
    const QString text = item.action->text();
    int width = int(kItemPadding);
    if (!item.action->icon().isNull()) {
        width += int(kIconSize + kIconGap);
    }
    if (!text.isEmpty()) {
        width += metrics.horizontalAdvance(text);
    }
    width += int(kItemPadding);
    if (item.role == FluentCommandBar::ItemRole::DropDown || item.role == FluentCommandBar::ItemRole::Split) {
        width += int(kChevronWidth);
    }
    return qMax(width, int(kItemHeight));
}

QRectF FluentCommandBarPrivate::overflowRect() const
{
    Q_Q(const FluentCommandBar);
    return centeredVertically(q, q->width() - kItemPadding / 2.0 - kOverflowWidth, kOverflowWidth, kItemHeight);
}

void FluentCommandBarPrivate::relayout()
{
    Q_Q(FluentCommandBar);
    if (!dirty && laidOutWidth == q->width() && laidOutHeight == q->height()) {
        return;
    }
    dirty = false;
    laidOutWidth = q->width();
    laidOutHeight = q->height();

    for (CommandItem &item : items) {
        item.rect = QRectF();
        item.chevron = QRectF();
    }
    // How many fit is not a sum and a compare: while anything is left over,
    // the room the overflow button needs is not the row's to give away.
    const int available = int(q->width() - 2 * kItemPadding);
    int used = 0;
    int last = -1;
    for (int i = 0; i < items.count(); ++i) {
        const CommandItem &item = items.at(i);
        if (!item.action->isVisible()) {
            continue;
        }
        bool more = false;
        for (int j = i + 1; j < items.count() && !more; ++j) {
            more = items.at(j).action->isVisible();
        }
        const int reserve = more ? int(kItemSpacing + kOverflowWidth) : 0;
        const int width = itemWidth(item);
        const int next = used + (last >= 0 ? int(kItemSpacing) : 0) + width;
        if (next + reserve > available) {
            break;
        }
        used = next;
        last = i;
    }

    qreal x = kItemPadding;
    for (int i = 0; i <= last; ++i) {
        CommandItem &item = items[i];
        if (!item.action->isVisible()) {
            continue;
        }
        const qreal width = itemWidth(item);
        item.rect = centeredVertically(q, x, width, kItemHeight);
        if (item.role == FluentCommandBar::ItemRole::Split) {
            item.chevron = QRectF(item.rect.right() - kChevronWidth, item.rect.top(), kChevronWidth, kItemHeight);
        }
        x += width + kItemSpacing;
    }

    // Everything that did not fit is not dropped: it is what the overflow
    // holds. The menu is only touched when the answer changed, and emptying it
    // means removing rather than clearing - the commands in it belong to this
    // bar, and a menu that was cleared would be free to delete what it never
    // owned.
    QList<QAction *> wanted;
    for (int i = last + 1; i < items.count(); ++i) {
        if (items.at(i).action->isVisible()) {
            wanted.append(items.at(i).action);
        }
    }
    if (wanted != overflow->actions()) {
        for (QAction *action : overflow->actions()) {
            overflow->removeAction(action);
        }
        for (QAction *action : wanted) {
            overflow->addAction(action);
        }
    }

    if (focused >= 0 && (focused >= items.count() || items.at(focused).rect.isEmpty())) {
        focused = -1;
    }
    if (hovered >= 0 && (hovered >= items.count() || items.at(hovered).rect.isEmpty())) {
        hovered = -1;
    }
}

int FluentCommandBarPrivate::itemAt(const QPointF &pos) const
{
    // The row may have been resized since it was last asked; what fits is
    // answered now, because a hit test against stale geometry is a lie.
    const_cast<FluentCommandBarPrivate *>(this)->relayout();
    if (!overflow->isEmpty() && overflowRect().contains(pos)) {
        return kOverflowIndex;
    }
    for (int i = 0; i < items.count(); ++i) {
        if (!items.at(i).rect.isEmpty() && items.at(i).rect.contains(pos)) {
            return i;
        }
    }
    return -1;
}

void FluentCommandBarPrivate::openMenu(const CommandItem &item)
{
    Q_Q(FluentCommandBar);
    QMenu *menu = item.action->menu();
    if (!menu) {
        return;
    }
    menu->popup(q->mapToGlobal(item.rect.bottomLeft().toPoint()));
}

void FluentCommandBarPrivate::activate(int index, const QPointF &pos)
{
    Q_Q(FluentCommandBar);
    if (index == kOverflowIndex) {
        overflow->popup(q->mapToGlobal(overflowRect().bottomLeft().toPoint()));
        return;
    }
    if (index < 0 || index >= items.count()) {
        return;
    }
    const CommandItem &item = items.at(index);
    if (!item.action->isEnabled()) {
        return;
    }
    if (item.role == FluentCommandBar::ItemRole::DropDown) {
        openMenu(item);
        return;
    }
    if (item.role == FluentCommandBar::ItemRole::Split && item.chevron.contains(pos)) {
        openMenu(item);
        return;
    }
    item.action->trigger();
}

FluentCommandBar::FluentCommandBar(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentCommandBarPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // Which command the pointer is on is part of what the row says, so moves
    // are wanted without a button - the same reason a rating wants them.
    setMouseTracking(true);
    d_ptr->overflow = new QMenu(this);
}

FluentCommandBar::~FluentCommandBar(){QX_FINI_PRIVATE()}

QAction *FluentCommandBar::addCommand(ItemRole role, const QString &text, const QIcon &icon)
{
    QAction *action = new QAction(icon, text, this);
    if (role == ItemRole::Toggle) {
        action->setCheckable(true);
    }
    if (role == ItemRole::DropDown || role == ItemRole::Split) {
        // A command that opens a menu arrives with the menu already made, so
        // filling it is one call and owning it is not the application's
        // problem: it is this bar's child.
        action->setMenu(new QMenu(this));
    }
    CommandItem item;
    item.action = action;
    item.role = role;
    d_ptr->items.append(item);
    d_ptr->dirty = true;
    d_ptr->relayout();
    updateGeometry();
    update();
    return action;
}

void FluentCommandBar::removeCommand(QAction *action)
{
    for (int i = 0; i < d_ptr->items.count(); ++i) {
        if (d_ptr->items.at(i).action == action) {
            d_ptr->items.removeAt(i);
            d_ptr->dirty = true;
            delete action;
            d_ptr->relayout();
            updateGeometry();
            update();
            return;
        }
    }
}

void FluentCommandBar::clear()
{
    if (d_ptr->items.isEmpty()) {
        return;
    }
    const QList<CommandItem> items = d_ptr->items;
    d_ptr->items.clear();
    d_ptr->dirty = true;
    for (const CommandItem &item : items) {
        delete item.action;
    }
    d_ptr->relayout();
    updateGeometry();
    update();
}

QList<QAction *> FluentCommandBar::commands() const
{
    QList<QAction *> actions;
    actions.reserve(d_ptr->items.count());
    for (const CommandItem &item : d_ptr->items) {
        actions.append(item.action);
    }
    return actions;
}

FluentCommandBar::ItemRole FluentCommandBar::roleOf(QAction *action) const
{
    for (const CommandItem &item : d_ptr->items) {
        if (item.action == action) {
            return item.role;
        }
    }
    return ItemRole::Button;
}

QList<QAction *> FluentCommandBar::visibleCommands() const
{
    d_ptr->relayout();
    QList<QAction *> actions;
    for (const CommandItem &item : d_ptr->items) {
        if (!item.rect.isEmpty()) {
            actions.append(item.action);
        }
    }
    return actions;
}

QMenu *FluentCommandBar::overflowMenu() const
{
    d_ptr->relayout();
    return d_ptr->overflow;
}

QSize FluentCommandBar::sizeHint() const
{
    qreal width = 2 * kItemPadding;
    int placed = 0;
    for (const CommandItem &item : d_ptr->items) {
        if (!item.action->isVisible()) {
            continue;
        }
        width += d_ptr->itemWidth(item) + (placed > 0 ? kItemSpacing : 0.0);
        ++placed;
    }
    return QSize(qMax(int(width), int(kOverflowWidth)), int(kBarHeight));
}

QSize FluentCommandBar::minimumSizeHint() const
{
    // A command bar can always give up room: everything collapses into the
    // overflow, so the least it can be given is the overflow itself.
    const int width = d_ptr->items.isEmpty() ? 0 : int(2 * kItemPadding + kOverflowWidth);
    return QSize(width, int(kBarHeight));
}

qreal FluentCommandBar::cornerRadius() const
{
    return FluentShape::radius(FluentShape::Radius::Medium);
}

QRectF FluentCommandBar::chromeRect() const
{
    // The wash and the ring belong to the one command the pointer or the
    // keyboard is on, not to the row: a row that lit up as a whole would say
    // nothing about which of its commands is about to run.
    d_ptr->relayout();
    const int index = d_ptr->hovered >= 0 ? d_ptr->hovered : d_ptr->focused;
    if (index < 0 || index >= d_ptr->items.count()) {
        return QRectF();
    }
    return d_ptr->items.at(index).rect;
}

void FluentCommandBar::paintSurface(QPainter *painter)
{
    d_ptr->relayout();
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    painter->setBrush(palette().color(QPalette::Window));
    painter->drawRect(rect());

    // A command that is in force is filled before the wash, so the pointer
    // still says where it is on a command that is already on. Filled through
    // the token, and inside the command's own corners - a square tint under a
    // rounded wash would show at the corners.
    const qreal radius = cornerRadius();
    const QColor fill =
        QxControls::QxStateLayer::layered(palette().color(QPalette::Window), accentColor(), kCheckedOpacity);
    painter->setBrush(fill);
    for (const CommandItem &item : d_ptr->items) {
        if (item.rect.isEmpty() || !item.action->isChecked()) {
            continue;
        }
        QPainterPath path;
        path.addRoundedRect(item.rect, radius, radius);
        painter->fillPath(path, fill);
    }
    painter->restore();
}

void FluentCommandBar::paintContent(QPainter *painter)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setFont(FluentType::font(FluentType::Ramp::Body));

    for (const CommandItem &item : d_ptr->items) {
        if (item.rect.isEmpty()) {
            continue;
        }
        const QAction *action = item.action;
        painter->save();
        if (!action->isEnabled()) {
            painter->setOpacity(kDisabledOpacity);
        }
        const QColor ink = action->isChecked() ? accentColor() : textColor();

        const qreal chevronRoom =
            (item.role == ItemRole::DropDown || item.role == ItemRole::Split) ? kChevronWidth : 0.0;
        const QRectF content = item.rect.adjusted(kItemPadding, 0.0, -kItemPadding - chevronRoom, 0.0);

        qreal left = content.left();
        if (!action->icon().isNull()) {
            const QRectF icon(left, item.rect.center().y() - kIconSize / 2.0, kIconSize, kIconSize);
            action->icon().paint(painter, icon.toRect(), Qt::AlignCenter, QIcon::Normal);
            left += kIconSize + kIconGap;
        }
        if (!action->text().isEmpty()) {
            painter->setPen(ink);
            const QRectF label(left, item.rect.top(), content.right() - left, item.rect.height());
            painter->drawText(label, Qt::AlignLeft | Qt::AlignVCenter, action->text());
        }
        if (chevronRoom > 0.0) {
            const QRectF chevron(item.rect.right() - kChevronWidth, item.rect.top(), kChevronWidth, kItemHeight);
            if (item.role == ItemRole::Split) {
                QColor divider = textColor();
                divider.setAlphaF(kDividerOpacity);
                painter->setPen(QPen(divider, 1.0));
                painter->drawLine(QPointF(chevron.left(), item.rect.top() + kDividerInset),
                                  QPointF(chevron.left(), item.rect.bottom() - kDividerInset));
            }
            paintChevron(painter, chevron, ink);
        }
        painter->restore();
    }

    if (!d_ptr->overflow->isEmpty()) {
        // What is left over: three dots, because the button is not a command
        // and must not look like one.
        const QRectF button = d_ptr->overflowRect();
        const QPointF center = button.center();
        painter->setPen(Qt::NoPen);
        painter->setBrush(textColor());
        for (int i = -1; i <= 1; ++i) {
            painter->drawEllipse(QPointF(center.x() + i * kDotStep, center.y()), kDotRadius, kDotRadius);
        }
    }
    painter->restore();
}

void FluentCommandBar::resizeEvent(QResizeEvent *event)
{
    QxControls::QxControl::resizeEvent(event);
    // What fits is a function of the room, so the room changing is what
    // decides it again - not a redraw.
    d_ptr->relayout();
}

void FluentCommandBar::mousePressEvent(QMouseEvent *event)
{
    QxControls::QxControl::mousePressEvent(event);
    if (!isEnabled() || event->button() != Qt::LeftButton) {
        return;
    }
    d_ptr->armed = d_ptr->itemAt(eventPos(event));
}

void FluentCommandBar::mouseReleaseEvent(QMouseEvent *event)
{
    QxControls::QxControl::mouseReleaseEvent(event);
    if (event->button() != Qt::LeftButton) {
        return;
    }
    // A press on one command and a release on another is not a click on
    // either, and the release is what decides - the same reading a button
    // gives, applied to a row of them.
    const int index = d_ptr->itemAt(eventPos(event));
    const int armed = d_ptr->armed;
    d_ptr->armed = -1;
    if (index < 0 || index != armed) {
        return;
    }
    d_ptr->activate(index, eventPos(event));
}

void FluentCommandBar::mouseMoveEvent(QMouseEvent *event)
{
    QxControls::QxControl::mouseMoveEvent(event);
    const int index = isEnabled() ? d_ptr->itemAt(eventPos(event)) : -1;
    if (index == d_ptr->hovered) {
        return;
    }
    d_ptr->hovered = index;
    update();
}

void FluentCommandBar::leaveEvent(QEvent *event)
{
    QxControls::QxControl::leaveEvent(event);
    if (d_ptr->hovered == -1) {
        return;
    }
    d_ptr->hovered = -1;
    update();
}

void FluentCommandBar::keyPressEvent(QKeyEvent *event)
{
    d_ptr->relayout();
    const int key = event->key();
    if (key == Qt::Key_Left || key == Qt::Key_Right) {
        const int step = key == Qt::Key_Right ? 1 : -1;
        int next = d_ptr->focused;
        for (int i = 0; i < d_ptr->items.count(); ++i) {
            next = next + step;
            if (next < 0 || next >= d_ptr->items.count()) {
                // The keyboard walks the row and stops at both ends: it does
                // not leave the bar, which is what Tab is for.
                next = qBound(0, next, d_ptr->items.count() - 1);
                break;
            }
            if (!d_ptr->items.at(next).rect.isEmpty()) {
                break;
            }
        }
        if (next != d_ptr->focused && next >= 0 && next < d_ptr->items.count() &&
            !d_ptr->items.at(next).rect.isEmpty()) {
            d_ptr->focused = next;
            update();
        }
        event->accept();
        return;
    }
    if (key == Qt::Key_Enter || key == Qt::Key_Return || key == Qt::Key_Space) {
        if (d_ptr->focused >= 0) {
            // The keyboard has no position of its own, so it presses the
            // command itself - which for a split is the command, not the
            // chevron.
            const QRectF item = d_ptr->items.at(d_ptr->focused).rect;
            d_ptr->activate(d_ptr->focused, QPointF(item.left() + 1.0, item.center().y()));
        }
        event->accept();
        return;
    }
    QxControls::QxControl::keyPressEvent(event);
}

QAccessible::Role FluentCommandBar::accessibleRole() const
{
    return QAccessible::ToolBar;
}

QX_FLUENT_END_NAMESPACE
