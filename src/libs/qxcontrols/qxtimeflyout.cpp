/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxtimeflyout.h"

#include "qxstatelayer.h"

#include "qxtheme/qxthememanager.h"

#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QWheelEvent>

QX_CONTROLS_BEGIN_NAMESPACE

namespace
{
// The columns' measures: two of fifty-six, six rows of thirty-two on show, and
// the taken row in a pill of its own.
constexpr qreal kColumnWidth = 56.0;
constexpr qreal kRowHeight = 32.0;
constexpr qreal kPadding = 12.0;
constexpr qreal kGap = 8.0;
constexpr int kVisibleRows = 6;
constexpr qreal kRowRadius = 4.0;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}

QPointF wheelPos(const QWheelEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->pos();
#endif
}
}   // namespace

class QxTimeFlyoutPrivate
{
public:
    QX_DECLARE_PUBLIC(QxTimeFlyout)

    int hour = 0;
    int minute = 0;
    int scroll[2] = {0, 0};
    int activeColumn = 0;
};

QxTimeFlyout::QxTimeFlyout(QWidget *anchor)
    : QxPopupSurface(anchor)
    , d_ptr(new QxTimeFlyoutPrivate)
{
    QX_SET_PRIVATE(d_ptr);
}

QxTimeFlyout::~QxTimeFlyout()
{
    QX_FINI_PRIVATE();
}

QTime QxTimeFlyout::time() const
{
    return QTime(d_ptr->hour, d_ptr->minute);
}

void QxTimeFlyout::setTime(QTime time)
{
    if (!time.isValid()) {
        return;
    }
    d_ptr->hour = time.hour();
    d_ptr->minute = time.minute();
    // The taken hour stands mid-column, with the day either side of it.
    centerScroll(0, d_ptr->hour);
    centerScroll(1, d_ptr->minute);
    update();
}

QFont QxTimeFlyout::rowFont() const
{
    return QFont();
}

QSize QxTimeFlyout::contentSizeHint() const
{
    return QSize(int(2 * kPadding + 2 * kColumnWidth + kGap), int(kVisibleRows * kRowHeight));
}

void QxTimeFlyout::paintContent(QPainter *painter, const QRectF &content)
{
    const QColor base = palette().color(QPalette::Window);
    const QColor ink = palette().color(QPalette::WindowText);
    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();

    // The hairline between the columns, the way a list keeps its columns apart
    // without drawing a wall.
    painter->fillRect(
        QRectF(content.left() + kPadding + kColumnWidth + kGap / 2.0, content.top(), 1.0, content.height()),
        QxStateLayer::layered(base, ink, 0.08));

    painter->setFont(rowFont());
    for (int column = 0; column < 2; ++column) {
        const QRectF room = columnRect(content, column);
        const int values = count(column);
        painter->save();
        painter->setClipRect(room);
        for (int row = 0; row < kVisibleRows; ++row) {
            const int at = scroll(column) + row;
            if (at >= values) {
                break;
            }
            const QRectF cell(room.left(), content.top() + row * kRowHeight, room.width(), kRowHeight);
            const bool isSelected = at == value(column);
            if (isSelected) {
                painter->setPen(Qt::NoPen);
                painter->setBrush(accent);
                painter->setRenderHint(QPainter::Antialiasing, true);
                painter->drawRoundedRect(cell.adjusted(3.0, 3.0, -3.0, -3.0), kRowRadius, kRowRadius);
                painter->setRenderHint(QPainter::Antialiasing, false);
                painter->setPen(QColor(Qt::white));
            } else {
                painter->setPen(ink);
            }
            painter->drawText(cell, Qt::AlignCenter, QStringLiteral("%1").arg(at, 2, 10, QLatin1Char('0')));
        }
        painter->restore();
    }
}

void QxTimeFlyout::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Left:
        d_ptr->activeColumn = 0;
        update();
        event->accept();
        return;
    case Qt::Key_Right:
        d_ptr->activeColumn = 1;
        update();
        event->accept();
        return;
    case Qt::Key_Up:
        // The day's hours and the hour's minutes both come round.
        take(d_ptr->activeColumn,
             (value(d_ptr->activeColumn) + count(d_ptr->activeColumn) - 1) % count(d_ptr->activeColumn));
        event->accept();
        return;
    case Qt::Key_Down:
        take(d_ptr->activeColumn, (value(d_ptr->activeColumn) + 1) % count(d_ptr->activeColumn));
        event->accept();
        return;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        close();
        event->accept();
        return;
    default:
        break;
    }
    QxPopupSurface::keyPressEvent(event);
}

void QxTimeFlyout::wheelEvent(QWheelEvent *event)
{
    const QPointF pos = wheelPos(event);
    const int column = pos.x() - contentRect().left() < kPadding + kColumnWidth + kGap / 2.0 ? 0 : 1;
    const int steps = event->angleDelta().y() / 120;
    if (steps == 0) {
        return;
    }
    // The wheel looks; it does not take - a turn is not an answer.
    setScroll(column, scroll(column) - steps);
    update();
    event->accept();
}

void QxTimeFlyout::mouseReleaseEvent(QMouseEvent *event)
{
    QxPopupSurface::mouseReleaseEvent(event);
    const QPointF pos = eventPos(event);
    for (int column = 0; column < 2; ++column) {
        const QRectF room = columnRect(contentRect(), column);
        if (!room.contains(pos)) {
            continue;
        }
        const int row = int((pos.y() - contentRect().top()) / kRowHeight);
        const int at = scroll(column) + row;
        if (at < count(column)) {
            take(column, at);
        }
        return;
    }
}

int QxTimeFlyout::value(int column) const
{
    return column == 0 ? d_ptr->hour : d_ptr->minute;
}

int QxTimeFlyout::count(int column)
{
    return column == 0 ? 24 : 60;
}

int QxTimeFlyout::scroll(int column) const
{
    return d_ptr->scroll[column];
}

void QxTimeFlyout::setScroll(int column, int row)
{
    d_ptr->scroll[column] = qBound(0, row, count(column) - kVisibleRows);
}

void QxTimeFlyout::centerScroll(int column, int at)
{
    setScroll(column, at - kVisibleRows / 2);
}

void QxTimeFlyout::take(int column, int at)
{
    if (column == 0) {
        d_ptr->hour = at;
    } else {
        d_ptr->minute = at;
    }
    d_ptr->activeColumn = column;
    centerScroll(column, at);
    update();
    Q_EMIT timeTaken(time());
}

QRectF QxTimeFlyout::columnRect(const QRectF &content, int column) const
{
    return QRectF(content.left() + kPadding + column * (kColumnWidth + kGap), content.top(), kColumnWidth,
                  content.height());
}

QX_CONTROLS_END_NAMESPACE
