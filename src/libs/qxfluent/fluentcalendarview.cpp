/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentcalendarview.h"

#include "fluenttokens.h"

#include "qxcontrols/qxstatelayer.h"

#include "qxtheme/qxthememanager.h"

#include <QtCore/QEvent>
#include <QtCore/QStringList>
#include <QtGui/QAccessible>
#include <QAccessibleWidget>
#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtWidgets/QScrollBar>

#include <algorithm>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// The month: a cell of forty square, six weeks of them, a name above the days
// of the week. A month stands clear of the next by a gap, so the run of months
// reads as months rather than as one long grid.
constexpr int kCell = 40;
constexpr int kColumns = 7;
constexpr int kWeeks = 6;
constexpr int kMonthHead = 40;
constexpr int kWeekdays = 28;
constexpr int kBlockHeight = kMonthHead + kWeekdays + kWeeks * kCell;
constexpr int kBlockGap = 28;
constexpr int kBlockPitch = kBlockHeight + kBlockGap;
constexpr int kGridWidth = kColumns * kCell;
constexpr int kSide = 8;
constexpr int kBarWidth = 14;

// The year and the decade: the twelve months three by four, the ten years two
// by five, both under a head that says what they are and steps them.
constexpr int kZoomHead = 44;
constexpr int kNavZone = 44;
constexpr int kMonthCellWidth = 88;
constexpr int kMonthCellHeight = 72;
constexpr int kMonthColumns = 3;
constexpr int kYearCellWidth = 128;
constexpr int kYearCellHeight = 56;
constexpr int kYearColumns = 2;

// How far the run of months reaches either side of today, and what the view
// asks for: high enough that a day a decade off is reachable by scrolling,
// low enough that the scroll bar keeps meaning something.
constexpr int kYearsEachSide = 10;
constexpr int kHintWidth = 300;
constexpr int kHintHeight = 420;
constexpr int kLeastWidth = 240;
constexpr int kLeastHeight = 240;

constexpr qreal kRowRadius = 4.0;
constexpr qreal kHoverOpacity = 0.06;
constexpr qreal kRunOpacity = 0.12;
constexpr qreal kMutedOpacity = 0.4;
constexpr qreal kDisabledOpacity = 0.38;

QPoint eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position().toPoint();
#else
    return event->pos();
#endif
}

/*! Whether \a left and \a right are joined on the same line of a month: a run
 *  of chosen days is drawn joined, so a day has to know about its neighbours. */
bool joined(QDate left, QDate right)
{
    return left.isValid() && right.isValid() && left.addDays(1) == right && left.dayOfWeek() != Qt::Sunday;
}

/*! The two marks either side of the head that step the pulled-back views: the
 *  same pair a month's own name carries in a date picker. */
void drawChevrons(QPainter *painter, const QRect &content, const QColor &ink, qreal opacity)
{
    painter->save();
    painter->setOpacity(opacity);
    painter->setPen(QPen(ink, 1.4));
    painter->setBrush(Qt::NoBrush);
    painter->setRenderHint(QPainter::Antialiasing, true);
    const QPointF left(content.left() + kNavZone / 2.0, kZoomHead / 2.0);
    const QPointF right(content.right() - kNavZone / 2.0, kZoomHead / 2.0);
    QPointF before[3] = {left + QPointF(3.0, -5.0), left + QPointF(-3.0, 0.0), left + QPointF(3.0, 5.0)};
    QPointF after[3] = {right + QPointF(-3.0, -5.0), right + QPointF(3.0, 0.0), right + QPointF(-3.0, 5.0)};
    painter->drawPolyline(before, 3);
    painter->drawPolyline(after, 3);
    painter->restore();
}

/*!
 * What the calendar says to an assistive tool: that it is a table of days -
 * which is what it is, seven to a line - under a name, with the day in force
 * as its value. Nothing here is a button of its own; the days are the cells.
 */
class CalendarViewAccessible : public QAccessibleWidget
{
public:
    explicit CalendarViewAccessible(FluentCalendarView *view)
        : QAccessibleWidget(view, QAccessible::Table, QStringLiteral("Calendar"))
    {
    }

    QString text(QAccessible::Text type) const override
    {
        if (type == QAccessible::Name) {
            return QStringLiteral("Calendar");
        }
        if (type == QAccessible::Value) {
            const QDate date = static_cast<FluentCalendarView *>(widget())->selectedDate();
            return date.isValid() ? date.toString(Qt::ISODate) : QString();
        }
        return QAccessibleWidget::text(type);
    }

    QAccessible::State state() const override
    {
        QAccessible::State state = QAccessibleWidget::state();
        state.focusable = true;
        state.selectable = true;
        return state;
    }
};

QAccessibleInterface *calendarViewAccessible(const QString &, QObject *object)
{
    if (auto *view = qobject_cast<FluentCalendarView *>(object)) {
        return new CalendarViewAccessible(view);
    }
    return Q_NULLPTR;
}
}   // namespace

