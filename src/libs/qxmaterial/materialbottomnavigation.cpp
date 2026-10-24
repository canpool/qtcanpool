/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialbottomnavigation.h"

#include "materialtokens.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtWidgets/QSizePolicy>
// QAction is in Widgets on Qt 5 and in Gui on Qt 6, so it is asked for without
// naming a module - the same way the rest of the libraries ask.
#include <QAction>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// Material's bar measures: a row fifty-six tall, a pill sixty-four by
// thirty-two behind the icon, an icon of twenty-four inside it, and the
// destination's name under both.
constexpr qreal kBarHeight = 56.0;
constexpr qreal kIndicatorWidth = 64.0;
constexpr qreal kIndicatorHeight = 32.0;
constexpr qreal kIndicatorTop = 3.0;
constexpr qreal kIconSize = 24.0;
constexpr qreal kLabelHeight = 18.0;
constexpr int kLeastItemWidth = 56;
constexpr int kHintItemWidth = 80;

// The pill behind the destination in force is the accent's own shape at the
// strength Material gives an active indicator; one nobody can use is the same
// drawing thinned.
constexpr qreal kIndicatorOpacity = 0.12;
constexpr qreal kDisabledOpacity = 0.38;
// A destination that is not the one in force is held back rather than drawn in
// another colour: what says "this one" is the accent and the pill, so the rest
// have to be quiet for it to be heard.
constexpr qreal kRestingOpacity = 0.6;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}
}   // namespace

class MaterialBottomNavigationPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialBottomNavigation)

    QList<QAction *> items;
    int hovered = -1;
    int armed = -1;
    int current = -1;
    int focused = -1;

    /*! Where destination \a index stands. The width is divided evenly, and the
     *  last one takes the remainder: a bar whose right edge stopped short of
     *  the room it was given would look like it had a destination missing. */
    QRectF itemRect(int index) const;
    QRectF indicatorOf(const QRectF &item) const;
    int itemAt(const QPointF &pos) const;
};

QRectF MaterialBottomNavigationPrivate::itemRect(int index) const
{
    Q_Q(const MaterialBottomNavigation);
    if (items.isEmpty() || index < 0 || index >= items.count()) {
        return QRectF();
    }
    const qreal total = qreal(q->width());
    const qreal each = total / qreal(items.count());
    const qreal left = qRound(each) * index;
    const qreal right = index == items.count() - 1 ? total : qRound(each) * (index + 1);
    return QRectF(left, 0.0, right - left, kBarHeight);
}

QRectF MaterialBottomNavigationPrivate::indicatorOf(const QRectF &item) const
{
    return QRectF(item.center().x() - kIndicatorWidth / 2.0, kIndicatorTop, kIndicatorWidth, kIndicatorHeight);
}

int MaterialBottomNavigationPrivate::itemAt(const QPointF &pos) const
{
    for (int i = 0; i < items.count(); ++i) {
        if (itemRect(i).contains(pos)) {
            return i;
        }
    }
    return -1;
}

