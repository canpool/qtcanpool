/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTNAVIGATIONVIEW_H
#define FLUENTNAVIGATIONVIEW_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

#include <QtGui/QIcon>

class QAction;
class QKeyEvent;
class QMouseEvent;
class QResizeEvent;

QX_FLUENT_BEGIN_NAMESPACE

class FluentNavigationViewPrivate;

/*!
 * Fluent's navigation view: the skeleton a Fluent application is navigated in.
 *
 * An application that has chosen Fluent has no ribbon, and a row of commands
 * is not enough to steer by: what it needs is a place the destinations live
 * and a place the destination shows up, side by side, with the one on the left
 * able to fold itself down to its icons when the room is needed. Qt's answer
 * is a QStackedWidget driven by a QListWidget beside it, which is two widgets
 * an application has to keep in step, arriving in whatever the style draws.
 *
 * A destination here is a QAction, because that is what a Qt application
 * already has: connect to it, disable it, put it in a menu of its own. What
 * the view adds is the pane - the column the destinations stand in, with the
 * one in force marked - and the content beside it, which is any widget the
 * application hands over and which this view keeps to its side of the line.
 *
 * Folding the pane is the one piece of behaviour here that has no counterpart
 * in a list: it is a state the view holds, not a size the application sets,
 * and when the pane is folded the words go and the icons stay.
 */
class QX_FLUENT_EXPORT FluentNavigationView : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(bool paneOpen READ isPaneOpen WRITE setPaneOpen NOTIFY paneOpenChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
public:
    explicit FluentNavigationView(QWidget *parent = Q_NULLPTR);
    ~FluentNavigationView() override;

    /*! Adds \a text as a destination and returns it. The view owns it. */
    QAction *addItem(const QString &text, const QIcon &icon = QIcon());
    void removeItem(QAction *action);
    void clear();
    QList<QAction *> items() const;

    /*! The destination in force, or null while there is none. */
    QAction *currentItem() const;
    int currentIndex() const;

    /*! Hands \a widget the room beside the pane. It becomes this view's child
     *  and is kept there; handing another one replaces it. */
    void setContent(QWidget *widget);
    QWidget *contentWidget() const;

    /*! True while the pane is showing its words as well as its icons. */
    bool isPaneOpen() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setPaneOpen(bool open);
    void setCurrentIndex(int index);
    void setCurrentItem(QAction *action);

Q_SIGNALS:
    void paneOpenChanged(bool open);
    void currentIndexChanged(int index);
    void currentItemChanged(QAction *action);
protected:
    /*!
     * Nothing: a view is a column of destinations and a room beside it, and
     * there is no one part of it that a wash or a ring could belong to. The
     * hover and the focus belong to a destination, and are drawn on that
     * destination's own rectangle rather than on the view's.
     */
    QRectF chromeRect() const override;

    void paintSurface(QPainter *painter) override;
    void paintContent(QPainter *painter) override;

    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    QAccessible::Role accessibleRole() const override;
private:
    Q_DISABLE_COPY(FluentNavigationView)
    QX_DECLARE_PRIVATE(FluentNavigationView)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTNAVIGATIONVIEW_H
