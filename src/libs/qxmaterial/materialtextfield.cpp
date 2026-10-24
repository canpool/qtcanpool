/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialtextfield.h"

#include "materialtokens.h"

#include <QtCore/QVariantAnimation>
#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtWidgets/QLineEdit>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// The field: Material's input row is 56 tall, and the text sits between the
// room the label needs above it and the line it stands on below.
constexpr int kFieldHeight = 56;
constexpr int kSideInset = 16;
constexpr int kEditorTop = 22;
constexpr int kEditorBottom = 10;

// Outlined fields are inset from the top, because their label floats onto the
// outline and half of it is above the field.
constexpr qreal kOutlinedTop = 8.0;

// The label moves between the body size it reads as and the label size it
// settles at, and it floats to just under the top of a filled field.
constexpr qreal kRestPointSize = 14.0;
constexpr qreal kFloatPointSize = 12.0;
constexpr qreal kFloatInset = 6.0;
// How much of the outline the label opens: enough for the text, plus the
// breathing room a stroke needs to read as interrupted rather than broken.
constexpr qreal kGapPadding = 4.0;

// The container's tint and the hairline that stands in for a stroke when the
// field is not the one with the keyboard - both the page's ink, thinned.
constexpr qreal kFillOpacity = 0.06;
constexpr qreal kHairlineOpacity = 0.42;

constexpr qreal kRestStroke = 1.0;
constexpr qreal kFocusStroke = 2.0;

/*! A rectangle rounded at the top only: a filled text field is an opening with
 *  a floor, not a card. */
QPainterPath topRoundedPath(const QRectF &rect, qreal radius)
{
    QPainterPath path;
    path.moveTo(rect.bottomLeft());
    path.lineTo(rect.left(), rect.top() + radius);
    path.arcTo(QRectF(rect.left(), rect.top(), 2 * radius, 2 * radius), 180.0, -90.0);
    path.lineTo(rect.right() - radius, rect.top());
    path.arcTo(QRectF(rect.right() - 2 * radius, rect.top(), 2 * radius, 2 * radius), 90.0, -90.0);
    path.lineTo(rect.bottomRight());
    path.closeSubpath();
    return path;
}
}   // namespace

class MaterialTextFieldPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialTextField)

    QLineEdit *edit = Q_NULLPTR;
    MaterialTextField::Variant variant = MaterialTextField::Variant::Filled;
    QString label;
    /*! How far the label has travelled: 0 while it reads as the prompt, 1 once
     *  it has moved out of the way. */
    qreal floatProgress = 0.0;
    QVariantAnimation animation;
};

MaterialTextField::MaterialTextField(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new MaterialTextFieldPrivate)
{
    QX_SET_PRIVATE(d_ptr)

    d_ptr->edit = new QLineEdit(this);
    d_ptr->edit->setFrame(false);
    d_ptr->edit->setStyleSheet(QStringLiteral("QLineEdit { background: transparent; border: none; }"));
    d_ptr->edit->installEventFilter(this);

    // The keyboard belongs to the editor - selection, the cursor, the clipboard
    // and an input method are its business - but the field is what the label
    // and the stroke answer to, so the proxy makes hasFocus() answer for both.
    setFocusPolicy(Qt::StrongFocus);
    setFocusProxy(d_ptr->edit);

    connect(d_ptr->edit, &QLineEdit::textChanged, this, [this](const QString &text) {
        updateLabel();
        Q_EMIT textChanged(text);
    });
    connect(d_ptr->edit, &QLineEdit::returnPressed, this, &MaterialTextField::returnPressed);
    connect(d_ptr->edit, &QLineEdit::editingFinished, this, &MaterialTextField::editingFinished);
    connect(&d_ptr->animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        d_ptr->floatProgress = value.toReal();
        update();
    });
}

MaterialTextField::~MaterialTextField(){QX_FINI_PRIVATE()}

MaterialTextField::Variant MaterialTextField::variant() const
{
    return d_ptr->variant;
}

QString MaterialTextField::label() const
{
    return d_ptr->label;
}

QString MaterialTextField::text() const
{
    return d_ptr->edit ? d_ptr->edit->text() : QString();
}

QLineEdit *MaterialTextField::lineEdit() const
{
    return d_ptr->edit;
}

void MaterialTextField::setVariant(Variant variant)
{
    if (d_ptr->variant == variant) {
        return;
    }
    d_ptr->variant = variant;
    resizeEvent(Q_NULLPTR);
    update();
}

void MaterialTextField::setLabel(const QString &label)
{
    if (d_ptr->label == label) {
        return;
    }
    d_ptr->label = label;
    update();
}

void MaterialTextField::setText(const QString &text)
{
    if (!d_ptr->edit) {
        return;
    }
    d_ptr->edit->setText(text);
}

void MaterialTextField::clear()
{
    if (d_ptr->edit) {
        d_ptr->edit->clear();
    }
}

QSize MaterialTextField::sizeHint() const
{
    return QSize(240, kFieldHeight);
}

QSize MaterialTextField::minimumSizeHint() const
{
    return QSize(2 * kSideInset + 48, kFieldHeight);
}

qreal MaterialTextField::cornerRadius() const
{
    return MaterialShape::radius(MaterialShape::Radius::Medium);
}

QRectF MaterialTextField::chromeRect() const
{
    return QRectF();
}

QRectF MaterialTextField::fieldRect() const
{
    const QRectF area = QRectF(rect());
    if (d_ptr->variant == Variant::Filled) {
        return area;
    }
    const qreal top = kOutlinedTop;
    return QRectF(area.left() + 0.5, area.top() + top, area.width() - 1.0, area.height() - top);
}

QFont MaterialTextField::labelFont(qreal progress) const
{
    QFont font = MaterialType::font(MaterialType::Scale::Label);
    font.setPointSizeF(kRestPointSize + (kFloatPointSize - kRestPointSize) * progress);
    return font;
}

QRectF MaterialTextField::labelRect(qreal progress) const
{
    const QFontMetricsF metrics(labelFont(progress));
    const qreal height = metrics.height();
    const QRectF field = fieldRect();
    const qreal left = field.left() + kSideInset;
    const qreal width = qMax(qreal(0.0), field.width() - 2 * kSideInset);

    // Where it reads as the prompt: the line the text will be typed on.
    const qreal restTop = field.center().y() - height / 2.0;
    // Where it goes: inside the top of a filled field, and on the outline of
    // an outlined one - which is why the outlined field is inset from the top.
    const qreal floatTop = d_ptr->variant == Variant::Filled ? field.top() + kFloatInset : field.top() - height / 2.0;

    return QRectF(left, restTop + (floatTop - restTop) * progress, width, height);
}

void MaterialTextField::paintSurface(QPainter *painter)
{
    const QRectF field = fieldRect();
    const qreal radius = cornerRadius();
    const bool focused = hasFocus();
    const QColor accent = accentColor();
    QColor ink = textColor();
    const QColor stroke = focused ? accent : QColor(ink.red(), ink.green(), ink.blue(), int(kHairlineOpacity * 255));
    const qreal strokeWidth = focused ? kFocusStroke : kRestStroke;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    if (d_ptr->variant == Variant::Filled) {
        // The container: the page's ink, thinned, with the corners turned at
        // the top. It is an opening with a floor, not a card.
        ink.setAlphaF(kFillOpacity);
        painter->setPen(Qt::NoPen);
        painter->setBrush(ink);
        painter->drawPath(topRoundedPath(field, radius));

        // The indicator under it: a hairline that thickens into the accent
        // when the field is the one with the keyboard.
        painter->setPen(QPen(stroke, strokeWidth));
        const qreal y = field.bottom() - strokeWidth / 2.0;
        painter->drawLine(QPointF(field.left(), y), QPointF(field.right(), y));
    } else {
        painter->setPen(QPen(stroke, strokeWidth));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(
            field.adjusted(strokeWidth / 2.0, strokeWidth / 2.0, -strokeWidth / 2.0, -strokeWidth / 2.0), radius,
            radius);
    }
    painter->restore();
}

