/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTCOMMANDBAR_H
#define FLUENTCOMMANDBAR_H

#include "qxfluent_global.h"

#include "qxcontrols/qxcontrol.h"

#include <QtGui/QIcon>

class QAction;
class QMenu;
class QResizeEvent;

QX_FLUENT_BEGIN_NAMESPACE

class FluentCommandBarPrivate;

/*!
 * Fluent's command bar: where the commands of a Fluent application live.
 *
 * An application that has chosen Fluent has no ribbon, and this is what it
 * has instead: one row of commands, the ones that matter in the order they
 * matter, and the ones that do not fit kept behind a button rather than
 * dropped. Qt's answer - a QToolBar - is a row of QToolButtons whose look
 * belongs to the style, and whose overflow is a menu the toolbar opens for
 * you with no say in what goes into it.
 *
 * A command here is a QAction, because that is what a Qt application already
 * has: connect to it, disable it, check it, read it back from a menu of its
 * own. What the bar adds is the answer to "what does a click here do", one of
 * four, and what happens to the row when there is not enough room.
 */
class QX_FLUENT_EXPORT FluentCommandBar : public QxControls::QxControl
{
    Q_OBJECT
public:
    /*! What a click on a command does. */
    enum class ItemRole {
        Button,   /*!< One command: a click runs it. */
        Toggle,   /*!< A command that stays in force: a click turns it on. */
        DropDown, /*!< A menu: a click opens it, wherever the click lands. */
        Split     /*!< Both: the command runs, and its chevron opens the menu. */
    };
    Q_ENUM(ItemRole)

    explicit FluentCommandBar(QWidget *parent = Q_NULLPTR);
    ~FluentCommandBar() override;

    /*! Adds \a text as a command that answers \a role, and returns it. The bar
     *  owns it. A DropDown or a Split is given an empty menu, which is
     *  \a action's own \c menu() to fill. */
    QAction *addCommand(ItemRole role, const QString &text, const QIcon &icon = QIcon());
    void removeCommand(QAction *action);
    void clear();

    QList<QAction *> commands() const;
    ItemRole roleOf(QAction *action) const;

    /*! The commands the row has room for; the rest are in the overflow. */
    QList<QAction *> visibleCommands() const;
    /*! The menu the rest are in, emptied and filled again as the row is laid
     *  out - read it, do not fill it. */
    QMenu *overflowMenu() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
protected:
    qreal cornerRadius() const override;
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
    Q_DISABLE_COPY(FluentCommandBar)
    QX_DECLARE_PRIVATE(FluentCommandBar)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTCOMMANDBAR_H
