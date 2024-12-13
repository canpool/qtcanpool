/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
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
 * @brief 应用RibbonCustomizeData到RibbonBar
 * @param bar
 * @return 如果应用失败，返回false,如果actionType==UnknowActionType直接返回false
 */
bool RibbonCustomizeData::apply(RibbonBar *bar)
{
    if (customized) {
        return true;
    }
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

RibbonActionsManager *RibbonCustomizeData::actionsManager()
{
    return m_actionsManager;
}

void RibbonCustomizeData::setActionsManager(RibbonActionsManager *mgr)
{
    m_actionsManager = mgr;
}

RibbonCustomizeData RibbonCustomizeData::makeAddPageCustomizeData(const QString &title, int index,
                                                                  const QString &objName)
{
    RibbonCustomizeData data(AddPageActionType);

    data.indexValue = index;
    data.keyValue = title;
    data.pageObjNameValue = objName;
#if QX_RIBBON_DEBUG
    qDebug() << QString("add page %1").arg(objName);
#endif
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
                                                                   const QString &pageObjName, const QString &objName)
{
    RibbonCustomizeData data(AddGroupActionType);

    data.indexValue = index;
    data.keyValue = title;
    data.groupObjNameValue = objName;
    data.pageObjNameValue = pageObjName;
#ifdef QX_RIBBON_DEBUG
    qDebug() << QString("add group %1 to page %2").arg(objName, pageObjName);
#endif
    return data;
}

/**
 * @brief 添加action
 * @param key action的索引
 * @param mgr action管理器
 * @param rp 定义action的占位情况
 * @param pageObjName action添加到的page的objname
 * @param groupObjName action添加到的page下的group的objname
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
#if QX_RIBBON_DEBUG
    qDebug() << QString("add action %1 to %2/%3").arg(key, pageObjName, groupObjName);
#endif
    return data;
}

RibbonCustomizeData RibbonCustomizeData::makeRenamePageCustomizeData(const QString &newName, const QString &pageObjName)
{
    RibbonCustomizeData data(RenamePageActionType);

    if (pageObjName.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize rename page,"
                                "but get an empty page object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
    data.keyValue = newName;
    data.pageObjNameValue = pageObjName;

    return data;
}

RibbonCustomizeData RibbonCustomizeData::makeRenameGroupCustomizeData(const QString &newName,
                                                                      const QString &pageObjName,
                                                                      const QString &groupObjName)
{
    RibbonCustomizeData data(RenameGroupActionType);

    if (groupObjName.isEmpty() || pageObjName.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize rename group,"
                                "but get an empty page/group object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
    data.keyValue = newName;
    data.groupObjNameValue = groupObjName;
    data.pageObjNameValue = pageObjName;
    return data;
}

/**
 * @brief 对应ChangePageOrderActionType
 * @param pageObjName 需要移动的pageobjName
 * @param moveIndex 移动位置，-1代表向上（向左）移动一个位置，1带表向下（向右）移动一个位置
 * @return
 */
RibbonCustomizeData RibbonCustomizeData::makeChangePageOrderCustomizeData(const QString &pageObjName, int moveIndex)
{
    RibbonCustomizeData data(ChangePageOrderActionType);

    if (pageObjName.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize change page order,"
                                "but get an empty page object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
#if QX_RIBBON_DEBUG
    qDebug() << QString("change page %1 order from %2 to %3").arg(pageObjName).arg(data.indexValue).arg(moveIndex);
#endif
    data.pageObjNameValue = pageObjName;
    data.indexValue = moveIndex;
    return data;
}

/**
 * @brief 对应ChangeGroupOrderActionType
 * @param pageObjName 需要移动的group对应的pageobjName
 * @param groupObjName 需要移动的groupObjName
 * @param moveIndex 移动位置，-1代表向上（向左）移动一个位置，1带表向下（向右）移动一个位置
 * @return
 */
RibbonCustomizeData RibbonCustomizeData::makeChangeGroupOrderCustomizeData(const QString &pageObjName,
                                                                           const QString &groupObjName, int moveIndex)
{
    RibbonCustomizeData data(ChangeGroupOrderActionType);

    if (pageObjName.isEmpty() || groupObjName.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize change group order,"
                                "but get an empty page/group object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
#if QX_RIBBON_DEBUG
    qDebug() << QString("change group %1 order from %2 to %3").arg(groupObjName).arg(data.indexValue).arg(moveIndex);
#endif
    data.pageObjNameValue = pageObjName;
    data.groupObjNameValue = groupObjName;
    data.indexValue = moveIndex;
    return data;
}

/**
 * @brief 对应ChangeActionOrderActionType
 * @param pageObjName 需要移动的group对应的pageobjName
 * @param groupObjName 需要移动的groupObjName
 * @param key RibbonActionsManager管理的key名
 * @param mgr RibbonActionsManager指针
 * @param moveIndex 移动位置，-1代表向上（向左）移动一个位置，1带表向下（向右）移动一个位置
 * @return
 */
RibbonCustomizeData RibbonCustomizeData::makeChangeActionOrderCustomizeData(const QString &pageObjName,
                                                                            const QString &groupObjName,
                                                                            const QString &key,
                                                                            RibbonActionsManager *mgr, int moveIndex)
{
    RibbonCustomizeData data(ChangeActionOrderActionType, mgr);

    if (pageObjName.isEmpty() || groupObjName.isEmpty() || key.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize change action order,"
                                "but get an empty page/group/action object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
#if QX_RIBBON_DEBUG
    qDebug() << QString("change action %1 order from %2 to %3").arg(key).arg(data.indexValue).arg(moveIndex);
#endif
    data.pageObjNameValue = pageObjName;
    data.groupObjNameValue = groupObjName;
    data.keyValue = key;
    data.indexValue = moveIndex;
    return data;
}

RibbonCustomizeData RibbonCustomizeData::makeRemovePageCustomizeData(const QString &pageObjName)
{
    RibbonCustomizeData data(RemovePageActionType);

    if (pageObjName.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize remove page,"
                                "but get an empty page object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
#if QX_RIBBON_DEBUG
    qDebug() << QString("remove page %1").arg(pageObjName);
#endif
    data.pageObjNameValue = pageObjName;
    return data;
}

RibbonCustomizeData RibbonCustomizeData::makeRemoveGroupCustomizeData(const QString &pageObjName,
                                                                      const QString &groupObjName)
{
    RibbonCustomizeData data(RemoveGroupActionType);

    if (pageObjName.isEmpty() || groupObjName.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize remove group,"
                                "but get an empty page/group object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
#if QX_RIBBON_DEBUG
    qDebug() << QString("remove group %1 from page %2").arg(groupObjName, pageObjName);
#endif
    data.pageObjNameValue = pageObjName;
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
RibbonCustomizeData RibbonCustomizeData::makeRemoveActionCustomizeData(const QString &pageObjName,
                                                                       const QString &groupObjName, const QString &key,
                                                                       RibbonActionsManager *mgr)
{
    RibbonCustomizeData data(RemoveActionActionType, mgr);

    if (pageObjName.isEmpty() || groupObjName.isEmpty() || key.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize remove action,"
                                "but get an empty page/group/action object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
#if QX_RIBBON_DEBUG
    qDebug() << QString("remove action %1 from %2/%3").arg(key, pageObjName, groupObjName);
#endif
    data.pageObjNameValue = pageObjName;
    data.groupObjNameValue = groupObjName;
    data.keyValue = key;
    return data;
}

/**
 * @brief RibbonCustomizeData::makeVisiblePageCustomizeData
 * @param pageObjName
 * @param isShow
 * @return
 */
RibbonCustomizeData RibbonCustomizeData::makeVisiblePageCustomizeData(const QString &pageObjName, bool isShow)
{
    RibbonCustomizeData data(VisiblePageActionType);

    if (pageObjName.isEmpty()) {
        qDebug() << QObject::tr("Ribbon Warning !!! customize visible page,"
                                "but get an empty page object name,"
                                "if you want to customize Ribbon,"
                                "please make sure every element has been set object name.");
    }
    data.pageObjNameValue = pageObjName;
    data.indexValue = isShow ? 1 : 0;
    return data;
}

bool RibbonCustomizeData::isCanCustomize(QObject *obj)
{
    QVariant v = obj->property(QX_RIBBON_PROP_CAN_CUSTOMIZE);

    if (v.isValid()) {
        return v.toBool();
    }
    return false;
}

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
 * 3、针对RenamePageActionType和RenameGroupActionType操作，先只保留最后一个，然后直接修改
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
                if ((csd[i].keyValue == csd[j].keyValue) && (csd[i].groupObjNameValue == csd[j].groupObjNameValue) &&
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
    QList<int> willNotCheckIndexs;   // 记录免检的index
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
            if (last != i) {   // 说明多次出现，last已经是最后了，下次可以免检了
                willNotCheckIndexs << last;
            }
        }
    }
    willNotCheckIndexs.clear();
    res = remove_indexs(res, willRemoveIndexs);
    willRemoveIndexs.clear();

    //! 针对RenamePageActionType和RenameGroupActionType操作，只需保留最后一个
    size = res.size();
    QList<int> willRenameIndexs;
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
            willRenameIndexs << last;
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
            willRenameIndexs << last;
        }
    }
    res = remove_indexs(res, willRemoveIndexs);
    willRemoveIndexs.clear();

    //! 对于RenamePageActionType和RenameGroupActionType操作，直接修改，不留到apply时
    foreach (int i, willRenameIndexs) {
        if (res[i].actionType() == RenamePageActionType) {
            for (int j = 0; j < i; ++j) {
                if (res[j].pageObjNameValue == res[i].pageObjNameValue) {
                    res[j].keyValue = res[i].keyValue;
                    break;
                }
            }
        } else if (res[i].actionType() == RenameGroupActionType) {
            for (int j = 0; j < i; ++j) {
                if ((res[j].groupObjNameValue == res[i].groupObjNameValue) &&
                    (res[j].pageObjNameValue == res[i].pageObjNameValue)) {
                    res[j].keyValue = res[i].keyValue;
                    break;
                }
            }
        }
    }
    res = remove_indexs(res, willRenameIndexs);
    willRenameIndexs.clear();

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
        if ((res[i].actionType() == ChangePageOrderActionType) || (res[i].actionType() == ChangeGroupOrderActionType) ||
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
