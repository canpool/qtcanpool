/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialpagination.h"

#include "materialtokens.h"

#include "qxcontrols/qxcontrol.h"

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
// MUI's paginator item is thirty-two square, and the row is those items laid
// side by side with nothing between them: the circle each one wears is what
// separates it from its neighbour, not a gap.
constexpr int kItemSize = 32;

// The page in force sits in the accent, at the strength Material gives a
// selected surface; the pages around it are the page's ink, and the arrows and
// the ellipsis are quieter than either, because they are not answers.
constexpr qreal kCurrentFillOpacity = 0.12;
constexpr qreal kNumberOpacity = 0.87;
constexpr qreal kChromeOpacity = 0.6;
constexpr qreal kDisabledOpacity = 0.38;

// The chevron inside the arrow buttons: two strokes of ten, four and a half
// either side of the point.
constexpr qreal kChevronReach = 5.0;
constexpr qreal kChevronSpan = 4.0;
constexpr qreal kChevronWidth = 2.0;

enum class SlotKind {
    Previous,
    Next,
    Page,
    Ellipsis
};

/*! One place in the row: an arrow, a page, or the mark that says pages were
 *  left out. \a value is the page number, and means nothing for the others. */
struct Slot {
    SlotKind kind;
    int value;
};

/*! The numbers from \a from up to but not including \a to. */
QList<int> range(int from, int to)
{
    QList<int> numbers;
    for (int number = from; number < to; ++number) {
        numbers.append(number);
    }
    return numbers;
}

/*!
 * The pages the bar shows, as MUI works them out: always the first and the last
 * \a boundary pages, the \a siblings pages either side of the one in force, and
 * an ellipsis wherever the two runs were not next to each other.
 *
 * Two readers of the same list should be told the same thing by the same
 * picture, so the arithmetic is kept as MUI states it rather than worked out
 * again - including the two places where a page is named twice over and only
 * one of the two is drawn.
 */
QList<Slot> buildSlots(int count, int page, int siblings, int boundary)
{
    const QList<int> startPages = range(1, qMin(boundary, count) + 1);
    const QList<int> endPages = range(qMax(count - boundary + 1, boundary + 1), count + 1);
    const int siblingsStart = qMax(qMin(page - siblings, count - boundary - siblings * 2 - 1), boundary + 2);
    const int siblingsEnd =
        qMin(qMax(page + siblings, boundary + siblings * 2 + 2), endPages.isEmpty() ? count - 1 : endPages.first() - 2);

    QList<int> numbers;
    numbers << startPages;
    if (siblingsStart > boundary + 2) {
        numbers << -1;
    } else if (boundary + 1 < count - boundary) {
        numbers << boundary + 1;
    }
    numbers << range(siblingsStart, siblingsEnd + 1);
    if (siblingsEnd < count - boundary - 1) {
        numbers << -1;
    } else if (count - boundary > boundary) {
        numbers << count - boundary;
    }
    numbers << endPages;

    QList<Slot> places;
    places.append(Slot{SlotKind::Previous, 0});
    for (int number : numbers) {
        places.append(Slot{number < 0 ? SlotKind::Ellipsis : SlotKind::Page, number});
    }
    places.append(Slot{SlotKind::Next, 0});
    return places;
}

/*! Draw the chevron of an arrow button, pointing \a left or right. */
void drawChevron(QPainter *painter, const QRectF &area, const QColor &ink, bool left)
{
    const QPointF tip = area.center() + QPointF(left ? -kChevronSpan / 2.0 : kChevronSpan / 2.0, 0.0);
    const QPointF back = tip + QPointF(left ? kChevronSpan : -kChevronSpan, 0.0);

    QPainterPath path(back + QPointF(0.0, -kChevronReach));
    path.lineTo(tip);
    path.lineTo(back + QPointF(0.0, kChevronReach));

    QPen pen(ink, kChevronWidth);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path);
}
}   // namespace

class PageItem;

class MaterialPaginationPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialPagination)

    int count = 1;
    int page = 1;
    int siblings = 1;
    int boundary = 1;

    /*! The places of the row, and the widgets that are showing them. The pool
     *  only grows: a page change reshuffles which widget shows which place, and
     *  nothing is destroyed while a key press is still coming out of one. */
    QList<Slot> places;
    QList<PageItem *> pool;

    void rebuild();
    void relayout();
    /*! Hand out the bar's one Tab stop: the page in force. */
    void updateStops();
    /*! Go to \a target, which must be a page that is there. */
    void goTo(int target);
    /*! The item showing the page in force, or null while there is none. */
    PageItem *current() const;
};

/*!
 * One place in the row. It is a control of its own, so the pointer's arrival,
 * the press and the keyboard are answered for it rather than worked out by the
 * row - and an assistive tool can read each place as the thing it is.
 */
class PageItem : public QxControls::QxControl
{
public:
    explicit PageItem(MaterialPaginationPrivate *owner)
        : QxControls::QxControl(owner->q_ptr)
        , m_owner(owner)
    {
        setFocusPolicy(Qt::NoFocus);
        show();
    }

    void setSlot(const Slot &slot)
    {
        m_kind = slot.kind;
        m_value = slot.value;
        switch (m_kind) {
        case SlotKind::Previous:
            setAccessibleName(QStringLiteral("Previous page"));
            break;
        case SlotKind::Next:
            setAccessibleName(QStringLiteral("Next page"));
            break;
        case SlotKind::Page:
            setAccessibleName(QString::number(m_value));
            break;
        case SlotKind::Ellipsis:
            setAccessibleName(QStringLiteral("\u2026"));
            break;
        }
        // The arrows are disabled at the ends of the list, so that the row does
        // not rearrange itself under the pointer as the page changes.
        if (m_kind == SlotKind::Previous) {
            setEnabled(m_owner->page > 1);
        } else if (m_kind == SlotKind::Next) {
            setEnabled(m_owner->page < m_owner->count);
        } else {
            setEnabled(true);
        }
        update();
    }

    SlotKind kind() const
    {
        return m_kind;
    }
    int value() const
    {
        return m_value;
    }
    bool isCurrent() const
    {
        return m_kind == SlotKind::Page && m_value == m_owner->page;
    }

    qreal cornerRadius() const override
    {
        return MaterialShape::radius(MaterialShape::Radius::Circular);
    }

    /*! The ellipsis is a mark, not a control: nothing about it is the thing a
     *  pointer can land on, so it carries neither a wash nor a ring. */
    QRectF chromeRect() const override
    {
        return m_kind == SlotKind::Ellipsis ? QRectF() : QRectF(rect());
    }
    bool hitTest(const QPointF &pos) const override
    {
        return m_kind != SlotKind::Ellipsis && QxControls::QxControl::hitTest(pos);
    }

    void paintSurface(QPainter *painter) override
    {
        if (!isCurrent()) {
            return;
        }
        QColor fill = accentColor();
        fill.setAlphaF(kCurrentFillOpacity);
        painter->setPen(Qt::NoPen);
        painter->setBrush(fill);
        const qreal radius = MaterialShape::radius(MaterialShape::Radius::Circular);
        painter->drawRoundedRect(QRectF(rect()), radius, radius);
    }

    void paintContent(QPainter *painter) override
    {
        painter->save();
        if (!isEnabled()) {
            painter->setOpacity(kDisabledOpacity);
        }
        const QRectF area = QRectF(rect());
        if (m_kind == SlotKind::Previous || m_kind == SlotKind::Next) {
            QColor ink = textColor();
            ink.setAlphaF(kChromeOpacity);
            drawChevron(painter, area, ink, m_kind == SlotKind::Previous);
        } else if (m_kind == SlotKind::Ellipsis) {
            painter->setPen(chromeInk());
            painter->setFont(MaterialType::font(MaterialType::Scale::Body));
            painter->drawText(area, Qt::AlignCenter, QStringLiteral("\u2026"));
        } else {
            if (isCurrent()) {
                painter->setPen(accentColor());
            } else {
                painter->setPen(numberInk());
            }
            painter->setFont(MaterialType::font(MaterialType::Scale::Body));
            painter->drawText(area, Qt::AlignCenter, QString::number(m_value));
        }
        painter->restore();
    }

