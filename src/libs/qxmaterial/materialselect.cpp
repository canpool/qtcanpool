/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialselect.h"

#include "materialtokens.h"

#include "qxcontrols/qxpopupsurface.h"
#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QPainter>
#include <QtGui/QScreen>
#include <QtGui/QWheelEvent>
#include <QtWidgets/QApplication>
#include <functional>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// The list opens at one row of forty-eight; the mark at a chosen row's left
// is the answer's own stroke.
constexpr qreal kRowHeight = 48.0;
constexpr qreal kRowPadding = 16.0;
constexpr qreal kMarkZone = 40.0;
constexpr qreal kMarkInset = 14.0;
constexpr qreal kMarkStroke = 1.6;
constexpr qreal kSelectedOpacity = 0.08;
constexpr qreal kHoverOpacity = 0.04;
constexpr qreal kChevronWidth = 14.0;
constexpr qreal kChevronStroke = 1.4;
constexpr qreal kDisabledOpacity = 0.38;

QPointF eventPosOf(QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}

/*! The surface the answers open into: the shared surface carrying this
 *  family's rows. */
class MaterialSelectPopup : public QxControls::QxPopupSurface
{
public:
    explicit MaterialSelectPopup(MaterialSelect *select)
        : QxControls::QxPopupSurface(select)
        , m_select(select)
    {
        setSurfaceRadius(4.0);
    }

    std::function<void(int)> onSelect;

    void open(int current)
    {
        m_highlight = current;
        m_offset = qBound(0, current - visibleRows() / 2, qMax(0, count() - visibleRows()));
        QxControls::QxPopupSurface::open();
    }

    int count() const
    {
        return m_select->count();
    }
    QString itemAt(int index) const
    {
        return m_select->items().value(index);
    }
    int visibleRows() const
    {
        // A screen holds what it holds; the list does not promise a number.
        const int room = QxControls::QxPopupSurface::screen()
                             ? int(QxControls::QxPopupSurface::screen()->availableGeometry().height() / 2 / kRowHeight)
                             : 8;
        return qMax(1, room);
    }

    QSize contentSizeHint() const override
    {
        const QFontMetrics metrics(MaterialType::font(MaterialType::Scale::Body));
        int widest = 0;
        const QStringList items = m_select->items();
        for (const QString &item : items) {
            widest = qMax(widest, metrics.horizontalAdvance(item));
        }
        return QSize(int(2 * kRowPadding + kMarkZone + widest + kRowPadding),
                     int(qMin(count(), visibleRows()) * kRowHeight));
    }

    void paintContent(QPainter *painter, const QRectF &content) override
    {
        painter->setRenderHint(QPainter::Antialiasing, true);
        const QFont font = MaterialType::font(MaterialType::Scale::Body);
        painter->setFont(font);
        const int rows = qMin(count() - m_offset, visibleRows());
        for (int row = 0; row < rows; ++row) {
            const int index = m_offset + row;
            const QRectF rowRect(content.left(), content.top() + row * kRowHeight, content.width(), kRowHeight);
            if (index == m_highlight) {
                painter->setPen(Qt::NoPen);
                painter->setBrush(QxControls::QxStateLayer::layered(
                    palette().color(QPalette::Window), palette().color(QPalette::WindowText), kSelectedOpacity));
                painter->drawRect(rowRect);
            } else if (index == m_hover) {
                painter->setPen(Qt::NoPen);
                painter->setBrush(QxControls::QxStateLayer::layered(
                    palette().color(QPalette::Window), palette().color(QPalette::WindowText), kHoverOpacity));
                painter->drawRect(rowRect);
            }
            if (index == m_highlight) {
                // The answer in force wears the mark at its left, where an
                // unchosen row keeps nothing.
                painter->setRenderHint(QPainter::Antialiasing, false);
                painter->setPen(QPen(m_select->accentColor(), kMarkStroke));
                const QPointF tip(rowRect.left() + kMarkInset + kMarkStroke / 2.0, rowRect.center().y());
                painter->drawLine(QPointF(rowRect.left() + kMarkInset * 0.4, rowRect.center().y()), tip);
                painter->drawLine(tip, QPointF(rowRect.left() + kMarkInset + kMarkZone / 4.0,
                                               rowRect.center().y() - kMarkStroke * 3.0));
                painter->setRenderHint(QPainter::Antialiasing, true);
            }
            painter->setPen(palette().color(QPalette::WindowText));
            painter->drawText(rowRect.adjusted(kMarkZone, 0.0, -kRowPadding, 0.0),
                              Qt::AlignVCenter | Qt::TextSingleLine, itemAt(index));
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        m_hover = rowAt(eventPosOf(event).y() - contentRect().top());
        update();
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        const int row = rowAt(eventPosOf(event).y() - contentRect().top());
        if (row >= 0) {
            close();
            if (onSelect) {
                onSelect(m_offset + row);
            }
        }
    }

    void wheelEvent(QWheelEvent *event) override
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
                close();
                if (onSelect) {
                    onSelect(m_highlight);
                }
            }
            break;
        default:
            QxControls::QxPopupSurface::keyPressEvent(event);
        }
    }