/*!
 * What the calendar keeps between paints: which of the three views it is in,
 * how many days may be chosen, the day the keyboard stands on, the days that
 * are chosen and the scroll bar that says how much more there is.
 *
 * The chosen days are kept sorted and without repeats, which is what every
 * drawing and every signal wants them to be; the first of them is the day in
 * force, and for a range the first and the last are its two ends.
 */
class FluentCalendarViewPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentCalendarView)

    FluentCalendarView::DisplayMode mode = FluentCalendarView::DisplayMode::Month;
    FluentCalendarView::SelectionMode selection = FluentCalendarView::SelectionMode::Single;
    QDate today = QDate::currentDate();
    QDate anchor = QDate(QDate::currentDate().year(), QDate::currentDate().month(), 1);
    QDate current = QDate::currentDate();
    QDate hovered;
    QList<QDate> chosen;
    QScrollBar *bar = Q_NULLPTR;
    // The view is built at a size it will not live at, so the month it says it
    // is showing cannot be scrolled to until there is a room to scroll it in.
    // The first real layout is where that happens.
    bool placed = false;

    QDate firstMonth() const
    {
        return QDate(today.year() - kYearsEachSide, 1, 1);
    }
    QDate lastMonth() const
    {
        return QDate(today.year() + kYearsEachSide, 12, 1);
    }
    int monthCount() const
    {
        return (lastMonth().year() - firstMonth().year()) * 12 + 11;
    }
    int decadeStart() const
    {
        return anchor.year() - anchor.year() % 10;
    }
    bool holds(QDate date) const
    {
        return chosen.contains(date);
    }
    bool runs(QDate date) const
    {
        return selection == FluentCalendarView::SelectionMode::Range && chosen.size() == 2 && date > chosen.first() &&
               date < chosen.last();
    }
};

FluentCalendarView::FluentCalendarView(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new FluentCalendarViewPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    d_ptr->bar = new QScrollBar(Qt::Vertical, this);
    d_ptr->bar->setCursor(Qt::ArrowCursor);
    connect(d_ptr->bar, &QScrollBar::valueChanged, this, [this]() {
        update();
    });
    updateBar();

    // The view answers as a table of days to an assistive tool, and there is
    // only one factory to install for the whole class.
    static const bool installed = []() {
        QAccessible::installFactory(calendarViewAccessible);
        return true;
    }();
    Q_UNUSED(installed)
}

FluentCalendarView::~FluentCalendarView(){QX_FINI_PRIVATE()}

FluentCalendarView::DisplayMode FluentCalendarView::displayMode() const
{
    return d_ptr->mode;
}

void FluentCalendarView::setDisplayMode(DisplayMode mode)
{
    if (d_ptr->mode == mode) {
        return;
    }
    d_ptr->mode = mode;
    // Coming back down, the view lands on the month it was told to show - the
    // one whose name was pressed on the way out, not wherever the keyboard
    // happened to be standing. Going back up, the year is the one that month
    // belongs to, which is what the view was showing.
    if (mode == DisplayMode::Month) {
        d_ptr->bar->setValue(monthIndex(d_ptr->anchor) * kBlockPitch);
    }
    updateBar();
    update();
    Q_EMIT displayModeChanged(mode);
}

QDate FluentCalendarView::visibleMonth() const
{
    return d_ptr->anchor;
}

void FluentCalendarView::setVisibleMonth(QDate month)
{
    if (!month.isValid()) {
        return;
    }
    const QDate first(month.year(), month.month(), 1);
    if (d_ptr->anchor == first) {
        return;
    }
    d_ptr->anchor = first;
    if (d_ptr->mode == DisplayMode::Month) {
        d_ptr->bar->setValue(monthIndex(first) * kBlockPitch);
    }
    update();
}

FluentCalendarView::SelectionMode FluentCalendarView::selectionMode() const
{
    return d_ptr->selection;
}

void FluentCalendarView::setSelectionMode(SelectionMode mode)
{
    if (d_ptr->selection == mode) {
        return;
    }
    d_ptr->selection = mode;
    // A single day is all the first two modes can hold, and a range needs its
    // two ends: what was chosen under the old mode is cut down to what the new
    // one means, rather than left saying something it no longer can.
    if (mode == SelectionMode::Single && d_ptr->chosen.size() > 1) {
        d_ptr->chosen = d_ptr->chosen.mid(0, 1);
    } else if (mode == SelectionMode::Range && d_ptr->chosen.size() > 2) {
        d_ptr->chosen = QList<QDate>() << d_ptr->chosen.first() << d_ptr->chosen.last();
    }
    update();
    Q_EMIT selectionModeChanged(mode);
    Q_EMIT selectionChanged();
}

QDate FluentCalendarView::selectedDate() const
{
    return d_ptr->chosen.isEmpty() ? QDate() : d_ptr->chosen.first();
}

void FluentCalendarView::setSelectedDate(QDate date)
{
    if (!date.isValid()) {
        clearSelection();
        return;
    }
    const QDate before = selectedDate();
    d_ptr->chosen = QList<QDate>() << date;
    d_ptr->current = date;
    update();
    if (before != date) {
        Q_EMIT selectedDateChanged(date);
    }
    Q_EMIT selectionChanged();
}

QList<QDate> FluentCalendarView::selectedDates() const
{
    return d_ptr->chosen;
}

void FluentCalendarView::setSelectedDates(const QList<QDate> &dates)
{
    QList<QDate> sorted;
    for (const QDate &date : dates) {
        if (date.isValid() && !sorted.contains(date)) {
            sorted.append(date);
        }
    }
    std::sort(sorted.begin(), sorted.end());
    if (d_ptr->selection != SelectionMode::Multiple) {
        sorted = sorted.mid(0, d_ptr->selection == SelectionMode::Range ? 2 : 1);
    }
    if (sorted == d_ptr->chosen) {
        return;
    }
    const QDate before = selectedDate();
    d_ptr->chosen = sorted;
    if (!sorted.isEmpty()) {
        d_ptr->current = sorted.first();
    }
    update();
    if (before != selectedDate()) {
        Q_EMIT selectedDateChanged(selectedDate());
    }
    Q_EMIT selectionChanged();
}

void FluentCalendarView::clearSelection()
{
    if (d_ptr->chosen.isEmpty()) {
        return;
    }
    d_ptr->chosen.clear();
    update();
    Q_EMIT selectedDateChanged(QDate());
    Q_EMIT selectionChanged();
}

QDate FluentCalendarView::today() const
{
    return d_ptr->today;
}

QDate FluentCalendarView::currentDate() const
{
    return d_ptr->current;
}

void FluentCalendarView::setCurrentDate(QDate date)
{
    if (!date.isValid() || d_ptr->current == date) {
        return;
    }
    d_ptr->current = date;
    scrollToDate(date);
    update();
}

QRect FluentCalendarView::contentRect() const
{
    const int room = d_ptr->mode == DisplayMode::Month ? width() - kBarWidth : width();
    return QRect(0, 0, room, height());
}

int FluentCalendarView::monthIndex(QDate month) const
{
    const QDate first = d_ptr->firstMonth();
    if (!month.isValid() || month < first || month > d_ptr->lastMonth()) {
        return -1;
    }
    return (month.year() - first.year()) * 12 + month.month() - 1;
}

QDate FluentCalendarView::monthAt(int index) const
{
    if (index < 0 || index >= d_ptr->monthCount()) {
        return QDate();
    }
    return d_ptr->firstMonth().addMonths(index);
}

QRect FluentCalendarView::dayRect(QDate date) const
{
    if (d_ptr->mode != DisplayMode::Month || !date.isValid()) {
        return QRect();
    }
    const QDate month(date.year(), date.month(), 1);
    const int index = monthIndex(month);
    if (index < 0) {
        return QRect();
    }
    const int offset = month.dayOfWeek() - 1;
    const int cell = offset + date.day() - 1;
    if (cell < 0 || cell >= kColumns * kWeeks) {
        return QRect();
    }
    const int left = contentRect().left() + qMax(kSide, (contentRect().width() - kGridWidth) / 2);
    const int top = index * kBlockPitch + kMonthHead + kWeekdays + (cell / kColumns) * kCell - d_ptr->bar->value();
    return QRect(left + (cell % kColumns) * kCell, top, kCell, kCell);
}

QRect FluentCalendarView::monthRect(QDate month) const
{
    if (d_ptr->mode != DisplayMode::Year || !month.isValid() || month.year() != d_ptr->anchor.year()) {
        return QRect();
    }
    const QRect content = contentRect();
    const int gridWidth = kMonthColumns * kMonthCellWidth;
    const int gridHeight = 4 * kMonthCellHeight;
    const int left = content.left() + qMax(0, (content.width() - gridWidth) / 2);
    const int top = kZoomHead + qMax(0, (content.height() - kZoomHead - gridHeight) / 2);
    const int slot = month.month() - 1;
    return QRect(left + (slot % kMonthColumns) * kMonthCellWidth, top + (slot / kMonthColumns) * kMonthCellHeight,
                 kMonthCellWidth, kMonthCellHeight);
}

QRect FluentCalendarView::yearRect(int year) const
{
    const int first = d_ptr->decadeStart();
    if (d_ptr->mode != DisplayMode::Decade || year < first || year > first + 9) {
        return QRect();
    }
    const QRect content = contentRect();
    const int gridWidth = kYearColumns * kYearCellWidth;
    const int gridHeight = 5 * kYearCellHeight;
    const int left = content.left() + qMax(0, (content.width() - gridWidth) / 2);
    const int top = kZoomHead + qMax(0, (content.height() - kZoomHead - gridHeight) / 2);
    const int slot = year - first;
    return QRect(left + (slot % kYearColumns) * kYearCellWidth, top + (slot / kYearColumns) * kYearCellHeight,
                 kYearCellWidth, kYearCellHeight);
}

