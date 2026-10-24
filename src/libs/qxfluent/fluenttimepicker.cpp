/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluenttimepicker.h"

#include "fluenttokens.h"

#include "qxcontrols/qxpopupsurface.h"
#include "qxcontrols/qxstatelayer.h"
#include "qxtheme/qxthememanager.h"

#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QWheelEvent>
#include <QtWidgets/QLineEdit>

#include <functional>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// Fluent's time picker measures: two columns the day's hours and the hour's
// minutes stand in, six of them on show, the taken one in the pill.
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

/*!
 * The two columns the time picker's field opens into. Mechanism is the popup
 * surface's; what belongs to the family is the radius and the columns painted
 * over it.
 */
class FluentTimeFlyout : public QxControls::QxPopupSurface
{
public:
    explicit FluentTimeFlyout(QWidget *anchor)
        : QxControls::QxPopupSurface(anchor)
    {
        setSurfaceRadius(FluentShape::radius(FluentShape::Radius::Large));
    }

    QTime time() const
    {
        return QTime(m_hour, m_minute);
    }

    void setTime(QTime time)
    {
        if (!time.isValid()) {
            return;
        }
        m_hour = time.hour();
        m_minute = time.minute();
        // The taken hour stands mid-column, with the day either side of it.
        centerScroll(0, m_hour);
        centerScroll(1, m_minute);
        update();
    }

    /*! A take in either column: the picker hears it and says the time. */
    std::function<void(QTime)> taken;
protected:
    QSize contentSizeHint() const override
    {
        return QSize(int(2 * kPadding + 2 * kColumnWidth + kGap), int(kVisibleRows * kRowHeight));
    }

    void paintContent(QPainter *painter, const QRectF &content) override
    {
        const QColor base = palette().color(QPalette::Window);
        const QColor ink = palette().color(QPalette::WindowText);
        const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();

        // The hairline between the columns, the way a list keeps its columns
        // apart without drawing a wall.
        painter->fillRect(
            QRectF(content.left() + kPadding + kColumnWidth + kGap / 2.0, content.top(), 1.0, content.height()),
            QxControls::QxStateLayer::layered(base, ink, 0.08));

        painter->setFont(FluentType::font(FluentType::Ramp::Body));
        for (int column = 0; column < 2; ++column) {
            const QRectF room = columnRect(content, column);
            const int count = this->count(column);
            painter->save();
            painter->setClipRect(room);
            for (int row = 0; row < kVisibleRows; ++row) {
                const int value = scroll(column) + row;
                if (value >= count) {
                    break;
                }
                const QRectF cell(room.left(), content.top() + row * kRowHeight, room.width(), kRowHeight);
                const bool isSelected = value == this->value(column);
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
                painter->drawText(cell, Qt::AlignCenter, QStringLiteral("%1").arg(value, 2, 10, QLatin1Char('0')));
            }
            painter->restore();
        }
    }

    void keyPressEvent(QKeyEvent *event) override
    {
        switch (event->key()) {
        case Qt::Key_Left:
            m_activeColumn = 0;
            update();
            event->accept();
            return;
        case Qt::Key_Right:
            m_activeColumn = 1;
            update();
            event->accept();
            return;
        case Qt::Key_Up:
            // The day's hours and the hour's minutes both come round.
            take(m_activeColumn, (value(m_activeColumn) + count(m_activeColumn) - 1) % count(m_activeColumn));
            event->accept();
            return;
        case Qt::Key_Down:
            take(m_activeColumn, (value(m_activeColumn) + 1) % count(m_activeColumn));
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
        QxControls::QxPopupSurface::keyPressEvent(event);
    }

    void wheelEvent(QWheelEvent *event) override
    {
        const QPointF pos = wheelPos(event);
        const int column = pos.x() - contentRect().left() < kPadding + kColumnWidth + kGap / 2.0 ? 0 : 1;
        const int steps = event->angleDelta().y() / 120;
        if (steps == 0) {
            return;
        }
        setScroll(column, scroll(column) - steps);
        update();
        event->accept();
    }

    void mouseReleaseEvent(QMouseEvent *event) override
    {
        QxControls::QxPopupSurface::mouseReleaseEvent(event);
        const QPointF pos = eventPos(event);
        for (int column = 0; column < 2; ++column) {
            const QRectF room = columnRect(contentRect(), column);
            if (!room.contains(pos)) {
                continue;
            }
            const int row = int((pos.y() - contentRect().top()) / kRowHeight);
            const int taken = scroll(column) + row;
            if (taken < count(column)) {
                take(column, taken);
            }
            return;
        }
    }
private:
    int value(int column) const
    {
        return column == 0 ? m_hour : m_minute;
    }
    static int count(int column)
    {
        return column == 0 ? 24 : 60;
    }
    int scroll(int column) const
    {
        return m_scroll[column];
    }

    void setScroll(int column, int row)
    {
        m_scroll[column] = qBound(0, row, count(column) - kVisibleRows);
    }

    void centerScroll(int column, int value)
    {
        setScroll(column, value - kVisibleRows / 2);
    }

    void take(int column, int value)
    {
        if (column == 0) {
            m_hour = value;
        } else {
            m_minute = value;
        }
        m_activeColumn = column;
        centerScroll(column, value);
        update();
        if (taken) {
            taken(QTime(m_hour, m_minute));
        }
    }

    QRectF columnRect(const QRectF &content, int column) const
    {
        return QRectF(content.left() + kPadding + column * (kColumnWidth + kGap), content.top(), kColumnWidth,
                      content.height());
    }

    int m_hour = 0;
    int m_minute = 0;
    int m_scroll[2] = {0, 0};
    int m_activeColumn = 0;
};

class FluentTimePickerPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentTimePicker)

    FluentTimeFlyout *flyout = nullptr;
    // The time said last. The columns already carry what was taken by the
    // time the picker hears of it, so "what changed" is remembered here.
    QTime last{0, 0};
};

FluentTimePicker::FluentTimePicker(QWidget *parent)
    : FluentTextBox(parent)
    , d_ptr(new FluentTimePickerPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // The writing is not the point: the field says, the columns change.
    lineEdit()->setReadOnly(true);
    lineEdit()->installEventFilter(this);
    d_ptr->flyout = new FluentTimeFlyout(this);
    d_ptr->flyout->taken = [this](QTime time) {
        setTime(time);
    };
    d_ptr->last = d_ptr->flyout->time();
}

FluentTimePicker::~FluentTimePicker(){QX_FINI_PRIVATE()}

QTime FluentTimePicker::time() const
{
    return d_ptr->flyout->time();
}

QWidget *FluentTimePicker::flyout() const
{
    return d_ptr->flyout;
}

void FluentTimePicker::setTime(QTime time)
{
    if (!time.isValid()) {
        return;
    }
    const QTime previous = d_ptr->last;
    d_ptr->flyout->setTime(time);
    setText(time.toString(QStringLiteral("HH:mm")));
    d_ptr->last = time;
    if (time != previous) {
        Q_EMIT timeChanged(time);
    }
}

void FluentTimePicker::openFlyout()
{
    if (!isEnabled()) {
        return;
    }
    d_ptr->flyout->setTime(time().isValid() ? time() : QTime::currentTime());
    d_ptr->flyout->open();
}

bool FluentTimePicker::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == lineEdit() && event->type() == QEvent::MouseButtonPress) {
        openFlyout();
        return true;
    }
    return FluentTextBox::eventFilter(watched, event);
}

QX_FLUENT_END_NAMESPACE
