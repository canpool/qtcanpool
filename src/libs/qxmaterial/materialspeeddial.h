/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALSPEEDDIAL_H
#define MATERIALSPEEDDIAL_H

#include "qxmaterial_global.h"

#include <QtGui/QIcon>
#include <QtWidgets/QWidget>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialSpeedDialPrivate;

/*!
 * Material's speed dial: the one thing a screen is for, with the rest of what it
 * can do tucked behind it.
 *
 * The button itself is a floating action button - round, the accent's own
 * colour, over the page - and pressing it lets the actions out, one above the
 * other. What comes out are *small* actions: forty across, the page's own
 * colour with a hairline round them, so that the accent still says which of them
 * is the one the screen is for.
 *
 * Three things this does differently from MUI, and why:
 *
 * - **The actions are children of the host, not of this widget.** An action
 *   that flies out has to leave the dial's own rectangle, and a widget cannot
 *   paint outside itself. So the dial is the button's own fifty-six squares and
 *   the actions stand beside it in the widget the dial was put in - the same
 *   reason the drawer's scrim is its own child of the host.
 * - **The main button draws a cross while it is open** instead of turning the
 *   icon the application gave it by forty-five degrees. The icon is the
 *   application's picture of what the screen is for; turning it is editing
 *   someone else's drawing, and a pencil turned forty-five degrees says nothing
 *   about closing.
 * - **The actions keep their names on show.** MUI hides them in a tooltip, which
 *   means the way to find out what a round button does is to point at it. A name
 *   beside the action costs a row of pixels and answers the question; it is
 *   drawn on the side the actions fly away from, so the column stays lined up
 *   with the button.
 *
 * Only \c Up and \c Down, and not MUI's \c Left and \c Right: the names are the
 * reason. A row of actions flying sideways has nowhere to put a name that does
 * not shove the next action along the same axis - which is why MUI's sideways
 * dial is icons and tooltips. A row of icon buttons along an edge is a toolbar,
 * and that is another widget.
 *
 * The action next to the button is action 0: the one that was added first is the
 * one the thumb reaches first.
 */
class QX_MATERIAL_EXPORT MaterialSpeedDial : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool open READ isOpen WRITE setOpen NOTIFY openChanged)
    Q_PROPERTY(Direction direction READ direction WRITE setDirection NOTIFY directionChanged)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon)
    Q_PROPERTY(QString text READ text WRITE setText)
public:
    /*! Which way the actions come out. */
    enum class Direction {
        Up,  /*!< Above the button: the dial at the foot of the screen. */
        Down /*!< Below the button: the dial at the top of it. */
    };
    Q_ENUM(Direction)

    explicit MaterialSpeedDial(QWidget *parent = Q_NULLPTR);
    ~MaterialSpeedDial() override;

    /*! Append an action and answer where it landed. \a name is drawn beside it
     *  and is its accessible name. */
    int addAction(const QString &name, const QIcon &icon = QIcon());
    void removeAction(int index);
    void clear();
    int actionCount() const;
    QString actionName(int index) const;
    QIcon actionIcon(int index) const;

    /*! The button's own icon; it is not drawn while the dial is open. */
    QIcon icon() const;
    /*! What the button says, for an assistive tool; not drawn. */
    QString text() const;

    Direction direction() const;
    bool isOpen() const;

    /*!
     * Where action \a index sits, in this widget's own coordinates - which is
     * why the numbers are negative for \c Up and past the height for \c Down:
     * the actions stand outside the button's square. Empty for an index that is
     * not there.
     */
    QRect actionRect(int index) const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setIcon(const QIcon &icon);
    void setText(const QString &text);
    void setActionName(int index, const QString &name);
    void setActionIcon(int index, const QIcon &icon);
    void setDirection(Direction direction);
    void setOpen(bool open);
    void open();
    void close();
    void toggle();

Q_SIGNALS:
    void openChanged(bool open);
    /*! An action was pressed. The dial has already put itself away by then: an
     *  action taken is an answer, and the question is closed. */
    void actionTriggered(int index);
    void directionChanged(Direction direction);
protected:
    void moveEvent(QMoveEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    void changeEvent(QEvent *event) override;
private:
    Q_DISABLE_COPY(MaterialSpeedDial)
    QX_DECLARE_PRIVATE(MaterialSpeedDial)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALSPEEDDIAL_H
