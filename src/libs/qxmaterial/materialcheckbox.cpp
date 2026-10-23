/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialcheckbox.h"

#include "materialripple.h"
#include "materialtokens.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// Material's check box measures: a box of eighteen turned by two at the
// corner, and eight between it and the words.
constexpr qreal kBoxSize = 18.0;
constexpr qreal kGap = 8.0;
constexpr qreal kMarkInset = 4.5;
constexpr qreal kTickWidth = 1.6;
constexpr qreal kBorderOpacity = 0.54;
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

class MaterialCheckBoxPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialCheckBox)

    QString text;
    Qt::CheckState state = Qt::Unchecked;
    bool tristate = false;
    MaterialRipple *ripple = Q_NULLPTR;

    /*! The box, in widget coordinates: the control itself, against the row
     *  that only looks like it. */
    QRectF boxRect() const;
    /*! The state a press asks for: the two-state box turns over, the
     *  three-state one walks the three answers a list can give. */
    Qt::CheckState nextState() const;
};

QRectF MaterialCheckBoxPrivate::boxRect() const
{
    Q_Q(const MaterialCheckBox);
    const qreal top = (qreal(q->height()) - kBoxSize) / 2.0;
    return QRectF(0.0, top, kBoxSize, kBoxSize);
}

Qt::CheckState MaterialCheckBoxPrivate::nextState() const
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

MaterialCheckBox::MaterialCheckBox(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new MaterialCheckBoxPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setFocusPolicy(Qt::StrongFocus);
    // A click turns the box over; the keyboard asks with the same signal.
    connect(this, &MaterialCheckBox::clicked, this, [this]() {
        setCheckState(d_ptr->nextState());
    });
    // The press is answered from the box, not from where the pointer landed:
    // the ripple starts at the box's own centre and stays inside it.
    d_ptr->ripple = new MaterialRipple(this);
    d_ptr->ripple->setWatchPress(false);
    d_ptr->ripple->setColor(textColor());
}

MaterialCheckBox::~MaterialCheckBox(){QX_FINI_PRIVATE()}

QString MaterialCheckBox::text() const
{
    return d_ptr->text;
}

Qt::CheckState MaterialCheckBox::checkState() const
{
    return d_ptr->state;
}

bool MaterialCheckBox::isChecked() const
{
    return d_ptr->state == Qt::Checked;
}

bool MaterialCheckBox::isTristate() const
{
    return d_ptr->tristate;
}

void MaterialCheckBox::setText(const QString &text)
{
    if (d_ptr->text == text) {
        return;
    }
    d_ptr->text = text;
    Q_EMIT textChanged(text);
    updateGeometry();
    update();
}

void MaterialCheckBox::setCheckState(Qt::CheckState state)
{
    if (d_ptr->state == state) {
        return;
    }
    d_ptr->state = state;
    Q_EMIT stateChanged(state);
    Q_EMIT toggled(state == Qt::Checked);
    update();
}

void MaterialCheckBox::setChecked(bool checked)
{
    setCheckState(checked ? Qt::Checked : Qt::Unchecked);
}

void MaterialCheckBox::setTristate(bool tristate)
{
    d_ptr->tristate = tristate;
}

QSize MaterialCheckBox::sizeHint() const
{
    const QFontMetrics metrics(MaterialType::font(MaterialType::Scale::Body));
    const int textWidth = d_ptr->text.isEmpty() ? 0 : metrics.horizontalAdvance(d_ptr->text);
    const int height = qMax(int(kBoxSize), metrics.height());
    return QSize(int(kBoxSize) + (textWidth ? int(kGap) + textWidth : 0), height);
}

QSize MaterialCheckBox::minimumSizeHint() const
{
    return sizeHint();
}

QRectF MaterialCheckBox::chromeRect() const
{
    return d_ptr->boxRect();
}

bool MaterialCheckBox::hitTest(const QPointF &pos) const
{
    // The row is what looks like the control: the label is not decoration
    // beside a small box, it is half of the same answer.
    return QRectF(0.0, 0.0, qreal(width()), qreal(height())).contains(pos);
}

void MaterialCheckBox::paintSurface(QPainter *painter)
{
    const QRectF box = d_ptr->boxRect();
    const qreal radius = 2.0;
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    if (d_ptr->state == Qt::Unchecked) {
        // The empty box is the page under a border that says where it is.
        painter->setOpacity(isEnabled() ? 1.0 : kDisabledOpacity);
        painter->setBrush(
            QxControls::QxStateLayer::layered(palette().color(QPalette::Window), textColor(), kBorderOpacity));
        painter->drawRoundedRect(box, radius, radius);
        painter->setBrush(palette().color(QPalette::Window));
        painter->drawRoundedRect(box.adjusted(1.0, 1.0, -1.0, -1.0), radius, radius);
    } else {
        // A box with an answer wears the primary, and the answer is a stroke
        // in the colour that reads on it.
        painter->setBrush(accentColor());
        painter->drawRoundedRect(box, radius, radius);
        painter->setRenderHint(QPainter::Antialiasing, false);
        painter->setPen(QPen(QColor(Qt::white), kTickWidth));
        if (d_ptr->state == Qt::Checked) {
            const QPolygonF tick({QPointF(box.left() + kMarkInset, box.center().y()),
                                  QPointF(box.center().x(), box.bottom() - kMarkInset),
                                  QPointF(box.right() - kMarkInset, box.top() + kMarkInset)});
            painter->drawPolyline(tick);
        } else {
            // The partial answer is the bar: some of it, said flatly.
            const qreal bar = kBoxSize - 2 * kMarkInset;
            painter->drawLine(QPointF(box.left() + kMarkInset, box.center().y()),
                              QPointF(box.left() + kMarkInset + bar, box.center().y()));
        }
    }
    // The ripple stays inside the answer it belongs to, whatever the answer.
    if (isEnabled()) {
        d_ptr->ripple->paint(painter, box, radius);
    }
    painter->restore();
}

void MaterialCheckBox::paintContent(QPainter *painter)
{
    if (d_ptr->text.isEmpty()) {
        return;
    }
    painter->save();
    painter->setOpacity(isEnabled() ? 1.0 : kDisabledOpacity);
    painter->setPen(textColor());
    painter->setFont(MaterialType::font(MaterialType::Scale::Body));
    const QRectF row(0.0, 0.0, qreal(width()), qreal(height()));
    painter->drawText(row.adjusted(kBoxSize + kGap, 0.0, 0.0, 0.0), Qt::AlignVCenter | Qt::TextSingleLine, d_ptr->text);
    painter->restore();
}

void MaterialCheckBox::mousePressEvent(QMouseEvent *event)
{
    if (isEnabled() && hitTest(eventPos(event))) {
        d_ptr->ripple->start(d_ptr->boxRect().center());
    }
    QxControls::QxControl::mousePressEvent(event);
}

void MaterialCheckBox::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space) {
        event->accept();
        return;
    }
    QxControls::QxControl::keyPressEvent(event);
}

void MaterialCheckBox::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        Q_EMIT clicked();
        event->accept();
        return;
    }
    QxControls::QxControl::keyReleaseEvent(event);
}

QAccessible::Role MaterialCheckBox::accessibleRole() const
{
    return QAccessible::CheckBox;
}

void MaterialCheckBox::fillAccessibleState(QAccessible::State *state) const
{
    state->checkable = true;
    state->checked = d_ptr->state == Qt::Checked;
    state->checkStateMixed = d_ptr->tristate && d_ptr->state == Qt::PartiallyChecked;
}

QX_MATERIAL_END_NAMESPACE
