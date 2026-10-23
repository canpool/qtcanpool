/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialnavigationrail.h"

#include "materialtokens.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QResizeEvent>
#include <QtGui/QWheelEvent>
#include <QtWidgets/QSizePolicy>
#include <QtWidgets/QWidget>
// QAction is in Widgets on Qt 5 and in Gui on Qt 6, so it is asked for without
// naming a module - the same way the rest of the libraries ask.
#include <QAction>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// Material's rail measures: a column eighty wide, an indicator fifty-six by
// thirty-two behind the icon, an icon of twenty-four, and a label under it.
constexpr qreal kRailWidth = 80.0;
constexpr qreal kItemHeight = 56.0;
constexpr qreal kIndicatorWidth = 56.0;
constexpr qreal kIndicatorHeight = 32.0;
constexpr qreal kIconSize = 24.0;
constexpr qreal kLabelHeight = 20.0;
constexpr qreal kHeaderGap = 12.0;
constexpr qreal kTopPadding = 12.0;

// The indicator is the accent's own shape at the strength Material gives an
// active indicator; a destination nobody can use is the same drawing thinned.
constexpr qreal kIndicatorOpacity = 0.12;
constexpr qreal kDisabledOpacity = 0.38;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}
}   // namespace

class MaterialNavigationRailPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialNavigationRail)

    QList<QAction *> items;
    int hovered = -1;
    int armed = -1;
    int current = -1;
    int focused = -1;
    QWidget *header = Q_NULLPTR;
    /*! How far the column has travelled up out of the room it was given, in
     *  pixels of the column's own extent. Zero while everything fits. */
    qreal offset = 0.0;

    qreal columnLeft() const;
    qreal itemsTop() const;
    QRectF itemRect(int index) const;
    void layoutHeader();
    int itemAt(const QPointF &pos) const;
    QRectF indicatorOf(const QRectF &item) const;
    QSize headerSize() const;
    /*! The band of the rail the column lives in: under the header, over the
     *  bottom padding. */
    qreal viewportTop() const;
    qreal viewportBottom() const;
    qreal maxOffset() const;
    void clampOffset();
    /*! Moves the column so destination \a index stands whole inside the band. */
    void ensureVisible(int index);
};

/*! The room the header takes. A widget that has been given a fixed size still
 *  answers an invalid sizeHint - QWidget has no hint of its own - so what it
 *  is being asked for falls back to what it was told to be. */
QSize MaterialNavigationRailPrivate::headerSize() const
{
    if (!header) {
        return QSize();
    }
    const QSize hint = header->sizeHint();
    return hint.isValid() ? hint : header->size();
}

/*! Where the column stands across the rail.
 *
 *  The column is eighty wide wherever the rail was made wider than that, and
 *  it stands in the middle of the room it was given rather than against the
 *  left edge of it: a width the rail did not ask for is a parent's doing, and
 *  what answers it is where the column goes - not a wider column. What is
 *  asked for is worked out when it is asked for, because a widget that has
 *  never been shown is not told it was resized: a rail laid out when the room
 *  changed answers from a room it no longer has, which is how a destination
 *  came to be aimed at where it used to be. */
qreal MaterialNavigationRailPrivate::columnLeft() const
{
    Q_Q(const MaterialNavigationRail);
    return qMax(0.0, (qreal(q->width()) - kRailWidth) / 2.0);
}

qreal MaterialNavigationRailPrivate::itemsTop() const
{
    if (!header) {
        return kTopPadding;
    }
    return kTopPadding + qreal(headerSize().height()) + kHeaderGap;
}

QRectF MaterialNavigationRailPrivate::itemRect(int index) const
{
    return QRectF(columnLeft(), itemsTop() + qreal(index) * kItemHeight - offset, kRailWidth, kItemHeight);
}

/*! Where the column's room starts and ends. The room is what is left between
 *  the header and the bottom padding, and it is what clips the column: a
 *  destination drawn past it is not on the rail, so it must not be one a
 *  pointer can reach either. */
qreal MaterialNavigationRailPrivate::viewportTop() const
{
    return itemsTop();
}

qreal MaterialNavigationRailPrivate::viewportBottom() const
{
    Q_Q(const MaterialNavigationRail);
    return qMax(viewportTop(), qreal(q->height()) - kTopPadding);
}

