/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTMENUFLYOUT_H
#define FLUENTMENUFLYOUT_H

#include "qxfluent_global.h"

#include "qxcontrols/qxpopupsurface.h"

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtGui/QAction>
#else
#include <QtWidgets/QAction>
#endif

QX_FLUENT_BEGIN_NAMESPACE

class FluentMenuFlyoutPrivate;

/*!
 * Fluent's menu flyout: the surface a list of commands opens into, drawn by
 * the family rather than styled into a QMenu.
 *
 * A QMenu arrives with a platform's opinions about what a menu looks like;
 * the flyout here is the family's answer, opened on the shared popup surface
 * - drawn, lifted, placed beside its anchor, gone on Escape or on a click
 * outside it. Its rows are actions: a command with words or an icon, a
 * command that keeps a state and wears the check when it is in force, a
 * hairline between groups, or a small heading that names a group.
 *
 * The keyboard walks the commands: Up and Down move among the ones that can
 * be worked, Enter works the one in hand, and Escape puts the flyout away.
 */
class QX_FLUENT_EXPORT FluentMenuFlyout : public QxControls::QxPopupSurface
{
    Q_OBJECT
public:
    explicit FluentMenuFlyout(QWidget *anchor);
    ~FluentMenuFlyout() override;

    /*! A command with \a words and an \a icon. The flyout keeps it; the
     *  action is the way to connect to it, disable it, check it. */
    QAction *addAction(const QString &text, const QIcon &icon = QIcon());
    /*! A command that keeps a state, in force when \a checked. */
    QAction *addCheckable(const QString &text, bool checked = false, const QIcon &icon = QIcon());
    /*! A hairline between groups of commands. */
    QAction *addSeparator();
    /*! A small heading that names the group the commands after it belong to. */
    QAction *addSection(const QString &title);

    /*! The commands the flyout carries, in the order it carries them. */
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
    /*! True when \a action is a command the flyout can put the keyboard on. */
    bool isWorkable(QAction *action) const;
    int indexOf(QAction *action) const;
    /*! The command at \a pos in surface coordinates, or null. */
    QAction *actionAt(const QPointF &pos) const;
    int rowHeight(int index) const;
    QRectF rowRect(int index, const QRectF &content) const;

    Q_DISABLE_COPY(FluentMenuFlyout)
    QX_DECLARE_PRIVATE(FluentMenuFlyout)
};

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTMENUFLYOUT_H
