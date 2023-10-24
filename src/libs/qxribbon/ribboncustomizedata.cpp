/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#include "ribboncustomizedata.h"
#include "ribbonbar.h"
#include "ribbonwindow.h"

#include <QDebug>
#include <QObject>

QX_RIBBON_BEGIN_NAMESPACE

/**
 * @def 属性，用于标记是否可以进行自定义，用于动态设置到 @ref RibbonPage 和 @ref RibbonGroup
 * 值为bool，在为true时，可以通过 @ref RibbonCustomizeWidget 改变这个RibbonPage和RibbonGroup的布局，
 * 默认不会有此属性，仅在有此属性且为true时才会在RibbonCustomizeWidget中能显示为可设置
 */
#ifndef QX_RIBBON_PROP_CAN_CUSTOMIZE
#define QX_RIBBON_PROP_CAN_CUSTOMIZE "_qx_isCanCustomize"
#endif

QList<RibbonCustomizeData> remove_indexs(const QList<RibbonCustomizeData> &csd, const QList<int> &willRemoveIndexs)
{
    QList<RibbonCustomizeData> res;

    for (int i = 0; i < csd.size(); ++i) {
        if (!willRemoveIndexs.contains(i)) {
            res << csd[i];
        }
    }
    return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/// RibbonCustomizeData
////////////////////////////////////////////////////////////////////////////////////////////////////////

RibbonCustomizeData::RibbonCustomizeData()
    : indexValue(-1)
    , actionRowProportionValue(RibbonGroup::Large)
    , m_type(UnknowActionType)
    , m_actionsManager(Q_NULLPTR)
    , customized(false)
{
}

RibbonCustomizeData::RibbonCustomizeData(ActionType type, RibbonActionsManager *mgr)
    : indexValue(-1)
    , actionRowProportionValue(RibbonGroup::Large)
    , m_type(type)
    , m_actionsManager(mgr)
    , customized(false)
{
}

/**
 * @brief 获取CustomizeData的action type
 * @return
 */
RibbonCustomizeData::ActionType RibbonCustomizeData::actionType() const
{
    return m_type;
}

/**
 * @brief 设置CustomizeData的action type
 * @param a
 */
void RibbonCustomizeData::setActionType(RibbonCustomizeData::ActionType a)
{
    m_type = a;
}

/**
 * @brief 判断是否是一个正常的CustomizeData
 *
 * 实际逻辑actionType() != UnknowActionType
 * @return 有用的CustomizeData返回true
 */
bool RibbonCustomizeData::isValid() const
{
    return (actionType() != UnknowActionType);
}

/**
 * @brief 应用RibbonCustomizeData到RibbonWindow
 * @param m
 * @return 如果应用失败，返回false,如果actionType==UnknowActionType直接返回false
 */
bool RibbonCustomizeData::apply(RibbonWindow *m)
{
    if (customized) {
        return true;
    }
    RibbonBar *bar = m->ribbonBar();
    if (Q_NULLPTR == bar) {
        return false;
    }
    switch (actionType()) {
    case UnknowActionType:
        return false;

    case AddPageActionType: {
        // 添加标签
        RibbonPage *page = bar->insertPage(indexValue, keyValue);
        if (Q_NULLPTR == page) {
            return false;
        }
        page->setObjectName(pageObjNameValue);
        RibbonCustomizeData::setCanCustomize(page);
        break;
    }

    case AddGroupActionType: {
        // 添加group
        RibbonPage *page = bar->pageByObjectName(pageObjNameValue);
        if (Q_NULLPTR == page) {
            return false;
        }
        RibbonGroup *group = page->insertGroup(indexValue, keyValue);
        group->setObjectName(groupObjNameValue);
        RibbonCustomizeData::setCanCustomize(group);
        break;
    }

    case AddActionActionType: {
        if (Q_NULLPTR == m_actionsManager) {
            return false;
        }
        RibbonPage *page = bar->pageByObjectName(pageObjNameValue);
        if (Q_NULLPTR == page) {
            return false;
        }
        RibbonGroup *group = page->groupByObjectName(groupObjNameValue);
        if (Q_NULLPTR == group) {
            return false;
        }
        QAction *act = m_actionsManager->action(keyValue);
        if (Q_NULLPTR == act) {
            return false;
        }
        RibbonCustomizeData::setCanCustomize(act);
        group->addAction(act, actionRowProportionValue);
        break;
    }

    case RemovePageActionType: {
        RibbonPage *page = bar->pageByObjectName(pageObjNameValue);
        if (Q_NULLPTR == page) {
            return false;
        }
        bar->removePage(page);
        break;
    }

    case RemoveGroupActionType: {
        RibbonPage *page = bar->pageByObjectName(pageObjNameValue);
        if (Q_NULLPTR == page) {
            return false;
        }
        RibbonGroup *group = page->groupByObjectName(groupObjNameValue);
        if (Q_NULLPTR == group) {
            return false;
        }
        page->removeGroup(group);
        break;
    }

    case RemoveActionActionType: {
        RibbonPage *page = bar->pageByObjectName(pageObjNameValue);
        if (Q_NULLPTR == page) {
            return false;
        }
        RibbonGroup *group = page->groupByObjectName(groupObjNameValue);
        if (Q_NULLPTR == group) {
            return false;
        }
        QAction *act = m_actionsManager->action(keyValue);
        if (Q_NULLPTR == act) {
            return false;
        }
        group->removeAction(act);
        break;
    }

    case ChangePageOrderActionType: {
        RibbonPage *page = bar->pageByObjectName(pageObjNameValue);
        if (Q_NULLPTR == page) {
            return false;
        }
        int currentindex = bar->pageIndex(page);
        if (-1 == currentindex) {
            return false;
        }
        int toindex = currentindex + indexValue;
        bar->movePage(currentindex, toindex);
        break;
    }

    case ChangeGroupOrderActionType: {
        RibbonPage *page = bar->pageByObjectName(pageObjNameValue);
        if (Q_NULLPTR == page) {
            return false;
        }
        RibbonGroup *group = page->groupByObjectName(groupObjNameValue);
        if (Q_NULLPTR == group) {
            return false;
        }
        int groupIndex = page->groupIndex(group);
        if (-1 == groupIndex) {
            return false;
        }
        page->moveGroup(groupIndex, groupIndex + indexValue);
        break;
    }

    case ChangeActionOrderActionType: {
        RibbonPage *page = bar->pageByObjectName(pageObjNameValue);
        if (Q_NULLPTR == page) {
            return false;
        }
        RibbonGroup *group = page->groupByObjectName(groupObjNameValue);
        if (Q_NULLPTR == group) {
            return false;
        }
        QAction *act = m_actionsManager->action(keyValue);
        if (Q_NULLPTR == act) {
            return false;
        }
        int actindex = group->actionIndex(act);
        if (actindex <= -1) {
            return false;
        }
        group->moveAction(actindex, actindex + indexValue);
        break;
    }

    case RenamePageActionType: {
        RibbonPage *page = bar->pageByObjectName(pageObjNameValue);
        if (Q_NULLPTR == page) {
            return false;
        }
        page->setWindowTitle(keyValue);
        break;
    }

    case RenameGroupActionType: {
        RibbonPage *page = bar->pageByObjectName(pageObjNameValue);
        if (Q_NULLPTR == page) {
            return false;
        }
        RibbonGroup *group = page->groupByObjectName(groupObjNameValue);
        if (Q_NULLPTR == group) {
            return false;
        }
        group->setWindowTitle(keyValue);
        break;
    }

    case VisiblePageActionType: {
        RibbonPage *page = bar->pageByObjectName(pageObjNameValue);
        if (Q_NULLPTR == page) {
            return false;
        }
        if (1 == indexValue) {
            bar->showPage(page);
        } else {
            bar->hidePage(page);
        }
        break;
    }

    default:
        return false;
    }
    customized = true;
    return true;
}

/**
 * @brief 获取actionsmanager指针
 * @return
 */
RibbonActionsManager *RibbonCustomizeData::actionsManager()
{
    return m_actionsManager;
}

/**
 * @brief 设置ActionsManager
 * @param mgr
 */
void RibbonCustomizeData::setActionsManager(RibbonActionsManager *mgr)
{
    m_actionsManager = mgr;
}

/**
 * @brief 创建一个AddPageActionType的RibbonCustomizeData
 * @param title page 的标题
 * @param index page要插入的位置
 * @param objName page的object name
 * @return 返回AddPageActionType的RibbonCustomizeData
 */
RibbonCustomizeData RibbonCustomizeData::makeAddPageCustomizeData(const QString &title, int index,
                                                                        const QString &objName)
{
    RibbonCustomizeData data(AddPageActionType);

    data.indexValue = index;
    data.keyValue = title;
    data.pageObjNameValue = objName;
    return data;
}

/**
 * @brief 创建一个AddGroupActionType的RibbonCustomizeData
 * @param title group的标题
 * @param index group的index
 * @param pageobjName group的page的objectname
 * @param objName group的objname
 * @return 返回AddGroupActionType的RibbonCustomizeData
 */
RibbonCustomizeData RibbonCustomizeData::makeAddGroupCustomizeData(const QString &title, int index,
                                                                      const QString &pageobjName,
                                                                      const QString &objName)
{
    RibbonCustomizeData data(AddGroupActionType);

    data.indexValue = index;
    data.keyValue = title;
    data.groupObjNameValue = objName;
    data.pageObjNameValue = pageobjName;
    return data;
}

/**
 * @brief 添加action
 * @param key action的索引
 * @param mgr action管理器
 * @param rp 定义action的占位情况
 * @param pageObjName action添加到的page的objname
 * @param groupObjName action添加到的page下的group的objname
 * @param index action添加到的group的索引
 * @return
 */
RibbonCustomizeData RibbonCustomizeData::makeAddActionCustomizeData(const QString &key, RibbonActionsManager *mgr,
                                                                      RibbonGroup::RowProportion rp,
                                                                      const QString &pageObjName,
                                                                      const QString &groupObjName)
{
    RibbonCustomizeData data(AddActionActionType, mgr);

    data.keyValue = key;
    data.pageObjNameValue = pageObjName;
    data.groupObjNameValue = groupObjName;
    data.actionRowProportionValue = rp;

    return data;
}

/**
 * @brief 创建一个RenamePageActionType的RibbonCustomizeData
 * @param newname 新名字
 * @param index page的索引
 * @return 返回RenamePageActionType的RibbonCustomizeData
 */
RibbonCustomizeData RibbonCustomizeData::makeRenamePageCustomizeData(const QString &newname,
                                                                           const QString &pageobjName)
{
    RibbonCustomizeData data(RenamePageActionType);

    if (pageobjName.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize rename page,"
                                "but get an empty page object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
    data.keyValue = newname;
    data.pageObjNameValue = pageobjName;
    return data;
}

/**
 * @brief 创建一个RenameGroupActionType的RibbonCustomizeData
 * @param newname group的名字
 * @param indexValue group的索引
 * @param pageobjName group对应的page的object name
 * @return 返回RenameGroupActionType的RibbonCustomizeData
 */
RibbonCustomizeData RibbonCustomizeData::makeRenameGroupCustomizeData(const QString &newname,
                                                                         const QString &pageobjName,
                                                                         const QString &groupObjName)
{
    RibbonCustomizeData data(RenameGroupActionType);

    if (groupObjName.isEmpty() || pageobjName.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize rename group,"
                                "but get an empty page/group object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
    data.keyValue = newname;
    data.groupObjNameValue = groupObjName;
    data.pageObjNameValue = pageobjName;
    return data;
}

/**
 * @brief 对应ChangePageOrderActionType
 * @param pageobjName 需要移动的pageobjName
 * @param moveindex 移动位置，-1代表向上（向左）移动一个位置，1带表向下（向右）移动一个位置
 * @return
 */
RibbonCustomizeData RibbonCustomizeData::makeChangePageOrderCustomizeData(const QString &pageobjName,
                                                                                int moveindex)
{
    RibbonCustomizeData data(ChangePageOrderActionType);

    if (pageobjName.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize change page order,"
                                "but get an empty page object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
    data.pageObjNameValue = pageobjName;
    data.indexValue = moveindex;
    return data;
}

/**
 * @brief 对应ChangeGroupOrderActionType
 * @param pageobjName 需要移动的group对应的pageobjName
 * @param groupObjName 需要移动的groupObjName
 * @param moveindex 移动位置，-1代表向上（向左）移动一个位置，1带表向下（向右）移动一个位置
 * @return
 */
RibbonCustomizeData RibbonCustomizeData::makeChangeGroupOrderCustomizeData(const QString &pageobjName,
                                                                              const QString &groupObjName,
                                                                              int moveindex)
{
    RibbonCustomizeData data(ChangeGroupOrderActionType);

    if (pageobjName.isEmpty() || groupObjName.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize change group order,"
                                "but get an empty page/group object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
    data.pageObjNameValue = pageobjName;
    data.groupObjNameValue = groupObjName;
    data.indexValue = moveindex;
    return data;
}

/**
 * @brief 对应ChangeActionOrderActionType
 * @param pageobjName 需要移动的group对应的pageobjName
 * @param groupObjName 需要移动的groupObjName
 * @param key RibbonActionsManager管理的key名
 * @param mgr RibbonActionsManager指针
 * @param moveindex 移动位置，-1代表向上（向左）移动一个位置，1带表向下（向右）移动一个位置
 * @return
 */
RibbonCustomizeData RibbonCustomizeData::makeChangeActionOrderCustomizeData(const QString &pageobjName,
                                                                              const QString &groupObjName,
                                                                              const QString &key,
                                                                              RibbonActionsManager *mgr, int moveindex)
{
    RibbonCustomizeData data(ChangeActionOrderActionType, mgr);

    if (pageobjName.isEmpty() || groupObjName.isEmpty() || key.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize change action order,"
                                "but get an empty page/group/action object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
    data.pageObjNameValue = pageobjName;
    data.groupObjNameValue = groupObjName;
    data.keyValue = key;
    data.indexValue = moveindex;
    return data;
}

/**
 * @brief 对应RemovePageActionType
 * @param pageobjName 需要移除的objname
 * @return
 */
RibbonCustomizeData RibbonCustomizeData::makeRemovePageCustomizeData(const QString &pageobjName)
{
    RibbonCustomizeData data(RemovePageActionType);

    if (pageobjName.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize remove page,"
                                "but get an empty page object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
    data.pageObjNameValue = pageobjName;
    return data;
}

/**
 * @brief 对应RemoveGroupActionType
 * @param pageobjName group对应的page的obj name
 * @param groupObjName group对应的 obj name
 * @return
 */
RibbonCustomizeData RibbonCustomizeData::makeRemoveGroupCustomizeData(const QString &pageobjName,
                                                                         const QString &groupObjName)
{
    RibbonCustomizeData data(RemoveGroupActionType);

    if (pageobjName.isEmpty() || groupObjName.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize remove group,"
                                "but get an empty page/group object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
    data.pageObjNameValue = pageobjName;
    data.groupObjNameValue = groupObjName;
    return data;
}

/**
 * @brief 对应RemoveActionActionType
 * @param pageobjName group对应的page的obj name
 * @param groupObjName group对应的 obj name
 * @param key RibbonActionsManager管理的key名
 * @param mgr RibbonActionsManager指针
 * @return
 */
RibbonCustomizeData RibbonCustomizeData::makeRemoveActionCustomizeData(const QString &pageobjName,
                                                                         const QString &groupObjName,
                                                                         const QString &key, RibbonActionsManager *mgr)
{
    RibbonCustomizeData data(RemoveActionActionType, mgr);

    if (pageobjName.isEmpty() || groupObjName.isEmpty() || key.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize remove action,"
                                "but get an empty page/group/action object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
    data.pageObjNameValue = pageobjName;
    data.groupObjNameValue = groupObjName;
    data.keyValue = key;
    return data;
}

/**
 * @brief RibbonCustomizeData::makeVisiblePageCustomizeData
 * @param pageobjName
 * @param isShow
 * @return
 */
RibbonCustomizeData RibbonCustomizeData::makeVisiblePageCustomizeData(const QString &pageobjName, bool isShow)
{
    RibbonCustomizeData data(VisiblePageActionType);

    if (pageobjName.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize visible page,"
                                "but get an empty page object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
    data.pageObjNameValue = pageobjName;
    data.indexValue = isShow ? 1 : 0;
    return data;
}

/**
 * @brief 判断外置属性，是否允许自定义
 * @param obj
 * @return
 */
bool RibbonCustomizeData::isCanCustomize(QObject *obj)
{
    QVariant v = obj->property(QX_RIBBON_PROP_CAN_CUSTOMIZE);

    if (v.isValid()) {
        return v.toBool();
    }
    return false;
}

/**
 * @brief 设置外置属性允许自定义
 * @param obj
 * @param canbe
 */
void RibbonCustomizeData::setCanCustomize(QObject *obj, bool canbe)
{
    obj->setProperty(QX_RIBBON_PROP_CAN_CUSTOMIZE, canbe);
}

/**
 * @brief 对QList<RibbonCustomizeData>进行简化操作
 *
 * 此函数会执行如下操作：
 * 1、针对同一个元素出现添加和删除操作的进行移除（先添加，后删除）
 * 2、针对VisiblePageActionType，对于多次出现的操作只保留最后一步
 * 3、针对RenamePageActionType和RenameGroupActionType操作，只保留最后一个
 * 4、针对连续的ChangePageOrderActionType，ChangeGroupOrderActionType，ChangeActionOrderActionType进行合并为一个动作，
 * 如果合并后原地不动，则删除
 *
 * @param csd
 * @return 返回简化的QList<RibbonCustomizeData>
 */
QList<RibbonCustomizeData> RibbonCustomizeData::simplify(const QList<RibbonCustomizeData> &csd)
{
    int size = csd.size();

    if (size <= 1) {
        return csd;
    }
    QList<RibbonCustomizeData> res;
    QList<int> willRemoveIndexs;   // 记录要删除的index

    //! 首先针对添加和删除操作进行优化
    // 说明：同一个元素不可能先删再加，只能先加再删，而且只能删一次，所以二级循环遍历，找到就跳出内循环
    for (int i = 0; i < size; ++i) {
        if (csd[i].actionType() == AddPageActionType) {
            for (int j = i + 1; j < size; ++j) {
                if (csd[j].actionType() != RemovePageActionType) {
                    continue;
                }
                if (csd[i].pageObjNameValue == csd[j].pageObjNameValue) {
                    willRemoveIndexs << i << j;
                    break;
                }
            }
        } else if (csd[i].actionType() == AddGroupActionType) {
            for (int j = i + 1; j < size; ++j) {
                if (csd[j].actionType() != RemoveGroupActionType) {
                    continue;
                }
                if ((csd[i].groupObjNameValue == csd[j].groupObjNameValue) &&
                    (csd[i].pageObjNameValue == csd[j].pageObjNameValue)) {
                    willRemoveIndexs << i << j;
                    break;
                }
            }
        } else if (csd[i].actionType() == AddActionActionType) {
            for (int j = i + 1; j < size; ++j) {
                if (csd[j].actionType() != RemoveActionActionType) {
                    continue;
                }
                if ((csd[i].keyValue == csd[j].keyValue) &&
                    (csd[i].groupObjNameValue == csd[j].groupObjNameValue) &&
                    (csd[i].pageObjNameValue == csd[j].pageObjNameValue)) {
                    willRemoveIndexs << i << j;
                    break;
                }
            }
        }
    }
    res = remove_indexs(csd, willRemoveIndexs);
    willRemoveIndexs.clear();

    //! 筛选VisiblePageActionType，对于多次出现的操作只保留最后一步
    // 说明：同一个元素可能会被反复隐藏和显示，所以二级循环遍历，找到就删除前一个、记录后一个，直到内循环结束
    // 下一次外循环，如果已被删除的或已经是最后的，就直接跳过
    QList<int> willNotCheckIndexs; // 记录免检的index
    size = res.size();
    for (int i = 0; i < size; ++i) {
        if (res[i].actionType() == VisiblePageActionType) {
            // 前置判断中使用的contains也是for循环，加了这个判断一级循环增加了处理能有多少性能收益？
            if (willRemoveIndexs.contains(i) || willNotCheckIndexs.contains(i)) {
                continue;
            }
            int last = i;
            for (int j = i + 1; j < size; ++j) {
                if (res[j].actionType() == VisiblePageActionType) {
                    if (res[i].pageObjNameValue == res[j].pageObjNameValue) {
                        willRemoveIndexs << last;   // 删除前一个只保留最后一个
                        last = j;
                    }
                }
            }
            if (last != i) { // 说明多次出现，last已经是最后了，下次可以免检了
                willNotCheckIndexs << last;
            }
        }
    }
    willNotCheckIndexs.clear();
    res = remove_indexs(res, willRemoveIndexs);
    willRemoveIndexs.clear();

    //! 针对RenamePageActionType和RenameGroupActionType操作，只需保留最后一个
    size = res.size();
    for (int i = 0; i < size; ++i) {
        if (res[i].actionType() == RenamePageActionType) {
            // 向后查询，如果查询到有同一个Page改名，把这个索引加入删除队列
            int last = i;
            for (int j = i + 1; j < size; ++j) {
                if ((res[j].actionType() == RenamePageActionType) &&
                    (res[i].pageObjNameValue == res[j].pageObjNameValue)) {
                    willRemoveIndexs << last;
                    last = j;
                }
            }
        } else if (res[i].actionType() == RenameGroupActionType) {
            // 向后查询，如果查询到有同一个group改名，把这个索引加入删除队列
            int last = i;
            for (int j = i + 1; j < size; ++j) {
                if ((res[j].actionType() == RenameGroupActionType) &&
                    (res[i].groupObjNameValue == res[j].groupObjNameValue) &&
                    (res[i].pageObjNameValue == res[j].pageObjNameValue)) {
                    willRemoveIndexs << last;
                    last = j;
                }
            }
        }
    }
    res = remove_indexs(res, willRemoveIndexs);
    willRemoveIndexs.clear();

    //! 针对连续的ChangePageOrderActionType，ChangeGroupOrderActionType，ChangeActionOrderActionType进行合并
    size = res.size();
    for (int i = 1; i < size; ++i) {
        if ((res[i - 1].actionType() == ChangePageOrderActionType) &&
            (res[i].actionType() == ChangePageOrderActionType) &&
            (res[i - 1].pageObjNameValue == res[i].pageObjNameValue)) {
            // 说明连续两个顺序调整，把前一个indexvalue和后一个indexvalue相加，前一个删除
            res[i].indexValue += res[i - 1].indexValue;
            willRemoveIndexs << i - 1;
        } else if ((res[i - 1].actionType() == ChangeGroupOrderActionType) &&
                   (res[i].actionType() == ChangeGroupOrderActionType) &&
                   (res[i - 1].groupObjNameValue == res[i].groupObjNameValue) &&
                   (res[i - 1].pageObjNameValue == res[i].pageObjNameValue)) {
            // 说明连续两个顺序调整，把前一个indexvalue和后一个indexvalue相加，前一个删除
            res[i].indexValue += res[i - 1].indexValue;
            willRemoveIndexs << i - 1;
        } else if ((res[i - 1].actionType() == ChangeActionOrderActionType) &&
                   (res[i].actionType() == ChangeActionOrderActionType) && (res[i - 1].keyValue == res[i].keyValue) &&
                   (res[i - 1].groupObjNameValue == res[i].groupObjNameValue) &&
                   (res[i - 1].pageObjNameValue == res[i].pageObjNameValue)) {
            // 说明连续两个顺序调整，把前一个indexvalue和后一个indexvalue相加，前一个删除
            res[i].indexValue += res[i - 1].indexValue;
            willRemoveIndexs << i - 1;
        }
    }
    res = remove_indexs(res, willRemoveIndexs);
    willRemoveIndexs.clear();

    //! 上一步操作可能会产生indexvalue为0的情况，此操作把indexvalue为0的删除
    size = res.size();
    for (int i = 0; i < size; ++i) {
        if ((res[i].actionType() == ChangePageOrderActionType) ||
            (res[i].actionType() == ChangeGroupOrderActionType) ||
            (res[i].actionType() == ChangeActionOrderActionType)) {
            if (0 == res[i].indexValue) {
                willRemoveIndexs << i;
            }
        }
    }
    res = remove_indexs(res, willRemoveIndexs);
    willRemoveIndexs.clear();
    return res;
}

QX_RIBBON_END_NAMESPACE
