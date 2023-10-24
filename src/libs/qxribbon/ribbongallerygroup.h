/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxglobal.h"
#include <QListView>

class QActionGroup;

QX_BEGIN_NAMESPACE

class RibbonGalleryGroupPrivate;

///
/// \brief Gallery 的组
///
/// 组负责显示管理 Gallery Item
///
class QX_WIDGET_EXPORT RibbonGalleryGroup : public QListView
{
    Q_OBJECT
public:
    /**
     * @brief GalleryGroup显示的样式
     */
    enum GalleryGroupStyle {
        IconWithText,           ///< 图标带文字
        IconWithWordWrapText,   ///< 图标带文字,文字会换行显示，此模式只会对DisplayOneRow生效，如果不是DisplayOneRow，等同IconWithText
        IconOnly                ///< 只有图标
    };

    /**
     * @brief 定义Gallery在一个group下面显示的图标行数
     */
    enum DisplayRow {
        DisplayOneRow = 1,   ///< 显示1行，默认
        DisplayTwoRow = 2,
        DisplayThreeRow = 3
    };
public:
    explicit RibbonGalleryGroup(QWidget *w = Q_NULLPTR);
    virtual ~RibbonGalleryGroup();

    void setRecalcGridSizeBlock(bool on = true);
    bool isRecalcGridSizeBlock() const;
    void recalcGridSize();
    void recalcGridSize(int galleryHeight);

    void setGalleryGroupStyle(GalleryGroupStyle style);
    GalleryGroupStyle galleryGroupStyle() const;

    void addItem(const QString &text, const QIcon &icon);
    void addItem(QAction *act);
    void addItems(const QList<QAction *> &acts);

    void setGroupTitle(const QString &title);
    QString groupTitle() const;

    void selectByIndex(int i);

    void setDisplayRow(DisplayRow r);
    DisplayRow displayRow() const;

    void setGridMinimumWidth(int w);
    int gridMinimumWidth() const;

    void setGridMaximumWidth(int w);
    int gridMaximumWidth() const;

    QActionGroup *actionGroup() const;
private slots:
    void onItemClicked(const QModelIndex &index);
    void onItemEntered(const QModelIndex &index);
signals:
    void groupTitleChanged(const QString &title);
    /**
     * @brief 等同QActionGroup的triggered
     * 所有加入RibbonGalleryGroup的action都会被一个QActionGroup管理,可以通过@sa actionGroup获取到对应的actiongroup
     * @param action
     */
    void triggered(QAction *action);
    /**
     * @brief 等同QActionGroup的triggered
     * 所有加入RibbonGalleryGroup的action都会被一个QActionGroup管理,可以通过@sa actionGroup获取到对应的actiongroup
     * @note 此属性需要通过QAbstractItemView::entered(const QModelIndex &index)激活，因此要保证设置了setMouseTracking(true)
     * @param action
     */
    void hovered(QAction *action);
private:
    QX_DECLARE_PRIVATE(RibbonGalleryGroup)
};

QX_END_NAMESPACE
