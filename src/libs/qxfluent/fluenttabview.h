/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTTABVIEW_H
#define FLUENTTABVIEW_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

#include <QtGui/QIcon>

class QAction;
class QKeyEvent;
class QMouseEvent;
class QResizeEvent;

QX_FLUENT_BEGIN_NAMESPACE

class FluentTabViewPrivate;

/*!
 * Fluent's tab view: several things open at once, one of them showing.
 *
 * A navigation view is a list of places and a room beside it; this is a row of
 * things that are all open, of which one is the one being looked at, and the
 * row is where the choice is made. The difference is why a tab carries content
 * of its own and a destination does not: a place is somewhere you go, a tab is
 * something you have open, and it is still there - still holding what you were
 * doing - when you look at another one.
 *
 * Qt has QTabWidget, and what this adds is the two things a Fluent tab row
 * answers differently: the mark of the one being looked at is a short line
 * under the tab rather than the tab being a different shape, and closing a tab
 * is asked about rather than done - the view says which one was closed and the
 * application decides, because a tab with unsaved work in it is the
 * application's business and not the view's.
 */
class QX_FLUENT_EXPORT FluentTabView : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(bool closable READ isClosable WRITE setClosable NOTIFY closableChanged)
public:
    explicit FluentTabView(QWidget *parent = Q_NULLPTR);
    ~FluentTabView() override;

    /*! Opens \a content as a tab called \a text and returns it. The tab owns
     *  neither: the action is this view's child, the content stays the
     *  application's and is only shown and hidden. */
    QAction *addTab(QWidget *content, const QString &text, const QIcon &icon = QIcon());
    void removeTab(QAction *action);
    void clear();
    QList<QAction *> tabs() const;

    /*! What \a action is showing, or null for a tab that has none. */
    QWidget *contentOf(QAction *action) const;

    QAction *currentTab() const;
    int currentIndex() const;

    /*! True while every tab carries a close mark. */
    bool isClosable() const;
    void setClosable(bool closable);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setCurrentIndex(int index);
    void setCurrentTab(QAction *action);

Q_SIGNALS:
    void currentIndexChanged(int index);
    void currentTabChanged(QAction *action);
    /*! The close mark on \a action was clicked. Nothing has been closed: the
     *  application answers, which is what \c removeTab is for. */
    void closeRequested(QAction *action);
    void closableChanged(bool closable);
protected:
    /*! Nothing: a tab view is a row and a room under it, and the wash and the
     *  ring belong to one tab. */
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
    Q_DISABLE_COPY(FluentTabView)
    QX_DECLARE_PRIVATE(FluentTabView)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTTABVIEW_H