qreal MaterialNavigationRailPrivate::maxOffset() const
{
    const qreal content = qreal(items.count()) * kItemHeight;
    return qMax(0.0, content - (viewportBottom() - viewportTop()));
}

void MaterialNavigationRailPrivate::clampOffset()
{
    offset = qBound(0.0, offset, maxOffset());
}

void MaterialNavigationRailPrivate::ensureVisible(int index)
{
    if (index < 0 || index >= items.count()) {
        return;
    }
    const qreal top = qreal(index) * kItemHeight;
    const qreal room = viewportBottom() - viewportTop();
    if (top < offset) {
        offset = top;
    } else if (top + kItemHeight > offset + room) {
        offset = top + kItemHeight - room;
    }
    clampOffset();
}

void MaterialNavigationRailPrivate::layoutHeader()
{
    if (!header) {
        return;
    }
    const QSize size = headerSize();
    header->setGeometry(int(columnLeft() + (kRailWidth - qreal(size.width())) / 2.0), int(kTopPadding), size.width(),
                        size.height());
}

QRectF MaterialNavigationRailPrivate::indicatorOf(const QRectF &item) const
{
    return QRectF(item.center().x() - kIndicatorWidth / 2.0, item.top(), kIndicatorWidth, kIndicatorHeight);
}

int MaterialNavigationRailPrivate::itemAt(const QPointF &pos) const
{
    // Outside the band there is no destination to hit, even where a clipped
    // one's rectangle would say otherwise: a press on the header is a press on
    // the header, and a press under the column is a press on nothing.
    if (pos.y() < viewportTop() || pos.y() > viewportBottom()) {
        return -1;
    }
    for (int i = 0; i < items.count(); ++i) {
        if (itemRect(i).contains(pos)) {
            return i;
        }
    }
    return -1;
}

MaterialNavigationRail::MaterialNavigationRail(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new MaterialNavigationRailPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // Which destination the pointer is on is what the pill answers, so moves
    // are wanted without a button - the same reason a rating wants them.
    setMouseTracking(true);
    // A rail is the width Material gives it and nothing else: it is a column
    // beside the room the destinations open into, and a parent that stretched
    // it would be widening the gap beside a column, not the column. Across is
    // therefore fixed at the token, while down is whatever the screen has - the
    // same answer a toolbar gives, and the one that keeps a layout from
    // handing the rail the space that belongs to the screen.
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
}

MaterialNavigationRail::~MaterialNavigationRail(){QX_FINI_PRIVATE()}

QAction *MaterialNavigationRail::addDestination(const QString &label, const QIcon &icon)
{
    QAction *action = new QAction(icon, label, this);
    d_ptr->items.append(action);
    if (d_ptr->focused < 0) {
        d_ptr->focused = 0;
    }
    if (d_ptr->current < 0) {
        d_ptr->current = 0;
    }
    updateGeometry();
    update();
    return action;
}

void MaterialNavigationRail::removeDestination(QAction *action)
{
    for (int i = 0; i < d_ptr->items.count(); ++i) {
        if (d_ptr->items.at(i) != action) {
            continue;
        }
        d_ptr->items.removeAt(i);
        // The one in force may be the one going, and an index is a position in
        // a column that just got shorter.
        if (d_ptr->current >= d_ptr->items.count()) {
            d_ptr->current = d_ptr->items.isEmpty() ? -1 : d_ptr->items.count() - 1;
        }
        if (d_ptr->focused >= d_ptr->items.count()) {
            d_ptr->focused = d_ptr->items.isEmpty() ? -1 : d_ptr->items.count() - 1;
        }
        d_ptr->hovered = -1;
        d_ptr->clampOffset();
        delete action;
        updateGeometry();
        update();
        return;
    }
}

void MaterialNavigationRail::clear()
{
    if (d_ptr->items.isEmpty()) {
        return;
    }
    const QList<QAction *> items = d_ptr->items;
    d_ptr->items.clear();
    d_ptr->current = -1;
    d_ptr->focused = -1;
    d_ptr->hovered = -1;
    d_ptr->offset = 0.0;
    for (QAction *action : items) {
        delete action;
    }
    updateGeometry();
    update();
}