    void keyPressEvent(QKeyEvent *event) override
    {
        switch (event->key()) {
        case Qt::Key_Space:
        case Qt::Key_Return:
        case Qt::Key_Enter:
            activate();
            event->accept();
            return;
        case Qt::Key_Left:
        case Qt::Key_PageUp:
            m_owner->goTo(m_owner->page - 1);
            event->accept();
            return;
        case Qt::Key_Right:
        case Qt::Key_PageDown:
            m_owner->goTo(m_owner->page + 1);
            event->accept();
            return;
        case Qt::Key_Home:
            m_owner->goTo(1);
            event->accept();
            return;
        case Qt::Key_End:
            m_owner->goTo(m_owner->count);
            event->accept();
            return;
        default:
            break;
        }
        QxControls::QxControl::keyPressEvent(event);
    }

    QAccessible::Role accessibleRole() const override
    {
        // The ellipsis is a mark on the page rather than something to press.
        return m_kind == SlotKind::Ellipsis ? QAccessible::StaticText : QAccessible::Button;
    }
    void fillAccessibleState(QAccessible::State *state) const override
    {
        if (m_kind == SlotKind::Ellipsis) {
            state->readOnly = true;
            return;
        }
        // Being the page on screen is what a list of links marks as selected,
        // and it is the one thing an assistive tool has to be told here.
        state->selectable = true;
        state->selected = isCurrent();
    }

    /*! Do what the place says: go to its page, or a page either side of it. */
    void activate()
    {
        switch (m_kind) {
        case SlotKind::Page:
            m_owner->goTo(m_value);
            break;
        case SlotKind::Previous:
            m_owner->goTo(m_owner->page - 1);
            break;
        case SlotKind::Next:
            m_owner->goTo(m_owner->page + 1);
            break;
        case SlotKind::Ellipsis:
            break;
        }
    }
private:
    QColor numberInk() const
    {
        QColor ink = textColor();
        ink.setAlphaF(kNumberOpacity);
        return ink;
    }
    QColor chromeInk() const
    {
        QColor ink = textColor();
        ink.setAlphaF(kChromeOpacity);
        return ink;
    }

    MaterialPaginationPrivate *m_owner;
    SlotKind m_kind = SlotKind::Page;
    int m_value = 0;
};

void MaterialPaginationPrivate::rebuild()
{
    places = buildSlots(count, page, siblings, boundary);
    while (pool.count() < places.count()) {
        PageItem *item = new PageItem(this);
        // The item knows which place it is showing, so the press is handed back
        // to it rather than worked out again from the place's index.
        QObject::connect(item, &QxControls::QxControl::clicked, q_ptr, [item]() {
            item->activate();
        });
        pool.append(item);
    }
    for (int i = 0; i < pool.count(); ++i) {
        PageItem *item = pool.at(i);
        if (i < places.count()) {
            item->setSlot(places.at(i));
            item->show();
        } else {
            item->hide();
        }
    }
    updateStops();
    relayout();
}

void MaterialPaginationPrivate::relayout()
{
    const int top = (q_ptr->height() - kItemSize) / 2;
    for (int i = 0; i < places.count() && i < pool.count(); ++i) {
        pool.at(i)->setGeometry(i * kItemSize, top, kItemSize, kItemSize);
    }
}

void MaterialPaginationPrivate::updateStops()
{
    for (int i = 0; i < pool.count(); ++i) {
        // One stop on the way through the keyboard, on the page in force: a
        // paginator is walked, not tabbed through.
        const PageItem *item = pool.at(i);
        const bool isStop = i < places.count() && item->isCurrent();
        pool.at(i)->setFocusPolicy(isStop ? Qt::StrongFocus : Qt::NoFocus);
    }
}

void MaterialPaginationPrivate::goTo(int target)
{
    if (target < 1 || target > count || target == page) {
        return;
    }
    page = target;
    // The keyboard is on a place in the row, and that place may not be the one
    // it was on: the stop follows the page rather than being lost with the item.
    const bool walked = q_ptr->focusWidget() != Q_NULLPTR;
    rebuild();
    if (walked) {
        PageItem *item = current();
        if (item) {
            item->setFocus();
        }
    }
    Q_EMIT q_ptr->pageChanged(page);
}