QSize FluentCalendarView::sizeHint() const
{
    return QSize(kHintWidth, kHintHeight);
}

QSize FluentCalendarView::minimumSizeHint() const
{
    return QSize(kLeastWidth, kLeastHeight);
}

QColor FluentCalendarView::ink() const
{
    return palette().color(QPalette::WindowText);
}

void FluentCalendarView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.fillRect(rect(), palette().color(QPalette::Window));

    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
    const QColor text = this->ink();
    const qreal faded = isEnabled() ? 1.0 : kDisabledOpacity;

    switch (d_ptr->mode) {
    case DisplayMode::Month: {
        const QRect content = contentRect();
        const int first = qMax(0, d_ptr->bar->value() / kBlockPitch);
        const int last = qMin(d_ptr->monthCount() - 1, (d_ptr->bar->value() + content.height()) / kBlockPitch);
        for (int index = first; index <= last; ++index) {
            const QDate month = monthAt(index);
            const int top = index * kBlockPitch - d_ptr->bar->value();
            const int left = content.left() + qMax(kSide, (content.width() - kGridWidth) / 2);

            // The month by its name - and the name is the way out of the
            // month, so it is drawn as something to press.
            painter.save();
            painter.setOpacity(faded);
            painter.setFont(FluentType::font(FluentType::Ramp::Body, true));
            painter.setPen(text);
            painter.drawText(QRect(left, top, kGridWidth, kMonthHead), Qt::AlignCenter,
                             month.toString(QStringLiteral("MMMM yyyy")));

            painter.setFont(FluentType::font(FluentType::Ramp::Caption));
            painter.setOpacity(faded * 0.6);
            const QStringList names = {QStringLiteral("M"), QStringLiteral("T"), QStringLiteral("W"),
                                       QStringLiteral("T"), QStringLiteral("F"), QStringLiteral("S"),
                                       QStringLiteral("S")};
            for (int column = 0; column < kColumns; ++column) {
                painter.drawText(QRect(left + column * kCell, top + kMonthHead, kCell, kWeekdays), Qt::AlignCenter,
                                 names.at(column));
            }
            painter.restore();

            // The days: the first of the month in the column the week puts it
            // in, and the ones before and after it that fill the weeks thinned
            // to a whisper.
            QDate walker = month.addDays(1 - month.dayOfWeek());
            for (int slot = 0; slot < int(kWeeks) * kColumns; ++slot) {
                const int row = slot / kColumns;
                const int column = slot % kColumns;
                const QRect cell(left + column * kCell, top + kMonthHead + kWeekdays + row * kCell, kCell, kCell);
                const bool inside = walker.month() == month.month();
                const bool chosen = d_ptr->holds(walker);
                const bool running = d_ptr->runs(walker);
                const bool hovered = walker == d_ptr->hovered;
                const bool isToday = walker == d_ptr->today;
                const bool walking = walker == d_ptr->current;

                painter.save();
                if (running) {
                    // A run of chosen days is drawn as a run: the ends round,
                    // the middle reaches across to its neighbours.
                    const bool before = joined(walker.addDays(-1), walker) && d_ptr->runs(walker.addDays(-1));
                    const bool after = joined(walker, walker.addDays(1)) && d_ptr->runs(walker.addDays(1));
                    const QRectF bar(cell.left() + (before ? 0 : 2), cell.top() + 2,
                                     cell.width() - (before ? 0 : 2) - (after ? 0 : 2), cell.height() - 4);
                    painter.setOpacity(faded);
                    if (before && after) {
                        painter.fillRect(bar, QxControls::QxStateLayer::layered(palette().color(QPalette::Window),
                                                                                accent, kRunOpacity));
                    } else {
                        QPainterPath wash;
                        wash.addRoundedRect(bar, kRowRadius, kRowRadius);
                        painter.fillPath(wash, QxControls::QxStateLayer::layered(palette().color(QPalette::Window),
                                                                                 accent, kRunOpacity));
                    }
                }
                if (chosen) {
                    painter.setOpacity(faded);
                    QPainterPath wash;
                    wash.addRoundedRect(QRectF(cell).adjusted(2.0, 2.0, -2.0, -2.0), kRowRadius, kRowRadius);
                    painter.fillPath(wash, accent);
                } else if (hovered && isEnabled()) {
                    painter.setOpacity(faded);
                    QPainterPath wash;
                    wash.addRoundedRect(QRectF(cell).adjusted(2.0, 2.0, -2.0, -2.0), kRowRadius, kRowRadius);
                    painter.fillPath(wash, QxControls::QxStateLayer::layered(palette().color(QPalette::Window), text,
                                                                             kHoverOpacity));
                }

                painter.setFont(FluentType::font(FluentType::Ramp::Body));
                painter.setOpacity(inside ? faded : faded * kMutedOpacity);
                painter.setPen(chosen ? QColor(Qt::white) : text);
                painter.drawText(cell, Qt::AlignCenter, QString::number(walker.day()));

                // Today keeps its own ring, whatever else it is.
                if (isToday && !chosen) {
                    painter.setOpacity(faded);
                    painter.setPen(QPen(accent, 1.4));
                    painter.setBrush(Qt::NoBrush);
                    painter.setRenderHint(QPainter::Antialiasing, true);
                    painter.drawEllipse(QRectF(cell).adjusted(3.0, 3.0, -3.0, -3.0));
                    painter.setRenderHint(QPainter::Antialiasing, false);
                }
                // And the keyboard's day carries the ring that says where the
                // keyboard is, which is not the same as what it has chosen.
                if (walking && hasFocus() && !chosen) {
                    painter.setOpacity(faded);
                    painter.setPen(QPen(accent, 1.0));
                    painter.setBrush(Qt::NoBrush);
                    painter.setRenderHint(QPainter::Antialiasing, true);
                    QPainterPath ring;
                    ring.addRoundedRect(QRectF(cell).adjusted(1.0, 1.0, -1.0, -1.0), kRowRadius, kRowRadius);
                    painter.drawPath(ring);
                    painter.setRenderHint(QPainter::Antialiasing, false);
                }
                painter.restore();
                walker = walker.addDays(1);
            }

            // The gap under the last month would only be blank room.
            if (index < d_ptr->monthCount() - 1) {
                painter.save();
                painter.setPen(QPen(text, 1.0));
                painter.setOpacity(faded * 0.12);
                painter.drawLine(QPoint(left, top + kBlockHeight + kBlockGap / 2),
                                 QPoint(left + kGridWidth, top + kBlockHeight + kBlockGap / 2));
                painter.restore();
            }
        }
        break;
    }
    case DisplayMode::Year: {
        const QRect content = contentRect();
        painter.save();
        painter.setOpacity(faded);
        painter.setFont(FluentType::font(FluentType::Ramp::Subtitle, true));
        painter.setPen(text);
        painter.drawText(QRect(content.left() + kNavZone, 0, content.width() - 2 * kNavZone, kZoomHead),
                         Qt::AlignCenter, QString::number(d_ptr->anchor.year()));
        drawChevrons(&painter, content, text, faded);
        painter.restore();

        const QDate todayMonth(d_ptr->today.year(), d_ptr->today.month(), 1);
        const QDate chosenMonth =
            selectedDate().isValid() ? QDate(selectedDate().year(), selectedDate().month(), 1) : QDate();
        for (int slot = 0; slot < 12; ++slot) {
            const QDate month(d_ptr->anchor.year(), slot + 1, 1);
            const QRect cell = monthRect(month);
            const bool hovered = month == d_ptr->hovered;
            const bool chosen = month == chosenMonth;
            const bool inside = QDate(d_ptr->current.year(), d_ptr->current.month(), 1) == month;

            painter.save();
            if (chosen) {
                painter.setOpacity(faded);
                QPainterPath wash;
                wash.addRoundedRect(QRectF(cell).adjusted(3.0, 3.0, -3.0, -3.0), kRowRadius, kRowRadius);
                painter.fillPath(wash, accent);
            } else if (hovered && isEnabled()) {
                painter.setOpacity(faded);
                QPainterPath wash;
                wash.addRoundedRect(QRectF(cell).adjusted(3.0, 3.0, -3.0, -3.0), kRowRadius, kRowRadius);
                painter.fillPath(
                    wash, QxControls::QxStateLayer::layered(palette().color(QPalette::Window), text, kHoverOpacity));
            }
            painter.setOpacity(faded);
            painter.setFont(FluentType::font(FluentType::Ramp::Body));
            painter.setPen(chosen ? QColor(Qt::white) : text);
            painter.drawText(cell, Qt::AlignCenter, month.toString(QStringLiteral("MMM")));
            if (month == todayMonth && !chosen) {
                painter.setPen(QPen(accent, 1.4));
                painter.setBrush(Qt::NoBrush);
                painter.setRenderHint(QPainter::Antialiasing, true);
                painter.drawEllipse(QRectF(cell).adjusted(10.0, 10.0, -10.0, -10.0));
                painter.setRenderHint(QPainter::Antialiasing, false);
            }
            if (inside && hasFocus()) {
                painter.setPen(QPen(accent, 1.0));
                painter.setBrush(Qt::NoBrush);
                painter.setRenderHint(QPainter::Antialiasing, true);
                QPainterPath ring;
                ring.addRoundedRect(QRectF(cell).adjusted(2.0, 2.0, -2.0, -2.0), kRowRadius, kRowRadius);
                painter.drawPath(ring);
                painter.setRenderHint(QPainter::Antialiasing, false);
            }
            painter.restore();
        }
        break;
    }
    case DisplayMode::Decade: {
        const QRect content = contentRect();
        const int first = d_ptr->decadeStart();
        painter.save();
        painter.setOpacity(faded);
        painter.setFont(FluentType::font(FluentType::Ramp::Subtitle, true));
        painter.setPen(text);
        painter.drawText(QRect(content.left() + kNavZone, 0, content.width() - 2 * kNavZone, kZoomHead),
                         Qt::AlignCenter, QStringLiteral("%1 - %2").arg(first).arg(first + 9));
        drawChevrons(&painter, content, text, faded);
        painter.restore();

        for (int slot = 0; slot < 10; ++slot) {
            const int year = first + slot;
            const QRect cell = yearRect(year);
            const bool hovered = year == d_ptr->hovered.year();
            const bool chosen = year == d_ptr->anchor.year();
            const bool inside = year == d_ptr->current.year();

            painter.save();
            if (chosen) {
                painter.setOpacity(faded);
                QPainterPath wash;
                wash.addRoundedRect(QRectF(cell).adjusted(3.0, 3.0, -3.0, -3.0), kRowRadius, kRowRadius);
                painter.fillPath(wash, accent);
            } else if (hovered && isEnabled()) {
                painter.setOpacity(faded);
                QPainterPath wash;
                wash.addRoundedRect(QRectF(cell).adjusted(3.0, 3.0, -3.0, -3.0), kRowRadius, kRowRadius);
                painter.fillPath(
                    wash, QxControls::QxStateLayer::layered(palette().color(QPalette::Window), text, kHoverOpacity));
            }
            painter.setOpacity(faded);
            painter.setFont(FluentType::font(FluentType::Ramp::Body));
            painter.setPen(chosen ? QColor(Qt::white) : text);
            painter.drawText(cell, Qt::AlignCenter, QString::number(year));
            if (year == d_ptr->today.year() && !chosen) {
                painter.setPen(QPen(accent, 1.4));
                painter.setBrush(Qt::NoBrush);
                painter.setRenderHint(QPainter::Antialiasing, true);
                painter.drawEllipse(QRectF(cell).adjusted(14.0, 8.0, -14.0, -8.0));
                painter.setRenderHint(QPainter::Antialiasing, false);
            }
            if (inside && hasFocus()) {
                painter.setPen(QPen(accent, 1.0));
                painter.setBrush(Qt::NoBrush);
                painter.setRenderHint(QPainter::Antialiasing, true);
                QPainterPath ring;
                ring.addRoundedRect(QRectF(cell).adjusted(2.0, 2.0, -2.0, -2.0), kRowRadius, kRowRadius);
                painter.drawPath(ring);
                painter.setRenderHint(QPainter::Antialiasing, false);
            }
            painter.restore();
        }
        break;
    }
    }
}

