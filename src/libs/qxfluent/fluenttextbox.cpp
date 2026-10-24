/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluenttextbox.h"

#include "fluenttokens.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QLineEdit>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// Fluent's box measures: thirty-two tall, the words keeping ten on each side,
// an icon of sixteen with eight between it and the words.
constexpr qreal kHeight = 32.0;
constexpr qreal kPadding = 10.0;
constexpr qreal kIconSize = 16.0;
constexpr qreal kIconGap = 8.0;
// The clear's room at the right, and the mark itself.
constexpr qreal kClearSize = 20.0;
constexpr qreal kClearMark = 8.0;
// The face is the page slightly inked under a hairline, as a standard
// command's is; the prompt is the ink held back; the underline is two tall.
constexpr qreal kFillOpacity = 0.06;
constexpr qreal kStrokeOpacity = 0.10;
constexpr qreal kPlaceholderOpacity = 0.6;
constexpr qreal kDisabledOpacity = 0.4;
constexpr qreal kUnderlineHeight = 2.0;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}
}   // namespace

class FluentTextBoxPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentTextBox)

    QString placeholderText;
    QIcon leadingIcon;
    QLineEdit *edit = nullptr;
    /*! The keyboard is in the editor: answered by the editor's focus events,
     *  and drawn as the accent along the foot. Kept rather than asked of the
     *  editor each paint, because the ask can trail the event that carries
     *  it. */
    bool keyboardIn = false;
    /*! True while a press that began on the clear is still a candidate: the
     *  mark answers a click, not a touch. */
    bool clearPressed = false;
};

FluentTextBox::FluentTextBox(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentTextBoxPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // The typing is the editor's; the face is the family's. The editor is
    // not on the keyboard's way through this widget - it is the way.
    d_ptr->edit = new QLineEdit(this);
    d_ptr->edit->setFrame(false);
    d_ptr->edit->setStyleSheet(QStringLiteral("QLineEdit { background: transparent; border: none; }"));
    d_ptr->edit->setFont(font());
    d_ptr->edit->installEventFilter(this);
    connect(d_ptr->edit, &QLineEdit::textChanged, this, [this](const QString &text) {
        updateEditor();
        update();
        Q_EMIT textChanged(text);
    });
    connect(d_ptr->edit, &QLineEdit::editingFinished, this, &FluentTextBox::editingFinished);
    connect(d_ptr->edit, &QLineEdit::returnPressed, this, &FluentTextBox::returnPressed);
    updateEditor();
}

FluentTextBox::~FluentTextBox(){QX_FINI_PRIVATE()}

QString FluentTextBox::text() const
{
    return d_ptr->edit->text();
}

QString FluentTextBox::placeholderText() const
{
    return d_ptr->placeholderText;
}

QIcon FluentTextBox::leadingIcon() const
{
    return d_ptr->leadingIcon;
}

QLineEdit *FluentTextBox::lineEdit() const
{
    return d_ptr->edit;
}

QSize FluentTextBox::sizeHint() const
{
    // The width is the layout's to give; the box asks for its height and the
    // room its words want, whichever is the wider.
    const QFontMetrics metrics(font());
    const qreal iconWidth = d_ptr->leadingIcon.isNull() ? 0.0 : kIconSize + kIconGap;
    const qreal words =
        metrics.horizontalAdvance(d_ptr->edit->text().isEmpty() ? d_ptr->placeholderText : d_ptr->edit->text());
    const qreal width = 2 * kPadding + iconWidth + words;
    return QSize(qMax(int(kHeight), int(width)), int(kHeight));
}

QSize FluentTextBox::minimumSizeHint() const
{
    return QSize(int(2 * kPadding + kHeight), int(kHeight));
}

void FluentTextBox::setText(const QString &text)
{
    if (d_ptr->edit->text() == text) {
        return;
    }
    d_ptr->edit->setText(text);
}

void FluentTextBox::setPlaceholderText(const QString &text)
{
    if (d_ptr->placeholderText == text) {
        return;
    }
    d_ptr->placeholderText = text;
    updateGeometry();
    update();
}

void FluentTextBox::setLeadingIcon(const QIcon &icon)
{
    if (d_ptr->leadingIcon.cacheKey() == icon.cacheKey()) {
        return;
    }
    d_ptr->leadingIcon = icon;
    updateEditor();
    updateGeometry();
    update();
}

void FluentTextBox::clear()
{
    const bool had = !d_ptr->edit->text().isEmpty();
    d_ptr->edit->clear();
    if (had) {
        Q_EMIT cleared();
    }
}

qreal FluentTextBox::cornerRadius() const
{
    return FluentShape::radius(FluentShape::Radius::Medium);
}

QRectF FluentTextBox::chromeRect() const
{
    return QRectF();
}

