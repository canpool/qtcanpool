/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialsegmentedbutton.h"

#include "materialtokens.h"

#include "qxcontrols/qxcontrol.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QResizeEvent>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// Material's segmented button is forty tall; a segment narrower than this is
// hard to aim at, so it is the floor a segment's width does not go under.
constexpr qreal kHeight = 40.0;
constexpr qreal kMinSegmentWidth = 64.0;
constexpr qreal kPadding = 12.0;

// The divider, and the outline round the row, are the one surface a step away
// from the page - not the outline of a control that can be pressed.
constexpr qreal kOutlineOpacity = 0.30;
// The fill of the segment in force: a tint, because the row is still on the
// page and not a button of its own.
constexpr qreal kSelectedFill = 0.16;

// The outline's own width, which the segments keep clear of so that it is not
// painted over by the fill of the one in force.
constexpr qreal kInset = 1.0;
}   // namespace

class Segment;

class MaterialSegmentedButtonPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialSegmentedButton)

    QList<Segment *> segments;
    int currentIndex = -1;

    /*! Make one segment per item of \a items, replacing the ones before. */
    void rebuild(const QStringList &items);
    /*! Give every segment an equal share of the row. */
    void relayout();
    /*! Put \a index in force, and take the keyboard there when asked to. */
    void setCurrent(int index, bool takeFocus);
};

/*!
 * One choice in the row: a segment is a control of its own, which is why the
 * row does not have to work out where the pointer is or which part of it was
 * pressed - each segment tracks that for itself, and washes, rings and clicks
 * the way every other control here does.
 */
class Segment : public QxControls::QxControl
{
public:
    Segment(MaterialSegmentedButtonPrivate *owner, int index, const QString &text)
        : QxControls::QxControl(owner->q_ptr)
        , m_owner(owner)
        , m_index(index)
    {
        setFocusPolicy(Qt::NoFocus);
        setText(text);
        show();
    }

    void setText(const QString &text)
    {
        if (m_text == text) {
            return;
        }
        // The label is the accessible name for as long as the application has
        // not given the segment a better one.
        if (accessibleName().isEmpty() || accessibleName() == m_text) {
            setAccessibleName(text);
        }
        m_text = text;
        update();
    }
    QString text() const
    {
        return m_text;
    }

    int index() const
    {
        return m_index;
    }
    bool isCurrent() const
    {
        return m_owner->currentIndex == m_index;
    }

    qreal cornerRadius() const override
    {
        return MaterialShape::radius(MaterialShape::Radius::Medium) - kInset;
    }

    void paintSurface(QPainter *painter) override
    {
        if (!isCurrent()) {
            return;
        }
        QColor fill = accentColor();
        fill.setAlphaF(kSelectedFill);
        painter->setPen(Qt::NoPen);
        painter->setBrush(fill);
        painter->drawRoundedRect(QRectF(rect()), cornerRadius(), cornerRadius());
    }

    void paintContent(QPainter *painter) override
    {
        const QRectF area = QRectF(rect());
        if (m_index > 0) {
            QColor divider = textColor();
            divider.setAlphaF(kOutlineOpacity);
            painter->setPen(divider);
            painter->drawLine(QPointF(area.left() + 0.5, area.top()), QPointF(area.left() + 0.5, area.bottom()));
        }
        painter->setPen(textColor());
        painter->setFont(MaterialType::font(MaterialType::Scale::Body));
        painter->drawText(area.adjusted(kPadding, 0.0, -kPadding, 0.0), Qt::AlignCenter, m_text);
    }

    void keyPressEvent(QKeyEvent *event) override
    {
        // The arrows move what is in force, the way they do in a group of
        // radio buttons: a segmented button is one of a fixed number of
        // answers, not a row of buttons the user walks along.
        int step = 0;
        switch (event->key()) {
        case Qt::Key_Left:
        case Qt::Key_Up:
            step = -1;
            break;
        case Qt::Key_Right:
        case Qt::Key_Down:
            step = 1;
            break;
        default:
            break;
        }
        if (step == 0 || m_owner->segments.count() < 2) {
            QxControls::QxControl::keyPressEvent(event);
            return;
        }
        const int count = m_owner->segments.count();
        m_owner->setCurrent((m_index + step + count) % count, true);
        event->accept();
    }

    QAccessible::Role accessibleRole() const override
    {
        return QAccessible::RadioButton;
    }
    void fillAccessibleState(QAccessible::State *state) const override
    {
        state->checkable = true;
        state->checked = isCurrent();
    }
private:
    MaterialSegmentedButtonPrivate *m_owner;
    int m_index;
    QString m_text;
};

