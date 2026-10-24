/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXTIMEFLYOUT_H
#define QXTIMEFLYOUT_H

#include "qxcontrols_global.h"

#include "qxpopupsurface.h"

#include <QtCore/QTime>

QX_CONTROLS_BEGIN_NAMESPACE

class QxTimeFlyoutPrivate;

/*!
 * The time flyout: the day's hours and the hour's minutes, two columns side by
 * side with six of each on show.
 *
 * A column is scrolled by the wheel and taken by a click, and the keyboard
 * walks the same two: Left and Right change the column in hand, Up and Down
 * take the value either side of the one in force - round, since an hour after
 * the last is the first. Taking a value in either column is a time, and the
 * flyout says so; it does not close, because a time has two halves and the
 * other half may be next.
 *
 * The mechanism is the same in every family and the type is not: what a family
 * answers is the size the numbers are written at, and the corner the surface
 * turns by, which it sets through the popup surface it already owns.
 */
class QX_CONTROLS_EXPORT QxTimeFlyout : public QxPopupSurface
{
    Q_OBJECT
public:
    explicit QxTimeFlyout(QWidget *anchor);
    ~QxTimeFlyout() override;

    /*! The time the two columns say. */
    QTime time() const;

public Q_SLOTS:
    void setTime(QTime time);
Q_SIGNALS:
    /*! A value was taken in one of the columns; the time says both. */
    void timeTaken(QTime time);
protected:
    /*! The type the numbers are written at. */
    virtual QFont rowFont() const;

    QSize contentSizeHint() const override;
    void paintContent(QPainter *painter, const QRectF &content) override;

    void keyPressEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    int value(int column) const;
    static int count(int column);
    int scroll(int column) const;
    void setScroll(int column, int row);
    void centerScroll(int column, int value);
    void take(int column, int value);
    QRectF columnRect(const QRectF &content, int column) const;

    Q_DISABLE_COPY(QxTimeFlyout)
    QX_DECLARE_PRIVATE(QxTimeFlyout)
};

QX_CONTROLS_END_NAMESPACE

#endif   // QXTIMEFLYOUT_H
