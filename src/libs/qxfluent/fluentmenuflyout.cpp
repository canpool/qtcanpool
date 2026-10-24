/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentmenuflyout.h"

#include "fluenttokens.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// Fluent's flyout measures: a row of thirty-two, a heading of twenty-eight,
// a hairline row of nine, the rows keeping twelve of their own from the
// surface's edges and four of margin at the top and the bottom. The glyph
// zone at the left is where an icon stands or a check is drawn - one zone,
// because a command wears one or the other.
constexpr qreal kItemHeight = 32.0;
constexpr qreal kSectionHeight = 28.0;
constexpr qreal kSeparatorHeight = 9.0;
constexpr qreal kPaddingH = 12.0;
constexpr qreal kPaddingV = 4.0;
constexpr qreal kGlyphZone = 24.0;
constexpr qreal kGlyphGap = 8.0;
constexpr qreal kRowInset = 4.0;
constexpr qreal kRowRadius = 4.0;
constexpr qreal kHoverOpacity = 0.06;
constexpr qreal kMutedOpacity = 0.6;
constexpr qreal kDisabledOpacity = 0.4;
// The property a heading action wears, so a command and a heading can be
// told apart at paint time.
const char kSectionProperty[] = "qxMenuSection";

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}
}   // namespace

class FluentMenuFlyoutPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentMenuFlyout)

    QList<QAction *> actions;
    int current = -1;
};

FluentMenuFlyout::FluentMenuFlyout(QWidget *anchor)
    : QxControls::QxPopupSurface(anchor)
    , d_ptr(new FluentMenuFlyoutPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setMouseTracking(true);
    setSurfaceRadius(FluentShape::radius(FluentShape::Radius::Large));
}

FluentMenuFlyout::~FluentMenuFlyout(){QX_FINI_PRIVATE()}

QAction *FluentMenuFlyout::addAction(const QString &text, const QIcon &icon)
{
    QAction *action = new QAction(text, this);
    action->setIcon(icon);
    d_ptr->actions.append(action);
    update();
    return action;
}

QAction *FluentMenuFlyout::addCheckable(const QString &text, bool checked, const QIcon &icon)
{
    QAction *action = addAction(text, icon);
    action->setCheckable(true);
    action->setChecked(checked);
    return action;
}

QAction *FluentMenuFlyout::addSeparator()
{
    QAction *action = new QAction(this);
    action->setSeparator(true);
    d_ptr->actions.append(action);
    update();
    return action;
}

QAction *FluentMenuFlyout::addSection(const QString &title)
{
    QAction *action = new QAction(title, this);
    action->setProperty(kSectionProperty, true);
    d_ptr->actions.append(action);
    update();
    return action;
}

QList<QAction *> FluentMenuFlyout::actions() const
{
    return d_ptr->actions;
}

void FluentMenuFlyout::clear()
{
    for (QAction *action : d_ptr->actions) {
        action->deleteLater();
    }
    d_ptr->actions.clear();
    d_ptr->current = -1;
    update();
}

QAction *FluentMenuFlyout::currentAction() const
{
    return d_ptr->current >= 0 && d_ptr->current < d_ptr->actions.count() ? d_ptr->actions.at(d_ptr->current)
                                                                          : Q_NULLPTR;
}

void FluentMenuFlyout::setCurrentAction(QAction *action)
{
    d_ptr->current = indexOf(action);
    update();
}

void FluentMenuFlyout::open()
{
    // The keyboard lands on the first command that can be worked, so the
    // flyout opens with an answer in hand.
    d_ptr->current = -1;
    for (int index = 0; index < d_ptr->actions.count(); ++index) {
        if (isWorkable(d_ptr->actions.at(index))) {
            d_ptr->current = index;
            break;
        }
    }
    QxControls::QxPopupSurface::open();
}