MaterialBottomNavigation::MaterialBottomNavigation(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new MaterialBottomNavigationPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // Which destination the pointer is on is what the pill answers, so moves
    // are wanted without a button.
    setMouseTracking(true);
    // A bar is as wide as the screen it stands on and as tall as Material says:
    // across is the parent's to decide, down is the token's.
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

MaterialBottomNavigation::~MaterialBottomNavigation(){QX_FINI_PRIVATE()}

QAction *MaterialBottomNavigation::addDestination(const QString &label, const QIcon &icon)
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

void MaterialBottomNavigation::removeDestination(QAction *action)
{
    for (int i = 0; i < d_ptr->items.count(); ++i) {
        if (d_ptr->items.at(i) != action) {
            continue;
        }
        d_ptr->items.removeAt(i);
        // The one in force may be the one going, and an index is a position in
        // a row that just got shorter.
        if (d_ptr->current >= d_ptr->items.count()) {
            d_ptr->current = d_ptr->items.isEmpty() ? -1 : d_ptr->items.count() - 1;
        }
        if (d_ptr->focused >= d_ptr->items.count()) {
            d_ptr->focused = d_ptr->items.isEmpty() ? -1 : d_ptr->items.count() - 1;
        }
        d_ptr->hovered = -1;
        delete action;
        updateGeometry();
        update();
        return;
    }
}

void MaterialBottomNavigation::clear()
{
    if (d_ptr->items.isEmpty()) {
        return;
    }
    const QList<QAction *> items = d_ptr->items;
    d_ptr->items.clear();
    d_ptr->current = -1;
    d_ptr->focused = -1;
    d_ptr->hovered = -1;
    for (QAction *action : items) {
        delete action;
    }
    updateGeometry();
    update();
}

QList<QAction *> MaterialBottomNavigation::destinations() const
{
    QList<QAction *> actions;
    actions.reserve(d_ptr->items.count());
    for (QAction *action : d_ptr->items) {
        actions.append(action);
    }
    return actions;
}

QAction *MaterialBottomNavigation::currentDestination() const
{
    if (d_ptr->current < 0 || d_ptr->current >= d_ptr->items.count()) {
        return Q_NULLPTR;
    }
    return d_ptr->items.at(d_ptr->current);
}

int MaterialBottomNavigation::currentIndex() const
{
    return d_ptr->current;
}

void MaterialBottomNavigation::setCurrentIndex(int index)
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
    Q_EMIT currentDestinationChanged(currentDestination());
}

void MaterialBottomNavigation::setCurrentDestination(QAction *action)
{
    for (int i = 0; i < d_ptr->items.count(); ++i) {
        if (d_ptr->items.at(i) == action) {
            setCurrentIndex(i);
            return;
        }
    }
}

QSize MaterialBottomNavigation::sizeHint() const
{
    return QSize(d_ptr->items.count() * kHintItemWidth, int(kBarHeight));
}

QSize MaterialBottomNavigation::minimumSizeHint() const
{
    return QSize(d_ptr->items.count() * kLeastItemWidth, int(kBarHeight));
}

QRectF MaterialBottomNavigation::chromeRect() const
{
    // Nothing about this bar is the control: it is a row of destinations with a
    // pill behind one of them, and the wash and the ring belong to a
    // destination.
    return QRectF();
}

void MaterialBottomNavigation::paintSurface(QPainter *painter)
{
    const QRectF room(0.0, 0.0, qreal(width()), qreal(height()));
    painter->fillRect(room, palette().color(QPalette::Window));
    // A bar along the bottom of a page is a surface the page ends at, so it
    // says where it starts with the hairline Material draws between surfaces.
    painter->setPen(Qt::NoPen);
    painter->setBrush(QxControls::QxStateLayer::layered(palette().color(QPalette::Window), textColor(), 0.12));
    painter->drawRect(QRectF(room.left(), room.top(), room.width(), 1.0));
}

