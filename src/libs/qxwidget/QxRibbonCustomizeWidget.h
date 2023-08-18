/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "QxGlobal.h"
#include "QxRibbonActionsManager.h"
#include "QxRibbonCustomizeData.h"
#include "QxRibbonGroup.h"

#include <QWidget>


class QStandardItemModel;
class QStandardItem;
class QAbstractButton;
class QXmlStreamWriter;
class QXmlStreamReader;

QX_BEGIN_NAMESPACE

class RibbonCustomizeWidgetUi;
class RibbonCustomizeWidgetPrivate;
class RibbonWindow;

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
        LevelRole = Qt::UserRole + 1,          ///< 代表这是层级，有0：page 1：group 2：item
        PointerRole,        ///< 代表这是存放指针。根据LevelRole来进行转
        CanCustomizeRole,   ///< 代表个item是可以自定义的.bool
        CustomizeRole,///< 代表这个是自定义的item,bool,主要用于那些自己添加的标签和group，有此角色必有CanCustomizeRole
        CustomizeObjNameRole,   ///< 记录了临时的自定义内容的obj名 QString
    };
public:
    RibbonCustomizeWidget(RibbonWindow *ribbonWindow, QWidget *parent = Q_NULLPTR,
                           Qt::WindowFlags f = Qt::WindowFlags());
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
    bool applys();

    // 转换为xml
    bool toXml(QXmlStreamWriter *xml) const;
    bool toXml(const QString &xmlpath) const;

    // 从xml中加载QList<RibbonCustomizeData>，对于基于配置文件的设置，对话框显示前建议调用此函数，保证叠加设置的正确记录
    void fromXml(QXmlStreamReader *xml);
    void fromXml(const QString &xmlpath);

    // 应用xml配置，可以结合customize_datas_from_xml和customize_datas_apply函数
    static bool fromXml(QXmlStreamReader *xml, RibbonWindow *w, RibbonActionsManager *mgr);

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

/**
 * @brief 转换为xml
 *
 * 此函数仅会写element，不会写document相关内容，因此如果需要写document，
 * 需要在此函数前调用QXmlStreamWriter::writeStartDocument(),在此函数后调用QXmlStreamWriter::writeEndDocument()
 * @param xml QXmlStreamWriter指针
 * @note 注意，在传入QXmlStreamWriter之前，需要设置编码为utf-8:xml->setCodec("utf-8");
 * @note 由于QXmlStreamWriter在QString作为io时，是不支持编码的，而此又无法保证自定义过程不出现中文字符，
 * 因此，QXmlStreamWriter不应该通过QString进行构造，如果需要用到string，也需要通过QByteArray构造，如：
 * @param cds 基于QList<RibbonCustomizeData>生成的步骤
 * @return 如果出现异常，返回false,如果没有自定义数据也会返回false
 */
bool QxRibbonCustomizeDataSetToXml(QXmlStreamWriter *xml, const QList<RibbonCustomizeData> &cds);

/**
 * @brief 通过xml获取QList<RibbonCustomizeData>
 * @param xml
 * @return QList<RibbonCustomizeData>
 */
QList<RibbonCustomizeData> QxRibbonCustomizeDataGetFromXml(QXmlStreamReader *xml, RibbonActionsManager *mgr);

/**
 * @brief 应用QList<RibbonCustomizeData>
 * @param cds
 * @param w RibbonWindow指针
 * @return 成功应用的个数
 */
int QxRibbonCustomizeDataApply(QList<RibbonCustomizeData> &cds, RibbonWindow *w);

QX_END_NAMESPACE
