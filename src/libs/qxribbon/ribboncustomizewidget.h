/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include "ribbonactionsmanager.h"
#include "ribboncustomizedata.h"
#include "ribbongroup.h"

#include <QWidget>

class QStandardItemModel;
class QStandardItem;
class QAbstractButton;
class QXmlStreamWriter;
class QXmlStreamReader;

QX_RIBBON_BEGIN_NAMESPACE

class RibbonCustomizeWidgetUi;
class RibbonCustomizeWidgetPrivate;
class RibbonBar;

/**
 * @brief 自定义界面窗口
 */
class RibbonCustomizeWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief 定义ribbon树的显示类型
     */
    enum RibbonTreeShowType {
        ShowAllPage,   ///< 显示所有的页，包括上下文
        ShowMainPage   ///< 显示主要的页，不包含上下文
    };

    /**
     * @brief QStandardItem对应的role
     */
    enum ItemRole {
        LevelRole = Qt::UserRole + 1,   ///< 代表这是层级，有0：page 1：group 2：item
        PointerRole,                    ///< 代表这是存放指针。根据LevelRole来进行转
        CanCustomizeRole,               ///< 代表个item是可以自定义的.bool
        CustomizeRole,   ///< 代表这个是自定义的item,bool,主要用于那些自己添加的标签和group，有此角色必有CanCustomizeRole
        CustomizeObjNameRole,   ///< 记录了临时的自定义内容的obj名 QString
    };
public:
    RibbonCustomizeWidget(RibbonBar *ribbonBar, QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~RibbonCustomizeWidget();

    // 设置action管理器
    void setupActionsManager(RibbonActionsManager *mgr);

    // 判断用户是否有改动内容
    bool isChanged() const;

    // 获取model
    const QStandardItemModel *model() const;

    // 根据当前的radiobutton选项来更新model
    void updateModel();

    // 更新model
    void updateModel(RibbonTreeShowType type);

    // 应用所有的设定
    bool apply();

    // 转换为xml
    bool toXml(QXmlStreamWriter *xml) const;
    bool toXml(const QString &xmlpath) const;

    // 从xml中加载QList<RibbonCustomizeData>，对于基于配置文件的设置，对话框显示前建议调用此函数，保证叠加设置的正确记录
    void fromXml(QXmlStreamReader *xml);
    void fromXml(const QString &xmlpath);

    // 应用xml配置，可以结合customize_datas_from_xml和customize_datas_apply函数
    static bool fromXml(QXmlStreamReader *xml, RibbonBar *bar, RibbonActionsManager *mgr);

    // 清除所有动作，在执行applys函数后，如果要继续调用，应该clear，否则会导致异常
    void clear();
protected:
    // 把QList<RibbonCustomizeData>进行裁剪,把一些动作合并
    void simplify();

    RibbonGroup::RowProportion selectedRowProportion() const;

    QAction *selectedAction() const;
    QAction *itemToAction(QStandardItem *item) const;

    QStandardItem *selectedItem() const;

    // 获取选中的ribbon tree 的level
    int selectedRibbonLevel() const;

    // 根据选中的item判断
    int itemLevel(QStandardItem *item) const;

    // 设置某个item被选中
    void setSelectItem(QStandardItem *item, bool ensureVisible = true);

    // 判断itemn能否改动，可以改动返回true
    bool isItemCanCustomize(QStandardItem *item) const;
    bool isSelectedItemCanCustomize() const;

    // 判断item是否是自定义的item
    bool isCustomizeItem(QStandardItem *item) const;
    bool isSelectedItemIsCustomize() const;

    // 删除一个item
    void removeItem(QStandardItem *item);

private slots:
    void onComboBoxActionIndexCurrentIndexChanged(int index);
    void onRadioButtonGroupButtonClicked(QAbstractButton *b);
    void onPushButtonNewPageClicked();
    void onPushButtonNewGroupClicked();
    void onPushButtonRenameClicked();
    void onPushButtonAddClicked();
    void onPushButtonDeleteClicked();
    void onListViewSelectClicked(const QModelIndex &index);
    void onTreeViewResultClicked(const QModelIndex &index);
    void onToolButtonUpClicked();
    void onToolButtonDownClicked();
    void onItemChanged(QStandardItem *item);
    void onLineEditSearchActionTextEdited(const QString &text);
    void onPushButtonResetClicked();
private:
    void initConnection();
private:
    RibbonCustomizeWidgetUi *ui;
    RibbonCustomizeWidgetPrivate *d;
};

QX_RIBBON_END_NAMESPACE