void MaterialBottomNavigation::paintContent(QPainter *painter)
{
    const bool enabled = isEnabled();
    const QFont label = MaterialType::font(MaterialType::Scale::Label);
    const QFontMetrics metrics(label);
    const QColor accent = accentColor();
    const QColor ink = textColor();
    const qreal radius = MaterialShape::radius(MaterialShape::Radius::Circular);

    for (int i = 0; i < d_ptr->items.count(); ++i) {
        const QRectF item = d_ptr->itemRect(i);
        if (!item.isValid()) {
            continue;
        }
        QAction *action = d_ptr->items.at(i);
        const bool usable = action->isEnabled();
        const bool inForce = i == d_ptr->current;
        const QRectF indicator = d_ptr->indicatorOf(item);

        // The state wash is a fill and the fill knows no corners; the pill's
        // shape comes from the clip, as the control's own wash does.
        const auto wash = [painter, &indicator, radius, &ink](qreal opacity) {
            QPainterPath clip;
            clip.addRoundedRect(indicator, radius, radius);
            painter->save();
            painter->setClipPath(clip);
            QxControls::QxStateLayer::paint(painter, indicator, ink, opacity);
            painter->restore();
        };

        painter->setOpacity(enabled ? (usable ? 1.0 : kDisabledOpacity) : kDisabledOpacity);
        if (inForce) {
            // The pill: the destination in force wears a shape, and the shape
            // is the accent's - the same answer the rail gives, because it is
            // the same question asked along a different edge.
            painter->setPen(Qt::NoPen);
            painter->setBrush(
                QxControls::QxStateLayer::layered(palette().color(QPalette::Window), accent, kIndicatorOpacity));
            painter->drawRoundedRect(indicator, radius, radius);
        } else if (i == d_ptr->hovered) {
            wash(QxControls::QxStateLayer::hoverOpacity);
        }
        if (hasFocus() && i == d_ptr->focused) {
            wash(QxControls::QxStateLayer::focusOpacity);
        }

        // What the icon and the name are drawn in: the accent when this is the
        // destination in force, the page's ink held back when it is not.
        painter->setOpacity(enabled ? (usable ? (inForce ? 1.0 : kRestingOpacity) : kDisabledOpacity)
                                    : kDisabledOpacity);
        const QColor drawn = inForce ? accent : ink;
        if (!action->icon().isNull()) {
            const QRectF iconRect(indicator.center().x() - kIconSize / 2.0, indicator.center().y() - kIconSize / 2.0,
                                  kIconSize, kIconSize);
            action->icon().paint(painter, iconRect.toRect(), Qt::AlignCenter, usable ? QIcon::Normal : QIcon::Disabled);
        }

        const QString text = action->text();
        if (!text.isEmpty()) {
            const QRectF labelRect(item.left() + 2.0, indicator.bottom() + 1.0, item.width() - 4.0, kLabelHeight);
            const QString shortened = metrics.elidedText(text, Qt::ElideRight, int(labelRect.width()));
            painter->setFont(label);
            painter->setPen(drawn);
            painter->drawText(labelRect, Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextSingleLine, shortened);
        }
    }
}

void MaterialBottomNavigation::mousePressEvent(QMouseEvent *event)
{
    d_ptr->armed = d_ptr->itemAt(eventPos(event));
    update();
}

void MaterialBottomNavigation::mouseReleaseEvent(QMouseEvent *event)
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

void MaterialBottomNavigation::mouseMoveEvent(QMouseEvent *event)
{
    const int hovered = d_ptr->itemAt(eventPos(event));
    if (hovered != d_ptr->hovered) {
        d_ptr->hovered = hovered;
        update();
    }
}

void MaterialBottomNavigation::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    d_ptr->hovered = -1;
    update();
}

void MaterialBottomNavigation::keyPressEvent(QKeyEvent *event)
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
    case Qt::Key_Right:
        d_ptr->focused = qMin(d_ptr->focused + 1, count - 1);
        setCurrentIndex(d_ptr->focused);
        event->accept();
        return;
    case Qt::Key_Left:
        d_ptr->focused = qMax(d_ptr->focused - 1, 0);
        setCurrentIndex(d_ptr->focused);
        event->accept();
        return;
    case Qt::Key_Home:
        d_ptr->focused = 0;
        setCurrentIndex(0);
        event->accept();
        return;
    case Qt::Key_End:
        d_ptr->focused = count - 1;
        setCurrentIndex(count - 1);
        event->accept();
        return;
    default:
        break;
    }
    QxControls::QxControl::keyPressEvent(event);
}

QAccessible::Role MaterialBottomNavigation::accessibleRole() const
{
    // A short row of destinations: an assistive tool walks a list, as it does
    // along the rail.
    return QAccessible::List;
}

QX_MATERIAL_END_NAMESPACE