QList<QAction *> MaterialNavigationRail::destinations() const
{
    QList<QAction *> actions;
    actions.reserve(d_ptr->items.count());
    for (QAction *action : d_ptr->items) {
        actions.append(action);
    }
    return actions;
}

QAction *MaterialNavigationRail::currentDestination() const
{
    if (d_ptr->current < 0 || d_ptr->current >= d_ptr->items.count()) {
        return Q_NULLPTR;
    }
    return d_ptr->items.at(d_ptr->current);
}

int MaterialNavigationRail::currentIndex() const
{
    return d_ptr->current;
}

void MaterialNavigationRail::setHeaderWidget(QWidget *widget)
{
    if (d_ptr->header == widget) {
        return;
    }
    if (d_ptr->header) {
        d_ptr->header->setParent(Q_NULLPTR);
        d_ptr->header->hide();
    }
    d_ptr->header = widget;
    if (widget) {
        widget->setParent(this);
        widget->show();
    }
    d_ptr->layoutHeader();
    updateGeometry();
    update();
}

QWidget *MaterialNavigationRail::headerWidget() const
{
    return d_ptr->header;
}

void MaterialNavigationRail::setCurrentIndex(int index)
{
    if (d_ptr->current == index) {
        return;
    }
    if (index >= 0 && index >= d_ptr->items.count()) {
        return;
    }
    d_ptr->current = index;
    // The destination in force is of no use where it cannot be seen: putting
    // one in force is saying "here", and "here" the reader has to be able to
    // look at. The column travels to it.
    d_ptr->ensureVisible(index);
    update();
    Q_EMIT currentIndexChanged(index);
    Q_EMIT currentDestinationChanged(currentDestination());
}

void MaterialNavigationRail::setCurrentDestination(QAction *action)
{
    for (int i = 0; i < d_ptr->items.count(); ++i) {
        if (d_ptr->items.at(i) == action) {
            setCurrentIndex(i);
            return;
        }
    }
}

QSize MaterialNavigationRail::sizeHint() const
{
    const int header = d_ptr->header ? d_ptr->headerSize().height() + int(kHeaderGap) : 0;
    const int items = int(d_ptr->items.count()) * int(kItemHeight);
    return QSize(int(kRailWidth), int(kTopPadding) + header + items + int(kTopPadding));
}

QSize MaterialNavigationRail::minimumSizeHint() const
{
    return QSize(int(kRailWidth), int(kTopPadding) + int(kItemHeight) + int(kTopPadding));
}

QRectF MaterialNavigationRail::chromeRect() const
{
    // Nothing about this rail is the control: it is a column of destinations
    // with a pill behind one of them, and the wash and the ring belong to a
    // destination.
    return QRectF();
}

void MaterialNavigationRail::paintSurface(QPainter *painter)
{
    painter->fillRect(QRectF(0.0, 0.0, qreal(width()), qreal(height())), palette().color(QPalette::Window));
}

void MaterialNavigationRail::paintContent(QPainter *painter)
{
    const bool enabled = isEnabled();
    const QFont label = MaterialType::font(MaterialType::Scale::Label);
    const QFontMetrics metrics(label);
    const QColor accent = accentColor();
    const qreal radius = MaterialShape::radius(MaterialShape::Radius::Circular);

    // The column is clipped to the room it was given: what has travelled out
    // of it is not on the rail, and drawing it would put a destination where
    // the header or the padding stands.
    painter->save();
    painter->setClipRect(
        QRectF(0.0, d_ptr->viewportTop(), qreal(width()), d_ptr->viewportBottom() - d_ptr->viewportTop()));
    for (int i = 0; i < d_ptr->items.count(); ++i) {
        const QRectF item = d_ptr->itemRect(i);
        if (!item.intersects(rect())) {
            continue;
        }
        QAction *action = d_ptr->items.at(i);
        const bool usable = action->isEnabled();
        painter->setOpacity(enabled ? (usable ? 1.0 : kDisabledOpacity) : kDisabledOpacity);

        const QRectF indicator = d_ptr->indicatorOf(item);
        // A state wash is a fill and the fill knows no corners; the pill's
        // shape comes from the clip, as the control's own wash does.
        const auto wash = [painter, &indicator, radius, this](qreal opacity) {
            QPainterPath clip;
            clip.addRoundedRect(indicator, radius, radius);
            painter->save();
            painter->setClipPath(clip);
            QxControls::QxStateLayer::paint(painter, indicator, textColor(), opacity);
            painter->restore();
        };
        if (i == d_ptr->current) {
            // The pill: the destination in force wears a shape, and the shape
            // is the accent's - not a tint over the whole row, which would say
            // "pressed" in a language Material does not speak.
            painter->setPen(Qt::NoPen);
            painter->setBrush(
                QxControls::QxStateLayer::layered(palette().color(QPalette::Window), accent, kIndicatorOpacity));
            painter->drawRoundedRect(indicator, radius, radius);
        }
        if (i == d_ptr->hovered && i != d_ptr->current) {
            wash(QxControls::QxStateLayer::hoverOpacity);
        }
        if (hasFocus() && i == d_ptr->focused) {
            wash(QxControls::QxStateLayer::focusOpacity);
        }

        if (!action->icon().isNull()) {
            const QRectF iconRect(indicator.center().x() - kIconSize / 2.0, indicator.center().y() - kIconSize / 2.0,
                                  kIconSize, kIconSize);
            action->icon().paint(painter, iconRect.toRect(), Qt::AlignCenter, usable ? QIcon::Normal : QIcon::Disabled);
        }

        const QRectF labelRect(item.left(), indicator.bottom(), kRailWidth, kLabelHeight);
        const QString text = metrics.elidedText(action->text(), Qt::ElideRight, int(kRailWidth) - 8);
        if (!text.isEmpty()) {
            painter->setFont(label);
            painter->setPen(textColor());
            painter->drawText(labelRect, Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextSingleLine, text);
        }
    }
    painter->restore();
}

void MaterialNavigationRail::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    // A taller room takes the travel back; a shorter one asks for more. Either
    // way the column must stand where the room now allows.
    d_ptr->clampOffset();
    d_ptr->layoutHeader();
    QxControls::QxControl::resizeEvent(event);
}

void MaterialNavigationRail::mousePressEvent(QMouseEvent *event)
{
    d_ptr->armed = d_ptr->itemAt(eventPos(event));
    update();
}

void MaterialNavigationRail::mouseReleaseEvent(QMouseEvent *event)
{
    const QPointF pos = eventPos(event);
    const int armed = d_ptr->armed;
    d_ptr->armed = -1;
    if (armed >= 0 && d_ptr->itemAt(pos) == armed) {
        d_ptr->focused = armed;
        if (d_ptr->items.at(armed)->isEnabled()) {
            setCurrentIndex(armed);
        }
    }
    update();
}

void MaterialNavigationRail::mouseMoveEvent(QMouseEvent *event)
{
    const int hovered = d_ptr->itemAt(eventPos(event));
    if (hovered != d_ptr->hovered) {
        d_ptr->hovered = hovered;
        update();
    }
}

void MaterialNavigationRail::wheelEvent(QWheelEvent *event)
{
    // A notch of the wheel is a destination: the column moves in whole steps
    // of the measure it is laid out with, which keeps "where did it go" a
    // question with a whole-number answer.
    if (d_ptr->maxOffset() <= 0.0) {
        QxControls::QxControl::wheelEvent(event);
        return;
    }
    const int delta = event->angleDelta().y();
    if (delta == 0) {
        QxControls::QxControl::wheelEvent(event);
        return;
    }
    d_ptr->offset -= delta > 0 ? kItemHeight : -kItemHeight;
    d_ptr->clampOffset();
    update();
    event->accept();
}

void MaterialNavigationRail::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    d_ptr->hovered = -1;
    update();
}

void MaterialNavigationRail::keyPressEvent(QKeyEvent *event)
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
        setCurrentIndex(d_ptr->focused);
        event->accept();
        return;
    case Qt::Key_Up:
        d_ptr->focused = qMax(d_ptr->focused - 1, 0);
        setCurrentIndex(d_ptr->focused);
        event->accept();
        return;
    default:
        break;
    }
    QxControls::QxControl::keyPressEvent(event);
}

QAccessible::Role MaterialNavigationRail::accessibleRole() const
{
    // A short column of destinations: an assistive tool walks a list.
    return QAccessible::List;
}

QX_MATERIAL_END_NAMESPACE