void MaterialTextField::paintContent(QPainter *painter)
{
    if (d_ptr->label.isEmpty()) {
        return;
    }

    const qreal progress = labelProgress();
    const QRectF target = labelRect(progress);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    // An outlined label floats onto the stroke, so it opens a window in it:
    // a stroke behind a twelve point label reads as struck through. The window
    // is the label's own width, not the room the label is allowed - opening
    // the whole row would take the stroke away from the rest of the field.
    if (d_ptr->variant == Variant::Outlined && progress > 0.0) {
        const QFontMetricsF metrics(labelFont(progress));
        const QRectF window(target.left() - kGapPadding, target.top(),
                            metrics.horizontalAdvance(d_ptr->label) + 2 * kGapPadding, target.height());
        painter->fillRect(window, palette().color(QPalette::Window));
    }

    QColor ink = textColor();
    if (!isEnabled()) {
        ink.setAlphaF(0.4);
    }
    painter->setPen(hasFocus() ? accentColor() : ink);
    painter->setFont(labelFont(progress));
    painter->drawText(target, Qt::AlignLeft | Qt::AlignVCenter, d_ptr->label);
    painter->restore();
}

void MaterialTextField::resizeEvent(QResizeEvent *event)
{
    if (event) {
        QxControls::QxControl::resizeEvent(event);
    }
    if (!d_ptr->edit) {
        return;
    }
    const QRectF field = fieldRect();
    const int left = int(field.left()) + kSideInset;
    const int top = int(field.top()) + kEditorTop;
    const int width = qMax(0, int(field.width()) - 2 * kSideInset);
    const int height = qMax(0, int(field.height()) - kEditorTop - kEditorBottom);
    d_ptr->edit->setGeometry(left, top, width, height);
}

bool MaterialTextField::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == d_ptr->edit && (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut)) {
        // The label's place depends on whether the field has the keyboard, and
        // the keyboard lives in the editor.
        updateLabel();
        update();
    }
    return QxControls::QxControl::eventFilter(watched, event);
}

void MaterialTextField::updateLabel()
{
    // Where it is now, read before the move is cancelled: a move that is
    // interrupted is continued from where it had got to, not from an end.
    const qreal from = labelProgress();
    const qreal target = (hasFocus() || !text().isEmpty()) ? 1.0 : 0.0;
    const QxTheme::QxMotion motion =
        MaterialMotion::forDuration(MaterialMotion::Duration::Medium, MaterialMotion::Easing::Standard);
    d_ptr->animation.stop();
    d_ptr->floatProgress = from;
    if (motion.duration <= 0 || qFuzzyCompare(from + 1.0, target + 1.0)) {
        d_ptr->floatProgress = target;
        update();
        return;
    }
    d_ptr->animation.setDuration(motion.duration);
    d_ptr->animation.setEasingCurve(motion.curve);
    d_ptr->animation.setStartValue(from);
    d_ptr->animation.setEndValue(target);
    d_ptr->animation.start();
}

qreal MaterialTextField::labelProgress() const
{
    // Where the label belongs is a fact about the field, not a thing to be
    // remembered: a keyboard that arrives while the widget is being shown can
    // arrive without a FocusIn this widget sees, and a remembered answer would
    // leave the label standing in the text it names. Only a move under way has
    // a position worth caching, because only then is there a middle to be at.
    if (d_ptr->animation.state() == QAbstractAnimation::Running) {
        return d_ptr->floatProgress;
    }
    return (hasFocus() || !text().isEmpty()) ? 1.0 : 0.0;
}

QAccessible::Role MaterialTextField::accessibleRole() const
{
    return QAccessible::EditableText;
}

QX_MATERIAL_END_NAMESPACE
