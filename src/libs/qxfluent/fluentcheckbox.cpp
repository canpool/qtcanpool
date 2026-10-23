/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentcheckbox.h"

#include "fluenttokens.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include <QtWidgets/QStyle>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// Fluent's check box measures: a box of twenty, two of them turned by the
// corner, and eight between it and the words.
constexpr qreal kBoxSize = 20.0;
constexpr qreal kGap = 8.0;
// The tick and the bar are strokes, not glyphs: a font's tick is the font's
// tick wherever the family goes.
constexpr qreal kMarkInset = 5.0;
constexpr qreal kBorderOpacity = 0.66;
constexpr qreal kDisabledOpacity = 0.4;
}   // namespace

class FluentCheckBoxPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentCheckBox)

    QString text;
    Qt::CheckState state = Qt::Unchecked;
    bool tristate = false;

    /*! The box, in widget coordinates: the wash and the ring belong to it,
     *  the label beside it does not. */
    QRectF boxRect() const;
    /*! The state a press asks for: the two-state box turns over, the
     *  three-state one walks the three answers a list can give. */
    Qt::CheckState nextState() const;
};

QRectF FluentCheckBoxPrivate::boxRect() const
{
    Q_Q(const FluentCheckBox);
    const qreal top = (qreal(q->height()) - kBoxSize) / 2.0;
    return QRectF(0.0, top, kBoxSize, kBoxSize);
}

Qt::CheckState FluentCheckBoxPrivate::nextState() const
{
    if (!tristate) {
        return state == Qt::Checked ? Qt::Unchecked : Qt::Checked;
    }
    switch (state) {
    case Qt::Unchecked:
        return Qt::Checked;
    case Qt::PartiallyChecked:
        return Qt::Unchecked;
    case Qt::Checked:
    default:
        return Qt::PartiallyChecked;
    }
}

FluentCheckBox::FluentCheckBox(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentCheckBoxPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setFocusPolicy(Qt::StrongFocus);
    // A click turns the box over; the three-state box walks the three
    // answers a list can give. The keyboard asks with the same signal.
    connect(this, &FluentCheckBox::clicked, this, [this]() {
        setCheckState(d_ptr->nextState());
    });
}

FluentCheckBox::~FluentCheckBox(){QX_FINI_PRIVATE()}

QString FluentCheckBox::text() const
{
    return d_ptr->text;
}

Qt::CheckState FluentCheckBox::checkState() const
{
    return d_ptr->state;
}

bool FluentCheckBox::isChecked() const
{
    return d_ptr->state == Qt::Checked;
}

bool FluentCheckBox::isTristate() const
{
    return d_ptr->tristate;
}

void FluentCheckBox::setText(const QString &text)
{
    if (d_ptr->text == text) {
        return;
    }
    d_ptr->text = text;
    Q_EMIT textChanged(text);
    updateGeometry();
    update();
}

void FluentCheckBox::setCheckState(Qt::CheckState state)
{
    if (d_ptr->state == state) {
        return;
    }
    d_ptr->state = state;
    Q_EMIT stateChanged(state);
    Q_EMIT toggled(state == Qt::Checked);
    update();
}

void FluentCheckBox::setChecked(bool checked)
{
    setCheckState(checked ? Qt::Checked : Qt::Unchecked);
}

void FluentCheckBox::setTristate(bool tristate)
{
    d_ptr->tristate = tristate;
}

QSize FluentCheckBox::sizeHint() const
{
    const QFontMetrics metrics(FluentType::font(FluentType::Ramp::Body));
    const int textWidth = d_ptr->text.isEmpty() ? 0 : metrics.horizontalAdvance(d_ptr->text);
    const int height = qMax(int(kBoxSize), metrics.height());
    return QSize(int(kBoxSize) + (textWidth ? int(kGap) + textWidth : 0), height);
}

QSize FluentCheckBox::minimumSizeHint() const
{
    return sizeHint();
}

qreal FluentCheckBox::cornerRadius() const
{
    return FluentShape::radius(FluentShape::Radius::Small);
}

QRectF FluentCheckBox::chromeRect() const
{
    return d_ptr->boxRect();
}

bool FluentCheckBox::hitTest(const QPointF &pos) const
{
    // The row is what looks like the control: the label is not decoration
    // beside a small box, it is half of the same answer.
    return QRectF(0.0, 0.0, qreal(width()), qreal(height())).contains(pos);
}

void FluentCheckBox::paintSurface(QPainter *painter)
{
    const QRectF box = d_ptr->boxRect();
    const qreal radius = cornerRadius();
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    if (d_ptr->state == Qt::Unchecked) {
        // The empty box is the page under a border that says where it is;
        // disabled, the border thins the way everything else does.
        painter->setOpacity(isEnabled() ? 1.0 : kDisabledOpacity);
        painter->setBrush(
            QxControls::QxStateLayer::layered(palette().color(QPalette::Window), textColor(), kBorderOpacity));
        painter->drawRoundedRect(box, radius, radius);
        painter->setBrush(palette().color(QPalette::Window));
        painter->drawRoundedRect(box.adjusted(1.0, 1.0, -1.0, -1.0), radius, radius);
    } else {
        // A box with an answer wears the accent, and the answer is a stroke
        // in the colour that reads on it.
        painter->setBrush(accentColor());
        painter->drawRoundedRect(box, radius, radius);
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->setPen(QPen(QColor(Qt::white), 1.6));
        if (d_ptr->state == Qt::Checked) {
            // The tick: three strokes of one polyline, drawn where the box
            // leaves room for it.
            const QPolygonF tick({QPointF(box.left() + kMarkInset, box.top() + kBoxSize / 2.0),
                                  QPointF(box.left() + kBoxSize / 2.0, box.bottom() - kMarkInset),
                                  QPointF(box.right() - kMarkInset, box.top() + kMarkInset)});
            painter->drawPolyline(tick);
        } else {
            // The partial answer is the bar: some of it, said flatly.
            const qreal bar = kBoxSize - 2 * kMarkInset;
            painter->drawLine(QPointF(box.left() + kMarkInset, box.center().y()),
                              QPointF(box.left() + kMarkInset + bar, box.center().y()));
        }
    }
    painter->restore();
}

void FluentCheckBox::paintContent(QPainter *painter)
{
    if (d_ptr->text.isEmpty()) {
        return;
    }
    painter->save();
    painter->setOpacity(isEnabled() ? 1.0 : kDisabledOpacity);
    painter->setPen(textColor());
    painter->setFont(FluentType::font(FluentType::Ramp::Body));
    const QRectF row(0.0, 0.0, qreal(width()), qreal(height()));
    painter->drawText(row.adjusted(kBoxSize + kGap, 0.0, 0.0, 0.0), Qt::AlignVCenter | Qt::TextSingleLine, d_ptr->text);
    painter->restore();
}

void FluentCheckBox::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space) {
        event->accept();
        return;
    }
    QxControls::QxControl::keyPressEvent(event);
}

void FluentCheckBox::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        // The turn itself follows from clicked(), the same answer a press
        // on the row gives.
        Q_EMIT clicked();
        event->accept();
        return;
    }
    QxControls::QxControl::keyReleaseEvent(event);
}

QAccessible::Role FluentCheckBox::accessibleRole() const
{
    return QAccessible::CheckBox;
}

void FluentCheckBox::fillAccessibleState(QAccessible::State *state) const
{
    state->checkable = true;
    state->checked = d_ptr->state == Qt::Checked;
    state->checkStateMixed = d_ptr->tristate && d_ptr->state == Qt::PartiallyChecked;
}

QX_FLUENT_END_NAMESPACE
