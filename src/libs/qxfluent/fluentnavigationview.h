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
class QRectF;
class QResizeEvent;
class QWheelEvent;

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
    /*!
     * Adds \a text under \a parent and returns it.
     *
     * This is the view's second shape: a destination that is a place of its
     * own and the head of a group of places under it - what WinUI calls a
     * hierarchical navigation view, and what separates a pane of nineteen
     * destinations from a pane of three things that each hold six.
     *
     * A parent is a destination like any other: it can be in force, pressing
     * it runs it, and pressing it also opens it. The arrow at its trailing
     * edge says which way it is. A heading is not a destination and cannot be
     * a parent.
     */
    QAction *addItem(const QString &text, const QIcon &icon, QAction *parent);
    /*!
     * Adds \a text as a heading over the destinations added after it.
     *
     * A heading is not a destination and does not become one: it is not
     * counted by currentIndex(), it is never in force, and neither the
     * pointer nor the keyboard can land on it. What it is for is saying what
     * the destinations under it are for, which a flat column of nineteen of
     * them is unable to say. Headings stand at the top level of the pane; a
     * group of destinations is what a parent destination is for.
     *
     * Folded, the heading is a rule rather than words and keeps its height:
     * the destinations under it stay where they were when the pane folds,
     * which is the one thing a reader needs of a fold.
     */
    void addHeader(const QString &text);
    void removeItem(QAction *action);
    void clear();
    QList<QAction *> items() const;

    /*! The destination \a item stands under, or null while it is one of the
     *  top level - or is not in this view at all. */
    QAction *parentItem(QAction *item) const;

    /*! True while the destinations under \a item are showing. A destination
     *  with nothing under it is never expanded. */
    bool isExpanded(QAction *item) const;

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
    /*! Shows or hides the destinations under \a item, and says so if it
     *  changed. A folded pane opens \a item as a flyout instead, which this
     *  does not touch. */
    void setExpanded(QAction *item, bool expanded);

Q_SIGNALS:
    void paneOpenChanged(bool open);
    void currentIndexChanged(int index);
    void currentItemChanged(QAction *action);
    void expandedChanged(QAction *item, bool expanded);
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
    void wheelEvent(QWheelEvent *event) override;

    QAccessible::Role accessibleRole() const override;
private:
    /*! Opens \a parent beside the pane, in a panel whose rows are what stands
     *  under it. Only a folded pane asks for one: open, there is room. */
    void showFlyout(QAction *parent, const QRectF &anchor);

    Q_DISABLE_COPY(FluentNavigationView)
    QX_DECLARE_PRIVATE(FluentNavigationView)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTNAVIGATIONVIEW_H
