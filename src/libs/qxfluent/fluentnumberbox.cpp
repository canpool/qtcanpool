/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentnumberbox.h"

#include "fluenttokens.h"

#include "qxcontrols/qxcontrolaccessible.h"
#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QWheelEvent>
#include <QtWidgets/QLineEdit>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// The field: as tall as Fluent's input row, with a stepper column wide enough
// for one chevron and an inset that keeps the text off the edge.
constexpr int kFieldHeight = 32;
constexpr int kStepperWidth = 24;
constexpr int kTextInset = 8;

// The chevrons are the point the command bar draws, one turned up and one
// turned down.
constexpr qreal kChevronGlyph = 8.0;
constexpr qreal kChevronStroke = 1.5;

// The hairline the field sits in - the page's ink, thinned - and the bar that
// says the field has the keyboard.
constexpr qreal kHairlineOpacity = 0.24;
constexpr qreal kUnderlineHeight = 2.0;
constexpr qreal kDisabledOpacity = 0.4;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}

/*! One chevron, pointing where \a up says. Drawn rather than typed: a glyph in
 *  a font is a glyph the theme cannot colour and an assistive tool would read. */
void paintChevron(QPainter *painter, const QRectF &rect, const QColor &color, bool up)
{
    const qreal x = rect.center().x() - kChevronGlyph / 2.0;
    const qreal y = rect.center().y() - kChevronGlyph / 4.0;
    QPainterPath chevron;
    if (up) {
        chevron.moveTo(x, y + kChevronGlyph / 2.0);
        chevron.lineTo(x + kChevronGlyph / 2.0, y);
        chevron.lineTo(x + kChevronGlyph, y + kChevronGlyph / 2.0);
    } else {
        chevron.moveTo(x, y);
        chevron.lineTo(x + kChevronGlyph / 2.0, y + kChevronGlyph / 2.0);
        chevron.lineTo(x + kChevronGlyph, y);
    }
    painter->setPen(QPen(color, kChevronStroke, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawPath(chevron);
}
}   // namespace

/*!
 * The accessible face of a number box: a spin box is one of the controls whose
 * answer is a number, so a role and a state do not describe it - an assistive
 * tool wants to read the number and to set it back.
 */
class FluentNumberBoxAccessible : public QxControls::QxControlAccessible, public QAccessibleValueInterface
{
public:
    explicit FluentNumberBoxAccessible(FluentNumberBox *box)
        : QxControls::QxControlAccessible(box, QAccessible::SpinBox)
    {
    }

    QVariant currentValue() const override
    {
        return box()->value();
    }
    void setCurrentValue(const QVariant &value) override
    {
        box()->setValue(value.toInt());
    }
    QVariant maximumValue() const override
    {
        return box()->maximum();
    }
    QVariant minimumValue() const override
    {
        return box()->minimum();
    }
    QVariant minimumStepSize() const override
    {
        return box()->singleStep();
    }

    void *interface_cast(QAccessible::InterfaceType type) override
    {
        if (type == QAccessible::ValueInterface) {
            return static_cast<QAccessibleValueInterface *>(this);
        }
        return QxControls::QxControlAccessible::interface_cast(type);
    }
private:
    FluentNumberBox *box() const
    {
        return static_cast<FluentNumberBox *>(widget());
    }
};

class FluentNumberBoxPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentNumberBox)

    QLineEdit *edit = Q_NULLPTR;
    int value = 0;
    int minimum = 0;
    int maximum = 100;
    int singleStep = 1;
    int pageStep = 10;
    /*! Which half of the stepper the pointer is on: -1 none, 0 the upper
     *  chevron, 1 the lower one. The two are two commands, so a hover says
     *  which one it would run. */
    int hotStep = -1;
};

FluentNumberBox::FluentNumberBox(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentNumberBoxPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    d_ptr->edit = new QLineEdit(this);
    d_ptr->edit->setFrame(false);
    d_ptr->edit->setText(QString::number(d_ptr->value));
    // The field is the control's to draw, so the editor brings no frame, no
    // background and no focus rectangle of its own - only the text.
    d_ptr->edit->setStyleSheet(QStringLiteral("QLineEdit { background: transparent; border: none; }"));
    d_ptr->edit->installEventFilter(this);

    // The keyboard belongs to the editor, so the control hands it over: with a
    // focus proxy, hasFocus() answers for the pair and the base's ring lands on
    // the field rather than on nothing.
    setFocusPolicy(Qt::StrongFocus);
    setFocusProxy(d_ptr->edit);

    // The stepper divides into two halves that light up one at a time, so the
    // pointer has to be followed without a button held.
    setMouseTracking(true);
}

