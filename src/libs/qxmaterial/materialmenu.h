/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALMENU_H
#define MATERIALMENU_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxpopupsurface.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtGui/QAction>
#else
#include <QtWidgets/QAction>
#endif

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialMenuPrivate;

/*!
 * Material's menu: the surface a list of commands opens into, drawn by the
 * family on the shared popup surface instead of styled into a QMenu.
 *
 * A QMenu arrives with a platform's opinions about what a menu looks like;
 * the menu here is the family's answer - a paper lifted to the level a menu
 * stands at, its rows of forty-eight carrying actions: a command with words
 * or an icon, a command that keeps a state and wears the check in the
 * primary when it is in force, a hairline between groups. The keyboard walks
 * the commands: Up and Down move among the ones that can be worked, Enter
 * works the one in hand, and Escape puts the menu away.
 */
class QX_MATERIAL_EXPORT MaterialMenu : public QxControls::QxPopupSurface
{
    Q_OBJECT
public:
    explicit MaterialMenu(QWidget *anchor);
    ~MaterialMenu() override;

    /*! A command with \a text and an \a icon. The menu keeps it; the action
     *  is the way to connect to it, disable it, check it. */
    QAction *addAction(const QString &text, const QIcon &icon = QIcon());
    /*! A command that keeps a state, in force when \a checked. */
    QAction *addCheckable(const QString &text, bool checked = false, const QIcon &icon = QIcon());
    /*! A hairline between groups of commands. */
    QAction *addSeparator();

    /*! The commands the menu carries, in the order it carries them. */
    QList<QAction *> actions() const;
    /*! Forgets every command. */
    void clear();

    /*! The command the keyboard is on, or the pointer is hovering over. */
    QAction *currentAction() const;

public Q_SLOTS:
    /*! Puts the keyboard on \a action, as a hover does. */
    void setCurrentAction(QAction *action);
    /*! Sizes itself from its commands, places itself beside the anchor, and
     *  shows, with the keyboard on the first command that can be worked. */
    void open();
Q_SIGNALS:
    /*! A command was worked, by the keyboard or by the pointer. */
    void triggered(QAction *action);
protected:
    QSize contentSizeHint() const override;
    void paintContent(QPainter *painter, const QRectF &content) override;

    void keyPressEvent(QKeyEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    /*! True when \a action is a command the menu can put the keyboard on. */
    bool isWorkable(QAction *action) const;
    int indexOf(QAction *action) const;
    /*! The command at \a pos in surface coordinates, or null. */
    QAction *actionAt(const QPointF &pos) const;
    int rowHeight(int index) const;

    Q_DISABLE_COPY(MaterialMenu)
    QX_DECLARE_PRIVATE(MaterialMenu)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALMENU_H
