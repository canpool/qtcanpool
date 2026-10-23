/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentcombobox.h"

#include "fluenttokens.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include <QtGui/QScreen>
#include <QtGui/QWheelEvent>
#include <QtWidgets/QApplication>
#include <QtMath>
#include <functional>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// The face is the standard control's: thirty-two tall, the words keeping
// twelve on the left, the chevron sixteen of ink on the right.
constexpr qreal kHeight = 32.0;
constexpr qreal kPadding = 12.0;
constexpr qreal kChevronWidth = 16.0;
constexpr qreal kChevronStroke = 1.4;
// The list opens at one row of forty; three hundred and twenty is the most
// of it the screen is asked to hold, and the rest walks with the wheel.
constexpr qreal kRowHeight = 40.0;
constexpr qreal kListMaxHeight = 320.0;
constexpr qreal kListPadding = 4.0;
constexpr qreal kRowPadding = 12.0;
constexpr qreal kListRadius = 8.0;
constexpr qreal kSelectedOpacity = 0.06;
constexpr qreal kHoverOpacity = 0.04;
constexpr qreal kFaceFillOpacity = 0.06;
constexpr qreal kFaceStrokeOpacity = 0.10;
constexpr qreal kDisabledOpacity = 0.4;
}   // namespace

namespace
{
/*! The surface the answers open into. It is a popup the combo owns: painted
 *  from the family's tokens, walked with the keys, closed by a click
 *  outside itself - the three things a list of answers owes. */
class FluentComboPopup : public QWidget
{
public:
    explicit FluentComboPopup(FluentComboBox *combo)
        : QWidget(nullptr, Qt::Popup | Qt::FramelessWindowHint)
        , m_combo(combo)
    {
        setFocusPolicy(Qt::StrongFocus);
    }

    /*! Where the chosen answer lands; set once, by the combo that owns it. */
    std::function<void(int)> onSelect;

    void open(int current)
    {
        m_highlight = current;
        m_offset = qBound(0, current - visibleRows() / 2, qMax(0, count() - visibleRows()));
        syncGeometry();
        show();
        setFocus();
        grabKeyboard();
    }

    int chosen() const
    {
        return m_chosen;
    }
    bool wasChosen() const
    {
        return m_chosen >= 0;
    }
    void reset()
    {
        m_chosen = -1;
    }

    int count() const
    {
        return m_combo->count();
    }
    QString itemAt(int index) const
    {
        return m_combo->items().value(index);
    }

    /*! The rows the list can hold at once, which is also how far it scrolls
     *  in items. */
    int visibleRows() const
    {
        return qMax(1, int((kListMaxHeight - 2 * kListPadding) / kRowHeight));
    }

    void syncGeometry()
    {
        const int rows = qMin(count(), visibleRows());
        const int height = int(2 * kListPadding + rows * kRowHeight);
        const int width = qMax(m_combo->width(), sizeHint().width());
        resize(width, height);
    }

    QSize sizeHint() const override
    {
        const QFontMetrics metrics(FluentType::font(FluentType::Ramp::Body));
        int widest = 0;
        const QStringList items = m_combo->items();
        for (const QString &item : items) {
            widest = qMax(widest, metrics.horizontalAdvance(item));
        }
        return QSize(int(2 * (kListPadding + kRowPadding) + widest + kChevronWidth),
                     int(2 * kListPadding + qMin(count(), visibleRows()) * kRowHeight));
    }

    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event)
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        const QRectF face(rect());
        painter.setPen(Qt::NoPen);
        painter.setBrush(palette().color(QPalette::Window));
        painter.drawRoundedRect(face, kListRadius, kListRadius);
        painter.setBrush(QxControls::QxStateLayer::layered(palette().color(QPalette::Window),
                                                           palette().color(QPalette::WindowText), 0.14));
        painter.drawRoundedRect(face.adjusted(0.5, 0.5, -0.5, -0.5), kListRadius, kListRadius);

