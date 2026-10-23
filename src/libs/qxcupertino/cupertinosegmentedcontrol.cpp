/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "cupertinosegmentedcontrol.h"

#include "cupertinotokens.h"

#include "qxcontrols/qxshadowpainter.h"

#include <QtCore/QVariantAnimation>
#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>

QX_CUPERTINO_BEGIN_NAMESPACE

namespace
{
// The family's segmented control measures: a row thirty-two tall, a pill that
// sits two in from every edge, and labels with a few points of room either side
// so the segments read as equal even when the words are not.
constexpr qreal kHeight = 32.0;
constexpr qreal kPillInset = 2.0;
constexpr qreal kLabelPadding = 12.0;
constexpr qreal kMinimumSegment = 44.0;

// The row's own fill, and the lines between the segments: one value and a
// thinner one of the same grey, so nothing on the row argues with the pill.
constexpr qreal kFillOpacity = 0.32;
constexpr qreal kLineOpacity = 0.18;
constexpr qreal kLineInset = 8.0;
constexpr qreal kLineWidth = 1.0;

// A label that has not been picked is the ink thinned, which is how one row
// says which of its words is the answer without moving anything.
constexpr qreal kMutedLabelOpacity = 0.55;

// A disabled control keeps its drawing and loses its ink, as in the rest of the
// family.
constexpr qreal kDisabledOpacity = 0.4;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}

QRectF rowRect(const QWidget *widget)
{
    return QRectF(0.0, (widget->height() - kHeight) / 2.0, widget->width(), kHeight);
}

qreal segmentWidth(const QWidget *widget, int count)
{
    if (count <= 0) {
        return 0.0;
    }
    return (widget->width() - 2.0 * kPillInset) / count;
}

// The pill's place for a pick, as it is drawn: \a position is the pick as a
// fraction of a segment, so an animation can be halfway between two of them.
QRectF pillRect(const QWidget *widget, int count, qreal position)
{
    const QRectF row = rowRect(widget);
    const qreal width = segmentWidth(widget, count) - 2.0 * kPillInset;
    const qreal left = row.left() + kPillInset + position * segmentWidth(widget, count) + kPillInset;
    return QRectF(left, row.top() + kPillInset, qMax(qreal(0.0), width), row.height() - 2.0 * kPillInset);
}

// Which face of the semantic colours the control is drawn on.
bool onDarkFace(const QWidget *widget)
{
    return widget->palette().color(QPalette::Window).lightness() < 128;
}
}   // namespace

class CupertinoSegmentedControlPrivate
{
public:
    QX_DECLARE_PUBLIC(CupertinoSegmentedControl)

    QStringList items;
    int current = -1;
    /*! The pick as a fraction of a segment: 1.5 is halfway between the second
     *  and the third segment, which is what the pill slides through. */
    qreal pillPosition = 0.0;
    QVariantAnimation animation;
};

CupertinoSegmentedControl::CupertinoSegmentedControl(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new CupertinoSegmentedControlPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    connect(&d_ptr->animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        d_ptr->pillPosition = value.toReal();
        update();
    });
}

CupertinoSegmentedControl::~CupertinoSegmentedControl(){QX_FINI_PRIVATE()}

QStringList CupertinoSegmentedControl::items() const
{
    return d_ptr->items;
}

int CupertinoSegmentedControl::count() const
{
    return d_ptr->items.count();
}

int CupertinoSegmentedControl::currentIndex() const
{
    return d_ptr->current;
}

QString CupertinoSegmentedControl::currentText() const
{
    return d_ptr->current >= 0 ? d_ptr->items.at(d_ptr->current) : QString();
}

QSize CupertinoSegmentedControl::sizeHint() const
{
    int widest = 0;
    const QFontMetrics metrics(CupertinoType::font(CupertinoType::Ramp::Body));
    for (const QString &item : d_ptr->items) {
        widest = qMax(widest, metrics.horizontalAdvance(item));
    }
    const qreal segment = qMax(kMinimumSegment, qreal(widest) + 2.0 * kLabelPadding);
    return QSize(int(segment * qMax(1, count()) + 2.0 * kPillInset), int(kHeight));
}

QSize CupertinoSegmentedControl::minimumSizeHint() const
{
    return QSize(int(kMinimumSegment * qMax(1, count()) + 2.0 * kPillInset), int(kHeight));
}

void CupertinoSegmentedControl::setItems(const QStringList &items)
{
    if (d_ptr->items == items) {
        return;
    }
    d_ptr->items = items;
    // The pick survives a change of labels while it still names a segment;
    // otherwise the control picks the first one, because a row of segments with
    // none of them picked is a row that has not answered yet.
    const int kept = d_ptr->current >= 0 && d_ptr->current < d_ptr->items.count() ? d_ptr->current : 0;
    d_ptr->current = d_ptr->items.isEmpty() ? -1 : kept;
    d_ptr->animation.stop();
    d_ptr->pillPosition = qMax(0, d_ptr->current);
    updateGeometry();
    update();
    Q_EMIT currentIndexChanged(d_ptr->current);
}

void CupertinoSegmentedControl::addItem(const QString &text)
{
    QStringList next = d_ptr->items;
    next.append(text);
    setItems(next);
}