FluentNumberBox::~FluentNumberBox()
{
    QX_FINI_PRIVATE()
}

int FluentNumberBox::value() const
{
    return d_ptr->value;
}

int FluentNumberBox::minimum() const
{
    return d_ptr->minimum;
}

int FluentNumberBox::maximum() const
{
    return d_ptr->maximum;
}

int FluentNumberBox::singleStep() const
{
    return d_ptr->singleStep;
}

int FluentNumberBox::pageStep() const
{
    return d_ptr->pageStep;
}

void FluentNumberBox::setValue(int value)
{
    const int bounded = qBound(d_ptr->minimum, value, d_ptr->maximum);
    if (d_ptr->value == bounded) {
        syncEditor(false);
        return;
    }
    d_ptr->value = bounded;
    syncEditor(false);
    update();
    Q_EMIT valueChanged(bounded);
}

void FluentNumberBox::setMinimum(int minimum)
{
    setRange(minimum, qMax(minimum, d_ptr->maximum));
}

void FluentNumberBox::setMaximum(int maximum)
{
    setRange(qMin(d_ptr->minimum, maximum), maximum);
}

void FluentNumberBox::setRange(int minimum, int maximum)
{
    const int low = qMin(minimum, maximum);
    const int high = qMax(minimum, maximum);
    if (d_ptr->minimum == low && d_ptr->maximum == high) {
        return;
    }
    d_ptr->minimum = low;
    d_ptr->maximum = high;
    setValue(qBound(low, d_ptr->value, high));
    update();
}

void FluentNumberBox::setSingleStep(int step)
{
    d_ptr->singleStep = qMax(1, step);
}

void FluentNumberBox::setPageStep(int step)
{
    d_ptr->pageStep = qMax(1, step);
}

QSize FluentNumberBox::sizeHint() const
{
    return QSize(120, kFieldHeight);
}

QSize FluentNumberBox::minimumSizeHint() const
{
    return QSize(2 * kTextInset + kStepperWidth + 16, kFieldHeight);
}

qreal FluentNumberBox::cornerRadius() const
{
    return FluentShape::radius(FluentShape::Radius::Medium);
}

void FluentNumberBox::paintSurface(QPainter *painter)
{
    // Half a pixel in, so the hairline lands on the field's edge rather than
    // straddling it.
    const QRectF field = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    const qreal radius = cornerRadius();
    const bool editing = hasFocus();

    QColor hairline = textColor();
    hairline.setAlphaF(kHairlineOpacity);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(QPen(editing ? accentColor() : hairline, 1.0));
    painter->setBrush(palette().color(QPalette::Base));
    painter->drawRoundedRect(field, radius, radius);

    // The bar under the field: Fluent says "this has the keyboard" with the
    // accent, not with a ring around the whole box.
    if (editing) {
        QPainterPath clip;
        clip.addRoundedRect(field, radius, radius);
        painter->setClipPath(clip);
        painter->setPen(Qt::NoPen);
        painter->setBrush(accentColor());
        painter->drawRect(
            QRectF(field.left(), field.bottom() + 1.0 - kUnderlineHeight, field.width(), kUnderlineHeight));
    }
    painter->restore();
}

void FluentNumberBox::paintContent(QPainter *painter)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    // A hover on one chevron tints that chevron's half, not the column: the
    // two halves are two commands and a tint across both would say otherwise.
    if (isEnabled() && d_ptr->hotStep >= 0) {
        QxControls::QxStateLayer::paint(painter, stepperRect(d_ptr->hotStep), washColor(),
                                        QxControls::QxStateLayer::hoverOpacity);
    }

    QColor ink = textColor();
    if (!isEnabled()) {
        ink.setAlphaF(kDisabledOpacity);
    }
    paintChevron(painter, stepperRect(0), ink, true);
    paintChevron(painter, stepperRect(1), ink, false);
    painter->restore();
}

void FluentNumberBox::resizeEvent(QResizeEvent *event)
{
    QxControls::QxControl::resizeEvent(event);
    if (d_ptr->edit) {
        const int editorWidth = qMax(0, width() - kStepperWidth - kTextInset);
        d_ptr->edit->setGeometry(kTextInset, 0, editorWidth, height());
    }
}

int FluentNumberBox::stepperAt(const QPointF &pos) const
{
    for (int half = 0; half < 2; ++half) {
        if (stepperRect(half).contains(pos)) {
            return half;
        }
    }
    return -1;
}

QRectF FluentNumberBox::stepperRect(int half) const
{
    const QRectF column(width() - kStepperWidth, 0.0, kStepperWidth, height());
    const qreal halfHeight = column.height() / 2.0;
    const qreal top = half == 0 ? column.top() : column.center().y();
    return QRectF(column.left(), top, column.width(), halfHeight);
}