        painter.setRenderHint(QPainter::Antialiasing, false);
        const QFont font = FluentType::font(FluentType::Ramp::Body);
        painter.setFont(font);
        const QFontMetrics metrics(font);
        const int rows = qMin(count() - m_offset, visibleRows());
        for (int row = 0; row < rows; ++row) {
            const int index = m_offset + row;
            const QRectF rowRect(kListPadding, kListPadding + row * kRowHeight, width() - 2 * kListPadding, kRowHeight);
            if (index == m_highlight) {
                painter.setPen(Qt::NoPen);
                painter.setBrush(QxControls::QxStateLayer::layered(
                    palette().color(QPalette::Window), palette().color(QPalette::WindowText), kSelectedOpacity));
                painter.drawRoundedRect(rowRect.adjusted(2.0, 2.0, -2.0, -2.0), 4.0, 4.0);
            } else if (index == m_hover) {
                painter.setPen(Qt::NoPen);
                painter.setBrush(QxControls::QxStateLayer::layered(
                    palette().color(QPalette::Window), palette().color(QPalette::WindowText), kHoverOpacity));
                painter.drawRoundedRect(rowRect.adjusted(2.0, 2.0, -2.0, -2.0), 4.0, 4.0);
            }
            painter.setPen(palette().color(QPalette::WindowText));
            painter.drawText(rowRect.adjusted(kRowPadding, 0.0, -kRowPadding, 0.0),
                             Qt::AlignVCenter | Qt::TextSingleLine, itemAt(index));
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        const QPointF pos = event->position();
#else
        const QPointF pos = event->localPos();
#endif
        m_hover = rowAt(pos.y());
        update();
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        const QPointF pos = event->position();
#else
        const QPointF pos = event->localPos();
#endif
        const int row = rowAt(pos.y());
        if (row >= 0) {
            choose(m_offset + row);
        }
    }

    void wheelEvent(QWheelEvent *event)
    {
        const int steps = event->angleDelta().y() > 0 ? -1 : 1;
        const int furthest = qMax(0, count() - visibleRows());
        const int moved = qBound(0, m_offset + steps, furthest);
        if (moved != m_offset) {
            m_offset = moved;
            m_hover = -1;
            update();
        }
    }

    void keyPressEvent(QKeyEvent *event) override
    {
        const int furthest = qMax(0, count() - visibleRows());
        switch (event->key()) {
        case Qt::Key_Down:
            moveHighlight(1);
            break;
        case Qt::Key_Up:
            moveHighlight(-1);
            break;
        case Qt::Key_Home:
            m_highlight = 0;
            m_offset = 0;
            update();
            break;
        case Qt::Key_End:
            m_highlight = count() - 1;
            m_offset = furthest;
            update();
            break;
        case Qt::Key_Enter:
        case Qt::Key_Return:
            if (m_highlight >= 0) {
                choose(m_highlight);
            }
            break;
        case Qt::Key_Escape:
            close();
            break;
        default:
            QWidget::keyPressEvent(event);
        }
    }
private:
    /*! The row the y coordinate falls in, in offset terms; -1 when it falls
     *  on the padding or past the list. */
    int rowAt(qreal y) const
    {
        const int row = int((y - kListPadding) / kRowHeight);
        const int rows = qMin(count() - m_offset, visibleRows());
        return (row >= 0 && row < rows) ? row : -1;
    }

    void moveHighlight(int steps)
    {
        if (count() == 0) {
            return;
        }
        m_highlight = qBound(0, m_highlight + steps, count() - 1);
        // The highlight asks the window to follow it when it walks out.
        if (m_highlight < m_offset) {
            m_offset = m_highlight;
        } else if (m_highlight >= m_offset + visibleRows()) {
            m_offset = m_highlight - visibleRows() + 1;
        }
        update();
    }

    void choose(int index)
    {
        m_chosen = index;
        close();
        if (onSelect) {
            onSelect(index);
        }
    }

