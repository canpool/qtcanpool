/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXCALENDARFLYOUT_H
#define QXCALENDARFLYOUT_H

#include "qxcontrols_global.h"

#include "qxcontrols/qxpopupsurface.h"

#include <QtCore/QDate>

QX_CONTROLS_BEGIN_NAMESPACE

class QxCalendarFlyoutPrivate;

/*!
 * The calendar flyout: a month laid out as the grid it is - the days of the
 * week across the top, the days below - with a month before and a month
 * after, and today standing where today stands.
 *
 * It is the picking surface a date picker opens, and the part of a full
 * calendar view that does the picking. A day is chosen with the pointer or
 * walked to with the keyboard - Left, Right, Up and Down move the selection a
 * day or a week, Enter takes it, Escape puts the month away.
 *
 * The mechanism is the same in every family and the type is not, so what a
 * family answers is the two faces: the size the month and its days are
 * written at, and the size the days of the week are written at. The corner the
 * surface turns by is the family's too, and it is set through the popup
 * surface the family already owns.
 */
class QX_CONTROLS_EXPORT QxCalendarFlyout : public QxPopupSurface
{
    Q_OBJECT
    Q_PROPERTY(QDate selectedDate READ selectedDate WRITE setSelectedDate NOTIFY selectedDateChanged)
public:
    explicit QxCalendarFlyout(QWidget *anchor);
    ~QxCalendarFlyout() override;

    /*! The month the grid is showing; the first of it. */
    QDate month() const;
    /*! Puts \a month's grid up - the same day of it, or the first if the
     *  day does not exist in it. */
    void setMonth(QDate month);

    /*! The day picked, or walked to. */
    QDate selectedDate() const;

    /*! The day this machine thinks it is, drawn in its own ring. */
    QDate today() const;

public Q_SLOTS:
    void setSelectedDate(QDate date);
    void showPreviousMonth();
    void showNextMonth();
Q_SIGNALS:
    /*! The day picked, or walked to, changed. */
    void selectedDateChanged();
    /*! A day was taken - by a click, or by Enter on the walked-to day. */
    void datePicked(QDate date);
protected:
    /*! The type the month's name and its days are written at. */
    virtual QFont headFont() const;
    /*! The type the days of the week are written at. */
    virtual QFont weekdayFont() const;

    QSize contentSizeHint() const override;
    void paintContent(QPainter *painter, const QRectF &content) override;

    void keyPressEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    QDate dateAt(const QPointF &pos) const;
    QRectF cellRect(int row, int column) const;
    void moveByDays(int days);

    Q_DISABLE_COPY(QxCalendarFlyout)
    QX_DECLARE_PRIVATE(QxCalendarFlyout)
};

QX_CONTROLS_END_NAMESPACE

#endif   // QXCALENDARFLYOUT_H
