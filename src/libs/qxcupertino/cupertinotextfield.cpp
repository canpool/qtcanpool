/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "cupertinotextfield.h"

#include "cupertinotokens.h"

#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>

QX_CUPERTINO_BEGIN_NAMESPACE

namespace
{
/*! The field's height and the room it keeps on either side. Thirty-six is a
 *  line of body text plus the padding this family gives a filled field; the
 *  inset is what keeps the text off the rounded ends. */
constexpr int kHeight = 36;
constexpr int kInset = 12;
/*! The strip a search field gives its magnifier, before the text starts. */
constexpr int kSearchLeading = 28;
/*! The clear button, and the gap between it and the text. */
constexpr int kClearSize = 16;
constexpr int kClearGap = 8;
/*! The field's fill: the system's grey over the page, lighter when the control
 *  is disabled so that it reads as gone rather than as a lighter field. */
constexpr qreal kFillOpacity = 0.12;
constexpr qreal kDisabledFillOpacity = 0.06;
/*! The placeholder is the ink thinned until it is a hint. */
constexpr qreal kPlaceholderOpacity = 0.3;

bool onDarkFace(const QWidget *widget)
{
    return widget->palette().color(QPalette::Window).lightness() < 128;
}

/*!
 * The button that empties the field: the system's small grey disc with a cross
 * in it.
 *
 * It is drawn here rather than being a QPushButton because it is not a button
 * of the page - it is a mark inside the field, and a styled QPushButton would
 * arrive with the theme's own frame, padding and hover.
 */
class ClearButton : public QAbstractButton
{
public:
    explicit ClearButton(QWidget *parent = Q_NULLPTR)
        : QAbstractButton(parent)
    {
        setFocusPolicy(Qt::NoFocus);
        setCursor(Qt::ArrowCursor);
        setFixedSize(kClearSize, kClearSize);
    }
protected:
    bool event(QEvent *e) override
    {
        // The disc darkens under the pointer, so it has to be repainted when
        // the pointer arrives or leaves rather than only when it is clicked.
        if (e->type() == QEvent::Enter || e->type() == QEvent::Leave) {
            update();
        }
        return QAbstractButton::event(e);
    }

    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        QColor disc =
            CupertinoColor::color(CupertinoColor::Role::Gray, palette().color(QPalette::Window).lightness() < 128);
        disc.setAlphaF(underMouse() ? 0.55 : 0.4);
        painter.setPen(Qt::NoPen);
        painter.setBrush(disc);
        painter.drawEllipse(QRectF(rect()));

        // The cross is the page's own colour: a light cross on a light disc, a
        // dark one on a dark disc, which is what makes it read as a cut.
        const bool dark = palette().color(QPalette::Window).lightness() < 128;
        painter.setPen(QPen(dark ? Qt::black : Qt::white, 1.2));
        const QRectF box = QRectF(rect()).adjusted(5, 5, -5, -5);
        painter.drawLine(box.topLeft(), box.bottomRight());
        painter.drawLine(box.topRight(), box.bottomLeft());
    }
};
}   // namespace

class CupertinoTextFieldPrivate
{
public:
    QX_DECLARE_PUBLIC(CupertinoTextField)

    QLineEdit *edit = Q_NULLPTR;
    ClearButton *clearButton = Q_NULLPTR;
    CupertinoTextField::Kind kind = CupertinoTextField::Kind::Plain;
    bool clearButtonEnabled = true;
    QString placeholder;
};

CupertinoTextField::CupertinoTextField(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new CupertinoTextFieldPrivate)
{
    QX_SET_PRIVATE(d_ptr)

    // The keyboard belongs to the line edit inside; the field around it is not
    // a stop of its own, or tabbing would visit one control twice.
    setFocusPolicy(Qt::NoFocus);

    d_ptr->edit = new QLineEdit(this);
    d_ptr->edit->setFrame(false);
    d_ptr->edit->setAttribute(Qt::WA_MacShowFocusRect, false);
    d_ptr->edit->setFont(CupertinoType::font(CupertinoType::Ramp::Body));
    d_ptr->edit->installEventFilter(this);
    syncEdit();

    d_ptr->clearButton = new ClearButton(this);
    connect(d_ptr->clearButton, &QAbstractButton::clicked, this, &CupertinoTextField::clear);
    connect(d_ptr->edit, &QLineEdit::textChanged, this, [this](const QString &text) {
        updateClearVisibility();
        Q_EMIT textChanged(text);
    });
    connect(d_ptr->edit, &QLineEdit::returnPressed, this, &CupertinoTextField::returnPressed);
    // The ring around the field is this control's, and it follows the line
    // edit's focus rather than its own.
    updateClearVisibility();
}

