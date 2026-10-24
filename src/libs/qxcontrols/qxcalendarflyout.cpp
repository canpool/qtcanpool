/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxcalendarflyout.h"

#include "qxstatelayer.h"
#include "qxtheme/qxthememanager.h"

#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

QX_CONTROLS_BEGIN_NAMESPACE

namespace
{
// The calendar's measures: a cell of forty square, six weeks of them, a
// head that names the month above a row that names the days.
constexpr qreal kCell = 40.0;
constexpr qreal kWeeks = 6.0;
constexpr qreal kHeadHeight = 48.0;
constexpr qreal kDayNamesHeight = 32.0;
constexpr int kColumns = 7;
constexpr qreal kNavZone = 48.0;
constexpr qreal kRowRadius = 4.0;
constexpr qreal kHoverOpacity = 0.06;
constexpr qreal kMutedOpacity = 0.4;
constexpr qreal kSelectedOpacity = 0.12;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}
}   // namespace

class QxCalendarFlyoutPrivate
{
public:
    QX_DECLARE_PUBLIC(QxCalendarFlyout)

    QDate month = QDate::currentDate();
    QDate selected = QDate::currentDate();
    QDate today = QDate::currentDate();
    QDate hovered;
};

QxCalendarFlyout::QxCalendarFlyout(QWidget *anchor)
    : QxPopupSurface(anchor)
    , d_ptr(new QxCalendarFlyoutPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setMouseTracking(true);
}

QxCalendarFlyout::~QxCalendarFlyout(){QX_FINI_PRIVATE()}

QDate QxCalendarFlyout::month() const
{
    return d_ptr->month;
}

void QxCalendarFlyout::setMonth(QDate month)
{
    if (!month.isValid()) {
        return;
    }
    d_ptr->month = QDate(month.year(), month.month(), 1);
    update();
}

QDate QxCalendarFlyout::selectedDate() const
{
    return d_ptr->selected;
}

void QxCalendarFlyout::setSelectedDate(QDate date)
{
    if (!date.isValid() || d_ptr->selected == date) {
        return;
    }
    d_ptr->selected = date;
    setMonth(date);
    Q_EMIT selectedDateChanged();
    update();
}

QDate QxCalendarFlyout::today() const
{
    return d_ptr->today;
}

void QxCalendarFlyout::showPreviousMonth()
{
    d_ptr->month = d_ptr->month.addMonths(-1);
    update();
}

void QxCalendarFlyout::showNextMonth()
{
    d_ptr->month = d_ptr->month.addMonths(1);
    update();
}

QFont QxCalendarFlyout::headFont() const
{
    // A family that says nothing writes with the application's own font: the
    // grid is still a grid.
    return QFont();
}

QFont QxCalendarFlyout::weekdayFont() const
{
    return QFont();
}

QSize QxCalendarFlyout::contentSizeHint() const
{
    return QSize(int(kColumns * kCell), int(kHeadHeight + kDayNamesHeight + kWeeks * kCell));
}

void QxCalendarFlyout::paintContent(QPainter *painter, const QRectF &content)
{
    const QColor ink = palette().color(QPalette::WindowText);
    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();

    painter->save();
    // The head: the month by its name, and the two months either side of it
    // a click away.
    painter->setFont(headFont());
    painter->setPen(ink);
    painter->drawText(QRectF(content.left() + kNavZone, content.top(), content.width() - 2 * kNavZone, kHeadHeight),
                      Qt::AlignCenter, d_ptr->month.toString(QStringLiteral("MMMM yyyy")));
    painter->setPen(QPen(ink, 1.4));
    painter->setBrush(Qt::NoBrush);
    painter->setRenderHint(QPainter::Antialiasing, true);
    const QPointF left(content.left() + kNavZone / 2.0, content.top() + kHeadHeight / 2.0);
    const QPointF right(content.right() - kNavZone / 2.0, content.top() + kHeadHeight / 2.0);
    painter->drawPolyline(
        QVector<QPointF>{left + QPointF(3.0, -5.0), left + QPointF(-3.0, 0.0), left + QPointF(3.0, 5.0)}.constData(),
        3);
    painter->drawPolyline(
        QVector<QPointF>{right + QPointF(-3.0, -5.0), right + QPointF(3.0, 0.0), right + QPointF(-3.0, 5.0)}
            .constData(),
        3);
    painter->setRenderHint(QPainter::Antialiasing, false);

    // The days of the week, in the ink held back. The grid runs Monday
    // first, as this calendar does.
    painter->setFont(weekdayFont());
    painter->setPen(ink);
    painter->setOpacity(kMutedOpacity);
    const QStringList dayNames = {QStringLiteral("M"), QStringLiteral("T"), QStringLiteral("W"), QStringLiteral("T"),
                                  QStringLiteral("F"), QStringLiteral("S"), QStringLiteral("S")};
    for (int column = 0; column < kColumns; ++column) {
        painter->drawText(cellRect(-1, column), Qt::AlignCenter, dayNames.at(column));
    }
    painter->setOpacity(1.0);

    // The grid: the first of the month, in the column the week puts it, and
    // the days after it. The days before and after the month are there to
    // fill the weeks, thinned to a whisper.
    QDate walker = d_ptr->month.addDays(1 - d_ptr->month.dayOfWeek());
    for (int index = 0; index < int(kWeeks * kColumns); ++index) {
        const int row = index / kColumns;
        const int column = index % kColumns;
        const QRectF cell = cellRect(row, column);
        const bool inMonth = walker.month() == d_ptr->month.month();
        const bool isSelected = walker == d_ptr->selected;
        const bool isToday = walker == d_ptr->today;
        const bool isHovered = walker == d_ptr->hovered;

        if (isSelected || isHovered) {
            // The wash is drawn at full strength, whatever the day before it
            // was thinned to.
            painter->setOpacity(1.0);
            QPainterPath wash;
            wash.addRoundedRect(cell.adjusted(2.0, 2.0, -2.0, -2.0), kRowRadius, kRowRadius);
            if (isSelected) {
                painter->fillPath(wash, accent);
            } else {
                painter->fillPath(wash, QxStateLayer::layered(palette().color(QPalette::Window), ink, kHoverOpacity));
            }
        }

        painter->setFont(headFont());
        painter->setOpacity(inMonth ? 1.0 : kMutedOpacity * (isHovered ? 2.0 : 1.0));
        painter->setPen(isSelected ? QColor(Qt::white) : ink);
        painter->drawText(cell, Qt::AlignCenter, QString::number(walker.day()));

        // Today keeps its own ring, whatever else it is.
        if (isToday && !isSelected) {
            painter->setOpacity(1.0);
            painter->setPen(QPen(accent, 1.4));
            painter->setBrush(Qt::NoBrush);
            painter->setRenderHint(QPainter::Antialiasing, true);
            painter->drawEllipse(cell.adjusted(3.0, 3.0, -3.0, -3.0));
            painter->setRenderHint(QPainter::Antialiasing, false);
        }
        walker = walker.addDays(1);
    }
    painter->restore();
}

void QxCalendarFlyout::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        moveByDays(-1);
        event->accept();
        return;
    case Qt::Key_Right:
        moveByDays(1);
        event->accept();
        return;
    case Qt::Key_Up:
        moveByDays(-kColumns);
        event->accept();
        return;
    case Qt::Key_Down:
        moveByDays(kColumns);
        event->accept();
        return;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        Q_EMIT datePicked(d_ptr->selected);
        close();
        event->accept();
        return;
    default:
        break;
    }
    QxPopupSurface::keyPressEvent(event);
}

void QxCalendarFlyout::mouseMoveEvent(QMouseEvent *event)
{
    d_ptr->hovered = dateAt(eventPos(event));
    update();
    QxPopupSurface::mouseMoveEvent(event);
}

void QxCalendarFlyout::mousePressEvent(QMouseEvent *event)
{
    QxPopupSurface::mousePressEvent(event);
    const QPointF pos = eventPos(event);
    // The two months either side of the one up are a click away.
    if (pos.x() - contentRect().left() < kNavZone && pos.y() - contentRect().top() < kHeadHeight) {
        showPreviousMonth();
        return;
    }
    if (contentRect().right() - pos.x() < kNavZone && pos.y() - contentRect().top() < kHeadHeight) {
        showNextMonth();
        return;
    }
    const QDate date = dateAt(pos);
    if (date.isValid()) {
        d_ptr->hovered = date;
        update();
    }
}

void QxCalendarFlyout::mouseReleaseEvent(QMouseEvent *event)
{
    QxPopupSurface::mouseReleaseEvent(event);
    const QDate date = dateAt(eventPos(event));
    if (date.isValid()) {
        setSelectedDate(date);
        Q_EMIT datePicked(date);
        close();
    }
}

QDate QxCalendarFlyout::dateAt(const QPointF &pos) const
{
    const QPointF local = pos - contentRect().topLeft();
    const qreal gridTop = kHeadHeight + kDayNamesHeight;
    if (local.y() < gridTop) {
        return QDate();
    }
    const int column = int((local.x() - contentRect().left() * 0) / kCell);
    const int row = int((local.y() - gridTop) / kCell);
    if (column < 0 || column >= kColumns || row < 0 || row >= int(kWeeks)) {
        return QDate();
    }
    return d_ptr->month.addDays(1 - d_ptr->month.dayOfWeek() + row * kColumns + column);
}

QRectF QxCalendarFlyout::cellRect(int row, int column) const
{
    const qreal gridTop = kHeadHeight + kDayNamesHeight;
    const QPointF origin = contentRect().topLeft();
    return QRectF(origin.x() + column * kCell, origin.y() + gridTop + row * kCell, kCell, kCell);
}

void QxCalendarFlyout::moveByDays(int days)
{
    setSelectedDate(d_ptr->selected.addDays(days));
}

QX_CONTROLS_END_NAMESPACE
