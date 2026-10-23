/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluenttooltip.h"

#include "fluenttokens.h"

#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtWidgets/QWidget>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// The arrival delay and the stay: a tooltip opens once an arrival has proved
// it means to stay, and closes on its own once its welcome is spent.
constexpr int kDelayMs = 400;
constexpr int kDwellMs = 5000;
// The room the tooltip keeps from its anchor, and the room its words keep
// from its own edges.
constexpr qreal kGap = 8.0;
constexpr qreal kPaddingWidth = 8.0;
constexpr qreal kPaddingHeight = 4.0;
}   // namespace

class FluentToolTipPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentToolTip)

    QWidget *anchor = nullptr;
    QString text;
    FluentToolTip::Placement placement = FluentToolTip::Placement::Top;
    QTimer delayer;
    QTimer dweller;
};

FluentToolTip::FluentToolTip(QWidget *parent)
    : QxControls::QxControl(nullptr)
    , d_ptr(new FluentToolTipPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // A tooltip is a note, not a window the user works: it sits over the
    // page, takes no mouse, no focus, and never steps in front of what the
    // user was doing when it appeared.
    setWindowFlags(Qt::ToolTip);
    setAttribute(Qt::WA_ShowWithoutActivating, true);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setFocusPolicy(Qt::NoFocus);

    d_ptr->delayer.setSingleShot(true);
    d_ptr->delayer.setInterval(kDelayMs);
    QObject::connect(&d_ptr->delayer, &QTimer::timeout, this, [this]() {
        popup();
    });
    d_ptr->dweller.setSingleShot(true);
    d_ptr->dweller.setInterval(kDwellMs);
    QObject::connect(&d_ptr->dweller, &QTimer::timeout, this, [this]() {
        hide();
    });

    if (parent) {
        setAnchor(parent);
    }
}

FluentToolTip::~FluentToolTip()
{
    stopWatching();
    QX_FINI_PRIVATE()
}

void FluentToolTip::setAnchor(QWidget *anchor)
{
    if (d_ptr->anchor == anchor) {
        return;
    }
    stopWatching();
    d_ptr->anchor = anchor;
    if (d_ptr->anchor) {
        d_ptr->anchor->installEventFilter(this);
        QObject::connect(d_ptr->anchor, &QObject::destroyed, this, [this]() {
            d_ptr->anchor = nullptr;
            stopWatching();
            hide();
        });
    }
}

QWidget *FluentToolTip::anchor() const
{
    return d_ptr->anchor;
}

QString FluentToolTip::text() const
{
    return d_ptr->text;
}

FluentToolTip::Placement FluentToolTip::placement() const
{
    return d_ptr->placement;
}

QSize FluentToolTip::sizeHint() const
{
    const QFontMetrics metrics(fontMetrics());
    return QSize(metrics.horizontalAdvance(d_ptr->text) + int(2 * kPaddingWidth),
                 metrics.height() + int(2 * kPaddingHeight));
}

void FluentToolTip::setText(const QString &text)
{
    if (d_ptr->text == text) {
        return;
    }
    d_ptr->text = text;
    updateGeometry();
    update();
}

void FluentToolTip::setPlacement(Placement placement)
{
    if (d_ptr->placement == placement) {
        return;
    }
    d_ptr->placement = placement;
    if (isVisible()) {
        place();
    }
}

void FluentToolTip::popup()
{
    if (!d_ptr->anchor) {
        return;
    }
    place();
    show();
    d_ptr->dweller.start();
}

void FluentToolTip::paintSurface(QPainter *painter)
{
    // The ink turns surface and the page's colour turns text: a note about
    // the page, not a part of it.
    QPainterPath path;
    const qreal radius = FluentShape::radius(FluentShape::Radius::Small);
    path.addRoundedRect(QRectF(rect()), radius, radius);
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->fillPath(path, textColor());
    painter->restore();
}

void FluentToolTip::paintContent(QPainter *painter)
{
    painter->save();
    painter->setPen(palette().color(QPalette::Window));
    painter->drawText(QRectF(rect()), Qt::AlignCenter, d_ptr->text);
    painter->restore();
}

bool FluentToolTip::eventFilter(QObject *watched, QEvent *event)
{
    if (watched != d_ptr->anchor) {
        return QxControls::QxControl::eventFilter(watched, event);
    }
    switch (event->type()) {
    case QEvent::Enter:
    case QEvent::FocusIn:
        // The arrival might be only passing through; the delay is where that
        // is found out. A second arrival restarts both clocks.
        d_ptr->dweller.stop();
        d_ptr->delayer.start();
        break;
    case QEvent::Leave:
    case QEvent::FocusOut:
    case QEvent::Hide:
        d_ptr->delayer.stop();
        d_ptr->dweller.stop();
        hide();
        break;
    default:
        break;
    }
    return QxControls::QxControl::eventFilter(watched, event);
}

QAccessible::Role FluentToolTip::accessibleRole() const
{
    return QAccessible::ToolTip;
}

void FluentToolTip::place()
{
    if (!d_ptr->anchor) {
        return;
    }
    const QSize hint = sizeHint();
    const QRectF anchorRect(d_ptr->anchor->mapToGlobal(QPoint(0, 0)), QSizeF(d_ptr->anchor->size()));
    qreal x = 0.0;
    qreal y = 0.0;
    switch (d_ptr->placement) {
    case Placement::Bottom:
        x = anchorRect.center().x() - hint.width() / 2.0;
        y = anchorRect.bottom() + kGap;
        break;
    case Placement::Left:
        x = anchorRect.left() - kGap - hint.width();
        y = anchorRect.center().y() - hint.height() / 2.0;
        break;
    case Placement::Right:
        x = anchorRect.right() + kGap;
        y = anchorRect.center().y() - hint.height() / 2.0;
        break;
    case Placement::Top:
    default:
        x = anchorRect.center().x() - hint.width() / 2.0;
        y = anchorRect.top() - kGap - hint.height();
        break;
    }
    setGeometry(int(x), int(y), hint.width(), hint.height());
}

void FluentToolTip::stopWatching()
{
    if (d_ptr->anchor) {
        d_ptr->anchor->removeEventFilter(this);
    }
    d_ptr->delayer.stop();
    d_ptr->dweller.stop();
}

QX_FLUENT_END_NAMESPACE