CupertinoTextField::~CupertinoTextField(){QX_FINI_PRIVATE()}

QString CupertinoTextField::text() const
{
    return d_ptr->edit->text();
}

QString CupertinoTextField::placeholderText() const
{
    return d_ptr->placeholder;
}

CupertinoTextField::Kind CupertinoTextField::kind() const
{
    return d_ptr->kind;
}

bool CupertinoTextField::isClearButtonEnabled() const
{
    return d_ptr->clearButtonEnabled;
}

QSize CupertinoTextField::sizeHint() const
{
    return QSize(220, kHeight);
}

QSize CupertinoTextField::minimumSizeHint() const
{
    return QSize(80, kHeight);
}

void CupertinoTextField::setText(const QString &text)
{
    d_ptr->edit->setText(text);
}

void CupertinoTextField::setPlaceholderText(const QString &text)
{
    d_ptr->placeholder = text;
    d_ptr->edit->setPlaceholderText(text);
}

void CupertinoTextField::setKind(Kind kind)
{
    if (d_ptr->kind == kind) {
        return;
    }
    d_ptr->kind = kind;
    // The magnifier takes room from the text, so the line edit moves with the
    // kind and not only the drawing.
    layoutEdit();
    update();
}

void CupertinoTextField::setClearButtonEnabled(bool enabled)
{
    if (d_ptr->clearButtonEnabled == enabled) {
        return;
    }
    d_ptr->clearButtonEnabled = enabled;
    updateClearVisibility();
}

void CupertinoTextField::clear()
{
    if (d_ptr->edit->text().isEmpty()) {
        return;
    }
    // Empties through the line edit, so textChanged() says so once and by the
    // same road as typing does.
    d_ptr->edit->clear();
    Q_EMIT cleared();
}

bool CupertinoTextField::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == d_ptr->edit) {
        switch (event->type()) {
        case QEvent::FocusIn:
        case QEvent::FocusOut:
            // The ring is drawn by the field, so it has to be repainted when
            // the focus moves in or out of the line edit inside it.
            update();
            break;
        case QEvent::KeyPress: {
            auto *key = static_cast<QKeyEvent *>(event);
            if (key->key() == Qt::Key_Escape && !d_ptr->edit->text().isEmpty()) {
                // Escape empties the field, which is what the clear button
                // does. Return is the application's - it emits returnPressed()
                // and leaves the text alone.
                clear();
                return true;
            }
            break;
        }
        default:
            break;
        }
    }
    return QxControls::QxControl::eventFilter(watched, event);
}

void CupertinoTextField::changeEvent(QEvent *event)
{
    QxControls::QxControl::changeEvent(event);
    switch (event->type()) {
    case QEvent::PaletteChange:
    case QEvent::FontChange:
    case QEvent::StyleChange:
        syncEdit();
        break;
    case QEvent::EnabledChange:
        d_ptr->edit->setEnabled(isEnabled());
        break;
    default:
        break;
    }
}

void CupertinoTextField::mousePressEvent(QMouseEvent *event)
{
    // The field is the widget: a press anywhere on the tinted surface belongs
    // to the text, and it puts the caret where the press was in the line edit.
    d_ptr->edit->setFocus(Qt::MouseFocusReason);
    event->accept();
}

void CupertinoTextField::resizeEvent(QResizeEvent *event)
{
    QxControls::QxControl::resizeEvent(event);
    layoutEdit();
}

void CupertinoTextField::paintSurface(QPainter *painter)
{
    const qreal radius = qMin(CupertinoShape::radius(CupertinoShape::Radius::Large), height() / 2.0);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);

    QColor fill = CupertinoColor::color(CupertinoColor::Role::Gray, onDarkFace(this));
    fill.setAlphaF(isEnabled() ? kFillOpacity : kDisabledFillOpacity);
    painter->setBrush(fill);
    painter->drawRoundedRect(fieldRect(), radius, radius);

    painter->restore();
}