void MaterialSegmentedButtonPrivate::rebuild(const QStringList &items)
{
    qDeleteAll(segments);
    segments.clear();
    for (const QString &item : items) {
        Segment *segment = new Segment(this, segments.count(), item);
        QObject::connect(segment, &QxControls::QxControl::clicked, q_ptr, [this, segment]() {
            setCurrent(segment->index(), true);
        });
        segments.append(segment);
    }
    relayout();
}

void MaterialSegmentedButtonPrivate::relayout()
{
    if (segments.isEmpty()) {
        return;
    }
    const QRect inner = q_ptr->rect().adjusted(int(kInset), int(kInset), -int(kInset), -int(kInset));
    const qreal each = qreal(inner.width()) / qreal(segments.count());
    for (int i = 0; i < segments.count(); ++i) {
        const int left = inner.left() + qRound(i * each);
        const int right = inner.left() + qRound((i + 1) * each);
        segments.at(i)->setGeometry(left, inner.top(), qMax(0, right - left), inner.height());
    }
}

void MaterialSegmentedButtonPrivate::setCurrent(int index, bool takeFocus)
{
    const int bounded = segments.isEmpty() ? -1 : qBound(0, index, segments.count() - 1);
    const bool changed = bounded != currentIndex;
    currentIndex = bounded;

    for (Segment *segment : segments) {
        // The row is one stop on the way through the keyboard, and the stop is
        // where what is in force is: Tab does not walk along the segments.
        segment->setFocusPolicy(segment->index() == bounded ? Qt::StrongFocus : Qt::NoFocus);
        segment->update();
    }
    if (takeFocus && bounded >= 0) {
        segments.at(bounded)->setFocus();
    }
    if (changed) {
        Q_EMIT q_ptr->currentIndexChanged(bounded);
    }
}

MaterialSegmentedButton::MaterialSegmentedButton(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new MaterialSegmentedButtonPrivate){QX_SET_PRIVATE(d_ptr)}

    MaterialSegmentedButton::~MaterialSegmentedButton()
{
    QX_FINI_PRIVATE()
}

void MaterialSegmentedButton::setItems(const QStringList &items)
{
    const int previous = d_ptr->currentIndex;
    d_ptr->rebuild(items);
    // Keep the choice in force where the list still has it; where it does not,
    // the first goes in force, because a row of choices with nothing in force
    // is a row that answers nothing.
    const int wanted =
        d_ptr->segments.isEmpty() ? -1 : (previous >= 0 && previous < d_ptr->segments.count() ? previous : 0);
    d_ptr->setCurrent(wanted, false);
    updateGeometry();
    update();
}

QStringList MaterialSegmentedButton::items() const
{
    QStringList items;
    items.reserve(d_ptr->segments.count());
    for (const Segment *segment : d_ptr->segments) {
        items.append(segment->text());
    }
    return items;
}

int MaterialSegmentedButton::count() const
{
    return d_ptr->segments.count();
}

int MaterialSegmentedButton::currentIndex() const
{
    return d_ptr->currentIndex;
}

QString MaterialSegmentedButton::currentText() const
{
    const Segment *segment = d_ptr->segments.value(d_ptr->currentIndex, Q_NULLPTR);
    return segment ? segment->text() : QString();
}

QSize MaterialSegmentedButton::sizeHint() const
{
    const QFontMetrics metrics(MaterialType::font(MaterialType::Scale::Body));
    qreal width = 2.0 * kInset;
    for (const Segment *segment : d_ptr->segments) {
        width += qMax(kMinSegmentWidth, metrics.horizontalAdvance(segment->text()) + 2.0 * kPadding);
    }
    return QSize(qRound(width), int(kHeight));
}

QSize MaterialSegmentedButton::minimumSizeHint() const
{
    const int count = qMax(1, d_ptr->segments.count());
    return QSize(int(2.0 * kInset) + count * int(kMinSegmentWidth), int(kHeight));
}

void MaterialSegmentedButton::setCurrentIndex(int index)
{
    d_ptr->setCurrent(index, false);
}

void MaterialSegmentedButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor outline = palette().color(QPalette::WindowText);
    outline.setAlphaF(kOutlineOpacity);
    painter.setPen(QPen(outline, 1.0));
    painter.setBrush(Qt::NoBrush);
    const qreal radius = MaterialShape::radius(MaterialShape::Radius::Medium);
    painter.drawRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
}

void MaterialSegmentedButton::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    d_ptr->relayout();
}

void MaterialSegmentedButton::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::FontChange || event->type() == QEvent::PaletteChange) {
        d_ptr->relayout();
        update();
    }
}

QX_MATERIAL_END_NAMESPACE