void FluentTextBox::paintSurface(QPainter *painter)
{
    const qreal radius = cornerRadius();
    const QRectF face(0.0, 0.0, qreal(width()), qreal(height()));
    painter->save();
    painter->setPen(Qt::NoPen);
    // The page slightly inked under a hairline; disabled, the fill drains to
    // the page and the hairline thins to a whisper of itself.
    painter->setBrush(QxControls::QxStateLayer::layered(palette().color(QPalette::Window), textColor(),
                                                        isEnabled() ? kFillOpacity : 0.0));
    painter->drawRoundedRect(face, radius, radius);
    painter->setPen(
        QxControls::QxStateLayer::layered(palette().color(QPalette::Window), textColor(),
                                          isEnabled() ? kStrokeOpacity : kStrokeOpacity * kDisabledOpacity));
    painter->setBrush(Qt::NoBrush);
    painter->drawRoundedRect(face.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
    // The keyboard's arrival is the accent along the foot, between the
    // corners: here is where the words go.
    if (d_ptr->keyboardIn) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(accentColor());
        painter->drawRect(
            QRectF(radius, qreal(height()) - kUnderlineHeight, qreal(width()) - 2 * radius, kUnderlineHeight));
    }
    painter->restore();
}

void FluentTextBox::paintContent(QPainter *painter)
{
    const bool hasIcon = !d_ptr->leadingIcon.isNull();
    painter->save();
    painter->setOpacity(isEnabled() ? 1.0 : kDisabledOpacity);
    if (hasIcon) {
        const QRectF iconRect(kPadding, (qreal(height()) - kIconSize) / 2.0, kIconSize, kIconSize);
        d_ptr->leadingIcon.paint(painter, iconRect.toRect(), Qt::AlignCenter,
                                 isEnabled() ? QIcon::Normal : QIcon::Disabled);
    }
    // The prompt stands where the words will, only held back, and goes the
    // moment there are words.
    if (d_ptr->edit->text().isEmpty() && !d_ptr->placeholderText.isEmpty()) {
        const QRectF room = editorRect();
        painter->setOpacity(isEnabled() ? kPlaceholderOpacity : kPlaceholderOpacity * kDisabledOpacity);
        painter->setPen(textColor());
        painter->setFont(font());
        painter->drawText(room, Qt::AlignVCenter | Qt::TextSingleLine, d_ptr->placeholderText);
    }
    // The clear: a cross in its room, once there is something to erase.
    if (clearWanted()) {
        const QRectF room = clearRect();
        const QRectF mark(room.center().x() - kClearMark / 2.0, room.center().y() - kClearMark / 2.0, kClearMark,
                          kClearMark);
        QPen pen(textColor(), 1.2);
        pen.setCapStyle(Qt::RoundCap);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->drawLine(mark.topLeft(), mark.bottomRight());
        painter->drawLine(mark.topRight(), mark.bottomLeft());
    }
    painter->restore();
}

void FluentTextBox::resizeEvent(QResizeEvent *event)
{
    QxControls::QxControl::resizeEvent(event);
    updateEditor();
}

void FluentTextBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && clearWanted() && clearRect().contains(eventPos(event))) {
        d_ptr->clearPressed = true;
        return;
    }
    QxControls::QxControl::mousePressEvent(event);
}

void FluentTextBox::mouseReleaseEvent(QMouseEvent *event)
{
    QxControls::QxControl::mouseReleaseEvent(event);
    if (d_ptr->clearPressed && event->button() == Qt::LeftButton && clearRect().contains(eventPos(event))) {
        d_ptr->clearPressed = false;
        const bool had = !d_ptr->edit->text().isEmpty();
        d_ptr->edit->clear();
        if (had) {
            Q_EMIT cleared();
        }
        update();
    } else if (event->button() == Qt::LeftButton) {
        d_ptr->clearPressed = false;
    }
}

bool FluentTextBox::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == d_ptr->edit && (event->type() == QEvent::FocusIn || event->type() == QEvent::FocusOut)) {
        // The keyboard arrived or left: the accent comes out along the foot,
        // or goes back in.
        d_ptr->keyboardIn = event->type() == QEvent::FocusIn;
        update();
    }
    return QxControls::QxControl::eventFilter(watched, event);
}

QAccessible::Role FluentTextBox::accessibleRole() const
{
    return QAccessible::EditableText;
}

QRectF FluentTextBox::editorRect() const
{
    const qreal left = kPadding + (d_ptr->leadingIcon.isNull() ? 0.0 : kIconSize + kIconGap);
    const qreal right = clearWanted() ? kPadding + kClearSize : kPadding;
    return QRectF(left, 0.0, qMax<qreal>(0.0, qreal(width()) - left - right), qreal(height()));
}

QRectF FluentTextBox::clearRect() const
{
    return QRectF(qreal(width()) - kPadding - kClearSize, (qreal(height()) - kClearSize) / 2.0, kClearSize, kClearSize);
}

bool FluentTextBox::clearWanted() const
{
    return isEnabled() && !d_ptr->edit->isReadOnly() && !d_ptr->edit->text().isEmpty();
}

void FluentTextBox::updateEditor()
{
    d_ptr->edit->setGeometry(editorRect().toRect());
}

QX_FLUENT_END_NAMESPACE
