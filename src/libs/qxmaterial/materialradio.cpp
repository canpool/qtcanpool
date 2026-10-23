/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialradio.h"

#include "materialripple.h"
#include "materialtokens.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// The circle is the box the check box keeps, drawn round; the dot inside it
// is what the answer looks like from the outside.
constexpr qreal kSize = 18.0;
constexpr qreal kGap = 8.0;
constexpr qreal kRingWidth = 1.6;
constexpr qreal kDotSize = 8.0;
constexpr qreal kRingOpacity = 0.54;
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

class MaterialRadioPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialRadio)

    QString text;
    bool checked = false;
    MaterialRipple *ripple = Q_NULLPTR;

    /*! The circle, in widget coordinates: the control itself, against the
     *  row that only looks like it. */
    QRectF circleRect() const;
};

QRectF MaterialRadioPrivate::circleRect() const
{
    Q_Q(const MaterialRadio);
    const qreal top = (qreal(q->height()) - kSize) / 2.0;
    return QRectF(0.0, top, kSize, kSize);
}

MaterialRadio::MaterialRadio(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new MaterialRadioPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setFocusPolicy(Qt::StrongFocus);
    // A press puts the pressed one in force; the turn itself follows from
    // clicked(), the same answer the keyboard gives.
    connect(this, &MaterialRadio::clicked, this, [this]() {
        setChecked(true);
    });
    // The press is answered from the circle, not from where the pointer
    // landed: the ripple starts at the circle's own centre and stays in it.
    d_ptr->ripple = new MaterialRipple(this);
    d_ptr->ripple->setWatchPress(false);
    d_ptr->ripple->setColor(textColor());
}

MaterialRadio::~MaterialRadio(){QX_FINI_PRIVATE()}

QString MaterialRadio::text() const
{
    return d_ptr->text;
}

bool MaterialRadio::isChecked() const
{
    return d_ptr->checked;
}

void MaterialRadio::setText(const QString &text)
{
    if (d_ptr->text == text) {
        return;
    }
    d_ptr->text = text;
    Q_EMIT textChanged(text);
    updateGeometry();
    update();
}

void MaterialRadio::setChecked(bool checked)
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
        const QList<MaterialRadio *> brothers =
            parentWidget()->findChildren<MaterialRadio *>(QString(), Qt::FindDirectChildrenOnly);
        for (MaterialRadio *brother : brothers) {
            if (brother != this && brother->isChecked()) {
                brother->setChecked(false);
            }
        }
    }
}

QSize MaterialRadio::sizeHint() const
{
    const QFontMetrics metrics(MaterialType::font(MaterialType::Scale::Body));
    const int textWidth = d_ptr->text.isEmpty() ? 0 : metrics.horizontalAdvance(d_ptr->text);
    const int height = qMax(int(kSize), metrics.height());
    return QSize(int(kSize) + (textWidth ? int(kGap) + textWidth : 0), height);
}

QSize MaterialRadio::minimumSizeHint() const
{
    return sizeHint();
}

QRectF MaterialRadio::chromeRect() const
{
    return d_ptr->circleRect();
}

bool MaterialRadio::hitTest(const QPointF &pos) const
{
    // The row is what looks like the control: the label is not decoration
    // beside a small circle, it is half of the same answer.
    return QRectF(0.0, 0.0, qreal(width()), qreal(height())).contains(pos);
}

void MaterialRadio::paintSurface(QPainter *painter)
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
        painter->setPen(Qt::NoPen);
        painter->setBrush(accentColor());
        painter->drawEllipse(circle.adjusted((kSize - kDotSize) / 2.0, (kSize - kDotSize) / 2.0,
                                             -(kSize - kDotSize) / 2.0, -(kSize - kDotSize) / 2.0));
    }
    // The ripple stays inside the answer it belongs to.
    if (isEnabled()) {
        d_ptr->ripple->paint(painter, circle, kSize / 2.0);
    }
    painter->restore();
}

void MaterialRadio::paintContent(QPainter *painter)
{
    if (d_ptr->text.isEmpty()) {
        return;
    }
    painter->save();
    painter->setOpacity(isEnabled() ? 1.0 : kDisabledOpacity);
    painter->setPen(textColor());
    painter->setFont(MaterialType::font(MaterialType::Scale::Body));
    const QRectF row(0.0, 0.0, qreal(width()), qreal(height()));
    painter->drawText(row.adjusted(kSize + kGap, 0.0, 0.0, 0.0), Qt::AlignVCenter | Qt::TextSingleLine, d_ptr->text);
    painter->restore();
}

void MaterialRadio::mousePressEvent(QMouseEvent *event)
{
    if (isEnabled() && hitTest(eventPos(event))) {
        d_ptr->ripple->start(d_ptr->circleRect().center());
    }
    QxControls::QxControl::mousePressEvent(event);
}

void MaterialRadio::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space) {
        event->accept();
        return;
    }
    QxControls::QxControl::keyPressEvent(event);
}

void MaterialRadio::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        Q_EMIT clicked();
        event->accept();
        return;
    }
    QxControls::QxControl::keyReleaseEvent(event);
}

QAccessible::Role MaterialRadio::accessibleRole() const
{
    return QAccessible::RadioButton;
}

void MaterialRadio::fillAccessibleState(QAccessible::State *state) const
{
    state->checkable = true;
    state->checked = d_ptr->checked;
}

QX_MATERIAL_END_NAMESPACE
