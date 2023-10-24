/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxribbon_global.h"
#include "ribbongroup.h"

#include <QWidgetItem>
#include <QLayout>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonGroupLayoutPrivate;

/**
 * @brief 是对 group 所有子窗口的抽象，参考 qt 的 toolbar
 *
 * 参考 qt 的 toolbar，group 所有子窗口内容都通过 QAction 进行抽象，包括 gallery 这些窗口，也是通过 QAction 进行抽象
 * QAction 最终会转换为 RibbonGroupItem，每个 RibbonGroupItem 都含有一个 widget，RibbonGroup 的布局就基于 RibbonGroupItem
 *
 * 无窗口的 action 会在内部生成一个 RibbonButton
 */
class RibbonGroupItem : public QWidgetItem
{
public:
    RibbonGroupItem(QWidget *widget);
    bool isEmpty() const Q_DECL_OVERRIDE;

    short rowIndex;             ///< 记录当前item属于第几行，hide模式下为-1
    int columnIndex;            ///< 记录当前item属于第几列，hide模式下为-1
    QRect willGeometry;         ///< 在调用RibbonGroupLayout::updateGeomArray会更新该成员，
                                ///< 实际设置的时候会传给QWidgetItem::setGeometry设置Geometry
    QAction *action;            ///< 记录action，参考QToolBarLayoutItem
    bool customWidget;          ///< 对于没有窗口的action，实际也会有一个RibbonButton，在销毁时要delete掉
    RibbonGroup::RowProportion rowProportion; ///< 行的占比，ribbon中有large，medium和small三种占比,见@ref RowProportion
};

/**
 * @brief 针对 RibbonGroup 的布局
 *
 * RibbonGroupLayout 实际是一个列布局，每一列有 2~3 行，看窗口定义占几行
 *
 * 核心函数： @ref RibbonGroupLayout::createItem
 *
 * @note QLayout::contentsMargins 函数不会起作用, 如果要设置 contentsMargins，使用 @sa setGroupContentsMargins
 */
class RibbonGroupLayout : public QLayout
{
    Q_OBJECT
public:
    RibbonGroupLayout(QWidget *p = Q_NULLPTR);
    ~RibbonGroupLayout();

    virtual int indexOf(QAction *action) const;

    void addItem(QLayoutItem *item) Q_DECL_OVERRIDE; // useless

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