PageItem *MaterialPaginationPrivate::current() const
{
    for (int i = 0; i < places.count() && i < pool.count(); ++i) {
        if (pool.at(i)->isCurrent()) {
            return pool.at(i);
        }
    }
    return Q_NULLPTR;
}

MaterialPagination::MaterialPagination(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new MaterialPaginationPrivate)
{
    QX_SET_PRIVATE(d_ptr);
    // The row is as wide as its places and no wider: it is navigation at the
    // foot of a page, and stretching it would only move its places apart.
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    d_ptr->rebuild();
}

MaterialPagination::~MaterialPagination()
{
    QX_FINI_PRIVATE();
}

int MaterialPagination::count() const
{
    return d_ptr->count;
}

int MaterialPagination::page() const
{
    return d_ptr->page;
}

int MaterialPagination::siblingCount() const
{
    return d_ptr->siblings;
}

int MaterialPagination::boundaryCount() const
{
    return d_ptr->boundary;
}

QList<int> MaterialPagination::items() const
{
    QList<int> numbers;
    for (const Slot &slot : d_ptr->places) {
        if (slot.kind == SlotKind::Page) {
            numbers.append(slot.value);
        } else if (slot.kind == SlotKind::Ellipsis) {
            numbers.append(-1);
        }
    }
    return numbers;
}

int MaterialPagination::itemCount() const
{
    return items().count();
}

QRect MaterialPagination::itemRect(int index) const
{
    // The arrows take the first and the last place of the row, so the pages
    // stand between them and neither of the two is a page's rectangle.
    const int at = index + 1;
    if (index < 0 || at >= d_ptr->places.count() - 1 || at >= d_ptr->pool.count()) {
        return QRect();
    }
    return d_ptr->pool.at(at)->geometry();
}

QRect MaterialPagination::previousRect() const
{
    return d_ptr->pool.isEmpty() ? QRect() : d_ptr->pool.first()->geometry();
}

QRect MaterialPagination::nextRect() const
{
    const int last = d_ptr->places.count() - 1;
    if (last < 0 || last >= d_ptr->pool.count()) {
        return QRect();
    }
    return d_ptr->pool.at(last)->geometry();
}

QSize MaterialPagination::sizeHint() const
{
    return QSize(d_ptr->places.count() * kItemSize, kItemSize);
}

QSize MaterialPagination::minimumSizeHint() const
{
    // Every place is needed to say what the bar says, so there is no narrower
    // bar: the two answers are the same one.
    return sizeHint();
}

void MaterialPagination::setCount(int count)
{
    const int wanted = qMax(1, count);
    if (d_ptr->count == wanted) {
        return;
    }
    d_ptr->count = wanted;
    const int previous = d_ptr->page;
    // The page it was on may not be there any more. That is not a page anyone
    // asked for, so it is not refused: it is given up for the last one there is.
    d_ptr->page = qBound(1, d_ptr->page, d_ptr->count);
    d_ptr->rebuild();
    updateGeometry();
    update();
    Q_EMIT countChanged(wanted);
    if (d_ptr->page != previous) {
        Q_EMIT pageChanged(d_ptr->page);
    }
}

void MaterialPagination::setPage(int page)
{
    d_ptr->goTo(page);
}

void MaterialPagination::setSiblingCount(int count)
{
    const int wanted = qMax(0, count);
    if (d_ptr->siblings == wanted) {
        return;
    }
    d_ptr->siblings = wanted;
    d_ptr->rebuild();
    updateGeometry();
    update();
}

void MaterialPagination::setBoundaryCount(int count)
{
    const int wanted = qMax(0, count);
    if (d_ptr->boundary == wanted) {
        return;
    }
    d_ptr->boundary = wanted;
    d_ptr->rebuild();
    updateGeometry();
    update();
}

void MaterialPagination::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    d_ptr->relayout();
}

void MaterialPagination::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::FontChange || event->type() == QEvent::PaletteChange) {
        d_ptr->relayout();
        update();
    }
}

QX_MATERIAL_END_NAMESPACE
