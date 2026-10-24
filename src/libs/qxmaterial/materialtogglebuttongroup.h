/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALTOGGLEBUTTONGROUP_H
#define MATERIALTOGGLEBUTTONGROUP_H

#include "qxmaterial_global.h"

#include <QtGui/QIcon>
#include <QtWidgets/QWidget>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialToggleButtonGroupPrivate;

/*!
 * Material's toggle button group: a row of buttons that are each on or off.
 *
 * \c MaterialSegmentedButton answers "which one of these"; this answers "which
 * of these are on". The two look like relatives - MUI drew them with the same
 * outline and the segment is the toggle's descendant - so where they part has
 * to be said plainly, and they part in three places:
 *
 * - **Nothing has to be on.** A segmented button always has one part in force,
 *   because a row of answers with none in force answers nothing. Here a group
 *   with nothing on is a normal state, and pressing the one that is on turns
 *   it off. \c exclusive says *at most* one is on - never *exactly* one.
 * - **Several may be on.** Without \c exclusive the buttons are independent
 *   switches: "Bold", "Italic", "Underline" is a set, not a choice.
 * - **The buttons are not equal.** A segmented button divides its row evenly,
 *   because the row is one answer made of parts. Here each button is as wide
 *   as its own label and icon ask, because each is its own switch - and the
 *   space left over when the row is stretched is shared out evenly, so the
 *   buttons that asked for more still stand wider.
 *
 * The keyboard divides the same way. Without \c exclusive a group is a row of
 * separate switches, so Tab stops on each of them and Space or Return throws
 * its own switch. An \c exclusive group is one stop with the arrows moving
 * between the buttons, and an arrow carries the choice with it - which is the
 * one place the two widgets agree, because that is what a keyboard does to a
 * radio group.
 *
 * What being on looks like follows from the outline being shared: the edge
 * between two buttons belongs to both of them, so a button that is on does not
 * repaint it. The fill under the label and the label's own colour say it - a
 * segmented button's label stays in the page's ink, because there the outline
 * round the whole row already carries the answer.
 */
class QX_MATERIAL_EXPORT MaterialToggleButtonGroup : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool exclusive READ isExclusive WRITE setExclusive NOTIFY exclusiveChanged)
public:
    explicit MaterialToggleButtonGroup(QWidget *parent = Q_NULLPTR);
    ~MaterialToggleButtonGroup() override;

    /*! Append a button and answer where it landed; \a text is also its
     *  accessible name while the application has not given it a better one. */
    int addButton(const QString &text, const QIcon &icon = QIcon());
    void removeButton(int index);
    void clear();
    int count() const;

    /*! The label of button \a index; empty for an index that is not there. */
    QString buttonText(int index) const;
    QIcon buttonIcon(int index) const;

    /*! Whether at most one button may be on. True by default. */
    bool isExclusive() const;
    bool isChecked(int index) const;
    /*! The buttons that are on, in the order they were added. */
    QList<int> checkedButtons() const;

    /*! Where button \a index sits in this widget's coordinates; an empty
     *  rectangle for an index that is not there. */
    QRect buttonRect(int index) const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    void setButtonText(int index, const QString &text);
    void setButtonIcon(int index, const QIcon &icon);

    /*!
     * Let at most one button be on, or any number of them. Turning this on
     * while several are on leaves the first of them on and turns the rest off,
     * one signal per button that really changed.
     */
    void setExclusive(bool exclusive);
    /*! Put button \a index on or off; an index that is not there is ignored. */
    void setChecked(int index, bool checked);
    /*! Turn every button off. */
    void clearChecked();

Q_SIGNALS:
    /*! Emitted once for each button whose state really changed, whoever
     *  changed it. An exclusive group that passes the choice from one button
     *  to another therefore emits twice: once for the one going off, once for
     *  the one coming on. */
    void buttonToggled(int index, bool checked);
    void exclusiveChanged(bool exclusive);
protected:
    void resizeEvent(QResizeEvent *event) override;
    void changeEvent(QEvent *event) override;
private:
    Q_DISABLE_COPY(MaterialToggleButtonGroup)
    QX_DECLARE_PRIVATE(MaterialToggleButtonGroup)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALTOGGLEBUTTONGROUP_H
