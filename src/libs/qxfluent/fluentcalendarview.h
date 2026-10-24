/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTCALENDARVIEW_H
#define FLUENTCALENDARVIEW_H

#include "qxfluent_global.h"

#include <QtCore/QDate>
#include <QtCore/QList>
#include <QtWidgets/QWidget>

class QScrollBar;

QX_FLUENT_BEGIN_NAMESPACE

class FluentCalendarViewPrivate;

/*!
 * Fluent's calendar view: a year and a half of months, one under the other,
 * with the day in force painted on the one it falls on.
 *
 * This is WinUI's CalendarView, and it is the whole calendar rather than the
 * grid a date picker opens: the months run on past the bottom of the view and
 * the wheel walks them, so a day can be hunted for rather than paged to. The
 * name of a month is a way out of the month - pressing it pulls the view back
 * to that year's twelve, pressing the year pulls it back further to the ten
 * years around it, and a year there drops back to its twelve, a month there
 * back to the days. That is how a day two years off is reached in three
 * presses instead of twenty-four.
 *
 * A day is chosen with the pointer or walked to with the keyboard. What
 * counts as chosen depends on the selection mode: one day, several days, or a
 * run of them between two ends. The run is drawn as a run - the ends round,
 * the middle joined - because a range that looked like a scatter of chosen
 * days would be saying the wrong thing.
 *
 * The view owns its scrolling, in the way a view does: the days are drawn for
 * the room there is and the scroll bar says how much of the run of months is
 * under them. It is not a column of widgets and it is not a model view -
 * a calendar is not a list of rows that a model describes, and the one number
 * that would have to come out of a delegate, which day a point falls on, is
 * the whole of the geometry anyway.
 *
 * \sa FluentDatePicker
 */
class QX_FLUENT_EXPORT FluentCalendarView : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QDate visibleMonth READ visibleMonth WRITE setVisibleMonth)
    Q_PROPERTY(QDate selectedDate READ selectedDate WRITE setSelectedDate NOTIFY selectedDateChanged)
    Q_PROPERTY(DisplayMode displayMode READ displayMode WRITE setDisplayMode NOTIFY displayModeChanged)
    Q_PROPERTY(SelectionMode selectionMode READ selectionMode WRITE setSelectionMode NOTIFY selectionModeChanged)
public:
    /*!
     * What the view is showing. \c Month is the days of one month after
     * another; \c Year is the twelve months of a year; \c Decade is the ten
     * years around one. Each is pulled back from the one before it by
     * pressing that one's name.
     */
    enum class DisplayMode {
        Month,
        Year,
        Decade
    };
    Q_ENUM(DisplayMode)

    /*!
     * How many days may be chosen at once. \c Range takes two presses - the
     * first end and the second - and everything between them is chosen.
     */
    enum class SelectionMode {
        Single,
        Multiple,
        Range
    };
    Q_ENUM(SelectionMode)

    explicit FluentCalendarView(QWidget *parent = Q_NULLPTR);
    ~FluentCalendarView() override;

    DisplayMode displayMode() const;
    void setDisplayMode(DisplayMode mode);

    /*! The month the view starts at - the one at its top, or the one the year
     *  and the decade are drawn around. The first of it. */
    QDate visibleMonth() const;
    void setVisibleMonth(QDate month);

    SelectionMode selectionMode() const;
    void setSelectionMode(SelectionMode mode);

    /*! The day in force: the chosen one, or the first of the chosen ones. */
    QDate selectedDate() const;
    /*! Chooses \a date alone, whatever the selection mode - a setter sets. */
    void setSelectedDate(QDate date);
    /*! Every chosen day, earliest first. */
    QList<QDate> selectedDates() const;
    /*! Chooses \a dates, and nothing else. */
    void setSelectedDates(const QList<QDate> &dates);
    /*! Chooses nothing. */
    void clearSelection();

    /*! The day this machine thinks it is, drawn in its own ring. */
    QDate today() const;

    /*! The day the keyboard stands on, which is not always a chosen one: a
     *  range is walked to before it is taken. */
    QDate currentDate() const;
    void setCurrentDate(QDate date);

    /*! Where \a date stands in the view, in the view's own coordinates; an
     *  empty rectangle when the view is not showing it. */
    QRect dayRect(QDate date) const;
    /*! Where the name of \a month stands; empty when it is not showing it. */
    QRect monthRect(QDate month) const;
    /*! Where \a year stands in the decade; empty when it is not showing it. */
    QRect yearRect(int year) const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
Q_SIGNALS:
    /*! The day in force changed. */
    void selectedDateChanged(QDate date);
    /*! What is chosen changed - by a press, by the keyboard, or by a setter. */
    void selectionChanged();
    void displayModeChanged(DisplayMode mode);
    void selectionModeChanged(SelectionMode mode);
    /*! A day was pressed. A press chooses it as well, but a caller that wants
     *  to close a surface on any press listens here. */
    void dateClicked(QDate date);
protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void changeEvent(QEvent *event) override;
private:
    void zoomOut();
    void zoomIn();
    void step(int days);
    void stepMonths(int months);
    void choose(QDate date);
    void take(QDate date);
    void scrollToDate(QDate date);
    void updateBar();

    QRect contentRect() const;
    int monthIndex(QDate month) const;
    QDate monthAt(int index) const;
    QDate dateAt(const QPoint &pos) const;
    QDate monthNameAt(const QPoint &pos) const;
    QDate yearAt(const QPoint &pos) const;
    QColor ink() const;

    Q_DISABLE_COPY(FluentCalendarView)
    QX_DECLARE_PRIVATE(FluentCalendarView)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTCALENDARVIEW_H
