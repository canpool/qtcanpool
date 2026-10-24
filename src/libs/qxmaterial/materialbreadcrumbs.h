/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MATERIALBREADCRUMBS_H
#define MATERIALBREADCRUMBS_H

#include "qxmaterial_global.h"

#include "qxcontrols/qxcontrol.h"

#include <QtCore/QStringList>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialBreadcrumbsPrivate;

/*!
 * Material's breadcrumbs: the path to where the user is, each step of it before
 * the last a way back.
 *
 * The same path widget the Fluent family has, and it is a different one for
 * three reasons - all of them Material's, none of them a preference:
 *
 * - **The path folds.** A Fluent breadcrumb bar shows every step it was given;
 *   Material's is a row in a top app bar, where the width belongs to the app
 *   and not to the path. Past \c maxItems steps the middle of the path folds
 *   away behind a mark, and the two ends stay: the start of the path, and where
 *   the user is. Unrolling it shows every step again.
 * - **The ways back wear the accent.** To Material a link is the accent colour,
 *   so a step that leads somewhere is accent ink and the step the user is on is
 *   the page's own ink. The Fluent bar bolds the current step instead; here the
 *   difference is colour, because the colour is what Material says a link is.
 * - **The separator is a character.** Material writes the path as one line of
 *   text - a slash with room on both sides - where the Fluent bar draws a
 *   chevron. It is still the gap and not part of either step: a "/" typed into
 *   a step would be part of the words an assistive tool reads.
 *
 * Four answers worth knowing, which is what a bar that can hide its own steps
 * has to decide:
 *
 * - **Every step is a control of its own.** So the pointer arriving, the press,
 *   the focus ring and the role are answered by the step rather than worked out
 *   by the bar, and an assistive tool can walk the path as a list of links.
 * - **The last step is where the user is.** It is not a link: the pointer
 *   cannot land on it, the keyboard does not stop on it, and choosing it emits
 *   nothing. Nothing about it is a move.
 * - **Unrolling happens once.** The bar does not fold itself back when the
 *   pointer leaves, because a path the user unrolled is a choice they made;
 *   \c collapse, or a new path through \c setItems, folds it again.
 * - **Where the user is never folds away.** \c itemsAfterCollapse is read as at
 *   least one: a bar that hid the step the user is on would be a path that does
 *   not say where they are.
 *
 * The bar asks for the width its steps need and never shortens one. A path too
 * long for its room is a path with too many steps, which is what \c maxItems
 * and the fold are for; a step cut in half would be a step the user cannot
 * read and cannot aim at.
 */
class QX_MATERIAL_EXPORT MaterialBreadcrumbs : public QxControls::QxControl
{
    Q_OBJECT
    Q_PROPERTY(QStringList items READ items WRITE setItems)
    Q_PROPERTY(int maxItems READ maxItems WRITE setMaxItems)
    Q_PROPERTY(int itemsBeforeCollapse READ itemsBeforeCollapse WRITE setItemsBeforeCollapse)
    Q_PROPERTY(int itemsAfterCollapse READ itemsAfterCollapse WRITE setItemsAfterCollapse)
    Q_PROPERTY(QString expandText READ expandText WRITE setExpandText)
public:
    explicit MaterialBreadcrumbs(QWidget *parent = Q_NULLPTR);
    ~MaterialBreadcrumbs() override;

    QStringList items() const;
    /*! How many steps the bar shows before it folds the middle of the path
     *  away. \c 0 or less never folds - and so does a path that is not longer
     *  than this. Eight by default, which is where MUI puts it. */
    int maxItems() const;
    /*! How many steps of the head of the path stay when it folds. */
    int itemsBeforeCollapse() const;
    /*! How many steps of its tail stay when it folds - read as at least one. */
    int itemsAfterCollapse() const;
    /*! What unrolling the path is called, for an assistive tool. MUI spells it
     *  "Show path"; it is not drawn, because the mark stands for it. */
    QString expandText() const;

    /*! Whether the path is unrolled - either because it was short enough to
     *  show whole, or because the user asked to see the rest of it. */
    bool isExpanded() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public Q_SLOTS:
    /*! Give the bar \a items to show, with the last one where the user is. A
     *  new path is not the old one: it is shown folded. */
    void setItems(const QStringList &items);
    void setMaxItems(int count);
    void setItemsBeforeCollapse(int count);
    void setItemsAfterCollapse(int count);
    void setExpandText(const QString &text);

    /*! Show every step, once. Folds it back to the steps a bar of this width
     *  shows - which is the same call whether or not it was unrolled. */
    void collapse();
    void expand();

Q_SIGNALS:
    /*! Step \a index was taken: pressed, or answered with the keyboard. The
     *  last step never is, because it is where the user already is. */
    void itemClicked(int index, const QString &text);
    /*! The path was unrolled because the user asked for the steps the fold was
     *  hiding. Emitted once per unrolling, and not for a path that had nothing
     *  folded. */
    void expanded();
protected:
    QRectF chromeRect() const override;
    void paintContent(QPainter *painter) override;

    void resizeEvent(QResizeEvent *event) override;
    void changeEvent(QEvent *event) override;

    QAccessible::Role accessibleRole() const override;
private:
    Q_DISABLE_COPY(MaterialBreadcrumbs)
    QX_DECLARE_PRIVATE(MaterialBreadcrumbs)
};

QX_MATERIAL_END_NAMESPACE

#endif   // MATERIALBREADCRUMBS_H