void CupertinoTextField::paintContent(QPainter *painter)
{
    const qreal radius = qMin(CupertinoShape::radius(CupertinoShape::Radius::Large), height() / 2.0);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    if (d_ptr->kind == Kind::Search) {
        // The magnifier: a circle and a handle, in the ink of the placeholder
        // because it is a hint about the field rather than a control in it.
        QColor ink = textColor();
        ink.setAlphaF(kPlaceholderOpacity + 0.25);
        painter->setPen(QPen(ink, 1.4));
        const QRectF lens = magnifierRect();
        painter->drawEllipse(lens.adjusted(0.0, 0.0, -2.0, -2.0));
        const QPointF handle(lens.right() - 1.5, lens.bottom() - 1.5);
        painter->drawLine(handle, handle + QPointF(3.5, 3.5));
    }

    if (d_ptr->edit->hasFocus() && isEnabled()) {
        // Focus is a ring in the accent, on the outside of the fill.
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(accentColor(), 1.4));
        painter->drawRoundedRect(fieldRect().adjusted(0.7, 0.7, -0.7, -0.7), radius, radius);
    }

    painter->restore();
}

bool CupertinoTextField::hitTest(const QPointF &pos) const
{
    // There is no wash on a text field: the pointer being over one means the
    // text is about to be typed into, not that the field is a thing to press.
    Q_UNUSED(pos)
    return false;
}

QAccessible::Role CupertinoTextField::accessibleRole() const
{
    // The editable thing here is the line edit inside, which brings its own
    // accessible object; this control is the frame around it and has nothing
    // to add.
    return QAccessible::NoRole;
}

void CupertinoTextField::syncEdit()
{
    QPalette palette = d_ptr->edit->palette();
    const QColor ink = textColor();
    palette.setColor(QPalette::Base, Qt::transparent);
    palette.setColor(QPalette::Window, Qt::transparent);
    palette.setColor(QPalette::Text, ink);
    palette.setColor(QPalette::WindowText, ink);
    QColor hint = ink;
    hint.setAlphaF(kPlaceholderOpacity);
    palette.setColor(QPalette::PlaceholderText, hint);
    palette.setColor(QPalette::Highlight, accentColor());
    palette.setColor(QPalette::HighlightedText, onDarkFace(this) ? Qt::black : Qt::white);
    d_ptr->edit->setPalette(palette);
    d_ptr->edit->setFont(CupertinoType::font(CupertinoType::Ramp::Body));

    // The themes ship rules for a QLineEdit - a background, a border, a padding
    // - and this one is none of those things: it is the text inside a surface
    // the control paints. A sheet of its own is what beats them, and a sheet set
    // on the widget wins over the application's whatever the two selectors say,
    // which is why the plain type selector is enough here. It has to be said
    // again whenever the theme changes, because the ink is baked into it.
    // Without it the field was one picture when the gallery started on this
    // style and another after switching to it: the theme's QLineEdit rule came
    // back on the switch and painted a white box inside the field.
    d_ptr->edit->setStyleSheet(QStringLiteral("QLineEdit { background: transparent; border: none; padding: 0px;"
                                              " color: %1; selection-background-color: %2; }")
                                   .arg(ink.name(), accentColor().name()));
}

void CupertinoTextField::updateClearVisibility()
{
    // A clear button that is there while there is nothing to clear is a mark
    // that can only do nothing.
    d_ptr->clearButton->setVisible(d_ptr->clearButtonEnabled && !d_ptr->edit->text().isEmpty());
    layoutEdit();
}

QRectF CupertinoTextField::fieldRect() const
{
    // Half a pixel in: the stroke sits inside the widget rather than straddling
    // its edge, which is what keeps a rounded corner from being clipped.
    return QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
}

QRectF CupertinoTextField::magnifierRect() const
{
    const qreal side = 11.0;
    const qreal left = 8.0;
    return QRectF(left, (height() - side) / 2.0, side, side);
}

void CupertinoTextField::layoutEdit()
{
    const int leading = d_ptr->kind == Kind::Search ? kSearchLeading : kInset;
    // isHidden() and not isVisible(): the intent to show the button is the
    // question, and a widget inside a page that has not been shown yet answers
    // isVisible() with false however often it is told to show. Asked that way,
    // a page built while hidden laid the text out over the room the clear
    // button was about to take and kept that geometry once it was shown - the
    // same field was 24 pixels wider on a page arrived at than on a page
    // started on.
    const int trailing = kInset + (d_ptr->clearButton->isHidden() ? 0 : kClearSize + kClearGap);
    // Two pixels above and below: the focus ring is drawn on the field and the
    // line edit is laid over it, so the ring needs that much to stay visible.
    d_ptr->edit->setGeometry(leading, 2, qMax(0, width() - leading - trailing), height() - 4);
    d_ptr->clearButton->move(width() - kInset - kClearSize, (height() - kClearSize) / 2);
}

QX_CUPERTINO_END_NAMESPACE