void FluentCalendarView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateBar();
    if (!d_ptr->placed) {
        d_ptr->placed = true;
        scrollToDate(d_ptr->anchor);
    }
}

void FluentCalendarView::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    switch (event->type()) {
    case QEvent::PaletteChange:
    case QEvent::FontChange:
    case QEvent::EnabledChange:
    case QEvent::StyleChange:
        update();
        break;
    default:
        break;
    }
}

void FluentCalendarView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        step(-1);
        event->accept();
        return;
    case Qt::Key_Right:
        step(1);
        event->accept();
        return;
    case Qt::Key_Up:
        step(d_ptr->mode == DisplayMode::Month ? -kColumns
                                               : (d_ptr->mode == DisplayMode::Year ? -kMonthColumns : -kYearColumns));
        event->accept();
        return;
    case Qt::Key_Down:
        step(d_ptr->mode == DisplayMode::Month ? kColumns
                                               : (d_ptr->mode == DisplayMode::Year ? kMonthColumns : kYearColumns));
        event->accept();
        return;
    case Qt::Key_PageUp:
        stepMonths(-1);
        event->accept();
        return;
    case Qt::Key_PageDown:
        stepMonths(1);
        event->accept();
        return;
    case Qt::Key_Home:
        setCurrentDate(QDate(d_ptr->current.year(), d_ptr->current.month(), 1));
        event->accept();
        return;
    case Qt::Key_End:
        setCurrentDate(QDate(d_ptr->current.year(), d_ptr->current.month(),
                             QDate(d_ptr->current.year(), d_ptr->current.month(), 1).daysInMonth()));
        event->accept();
        return;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        take(d_ptr->current);
        event->accept();
        return;
    case Qt::Key_Escape:
        zoomOut();
        event->accept();
        return;
    default:
        break;
    }
    QWidget::keyPressEvent(event);
}

void FluentCalendarView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        setFocus(Qt::MouseFocusReason);
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void FluentCalendarView::mouseMoveEvent(QMouseEvent *event)
{
    const QPoint pos = eventPos(event);
    QDate hovered;
    if (d_ptr->mode == DisplayMode::Month) {
        hovered = dateAt(pos);
    } else if (d_ptr->mode == DisplayMode::Year) {
        for (int slot = 0; slot < 12; ++slot) {
            const QDate month(d_ptr->anchor.year(), slot + 1, 1);
            if (monthRect(month).contains(pos)) {
                hovered = month;
                break;
            }
        }
    } else {
        const int first = d_ptr->decadeStart();
        for (int slot = 0; slot < 10; ++slot) {
            if (yearRect(first + slot).contains(pos)) {
                hovered = QDate(first + slot, 1, 1);
                break;
            }
        }
    }
    if (hovered != d_ptr->hovered) {
        d_ptr->hovered = hovered;
        update();
    }
    QWidget::mouseMoveEvent(event);
}

void FluentCalendarView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        QWidget::mouseReleaseEvent(event);
        return;
    }
    const QPoint pos = eventPos(event);
    switch (d_ptr->mode) {
    case DisplayMode::Month: {
        const QDate month = monthNameAt(pos);
        if (month.isValid()) {
            d_ptr->anchor = month;
            setDisplayMode(DisplayMode::Year);
            return;
        }
        const QDate date = dateAt(pos);
        if (date.isValid()) {
            choose(date);
        }
        return;
    }
    case DisplayMode::Year: {
        for (int slot = 0; slot < 12; ++slot) {
            const QDate month(d_ptr->anchor.year(), slot + 1, 1);
            if (monthRect(month).contains(pos)) {
                d_ptr->anchor = month;
                d_ptr->current = month;
                setDisplayMode(DisplayMode::Month);
                return;
            }
        }
        if (QRect(contentRect().left() + kNavZone, 0, contentRect().width() - 2 * kNavZone, kZoomHead).contains(pos)) {
            zoomOut();
        }
        return;
    }
    case DisplayMode::Decade: {
        const QDate year = yearAt(pos);
        if (year.isValid()) {
            d_ptr->anchor = QDate(year.year(), d_ptr->anchor.month(), 1);
            d_ptr->current = QDate(year.year(), d_ptr->current.month(), qMin(d_ptr->current.day(), 28));
            setDisplayMode(DisplayMode::Year);
        }
        return;
    }
    }
}

