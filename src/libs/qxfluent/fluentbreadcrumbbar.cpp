/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentbreadcrumbbar.h"

#include "fluenttokens.h"

#include <QtGui/QFontMetricsF>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtMath>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// The room a step takes round its text, and the separator between two steps.
constexpr qreal kStepPadding = 8.0;
constexpr qreal kSeparatorWidth = 12.0;
constexpr qreal kChevronSize = 5.0;

// The step a control takes down when nobody can use it - the same .4 the rest
// of the family keeps.
constexpr qreal kDisabledOpacity = 0.4;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}
}   // namespace

class FluentBreadcrumbBarPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentBreadcrumbBar)

    QStringList items;
    int hovered = -1;
    int focused = -1;

    int stepWidth(int index) const
    {
        if (index < 0 || index >= items.count()) {
            return 0;
        }
        // A step is as wide as the word it holds, measured in the ramp that
        // word is drawn in - and the last step is drawn strong while the ones
        // before it are not. Measured in one ramp and drawn in the other, the
        // room is short by the weight, and the last letter goes over the edge:
        // that is how "Controls" came out as "Control".
        const bool strong = index == items.count() - 1;
        const QFontMetricsF metrics(FluentType::font(FluentType::Ramp::Body, strong));
        return int(qCeil(metrics.horizontalAdvance(items.at(index)) + 2 * kStepPadding));
    }

    int stepLeft(int index) const
    {
        int x = 0;
        for (int i = 0; i < index; ++i) {
            x += stepWidth(i) + int(kSeparatorWidth);
        }
        return x;
    }

    QRectF stepRect(int index) const
    {
        return QRectF(stepLeft(index), 0.0, stepWidth(index), 0.0);
    }

    int stepAt(const QPointF &pos, int height) const
    {
        Q_UNUSED(height)
        for (int i = 0; i < items.count(); ++i) {
            const int left = stepLeft(i);
            if (pos.x() >= left && pos.x() < left + stepWidth(i)) {
                return i;
            }
        }
        return -1;
    }
};

FluentBreadcrumbBar::FluentBreadcrumbBar(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentBreadcrumbBarPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // Which step the pointer is on is part of what the bar says, so moves are
    // wanted without a button.
    setMouseTracking(true);
}

FluentBreadcrumbBar::~FluentBreadcrumbBar(){QX_FINI_PRIVATE()}

QStringList FluentBreadcrumbBar::items() const
{
    return d_ptr->items;
}

void FluentBreadcrumbBar::setItems(const QStringList &items)
{
    if (d_ptr->items == items) {
        return;
    }
    d_ptr->items = items;
    d_ptr->hovered = -1;
    d_ptr->focused = -1;
    // What an assistive tool reads is the path, not a row of buttons: the same
    // thing a sighted user reads, in one sentence.
    setAccessibleName(items.join(QStringLiteral(" / ")));
    updateGeometry();
    update();
}

QSize FluentBreadcrumbBar::sizeHint() const
{
    if (d_ptr->items.isEmpty()) {
        const QFontMetricsF metrics(FluentType::font(FluentType::Ramp::Body));
        return QSize(0, int(qCeil(metrics.height() + 2 * kStepPadding)));
    }
    const int width = d_ptr->stepLeft(d_ptr->items.count() - 1) + d_ptr->stepWidth(d_ptr->items.count() - 1);
    const QFontMetricsF metrics(FluentType::font(FluentType::Ramp::Body));
    return QSize(width, int(qCeil(metrics.height() + 2 * kStepPadding)));
}

QSize FluentBreadcrumbBar::minimumSizeHint() const
{
    return sizeHint();
}

QRectF FluentBreadcrumbBar::chromeRect() const
{
    // The wash and the ring belong to the step the user is on - the one the
    // pointer is over, or the one the keyboard walked to.
    const int index = d_ptr->hovered >= 0 ? d_ptr->hovered : d_ptr->focused;
    if (index < 0) {
        return QRectF();
    }
    QRectF step = d_ptr->stepRect(index);
    step.setHeight(height());
    return step;
}

void FluentBreadcrumbBar::paintContent(QPainter *painter)
{
    if (d_ptr->items.isEmpty()) {
        return;
    }
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    // A path nobody can walk is still the path, written fainter: the steps, the
    // separators and the chevron all step down together.
    painter->setOpacity(isEnabled() ? 1.0 : kDisabledOpacity);

    for (int i = 0; i < d_ptr->items.count(); ++i) {
        const QRectF step = d_ptr->stepRect(i);
        const bool current = i == d_ptr->items.count() - 1;
        painter->setFont(FluentType::font(FluentType::Ramp::Body, current));
        painter->setPen(textColor());
        painter->drawText(QRectF(step.left() + kStepPadding, 0.0, step.width() - 2 * kStepPadding, height()),
                          Qt::AlignLeft | Qt::AlignVCenter, d_ptr->items.at(i));

        if (current) {
            break;
        }
        // The separator is not part of either step: it is the gap, drawn - a
        // ">" typed into a label would be part of the text an assistive tool
        // reads, and a chevron drawn here is not.
        const qreal x = step.right() + (kSeparatorWidth - kChevronSize) / 2.0;
        const qreal y = height() / 2.0;
        painter->setPen(QPen(textColor(), 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        QPainterPath chevron;
        chevron.moveTo(x, y - kChevronSize);
        chevron.lineTo(x + kChevronSize, y);
        chevron.lineTo(x, y + kChevronSize);
        painter->drawPath(chevron);
    }
    painter->restore();
}

void FluentBreadcrumbBar::mousePressEvent(QMouseEvent *event)
{
    QxControls::QxControl::mousePressEvent(event);
    if (!isEnabled() || event->button() != Qt::LeftButton) {
        return;
    }
    const int index = d_ptr->stepAt(eventPos(event), height());
    // The last step is where the user is, and going there is not a move.
    if (index < 0 || index >= d_ptr->items.count() - 1) {
        return;
    }
    Q_EMIT itemClicked(index, d_ptr->items.at(index));
}

void FluentBreadcrumbBar::mouseMoveEvent(QMouseEvent *event)
{
    QxControls::QxControl::mouseMoveEvent(event);
    const int index = isEnabled() ? d_ptr->stepAt(eventPos(event), height()) : -1;
    if (index == d_ptr->hovered) {
        return;
    }
    d_ptr->hovered = index;
    update();
}

void FluentBreadcrumbBar::leaveEvent(QEvent *event)
{
    QxControls::QxControl::leaveEvent(event);
    if (d_ptr->hovered == -1) {
        return;
    }
    d_ptr->hovered = -1;
    update();
}

void FluentBreadcrumbBar::keyPressEvent(QKeyEvent *event)
{
    const int last = d_ptr->items.count() - 1;
    const int key = event->key();
    if (key == Qt::Key_Left || key == Qt::Key_Right) {
        const int from = d_ptr->focused >= 0 ? d_ptr->focused : (key == Qt::Key_Right ? -1 : last);
        const int next = qBound(0, from + (key == Qt::Key_Right ? 1 : -1), last);
        d_ptr->focused = next;
        update();
        event->accept();
        return;
    }
    if (key == Qt::Key_Enter || key == Qt::Key_Return || key == Qt::Key_Space) {
        if (d_ptr->focused >= 0 && d_ptr->focused < last) {
            Q_EMIT itemClicked(d_ptr->focused, d_ptr->items.at(d_ptr->focused));
        }
        event->accept();
        return;
    }
    QxControls::QxControl::keyPressEvent(event);
}

QAccessible::Role FluentBreadcrumbBar::accessibleRole() const
{
    return QAccessible::StaticText;
}

QX_FLUENT_END_NAMESPACE
