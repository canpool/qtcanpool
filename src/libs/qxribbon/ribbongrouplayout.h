/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include "ribbongroup.h"

#include <QWidgetItem>
#include <QLayout>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonGroupLayoutPrivate;

/**
 * @brief An abstraction of all child widgets of a group, modeled after the Qt toolbar
 *
 * Following the Qt toolbar, all child widget content of a group is abstracted through QAction,
 * including widgets such as the gallery; QAction is ultimately converted into a RibbonGroupItem,
 * each of which contains a widget, and the RibbonGroup layout is based on RibbonGroupItem
 *
 * An action without a widget creates a RibbonButton internally
 */
class RibbonGroupItem : public QWidgetItem
{
public:
    RibbonGroupItem(QWidget *widget);
    bool isEmpty() const Q_DECL_OVERRIDE;

    short rowIndex;       ///< Records which row the current item belongs to; -1 in hide mode
    int columnIndex;      ///< Records which column the current item belongs to; -1 in hide mode
    QRect willGeometry;   ///< Updated when RibbonGroupLayout::updateGeomArray is called,
                          ///< passed to QWidgetItem::setGeometry to set the geometry when applied
    QAction *action;      ///< Records the action; see QToolBarLayoutItem
    bool customWidget;    ///< For an action without a widget a RibbonButton still exists and must be deleted on
                          ///< destruction
    RibbonGroup::RowProportion rowProportion;   ///< Row proportion; ribbon has three kinds, large, medium and small,
                                                ///< see @ref RibbonGroup::RowProportion
};

/**
 * @brief Layout for RibbonGroup
 *
 * RibbonGroupLayout is actually a column layout where each column has 2~3 rows, depending on how
 * many rows the widgets are defined to occupy
 *
 * Core function: createItem (converts a QAction into the layout item RibbonGroupItem)
 *
 * @note QLayout::contentsMargins has no effect; to set contentsMargins use @sa setGroupContentsMargins
 */
class RibbonGroupLayout : public QLayout
{
    Q_OBJECT
public:
    RibbonGroupLayout(QWidget *p = Q_NULLPTR);
    ~RibbonGroupLayout();

    virtual int indexOf(QAction *action) const;

    void addItem(QLayoutItem *item) Q_DECL_OVERRIDE;   // useless

    void insertAction(int index, QAction *act, RibbonGroup::RowProportion rp = RibbonGroup::Auto);

    QLayoutItem *itemAt(int index) const Q_DECL_OVERRIDE;
    QLayoutItem *takeAt(int index) Q_DECL_OVERRIDE;
    int count() const Q_DECL_OVERRIDE;

    bool isEmpty() const Q_DECL_OVERRIDE;
    void invalidate() Q_DECL_OVERRIDE;
    Qt::Orientations expandingDirections() const Q_DECL_OVERRIDE;

    void setGeometry(const QRect &rect) Q_DECL_OVERRIDE;

    QSize minimumSize() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

    RibbonGroupItem *groupItem(QAction *action) const;
    RibbonGroupItem *lastItem() const;

    QWidget *lastWidget() const;

    void move(int from, int to);

    bool isDirty() const;

    static int calcLargeHeight(const QRect &setrect, const RibbonGroup *group);

    static const QMargins &groupContentsMargins();
    static void setGroupContentsMargins(const QMargins &m);
protected:
    const QList<RibbonGroupItem *> &ribbonGroupItems() const;
private:
    QX_DECLARE_PRIVATE(RibbonGroupLayout)
    friend class RibbonGroup;
    friend class RibbonGroupPrivate;
};

QX_RIBBON_END_NAMESPACE