void FluentCalendarView::wheelEvent(QWheelEvent *event)
{
    if (d_ptr->mode != DisplayMode::Month || d_ptr->bar->maximum() == 0) {
        event->ignore();
        return;
    }
    const int delta = event->angleDelta().y();
    d_ptr->bar->setValue(d_ptr->bar->value() - (delta / 120) * kCell * 3);
    event->accept();
}

void FluentCalendarView::zoomOut()
{
    if (d_ptr->mode == DisplayMode::Month) {
        d_ptr->anchor = QDate(d_ptr->current.year(), d_ptr->current.month(), 1);
        setDisplayMode(DisplayMode::Year);
    } else if (d_ptr->mode == DisplayMode::Year) {
        setDisplayMode(DisplayMode::Decade);
    }
}

void FluentCalendarView::step(int units)
{
    if (d_ptr->mode == DisplayMode::Month) {
        setCurrentDate(d_ptr->current.addDays(units));
    } else if (d_ptr->mode == DisplayMode::Year) {
        setCurrentDate(d_ptr->current.addMonths(units));
        d_ptr->anchor = QDate(d_ptr->current.year(), d_ptr->current.month(), 1);
        update();
    } else {
        setCurrentDate(QDate(d_ptr->current.year() + units, d_ptr->current.month(), qMin(d_ptr->current.day(), 28)));
        d_ptr->anchor = QDate(d_ptr->current.year(), d_ptr->current.month(), 1);
        update();
    }
}

void FluentCalendarView::stepMonths(int months)
{
    if (d_ptr->mode == DisplayMode::Month) {
        setCurrentDate(d_ptr->current.addMonths(months));
    } else if (d_ptr->mode == DisplayMode::Year) {
        d_ptr->anchor = d_ptr->anchor.addYears(months);
        d_ptr->current = d_ptr->current.addYears(months);
        update();
    } else {
        d_ptr->anchor = d_ptr->anchor.addYears(months * 10);
        d_ptr->current = d_ptr->current.addYears(months * 10);
        update();
    }
}

void FluentCalendarView::choose(QDate date)
{
    const QDate before = selectedDate();
    switch (d_ptr->selection) {
    case SelectionMode::Single:
        d_ptr->chosen = QList<QDate>() << date;
        break;
    case SelectionMode::Multiple:
        if (d_ptr->chosen.contains(date)) {
            d_ptr->chosen.removeAll(date);
        } else {
            d_ptr->chosen.append(date);
            std::sort(d_ptr->chosen.begin(), d_ptr->chosen.end());
        }
        break;
    case SelectionMode::Range:
        // The first press sets one end, the second the other, and a third
        // starts again: a range of one end is not a range, and pressing a
        // third time to move an end would leave the choice in a state no one
        // asked for.
        if (d_ptr->chosen.size() >= 2) {
            d_ptr->chosen = QList<QDate>();
        }
        d_ptr->chosen.append(date);
        std::sort(d_ptr->chosen.begin(), d_ptr->chosen.end());
        break;
    }
    d_ptr->current = date;
    scrollToDate(date);
    update();
    if (before != selectedDate()) {
        Q_EMIT selectedDateChanged(selectedDate());
    }
    Q_EMIT selectionChanged();
    Q_EMIT dateClicked(date);
}

void FluentCalendarView::take(QDate date)
{
    if (!date.isValid()) {
        return;
    }
    if (d_ptr->mode == DisplayMode::Month) {
        choose(date);
        return;
    }
    // In the two pulled-back views the keyboard is walking months and years,
    // and taking one is how it goes back down a level.
    if (d_ptr->mode == DisplayMode::Year) {
        d_ptr->anchor = QDate(date.year(), date.month(), 1);
        setDisplayMode(DisplayMode::Month);
    } else {
        d_ptr->anchor = QDate(date.year(), d_ptr->anchor.month(), 1);
        setDisplayMode(DisplayMode::Year);
    }
}

void FluentCalendarView::scrollToDate(QDate date)
{
    if (d_ptr->mode != DisplayMode::Month || !date.isValid()) {
        return;
    }
    const int index = monthIndex(QDate(date.year(), date.month(), 1));
    if (index < 0) {
        return;
    }
    const int top = index * kBlockPitch;
    const int room = contentRect().height();
    if (top < d_ptr->bar->value()) {
        d_ptr->bar->setValue(top);
    } else if (top + kBlockHeight > d_ptr->bar->value() + room) {
        d_ptr->bar->setValue(qMin(top + kBlockHeight - room, d_ptr->bar->maximum()));
    }
}

QDate FluentCalendarView::dateAt(const QPoint &pos) const
{
    if (d_ptr->mode != DisplayMode::Month) {
        return QDate();
    }
    const QRect content = contentRect();
    const int left = content.left() + qMax(kSide, (content.width() - kGridWidth) / 2);
    if (pos.x() < left || pos.x() >= left + kGridWidth) {
        return QDate();
    }
    const int y = pos.y() + d_ptr->bar->value();
    const int index = y / kBlockPitch;
    const QDate month = monthAt(index);
    if (!month.isValid()) {
        return QDate();
    }
    const int within = y % kBlockPitch;
    const int gridTop = kMonthHead + kWeekdays;
    if (within < gridTop) {
        return QDate();
    }
    const int row = (within - gridTop) / kCell;
    const int column = (pos.x() - left) / kCell;
    if (row < 0 || row >= kWeeks || column < 0 || column >= kColumns) {
        return QDate();
    }
    return month.addDays(1 - month.dayOfWeek() + row * kColumns + column);
}

QDate FluentCalendarView::monthNameAt(const QPoint &pos) const
{
    if (d_ptr->mode != DisplayMode::Month) {
        return QDate();
    }
    const QRect content = contentRect();
    const int left = content.left() + qMax(kSide, (content.width() - kGridWidth) / 2);
    if (pos.x() < left || pos.x() >= left + kGridWidth) {
        return QDate();
    }
    const int y = pos.y() + d_ptr->bar->value();
    const int index = y / kBlockPitch;
    const QDate month = monthAt(index);
    if (!month.isValid()) {
        return QDate();
    }
    return (y % kBlockPitch) < kMonthHead ? month : QDate();
}

QDate FluentCalendarView::yearAt(const QPoint &pos) const
{
    const int first = d_ptr->decadeStart();
    for (int slot = 0; slot < 10; ++slot) {
        if (yearRect(first + slot).contains(pos)) {
            return QDate(first + slot, 1, 1);
        }
    }
    return QDate();
}

void FluentCalendarView::updateBar()
{
    if (d_ptr->mode == DisplayMode::Month) {
        const int room = contentRect().height();
        const int total = d_ptr->monthCount() * kBlockPitch - kBlockGap;
        d_ptr->bar->setRange(0, qMax(0, total - room));
        d_ptr->bar->setPageStep(qMax(room, 1));
        d_ptr->bar->setSingleStep(kCell);
        d_ptr->bar->setGeometry(width() - kBarWidth, 0, kBarWidth, height());
        d_ptr->bar->setVisible(d_ptr->bar->maximum() > 0);
    } else {
        // The two pulled-back views are smaller than the room they are given,
        // so there is nothing to scroll and no bar to say so.
        d_ptr->bar->setRange(0, 0);
        d_ptr->bar->setVisible(false);
    }
}

QX_FLUENT_END_NAMESPACE