    FluentComboBox *m_combo;
    int m_highlight = -1;
    int m_hover = -1;
    int m_offset = 0;
    int m_chosen = -1;
};
}   // namespace

class FluentComboBoxPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentComboBox)

    QStringList items;
    int current = -1;
    FluentComboPopup *popup = Q_NULLPTR;

    QRectF chevronRect(const QSizeF &face) const;
    void placePopup();
};

QRectF FluentComboBoxPrivate::chevronRect(const QSizeF &face) const
{
    return QRectF(face.width() - kPadding - kChevronWidth, (face.height() - kChevronWidth) / 2.0, kChevronWidth,
                  kChevronWidth);
}

void FluentComboBoxPrivate::placePopup()
{
    Q_Q(FluentComboBox);
    popup->syncGeometry();
    const QScreen *screen = q->screen() ? q->screen() : QGuiApplication::primaryScreen();
    const QRect room = screen->availableGeometry();
    const QPoint below = q->mapToGlobal(QPoint(0, q->height()));
    const QPoint above = q->mapToGlobal(QPoint(0, 0));
    const int height = popup->height();

    // Below first; above when below has no room for it; and when neither
    // has, the list keeps inside the screen, over the face if it must.
    int y = below.y() + 1;
    if (y + height > room.bottom() + 1) {
        if (above.y() - 1 - height >= room.top()) {
            y = above.y() - 1 - height;
        } else {
            y = qBound(room.top(), below.y() + 1, room.bottom() + 1 - height);
        }
    }
    int x = q->mapToGlobal(QPoint(0, 0)).x();
    x = qBound(room.left(), x, room.right() + 1 - popup->width());
    popup->move(x, y);
}

FluentComboBox::FluentComboBox(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentComboBoxPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setFocusPolicy(Qt::StrongFocus);
}

FluentComboBox::~FluentComboBox(){QX_FINI_PRIVATE()}

QStringList FluentComboBox::items() const
{
    return d_ptr->items;
}

int FluentComboBox::count() const
{
    return d_ptr->items.count();
}

int FluentComboBox::currentIndex() const
{
    return d_ptr->current;
}

QString FluentComboBox::currentText() const
{
    return d_ptr->items.value(d_ptr->current);
}

QWidget *FluentComboBox::popup() const
{
    return d_ptr->popup;
}

void FluentComboBox::setItems(const QStringList &items)
{
    if (d_ptr->items == items) {
        return;
    }
    d_ptr->items = items;
    const int moved = d_ptr->current >= d_ptr->items.count() ? d_ptr->items.count() - 1 : d_ptr->current;
    if (moved != d_ptr->current) {
        d_ptr->current = moved;
        Q_EMIT currentIndexChanged(moved);
    }
    updateGeometry();
    update();
}

void FluentComboBox::addItem(const QString &item)
{
    d_ptr->items.append(item);
    updateGeometry();
    update();
}

void FluentComboBox::clear()
{
    if (d_ptr->items.isEmpty() && d_ptr->current == -1) {
        return;
    }
    d_ptr->items.clear();
    if (d_ptr->current != -1) {
        d_ptr->current = -1;
        Q_EMIT currentIndexChanged(-1);
    }
    updateGeometry();
    update();
}

void FluentComboBox::setCurrentIndex(int index)
{
    // -1 is "nothing in force"; anything else must name a real answer, and
    // naming one that is not there changes nothing - the standing choice
    // stays standing, and stays silent about it.
    const int bounded = (index == -1 || (index >= 0 && index < d_ptr->items.count())) ? index : d_ptr->current;
    if (d_ptr->current == bounded) {
        // Only a real move speaks: the same choice said twice is silence.
        return;
    }
    d_ptr->current = bounded;
    Q_EMIT currentIndexChanged(bounded);
    update();
}

