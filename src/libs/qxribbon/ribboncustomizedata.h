/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "qxribbon_global.h"
#include "ribbonactionsmanager.h"
#include "ribbongroup.h"

#include <QList>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonWindow;

/**
 * @brief 记录所有自定义操作的数据类
 * @note 此数据依赖于 @ref RibbonActionsManager 要在 RibbonActionsManager 之后使用此类
 */
class RibbonCustomizeData
{
public:
    enum ActionType {
        UnknowActionType = 0,           ///< 未知操作
        AddPageActionType,              ///< 添加page操作
        AddGroupActionType,             ///< 添加group操作
        AddActionActionType,            ///< 添加action操作
        RemovePageActionType,           ///< 删除page操作
        RemoveGroupActionType,          ///< 删除group操作
        RemoveActionActionType,         ///< 删除action操作
        ChangePageOrderActionType,      ///< 改变page顺序的操作
        ChangeGroupOrderActionType,     ///< 改变group顺序的操作
        ChangeActionOrderActionType,    ///< 改变action顺序的操作
        RenamePageActionType,           ///< 对page更名操作
        RenameGroupActionType,          ///< 对Group更名操作
        VisiblePageActionType,          ///< 对page执行隐藏/显示操作
    };
public:
    RibbonCustomizeData();
    RibbonCustomizeData(ActionType type, RibbonActionsManager *mgr = Q_NULLPTR);

    // 获取CustomizeData的action type
    ActionType actionType() const;

    // 设置CustomizeData的action type
    void setActionType(ActionType a);

    // 判断是否是一个正常的CustomizeData
    bool isValid() const;

    // 应用RibbonCustomizeData
    bool apply(RibbonWindow *m);

    // 获取actionsmanager指针
    RibbonActionsManager *actionsManager();

    // 设置ActionsManager
    void setActionsManager(RibbonActionsManager *mgr);

    // 对应AddPageActionType
    static RibbonCustomizeData makeAddPageCustomizeData(const QString &title, int index, const QString &objName);

    // 对应AddGroupActionType
    static RibbonCustomizeData makeAddGroupCustomizeData(const QString &title, int index,
                                                           const QString &pageobjName, const QString &objName);

    // 对应AddActionActionType
    static RibbonCustomizeData makeAddActionCustomizeData(const QString &key, RibbonActionsManager *mgr,
                                                           RibbonGroup::RowProportion rp,
                                                           const QString &pageObjName,
                                                           const QString &groupObjName);

    // 对应RenamePageActionType
    static RibbonCustomizeData makeRenamePageCustomizeData(const QString &newname, const QString &pageobjName);

    // 对应RenameGroupActionType
    static RibbonCustomizeData makeRenameGroupCustomizeData(const QString &newname, const QString &pageobjName,
                                                              const QString &groupObjName);

    // 对应RemovePageActionType
    static RibbonCustomizeData makeRemovePageCustomizeData(const QString &pageobjName);

    // 对应ChangePageOrderActionType
    static RibbonCustomizeData makeChangePageOrderCustomizeData(const QString &pageobjName, int moveindex);

    // 对应ChangeGroupOrderActionType
    static RibbonCustomizeData makeChangeGroupOrderCustomizeData(const QString &pageobjName,
                                                                   const QString &groupObjName, int moveindex);

    // 对应ChangeActionOrderActionType
    static RibbonCustomizeData makeChangeActionOrderCustomizeData(const QString &pageobjName,
                                                                   const QString &groupObjName, const QString &key,
                                                                   RibbonActionsManager *mgr, int moveindex);

    // 对应RemoveGroupActionType
    static RibbonCustomizeData makeRemoveGroupCustomizeData(const QString &pageobjName,
                                                              const QString &groupObjName);

    // 对应RemoveActionActionType
    static RibbonCustomizeData makeRemoveActionCustomizeData(const QString &pageobjName,
                                                              const QString &groupObjName, const QString &key,
                                                              RibbonActionsManager *mgr);

    // 对应VisiblePageActionType
    static RibbonCustomizeData makeVisiblePageCustomizeData(const QString &pageobjName, bool isShow);

    // 判断是否可以自定义,如果某个action不想被编辑，可以通过此函数设置
    static bool isCanCustomize(QObject *obj);
    static void setCanCustomize(QObject *obj, bool canbe = true);

    // 对QList<RibbonCustomizeData>进行简化
    static QList<RibbonCustomizeData> simplify(const QList<RibbonCustomizeData> &csd);
public:
    /**
     * @brief 记录顺序的参数
     *
     * 在actionType==AddPageActionType时，此参数记录Page的insert位置,
     * 在actionType==AddGroupActionType时，此参数记录group的insert位置,
     * 在actionType==AddActionActionType时，此参数记录group的insert位置
     */
    int indexValue;

    /**
     * @brief 记录标题、索引等参数
     *
     * 在actionType==AddPageActionType时，key为page标题，
     * 在actionType==AddGroupActionType时，key为group标题，
     * 在actionType==AddActionActionType时，key为action的查询依据，基于RibbonActionsManager::action查询
     */
    QString keyValue;

    /**
     * @brief 记录pageObjName，用于定位Page
     */
    QString pageObjNameValue;

    /**
     * @brief 记录groupObjName，ribbon的Customize索引大部分基于objname
     */
    QString groupObjNameValue;

    ///< 行的占比，ribbon中有large，medium和small三种占比,见@ref RowProportion
    RibbonGroup::RowProportion actionRowProportionValue;
private:
    ActionType m_type;              ///< 标记这个data是page还是group亦或是action
    RibbonActionsManager *m_actionsManager;
    bool customized;
};

typedef QList<RibbonCustomizeData> RibbonCustomizeDataList;

QX_RIBBON_END_NAMESPACE

Q_DECLARE_METATYPE(QX_RIBBON_PREPEND_NAMESPACE(RibbonCustomizeData))