void CupertinoSegmentedControl::setCurrentIndex(int index)
{
    if (d_ptr->items.isEmpty()) {
        return;
    }
    const int bounded = qBound(0, index, d_ptr->items.count() - 1);
    if (d_ptr->current == bounded) {
        return;
    }
    d_ptr->current = bounded;

    // The pill slides on the family's normal duration: what moves is where the
    // pick is, and a pill that jumped would say the pick had always been there.
    const QxTheme::QxMotion motion =
        CupertinoMotion::forDuration(CupertinoMotion::Duration::Normal, CupertinoMotion::Easing::Standard);
    d_ptr->animation.stop();
    if (motion.duration <= 0) {
        d_ptr->pillPosition = bounded;
        update();
    } else {
        d_ptr->animation.setDuration(motion.duration);
        d_ptr->animation.setEasingCurve(motion.curve);
        d_ptr->animation.setStartValue(d_ptr->pillPosition);
        d_ptr->animation.setEndValue(qreal(bounded));
        d_ptr->animation.start();
    }
    Q_EMIT currentIndexChanged(bounded);
}

qreal CupertinoSegmentedControl::cornerRadius() const
{
    return CupertinoShape::radius(CupertinoShape::Radius::Medium);
}

void CupertinoSegmentedControl::paintSurface(QPainter *painter)
{
    const QRectF row = rowRect(this);
    const qreal radius = CupertinoShape::radius(CupertinoShape::Radius::Medium);
    const int segments = count();
    const bool dark = onDarkFace(this);

    painter->save();
    painter->setPen(Qt::NoPen);
    if (!isEnabled()) {
        painter->setOpacity(kDisabledOpacity);
    }

    // The row: the family's grey thinned over the page, so the segments read as
    // one object with the page showing through its edges.
    QColor fill = CupertinoColor::color(CupertinoColor::Role::Gray, dark);
    fill.setAlphaF(kFillOpacity);
    painter->setBrush(fill);
    painter->drawRoundedRect(row, radius, radius);

    // The lines between the segments, before the pill: the pill then covers the
    // ones it passes over, which is the movement read as one thing rather than
    // as a shape appearing in a new cell.
    if (segments > 1) {
        QColor line = CupertinoColor::color(CupertinoColor::Role::Gray, dark);
        line.setAlphaF(kLineOpacity);
        painter->setPen(QPen(line, kLineWidth));
        const qreal width = segmentWidth(this, segments);
        for (int i = 1; i < segments; ++i) {
            const qreal x = row.left() + kPillInset + i * width;
            painter->drawLine(QPointF(x, row.top() + kLineInset), QPointF(x, row.bottom() - kLineInset));
        }
        painter->setPen(Qt::NoPen);
    }

    // The pill: the one raised thing on the row, and the reason this control is
    // where the family's raised level is spent.
    const QRectF pill = pillRect(this, segments, d_ptr->pillPosition);
    if (pill.isValid()) {
        const qreal pillRadius = CupertinoShape::radius(CupertinoShape::Radius::Small);
        QxControls::QxShadowPainter::paint(painter, pill, pillRadius,
                                           CupertinoColor::color(CupertinoColor::Role::Gray, dark),
                                           CupertinoElevation::shadow(CupertinoElevation::Level::Raised));
        painter->setBrush(Qt::white);
        painter->drawRoundedRect(pill, pillRadius, pillRadius);
    }

    painter->restore();
}

void CupertinoSegmentedControl::paintContent(QPainter *painter)
{
    const int segments = count();
    if (segments <= 0) {
        return;
    }
    const QRectF row = rowRect(this);
    const qreal width = segmentWidth(this, segments);

    painter->save();
    if (!isEnabled()) {
        painter->setOpacity(kDisabledOpacity);
    }
    painter->setFont(CupertinoType::font(CupertinoType::Ramp::Body));
    for (int i = 0; i < segments; ++i) {
        const QRectF cell(row.left() + kPillInset + i * width, row.top(), width, row.height());
        QColor ink = textColor();
        if (i != d_ptr->current) {
            ink.setAlphaF(kMutedLabelOpacity);
        }
        painter->setPen(ink);
        painter->drawText(cell, Qt::AlignCenter, d_ptr->items.at(i));
    }
    painter->restore();
}

void CupertinoSegmentedControl::mousePressEvent(QMouseEvent *event)
{
    QxControls::QxControl::mousePressEvent(event);
    const int segments = count();
    if (!isEnabled() || event->button() != Qt::LeftButton || segments <= 0) {
        return;
    }
    const qreal width = segmentWidth(this, segments);
    const qreal offset = eventPos(event).x() - kPillInset;
    if (width <= 0.0 || offset < 0.0) {
        return;
    }
    setCurrentIndex(qBound(0, int(offset / width), segments - 1));
}

void CupertinoSegmentedControl::keyPressEvent(QKeyEvent *event)
{
    // The row is one stop, so the arrows walk the pick rather than the focus:
    // that is the whole reason this control is not a row of radio buttons.
    const int key = event->key();
    switch (key) {
    case Qt::Key_Left:
    case Qt::Key_Up:
        setCurrentIndex(d_ptr->current - 1);
        event->accept();
        return;
    case Qt::Key_Right:
    case Qt::Key_Down:
        setCurrentIndex(d_ptr->current + 1);
        event->accept();
        return;
    case Qt::Key_Home:
        setCurrentIndex(0);
        event->accept();
        return;
    case Qt::Key_End:
        setCurrentIndex(count() - 1);
        event->accept();
        return;
    default:
        break;
    }
    QxControls::QxControl::keyPressEvent(event);
}

QAccessible::Role CupertinoSegmentedControl::accessibleRole() const
{
    // Qt has no role for a segmented control; the closest true answer is a tab
    // bar - one of several, one of them current - and the labels say which.
    return QAccessible::PageTabList;
}

void CupertinoSegmentedControl::fillAccessibleState(QAccessible::State *state) const
{
    state->focusable = true;
}

QX_CUPERTINO_END_NAMESPACE