void FluentComboBox::showPopup()
{
    if (!isEnabled() || d_ptr->items.isEmpty()) {
        return;
    }
    if (!d_ptr->popup) {
        d_ptr->popup = new FluentComboPopup(this);
        connect(d_ptr->popup, &QObject::destroyed, this, [this]() {
            d_ptr->popup = Q_NULLPTR;
        });
        d_ptr->popup->onSelect = [this](int index) {
            setCurrentIndex(index);
        };
    }
    d_ptr->popup->reset();
    d_ptr->placePopup();
    d_ptr->popup->open(d_ptr->current);
}

void FluentComboBox::hidePopup()
{
    if (d_ptr->popup) {
        d_ptr->popup->close();
    }
}

QSize FluentComboBox::sizeHint() const
{
    const QFontMetrics metrics(FluentType::font(FluentType::Ramp::Body));
    int widest = metrics.horizontalAdvance(currentText());
    const QStringList entries = d_ptr->items;
    for (const QString &entry : entries) {
        widest = qMax(widest, metrics.horizontalAdvance(entry));
    }
    return QSize(qCeil(2 * kPadding + widest + kChevronWidth + kPadding / 2.0), int(kHeight));
}

QSize FluentComboBox::minimumSizeHint() const
{
    return sizeHint();
}

qreal FluentComboBox::cornerRadius() const
{
    return FluentShape::radius(FluentShape::Radius::Medium);
}

void FluentComboBox::paintSurface(QPainter *painter)
{
    const qreal radius = cornerRadius();
    const QRectF face(0.0, 0.0, qreal(width()), qreal(height()));
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(QxControls::QxStateLayer::layered(palette().color(QPalette::Window), textColor(),
                                                        isEnabled() ? kFaceFillOpacity : 0.0));
    painter->drawRoundedRect(face, radius, radius);
    if (isEnabled()) {
        painter->setPen(
            QxControls::QxStateLayer::layered(palette().color(QPalette::Window), textColor(), kFaceStrokeOpacity));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(face.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
    }
    painter->restore();
}

void FluentComboBox::paintContent(QPainter *painter)
{
    painter->save();
    painter->setOpacity(isEnabled() ? 1.0 : kDisabledOpacity);
    painter->setFont(FluentType::font(FluentType::Ramp::Body));
    painter->setPen(textColor());
    const QRectF face(0.0, 0.0, qreal(width()), qreal(height()));
    const QRectF words(kPadding, 0.0, face.width() - kPadding - kChevronWidth - kPadding / 2.0, face.height());
    painter->drawText(words, Qt::AlignVCenter | Qt::TextSingleLine, currentText());
    // The chevron: two strokes of one angle, the ink the face is drawn in.
    const QRectF chevron = d_ptr->chevronRect(face.size());
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(QPen(textColor(), kChevronStroke));
    const QPointF tip(chevron.center().x(), chevron.bottom() - chevron.height() / 4.0);
    painter->drawLine(QPointF(chevron.left(), chevron.top() + chevron.height() / 4.0), tip);
    painter->drawLine(tip, QPointF(chevron.right(), chevron.top() + chevron.height() / 4.0));
    painter->restore();
}

void FluentComboBox::mousePressEvent(QMouseEvent *event)
{
    const QPointF pos =
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        event->position();
#else
        event->localPos();
#endif
    if (isEnabled() && QRectF(rect()).contains(pos)) {
        showPopup();
        event->accept();
        return;
    }
    QxControls::QxControl::mousePressEvent(event);
}

void FluentComboBox::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
    case Qt::Key_Space:
    case Qt::Key_Down:
    case Qt::Key_Up:
        showPopup();
        event->accept();
        return;
    default:
        QxControls::QxControl::keyPressEvent(event);
    }
}

QAccessible::Role FluentComboBox::accessibleRole() const
{
    return QAccessible::ComboBox;
}

void FluentComboBox::fillAccessibleState(QAccessible::State *state) const
{
    state->expandable = true;
    state->expanded = d_ptr->popup && d_ptr->popup->isVisible();
}

QX_FLUENT_END_NAMESPACE