void FluentNumberBox::step(int delta)
{
    setValue(d_ptr->value + delta);
    // A step is the control talking rather than the user typing, so the field
    // follows even while it has the keyboard - including when the value did not
    // change because the range said no.
    syncEditor(true);
}

void FluentNumberBox::commitEditor()
{
    bool ok = false;
    const int parsed = d_ptr->edit->text().trimmed().toInt(&ok);
    if (ok) {
        setValue(parsed);
    }
    // Whether it parsed or not, the field goes back to what the value is: a
    // field that keeps text the control does not believe is a field lying about
    // its own number.
    syncEditor(true);
}

void FluentNumberBox::syncEditor(bool force)
{
    if (!d_ptr->edit) {
        return;
    }
    if (!force && d_ptr->edit->hasFocus()) {
        return;
    }
    const QString text = QString::number(d_ptr->value);
    if (d_ptr->edit->text() != text) {
        d_ptr->edit->setText(text);
    }
}

bool FluentNumberBox::handleKey(QKeyEvent *event)
{
    int delta = 0;
    switch (event->key()) {
    case Qt::Key_Up:
        delta = d_ptr->singleStep;
        break;
    case Qt::Key_Down:
        delta = -d_ptr->singleStep;
        break;
    case Qt::Key_PageUp:
        delta = d_ptr->pageStep;
        break;
    case Qt::Key_PageDown:
        delta = -d_ptr->pageStep;
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        commitEditor();
        event->accept();
        return true;
    case Qt::Key_Escape:
        syncEditor(true);
        event->accept();
        return true;
    default:
        return false;
    }
    step(delta);
    event->accept();
    return true;
}

bool FluentNumberBox::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == d_ptr->edit) {
        switch (event->type()) {
        case QEvent::KeyPress:
            if (isEnabled() && handleKey(static_cast<QKeyEvent *>(event))) {
                return true;
            }
            break;
        case QEvent::Wheel:
            if (isEnabled()) {
                const int delta = static_cast<QWheelEvent *>(event)->angleDelta().y();
                if (delta != 0) {
                    step(delta > 0 ? d_ptr->singleStep : -d_ptr->singleStep);
                    return true;
                }
            }
            break;
        case QEvent::FocusIn:
        case QEvent::FocusOut:
            // The field's frame is the control's to draw, so the control has to
            // hear when the keyboard came and went.
            if (event->type() == QEvent::FocusOut) {
                commitEditor();
            }
            update();
            break;
        default:
            break;
        }
    }
    return QxControls::QxControl::eventFilter(watched, event);
}

void FluentNumberBox::mousePressEvent(QMouseEvent *event)
{
    QxControls::QxControl::mousePressEvent(event);
    if (!isEnabled() || event->button() != Qt::LeftButton) {
        return;
    }
    const int half = stepperAt(eventPos(event));
    if (half >= 0) {
        step(half == 0 ? d_ptr->singleStep : -d_ptr->singleStep);
        event->accept();
    }
}

void FluentNumberBox::mouseMoveEvent(QMouseEvent *event)
{
    QxControls::QxControl::mouseMoveEvent(event);
    const int half = isEnabled() ? stepperAt(eventPos(event)) : -1;
    if (d_ptr->hotStep != half) {
        d_ptr->hotStep = half;
        update();
    }
}

void FluentNumberBox::leaveEvent(QEvent *event)
{
    QxControls::QxControl::leaveEvent(event);
    if (d_ptr->hotStep >= 0) {
        d_ptr->hotStep = -1;
        update();
    }
}

void FluentNumberBox::wheelEvent(QWheelEvent *event)
{
    const int delta = event->angleDelta().y();
    if (isEnabled() && delta != 0) {
        step(delta > 0 ? d_ptr->singleStep : -d_ptr->singleStep);
        event->accept();
        return;
    }
    QxControls::QxControl::wheelEvent(event);
}

void FluentNumberBox::keyPressEvent(QKeyEvent *event)
{
    // The keyboard normally reaches the editor and comes back through the
    // filter; this is the same answer for the case where it does not.
    if (isEnabled() && handleKey(event)) {
        return;
    }
    QxControls::QxControl::keyPressEvent(event);
}

QAccessible::Role FluentNumberBox::accessibleRole() const
{
    return QAccessible::SpinBox;
}

QAccessibleInterface *FluentNumberBox::accessibleInterface()
{
    return new FluentNumberBoxAccessible(this);
}

QX_FLUENT_END_NAMESPACE
