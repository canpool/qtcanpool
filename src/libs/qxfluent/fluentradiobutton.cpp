/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentradiobutton.h"

#include "fluenttokens.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// The circle is the box the check box keeps, drawn round; the dot inside it
// is what the answer looks like from the outside.
constexpr qreal kSize = 20.0;
constexpr qreal kGap = 8.0;
constexpr qreal kRingWidth = 1.6;
constexpr qreal kDotSize = 8.0;
constexpr qreal kRingOpacity = 0.66;
constexpr qreal kDisabledOpacity = 0.4;
}   // namespace

class FluentRadioButtonPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentRadioButton)

    QString text;
    bool checked = false;

    /*! The circle, in widget coordinates: the wash and the ring belong to
     *  it, the label beside it does not. */
    QRectF circleRect() const;
};

QRectF FluentRadioButtonPrivate::circleRect() const
{
    Q_Q(const FluentRadioButton);
    const qreal top = (qreal(q->height()) - kSize) / 2.0;
    return QRectF(0.0, top, kSize, kSize);
}

FluentRadioButton::FluentRadioButton(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentRadioButtonPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setFocusPolicy(Qt::StrongFocus);
    // A press puts the pressed one in force; the turn itself follows from
    // clicked(), the same answer the keyboard gives.
    connect(this, &FluentRadioButton::clicked, this, [this]() {
        setChecked(true);
    });
}

FluentRadioButton::~FluentRadioButton(){QX_FINI_PRIVATE()}

QString FluentRadioButton::text() const
{
    return d_ptr->text;
}

bool FluentRadioButton::isChecked() const
{
    return d_ptr->checked;
}

void FluentRadioButton::setText(const QString &text)
{
    if (d_ptr->text == text) {
        return;
    }
    d_ptr->text = text;
    Q_EMIT textChanged(text);
    updateGeometry();
    update();
}

void FluentRadioButton::setChecked(bool checked)
{
    if (d_ptr->checked == checked) {
        return;
    }
    d_ptr->checked = checked;
    Q_EMIT toggled(checked);
    update();
    // The choice is one of several: the brothers that share its parent come
    // out of force with it, and they say so themselves.
    if (checked && parentWidget()) {
        const QList<FluentRadioButton *> brothers =
            parentWidget()->findChildren<FluentRadioButton *>(QString(), Qt::FindDirectChildrenOnly);
        for (FluentRadioButton *brother : brothers) {
            if (brother != this && brother->isChecked()) {
                brother->setChecked(false);
            }
        }
    }
}

QSize FluentRadioButton::sizeHint() const
{
    const QFontMetrics metrics(FluentType::font(FluentType::Ramp::Body));
    const int textWidth = d_ptr->text.isEmpty() ? 0 : metrics.horizontalAdvance(d_ptr->text);
    const int height = qMax(int(kSize), metrics.height());
    return QSize(int(kSize) + (textWidth ? int(kGap) + textWidth : 0), height);
}

QSize FluentRadioButton::minimumSizeHint() const
{
    return sizeHint();
}

QRectF FluentRadioButton::chromeRect() const
{
    return d_ptr->circleRect();
}

bool FluentRadioButton::hitTest(const QPointF &pos) const
{
    // The row is what looks like the control: the label is not decoration
    // beside a small circle, it is half of the same answer.
    return QRectF(0.0, 0.0, qreal(width()), qreal(height())).contains(pos);
}

void FluentRadioButton::paintSurface(QPainter *painter)
{
    const QRectF circle = d_ptr->circleRect();
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setOpacity(isEnabled() ? 1.0 : kDisabledOpacity);
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(d_ptr->checked ? accentColor()
                                        : QxControls::QxStateLayer::layered(palette().color(QPalette::Window),
                                                                            textColor(), kRingOpacity),
                         kRingWidth));
    painter->drawEllipse(circle.adjusted(kRingWidth / 2.0, kRingWidth / 2.0, -kRingWidth / 2.0, -kRingWidth / 2.0));
    if (d_ptr->checked) {
        // The answer inside: the accent's own dot, said once.
        painter->setPen(Qt::NoPen);
        painter->setBrush(accentColor());
        painter->drawEllipse(circle.adjusted((kSize - kDotSize) / 2.0, (kSize - kDotSize) / 2.0,
                                             -(kSize - kDotSize) / 2.0, -(kSize - kDotSize) / 2.0));
    }
    painter->restore();
}

void FluentRadioButton::paintContent(QPainter *painter)
{
    if (d_ptr->text.isEmpty()) {
        return;
    }
    painter->save();
    painter->setOpacity(isEnabled() ? 1.0 : kDisabledOpacity);
    painter->setPen(textColor());
    painter->setFont(FluentType::font(FluentType::Ramp::Body));
    const QRectF row(0.0, 0.0, qreal(width()), qreal(height()));
    painter->drawText(row.adjusted(kSize + kGap, 0.0, 0.0, 0.0), Qt::AlignVCenter | Qt::TextSingleLine, d_ptr->text);
    painter->restore();
}

void FluentRadioButton::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space) {
        event->accept();
        return;
    }
    QxControls::QxControl::keyPressEvent(event);
}

void FluentRadioButton::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        Q_EMIT clicked();
        event->accept();
        return;
    }
    QxControls::QxControl::keyReleaseEvent(event);
}

QAccessible::Role FluentRadioButton::accessibleRole() const
{
    return QAccessible::RadioButton;
}

void FluentRadioButton::fillAccessibleState(QAccessible::State *state) const
{
    state->checkable = true;
    state->checked = d_ptr->checked;
}

QX_FLUENT_END_NAMESPACE