QSize FluentMenuFlyout::contentSizeHint() const
{
    qreal height = 2 * kPaddingV;
    for (int index = 0; index < d_ptr->actions.count(); ++index) {
        height += rowHeight(index);
    }
    qreal width = 0.0;
    const QFontMetrics metrics(FluentType::font(FluentType::Ramp::Body));
    for (QAction *action : d_ptr->actions) {
        if (action->isSeparator()) {
            continue;
        }
        width = qMax(width, kPaddingH + kGlyphZone + kGlyphGap + metrics.horizontalAdvance(action->text()));
    }
    return QSize(int(width + kPaddingH + 40.0), int(height));
}

void FluentMenuFlyout::paintContent(QPainter *painter, const QRectF &content)
{
    const QFont body = FluentType::font(FluentType::Ramp::Body);
    const QFont caption = FluentType::font(FluentType::Ramp::Caption);
    const QFontMetrics metrics(body);
    const QColor ink = palette().color(QPalette::WindowText);
    const QColor muted = QxControls::QxStateLayer::layered(palette().color(QPalette::Window), ink, kMutedOpacity);
    const QColor hairline = QxControls::QxStateLayer::layered(palette().color(QPalette::Window), ink, 0.08);

    qreal y = content.top() + kPaddingV;
    for (int index = 0; index < d_ptr->actions.count(); ++index) {
        QAction *action = d_ptr->actions.at(index);
        const QRectF row(content.left(), y, content.width(), rowHeight(index));

        if (action->isSeparator()) {
            painter->fillRect(QRectF(content.left() + 1.0, row.center().y(), content.width() - 2.0, 1.0), hairline);
        } else if (action->property(kSectionProperty).toBool()) {
            painter->save();
            painter->setFont(caption);
            painter->setPen(muted);
            painter->drawText(
                QRectF(content.left() + kPaddingH, row.top(), content.width() - 2 * kPaddingH, row.height()),
                Qt::AlignLeft | Qt::AlignVCenter, action->text());
            painter->restore();
        } else {
            // The row's own wash: the keyboard's or the pointer's answer.
            if (index == d_ptr->current && isEnabled()) {
                QPainterPath wash;
                wash.addRoundedRect(
                    QRectF(row.left() + kRowInset, row.top(), row.width() - 2 * kRowInset, row.height()), kRowRadius,
                    kRowRadius);
                painter->fillPath(
                    wash, QxControls::QxStateLayer::layered(palette().color(QPalette::Window), ink, kHoverOpacity));
            }

            painter->save();
            painter->setOpacity(action->isEnabled() ? 1.0 : kDisabledOpacity);

            const QRectF glyph(content.left() + kPaddingH, row.top(), kGlyphZone, row.height());
            if (action->isCheckable() && action->isChecked()) {
                // The check: two strokes meeting, drawn rather than typed.
                const QRectF mark(glyph.center().x() - 5.0, glyph.center().y() - 4.0, 10.0, 8.0);
                QPen pen(ink, 1.4);
                pen.setCapStyle(Qt::RoundCap);
                painter->setPen(pen);
                painter->setBrush(Qt::NoBrush);
                painter->setRenderHint(QPainter::Antialiasing, true);
                QPainterPath tick;
                tick.moveTo(mark.left(), mark.center().y());
                tick.lineTo(mark.center().x(), mark.bottom());
                tick.lineTo(mark.right(), mark.top());
                painter->drawPath(tick);
            } else if (!action->icon().isNull()) {
                action->icon().paint(painter, glyph.toRect(), Qt::AlignCenter,
                                     action->isEnabled() ? QIcon::Normal : QIcon::Disabled);
            }

            const qreal wordsLeft = content.left() + kPaddingH + kGlyphZone + kGlyphGap;
            painter->setFont(body);
            painter->setPen(ink);
            painter->drawText(QRectF(wordsLeft, row.top(), content.right() - wordsLeft - kPaddingH, row.height()),
                              Qt::AlignLeft | Qt::AlignVCenter, action->text());

            const QString shortcut = action->shortcut().toString(QKeySequence::NativeText);
            if (!shortcut.isEmpty()) {
                painter->setPen(muted);
                painter->drawText(QRectF(content.right() - kPaddingH - 120.0, row.top(), 120.0, row.height()),
                                  Qt::AlignRight | Qt::AlignVCenter, shortcut);
            }
            painter->restore();
        }
        y += row.height();
    }
}

void FluentMenuFlyout::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:
    case Qt::Key_Down: {
        const int count = d_ptr->actions.count();
        if (count == 0) {
            break;
        }
        const int step = event->key() == Qt::Key_Down ? 1 : -1;
        int index = d_ptr->current;
        for (int walked = 0; walked < count; ++walked) {
            index = (index + step + count) % count;
            if (isWorkable(d_ptr->actions.at(index))) {
                d_ptr->current = index;
                update();
                event->accept();
                return;
            }
        }
        break;
    }
    case Qt::Key_Return:
    case Qt::Key_Enter:
        if (currentAction() && currentAction()->isEnabled()) {
            QAction *action = currentAction();
            Q_EMIT triggered(action);
            action->trigger();
            close();
            event->accept();
            return;
        }
        break;
    default:
        break;
    }
    QxControls::QxPopupSurface::keyPressEvent(event);
}

void FluentMenuFlyout::mouseMoveEvent(QMouseEvent *event)
{
    // The pointer's answer and the keyboard's are the same one: hovering a
    // command puts the keyboard on it, and hovering elsewhere leaves the
    // keyboard where it was.
    QAction *action = actionAt(eventPos(event));
    if (action && isWorkable(action)) {
        setCurrentAction(action);
    }
    QxControls::QxPopupSurface::mouseMoveEvent(event);
}

void FluentMenuFlyout::mousePressEvent(QMouseEvent *event)
{
    QxControls::QxPopupSurface::mousePressEvent(event);
    QAction *action = actionAt(eventPos(event));
    if (action && isWorkable(action)) {
        setCurrentAction(action);
    }
}

void FluentMenuFlyout::mouseReleaseEvent(QMouseEvent *event)
{
    QxControls::QxPopupSurface::mouseReleaseEvent(event);
    QAction *action = actionAt(eventPos(event));
    if (action && isWorkable(action)) {
        Q_EMIT triggered(action);
        action->trigger();
        close();
    }
}

bool FluentMenuFlyout::isWorkable(QAction *action) const
{
    return action && !action->isSeparator() && !action->property(kSectionProperty).toBool() && action->isEnabled();
}

int FluentMenuFlyout::indexOf(QAction *action) const
{
    return action ? d_ptr->actions.indexOf(action) : -1;
}

QAction *FluentMenuFlyout::actionAt(const QPointF &pos) const
{
    const QPointF local = pos - contentRect().topLeft();
    qreal y = kPaddingV;
    for (int index = 0; index < d_ptr->actions.count(); ++index) {
        const qreal height = rowHeight(index);
        if (local.y() >= y && local.y() < y + height) {
            return d_ptr->actions.at(index);
        }
        y += height;
    }
    return Q_NULLPTR;
}

int FluentMenuFlyout::rowHeight(int index) const
{
    QAction *action = d_ptr->actions.at(index);
    if (action->isSeparator()) {
        return int(kSeparatorHeight);
    }
    if (action->property(kSectionProperty).toBool()) {
        return int(kSectionHeight);
    }
    return int(kItemHeight);
}

QRectF FluentMenuFlyout::rowRect(int index, const QRectF &content) const
{
    qreal y = content.top() + kPaddingV;
    for (int walked = 0; walked < index; ++walked) {
        y += rowHeight(walked);
    }
    return QRectF(content.left(), y, content.width(), rowHeight(index));
}

QX_FLUENT_END_NAMESPACE