private:
    int rowAt(qreal y) const
    {
        if (y < 0 || y >= qMin(count() - m_offset, visibleRows()) * kRowHeight) {
            return -1;
        }
        return int(y / kRowHeight);
    }

    void moveHighlight(int steps)
    {
        if (count() == 0) {
            return;
        }
        m_highlight = qBound(0, m_highlight + steps, count() - 1);
        if (m_highlight < m_offset) {
            m_offset = m_highlight;
        } else if (m_highlight >= m_offset + visibleRows()) {
            m_offset = m_highlight - visibleRows() + 1;
        }
        update();
    }

    MaterialSelect *m_select;
    int m_highlight = -1;
    int m_hover = -1;
    int m_offset = 0;
};
}   // namespace

class MaterialSelectPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialSelect)

    QStringList items;
    int current = -1;
    MaterialSelectPopup *popup = Q_NULLPTR;
};

MaterialSelect::MaterialSelect(QWidget *parent)
    : MaterialTextField(parent)
    , d_ptr(new MaterialSelectPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setLabel(QString());
}

MaterialSelect::~MaterialSelect(){QX_FINI_PRIVATE()}

QStringList MaterialSelect::items() const
{
    return d_ptr->items;
}

int MaterialSelect::count() const
{
    return d_ptr->items.count();
}

int MaterialSelect::currentIndex() const
{
    return d_ptr->current;
}

QString MaterialSelect::currentText() const
{
    return d_ptr->items.value(d_ptr->current);
}

QWidget *MaterialSelect::popup() const
{
    return d_ptr->popup;
}

void MaterialSelect::setItems(const QStringList &items)
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
    setText(currentText());
    update();
}

void MaterialSelect::addItem(const QString &item)
{
    d_ptr->items.append(item);
    update();
}

void MaterialSelect::clear()
{
    if (d_ptr->items.isEmpty() && d_ptr->current == -1) {
        return;
    }
    d_ptr->items.clear();
    if (d_ptr->current != -1) {
        d_ptr->current = -1;
        Q_EMIT currentIndexChanged(-1);
    }
    setText(QString());
    update();
}

void MaterialSelect::setCurrentIndex(int index)
{
    // -1 is "nothing in force"; anything else must name a real answer, and
    // naming one that is not there changes nothing.
    const int bounded = (index == -1 || (index >= 0 && index < d_ptr->items.count())) ? index : d_ptr->current;
    if (d_ptr->current == bounded) {
        return;
    }
    d_ptr->current = bounded;
    Q_EMIT currentIndexChanged(bounded);
    setText(currentText());
    update();
}

void MaterialSelect::showPopup()
{
    if (!isEnabled() || d_ptr->items.isEmpty()) {
        return;
    }
    if (!d_ptr->popup) {
        d_ptr->popup = new MaterialSelectPopup(this);
        connect(d_ptr->popup, &QObject::destroyed, this, [this]() {
            d_ptr->popup = Q_NULLPTR;
        });
        d_ptr->popup->onSelect = [this](int index) {
            setCurrentIndex(index);
        };
    }
    d_ptr->popup->open(d_ptr->current);
}

void MaterialSelect::hidePopup()
{
    if (d_ptr->popup) {
        d_ptr->popup->close();
    }
}

void MaterialSelect::paintContent(QPainter *painter)
{
    // The field paints its text - the answer in force; the select adds only
    // the chevron that says there are more where that came from.
    MaterialTextField::paintContent(painter);
    if (!isEnabled()) {
        return;
    }
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setOpacity(isEnabled() ? 1.0 : kDisabledOpacity);
    const QRectF face(0.0, 0.0, qreal(width()), qreal(height()));
    const QRectF chevron(face.right() - kRowPadding - kChevronWidth, face.center().y() - kChevronWidth / 2.0,
                         kChevronWidth, kChevronWidth);
    painter->setPen(QPen(textColor(), kChevronStroke));
    const QPointF tip(chevron.center().x(), chevron.bottom() - chevron.height() / 4.0);
    painter->drawLine(QPointF(chevron.left(), chevron.top() + chevron.height() / 4.0), tip);
    painter->drawLine(tip, QPointF(chevron.right(), chevron.top() + chevron.height() / 4.0));
    painter->restore();
}

void MaterialSelect::mousePressEvent(QMouseEvent *event)
{
    if (isEnabled()) {
        showPopup();
        event->accept();
        return;
    }
    MaterialTextField::mousePressEvent(event);
}

void MaterialSelect::keyPressEvent(QKeyEvent *event)
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
        MaterialTextField::keyPressEvent(event);
    }
}

QAccessible::Role MaterialSelect::accessibleRole() const
{
    return QAccessible::ComboBox;
}

void MaterialSelect::fillAccessibleState(QAccessible::State *state) const
{
    state->expandable = true;
    state->expanded = d_ptr->popup && d_ptr->popup->isVisible();
}

QX_MATERIAL_END_NAMESPACE
