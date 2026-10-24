/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialtogglebuttongroup.h"

#include "materialtokens.h"

#include "qxcontrols/qxcontrol.h"
#include "qxcontrols/qxstatelayer.h"

#include <QtCore/QtMath>
#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QSizePolicy>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// A button of this row is as tall as the buttons it belongs among, and as wide
// as its own label and icon ask - which is the first of the three places the
// row parts company with a segmented button, where every part gets the same
// share of the row whatever it says.
constexpr qreal kHeight = 40.0;
constexpr qreal kTextPadding = 16.0;
constexpr qreal kIconSize = 18.0;
constexpr qreal kIconGap = 8.0;
// Narrower than this is a button nobody can aim at, however short its label.
constexpr int kLeastWidth = 48;

// The outline a button shares with its neighbour: the page's ink, kept quiet
// because the edge is not what says whether a button is on - both buttons it
// stands between would be repainting it in a colour about one of them.
constexpr qreal kOutlineOpacity = 0.12;
// What being on says instead: the accent under the label, at the strength
// Material gives a selected surface, and the label itself in the accent.
constexpr qreal kCheckedFillOpacity = 0.12;
constexpr qreal kDisabledOpacity = 0.38;

/*! The radius a row of joined buttons has at the ends of the row. */
qreal cellRadius()
{
    return MaterialShape::radius(MaterialShape::Radius::Small);
}

/*! Turn the corner into \a path as a quarter circle of \a radius at \a corner,
 *  starting at \a from degrees and going clockwise. */
void turn(QPainterPath &path, const QPointF &corner, qreal radius, qreal from)
{
    path.arcTo(QRectF(corner.x(), corner.y(), 2.0 * radius, 2.0 * radius), from, -90.0);
}

/*!
 * The shape of one cell of the row: round at the ends of the row, square where
 * it meets its neighbour, because a row of joined buttons is one outline with
 * corners only on the outside.
 *
 * \a leftEdge is what the fill and the outline ask differently about: the fill
 * covers the whole cell and closes back on itself, while the outline leaves out
 * the edge it shares - the neighbour draws that one, and drawing it twice would
 * make the divider between two buttons twice as heavy as the ends of the row.
 * An outline that leaves the edge out is therefore left open; its caller
 * strokes it and fills nothing.
 */
QPainterPath cellPath(const QRectF &area, bool first, bool last, qreal radius, bool leftEdge)
{
    const qreal topLeft = first ? radius : 0.0;
    const qreal bottomLeft = first ? radius : 0.0;
    const qreal topRight = last ? radius : 0.0;
    const qreal bottomRight = last ? radius : 0.0;

    QPainterPath path;
    if (leftEdge) {
        path.moveTo(area.left() + topLeft, area.top());
    } else {
        path.moveTo(area.left(), area.top());
    }
    path.lineTo(area.right() - topRight, area.top());
    if (topRight > 0.0) {
        turn(path, QPointF(area.right() - 2.0 * topRight, area.top()), topRight, 90.0);
    } else {
        path.lineTo(area.right(), area.top());
    }
    path.lineTo(area.right(), area.bottom() - bottomRight);
    if (bottomRight > 0.0) {
        turn(path, QPointF(area.right() - 2.0 * bottomRight, area.bottom() - 2.0 * bottomRight), bottomRight, 0.0);
    } else {
        path.lineTo(area.right(), area.bottom());
    }
    path.lineTo(area.left() + bottomLeft, area.bottom());
    if (bottomLeft > 0.0) {
        turn(path, QPointF(area.left(), area.bottom() - 2.0 * bottomLeft), bottomLeft, 270.0);
    } else {
        path.lineTo(area.left(), area.bottom());
    }
    if (leftEdge) {
        path.lineTo(area.left(), area.top() + topLeft);
        if (topLeft > 0.0) {
            path.arcTo(QRectF(area.left(), area.top(), 2.0 * topLeft, 2.0 * topLeft), 180.0, -90.0);
        }
        path.closeSubpath();
    }
    return path;
}

/*! How wide a button that says \a text beside \a icon asks to be. */
int askedWidth(const QString &text, const QIcon &icon)
{
    qreal content = 0.0;
    if (!icon.isNull()) {
        content += kIconSize;
        if (!text.isEmpty()) {
            content += kIconGap;
        }
    }
    if (!text.isEmpty()) {
        content += QFontMetrics(MaterialType::font(MaterialType::Scale::Body)).horizontalAdvance(text);
    }
    return qMax(kLeastWidth, int(qCeil(content)) + 2 * int(kTextPadding));
}
}   // namespace

class Toggle;

class MaterialToggleButtonGroupPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialToggleButtonGroup)

    QList<Toggle *> buttons;
    bool exclusive = true;

    Toggle *buttonAt(int index) const;
    /*! The first button that is on, or -1 when none is. */
    int firstChecked() const;
    /*! Give every button the geometry the row asks for. */
    void relayout();
    /*! Hand out the group's one Tab stop, or every button its own. */
    void updateStops();
    /*! Put \a index on or off, answering whether anything really changed. */
    bool toggle(int index, bool checked);
};

/*!
 * One button of the row. It is a control of its own, so where the pointer is,
 * what is being pressed and whether the keyboard is on it are answered for it
 * rather than worked out by the row - the row only decides what on and off mean
 * and how wide each button stands.
 */
class Toggle : public QxControls::QxControl
{
public:
    Toggle(MaterialToggleButtonGroupPrivate *owner, int index)
        : QxControls::QxControl(owner->q_ptr)
        , m_owner(owner)
        , m_index(index)
    {
        show();
    }

    void setIndex(int index)
    {
        m_index = index;
    }
    int index() const
    {
        return m_index;
    }

    void setText(const QString &text)
    {
        // The label is the accessible name while the application has not given
        // the button a better one.
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

    void setIcon(const QIcon &icon)
    {
        m_icon = icon;
        update();
    }
    QIcon icon() const
    {
        return m_icon;
    }

    bool isChecked() const
    {
        return m_checked;
    }
    void setChecked(bool checked)
    {
        m_checked = checked;
    }

    bool isFirst() const
    {
        return m_index == 0;
    }
    bool isLast() const
    {
        return m_index == m_owner->buttons.count() - 1;
    }

    /*! The wash and the focus ring follow the cell's own corners. */
    qreal cornerRadius() const override
    {
        return cellRadius();
    }

    void paintSurface(QPainter *painter) override
    {
        if (!m_checked) {
            return;
        }
        painter->save();
        if (!isEnabled()) {
            painter->setOpacity(kDisabledOpacity);
        }
        QColor fill = accentColor();
        fill.setAlphaF(kCheckedFillOpacity);
        painter->setPen(Qt::NoPen);
        painter->setBrush(fill);
        painter->drawPath(cellPath(QRectF(rect()), isFirst(), isLast(), cellRadius(), true));
        painter->restore();
    }

    void paintContent(QPainter *painter) override
    {
        const QRectF area = QRectF(rect());
        // The edge is drawn whether the button is on or not: it stands between
        // this button and its neighbour, and it is not this button's to repaint.
        // A cell that is not the first leaves out its left edge, which the one
        // before it drew; the stroke rect is inset by half a pixel so that the
        // one-pixel pen lands exactly on the cell's boundary.
        QColor edge = textColor();
        edge.setAlphaF(kOutlineOpacity);
        painter->save();
        painter->setPen(QPen(edge, 1.0));
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(cellPath(area.adjusted(0.5, 0.5, -0.5, -0.5), isFirst(), isLast(), cellRadius(), isFirst()));
        painter->restore();

        painter->save();
        if (!isEnabled()) {
            painter->setOpacity(kDisabledOpacity);
        }
        const QRectF inkArea = area.adjusted(kTextPadding, 0.0, -kTextPadding, 0.0);
        qreal left = inkArea.left();
        if (!m_icon.isNull()) {
            const QRectF box(left, area.center().y() - kIconSize / 2.0, kIconSize, kIconSize);
            m_icon.paint(painter, box.toRect(), Qt::AlignCenter, isEnabled() ? QIcon::Normal : QIcon::Disabled,
                         m_checked ? QIcon::On : QIcon::Off);
            left += kIconSize + (m_text.isEmpty() ? 0.0 : kIconGap);
        }
        if (!m_text.isEmpty()) {
            const QRectF room(left, area.top(), qMax(0.0, inkArea.right() - left), area.height());
            painter->setPen(m_checked ? accentColor() : textColor());
            painter->setFont(MaterialType::font(MaterialType::Scale::Body));
            const QString shown = QFontMetrics(painter->font()).elidedText(m_text, Qt::ElideRight, int(room.width()));
            painter->drawText(room, Qt::AlignCenter, shown);
        }
        painter->restore();
    }

    void keyPressEvent(QKeyEvent *event) override
    {
        switch (event->key()) {
        case Qt::Key_Space:
        case Qt::Key_Return:
        case Qt::Key_Enter:
            // A switch is thrown by the key every other button here is thrown
            // by, and pressing the one that is on is how it is turned off.
            m_owner->toggle(m_index, !m_checked);
            event->accept();
            return;
        case Qt::Key_Left:
        case Qt::Key_Up:
        case Qt::Key_Right:
        case Qt::Key_Down: {
            // Where at most one may be on, the arrows pass the choice along;
            // where every button is its own switch, they are not the row's
            // business and the base leaves them alone.
            if (!m_owner->exclusive) {
                break;
            }
            const int count = m_owner->buttons.count();
            if (count < 2) {
                event->accept();
                return;
            }
            const int step = (event->key() == Qt::Key_Left || event->key() == Qt::Key_Up) ? -1 : 1;
            const int at = (m_index + step + count) % count;
            m_owner->toggle(at, true);
            m_owner->buttons.at(at)->setFocus();
            event->accept();
            return;
        }
        default:
            break;
        }
        QxControls::QxControl::keyPressEvent(event);
    }

    QAccessible::Role accessibleRole() const override
    {
        return QAccessible::Button;
    }
    void fillAccessibleState(QAccessible::State *state) const override
    {
        state->checkable = true;
        state->checked = m_checked;
    }
private:
    MaterialToggleButtonGroupPrivate *m_owner;
    int m_index;
    QString m_text;
    QIcon m_icon;
    bool m_checked = false;
};

Toggle *MaterialToggleButtonGroupPrivate::buttonAt(int index) const
{
    if (index < 0 || index >= buttons.count()) {
        return Q_NULLPTR;
    }
    return buttons.at(index);
}

int MaterialToggleButtonGroupPrivate::firstChecked() const
{
    for (const Toggle *button : buttons) {
        if (button->isChecked()) {
            return button->index();
        }
    }
    return -1;
}

void MaterialToggleButtonGroupPrivate::relayout()
{
    const int count = buttons.count();
    if (count == 0) {
        return;
    }

    QList<int> widths;
    widths.reserve(count);
    int asked = 0;
    for (const Toggle *button : buttons) {
        const int width = askedWidth(button->text(), button->icon());
        widths.append(width);
        asked += width;
    }

    // The row is as wide as its buttons ask. Given more room it shares what is
    // left over evenly rather than evenly dividing the whole row: each button
    // keeps the width its own label needs, and the extra is the row's, not one
    // button's.
    const int room = q_ptr->width();
    if (room > asked) {
        const int extra = room - asked;
        const int each = extra / count;
        const int rest = extra % count;
        for (int i = 0; i < count; ++i) {
            widths[i] += each + (i >= count - rest ? 1 : 0);
        }
    } else if (room < asked) {
        // Squeezed, every button gives up the same amount, down to the floor
        // it cannot go under; what still does not fit is the parent's to clip.
        const int shortfall = asked - room;
        const int each = shortfall / count + (shortfall % count ? 1 : 0);
        for (int i = 0; i < count; ++i) {
            widths[i] = qMax(kLeastWidth, widths[i] - each);
        }
    }

    int left = 0;
    for (int i = 0; i < count; ++i) {
        buttons.at(i)->setGeometry(left, 0, widths.at(i), q_ptr->height());
        left += widths.at(i);
    }
}

void MaterialToggleButtonGroupPrivate::updateStops()
{
    const int stop = exclusive ? qMax(0, firstChecked()) : -1;
    for (Toggle *button : buttons) {
        // An exclusive group is one stop on the way through the keyboard, and
        // the stop is where the choice is; without that, every button is its own.
        const bool isStop = exclusive ? (button->index() == stop) : true;
        button->setFocusPolicy(isStop ? Qt::StrongFocus : Qt::NoFocus);
    }
}

bool MaterialToggleButtonGroupPrivate::toggle(int index, bool checked)
{
    Toggle *button = buttonAt(index);
    if (!button) {
        return false;
    }

    bool changed = false;
    // An exclusive group passes the choice along instead of adding to it, and
    // the button going off is reported before the one coming on: a listener
    // then never hears about a row it was never left with.
    if (exclusive && checked) {
        for (Toggle *other : buttons) {
            if (other == button || !other->isChecked()) {
                continue;
            }
            other->setChecked(false);
            other->update();
            changed = true;
            Q_EMIT q_ptr->buttonToggled(other->index(), false);
        }
    }
    if (button->isChecked() != checked) {
        button->setChecked(checked);
        button->update();
        changed = true;
        Q_EMIT q_ptr->buttonToggled(index, checked);
    }
    if (changed) {
        updateStops();
    }
    return changed;
}

MaterialToggleButtonGroup::MaterialToggleButtonGroup(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new MaterialToggleButtonGroupPrivate)
{
    QX_SET_PRIVATE(d_ptr);
    // The row is as wide as its buttons ask and as tall as they are; a parent
    // that gives it more width gets the extra shared out, not a hole at the end.
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

MaterialToggleButtonGroup::~MaterialToggleButtonGroup()
{
    QX_FINI_PRIVATE();
}

int MaterialToggleButtonGroup::addButton(const QString &text, const QIcon &icon)
{
    Toggle *button = new Toggle(d_ptr, d_ptr->buttons.count());
    button->setIcon(icon);
    button->setText(text);
    QObject::connect(button, &QxControls::QxControl::clicked, this, [this, button]() {
        d_ptr->toggle(button->index(), !button->isChecked());
    });
    d_ptr->buttons.append(button);
    d_ptr->updateStops();
    d_ptr->relayout();
    updateGeometry();
    update();
    return button->index();
}

void MaterialToggleButtonGroup::removeButton(int index)
{
    Toggle *button = d_ptr->buttonAt(index);
    if (!button) {
        return;
    }
    d_ptr->buttons.removeAt(index);
    button->hide();
    button->deleteLater();
    for (int i = 0; i < d_ptr->buttons.count(); ++i) {
        d_ptr->buttons.at(i)->setIndex(i);
    }
    d_ptr->updateStops();
    d_ptr->relayout();
    updateGeometry();
    update();
}

void MaterialToggleButtonGroup::clear()
{
    const QList<Toggle *> gone = d_ptr->buttons;
    d_ptr->buttons.clear();
    for (Toggle *button : gone) {
        button->hide();
        button->deleteLater();
    }
    updateGeometry();
    update();
}

int MaterialToggleButtonGroup::count() const
{
    return d_ptr->buttons.count();
}

QString MaterialToggleButtonGroup::buttonText(int index) const
{
    const Toggle *button = d_ptr->buttonAt(index);
    return button ? button->text() : QString();
}

QIcon MaterialToggleButtonGroup::buttonIcon(int index) const
{
    const Toggle *button = d_ptr->buttonAt(index);
    return button ? button->icon() : QIcon();
}

bool MaterialToggleButtonGroup::isExclusive() const
{
    return d_ptr->exclusive;
}

bool MaterialToggleButtonGroup::isChecked(int index) const
{
    const Toggle *button = d_ptr->buttonAt(index);
    return button ? button->isChecked() : false;
}

QList<int> MaterialToggleButtonGroup::checkedButtons() const
{
    QList<int> checked;
    for (const Toggle *button : d_ptr->buttons) {
        if (button->isChecked()) {
            checked.append(button->index());
        }
    }
    return checked;
}

QRect MaterialToggleButtonGroup::buttonRect(int index) const
{
    const Toggle *button = d_ptr->buttonAt(index);
    return button ? button->geometry() : QRect();
}

QSize MaterialToggleButtonGroup::sizeHint() const
{
    int width = 0;
    for (const Toggle *button : d_ptr->buttons) {
        width += askedWidth(button->text(), button->icon());
    }
    return QSize(width, int(kHeight));
}

QSize MaterialToggleButtonGroup::minimumSizeHint() const
{
    return QSize(d_ptr->buttons.count() * kLeastWidth, int(kHeight));
}

void MaterialToggleButtonGroup::setButtonText(int index, const QString &text)
{
    Toggle *button = d_ptr->buttonAt(index);
    if (!button || button->text() == text) {
        return;
    }
    button->setText(text);
    d_ptr->relayout();
    updateGeometry();
    update();
}

void MaterialToggleButtonGroup::setButtonIcon(int index, const QIcon &icon)
{
    Toggle *button = d_ptr->buttonAt(index);
    if (!button) {
        return;
    }
    button->setIcon(icon);
    d_ptr->relayout();
    updateGeometry();
    update();
}

void MaterialToggleButtonGroup::setExclusive(bool exclusive)
{
    if (d_ptr->exclusive == exclusive) {
        return;
    }
    d_ptr->exclusive = exclusive;
    // Becoming exclusive leaves the first button that was on where it is and
    // turns the rest off - the row cannot answer with two choices once it is
    // allowed only one, and picking which one is not the switch's to decide, so
    // the earliest one stays.
    bool kept = false;
    for (Toggle *button : d_ptr->buttons) {
        if (!button->isChecked()) {
            continue;
        }
        if (!exclusive || !kept) {
            kept = true;
            continue;
        }
        button->setChecked(false);
        button->update();
        Q_EMIT buttonToggled(button->index(), false);
    }
    d_ptr->updateStops();
    update();
    Q_EMIT exclusiveChanged(exclusive);
}

void MaterialToggleButtonGroup::setChecked(int index, bool checked)
{
    d_ptr->toggle(index, checked);
}

void MaterialToggleButtonGroup::clearChecked()
{
    for (Toggle *button : d_ptr->buttons) {
        if (!button->isChecked()) {
            continue;
        }
        button->setChecked(false);
        button->update();
        Q_EMIT buttonToggled(button->index(), false);
    }
    d_ptr->updateStops();
    update();
}

void MaterialToggleButtonGroup::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    d_ptr->relayout();
}

void MaterialToggleButtonGroup::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::FontChange || event->type() == QEvent::PaletteChange) {
        d_ptr->relayout();
        update();
    }
}

QX_MATERIAL_END_NAMESPACE
