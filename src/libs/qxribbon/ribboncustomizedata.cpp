/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribboncustomizedata.h"
#include "ribbonbar.h"
#include "ribbonmainwindow.h"

#include <QDebug>
#include <QObject>

QX_RIBBON_BEGIN_NAMESPACE

/**
 * @brief Property that marks whether customization is allowed; dynamically set on RibbonPage and RibbonGroup.
 * Its value is bool; when true, the layout of this RibbonPage and RibbonGroup can be changed via RibbonCustomizeWidget.
 * This property is absent by default; only when it is present and true can the item be shown as customizable in
 * RibbonCustomizeWidget
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
 * @brief Gets the action type of the CustomizeData
 * @return
 */
RibbonCustomizeData::ActionType RibbonCustomizeData::actionType() const
{
    return m_type;
}

/**
 * @brief Sets the action type of the CustomizeData
 * @param a
 */
void RibbonCustomizeData::setActionType(RibbonCustomizeData::ActionType a)
{
    m_type = a;
}

/**
 * @brief Checks whether this is a valid CustomizeData
 *
 * The actual logic is actionType() != UnknowActionType
 * @return Returns true for a usable CustomizeData
 */
bool RibbonCustomizeData::isValid() const
{
    return (actionType() != UnknowActionType);
}

/**
 * @brief Applies the RibbonCustomizeData to the RibbonBar
 * @param bar
 * @return Returns false if the application fails; returns false directly if actionType==UnknowActionType
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
        // Add page
        RibbonPage *page = bar->insertPage(indexValue, keyValue);
        if (Q_NULLPTR == page) {
            return false;
        }
        page->setObjectName(pageObjNameValue);
        RibbonCustomizeData::setCanCustomize(page);
        break;
    }

    case AddGroupActionType: {
        // Add group
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
 * @brief Creates a RibbonCustomizeData of type AddGroupActionType
 * @param title The title of the group
 * @param index The index of the group
 * @param pageObjName The object name of the group's page
 * @param objName The object name of the group
 * @return Returns a RibbonCustomizeData of type AddGroupActionType
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
 * @brief Adds an action
 * @param key The index of the action
 * @param mgr The action manager
 * @param rp Defines the space proportion of the action
 * @param pageObjName The object name of the page to which the action is added
 * @param groupObjName The object name of the group under the page to which the action is added
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
 * @brief Corresponds to ChangePageOrderActionType
 * @param pageObjName The pageObjName to move
 * @param moveIndex The move position; -1 moves up (left) by one position, 1 moves down (right) by one position
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
 * @brief Corresponds to ChangeGroupOrderActionType
 * @param pageObjName The pageObjName corresponding to the group to move
 * @param groupObjName The groupObjName to move
 * @param moveIndex The move position; -1 moves up (left) by one position, 1 moves down (right) by one position
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
 * @brief Corresponds to ChangeActionOrderActionType
 * @param pageObjName The pageObjName corresponding to the group to move
 * @param groupObjName The groupObjName to move
 * @param key The key name managed by RibbonActionsManager
 * @param mgr RibbonActionsManager pointer
 * @param moveIndex The move position; -1 moves up (left) by one position, 1 moves down (right) by one position
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
 * @brief Corresponds to RemoveActionActionType
 * @param pageObjName The obj name of the page corresponding to the group
 * @param groupObjName The obj name corresponding to the group
 * @param key The key name managed by RibbonActionsManager
 * @param mgr RibbonActionsManager pointer
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
 * @brief Simplifies QList<RibbonCustomizeData>
 *
 * This function performs the following operations:
 * 1. Removes add-then-remove operations on the same element (added first, removed later)
 * 2. For VisiblePageActionType, keeps only the last step among repeated occurrences
 * 3. For RenamePageActionType and RenameGroupActionType, keeps only the last one and then applies it directly
 * 4. Merges consecutive ChangePageOrderActionType, ChangeGroupOrderActionType and ChangeActionOrderActionType into a
 * single operation; if the merge results in no movement, it is removed
 *
 * @param csd
 * @return Returns the simplified QList<RibbonCustomizeData>
 */
QList<RibbonCustomizeData> RibbonCustomizeData::simplify(const QList<RibbonCustomizeData> &csd)
{
    int size = csd.size();

    if (size <= 1) {
        return csd;
    }
    QList<RibbonCustomizeData> res;
    QList<int> willRemoveIndexs;   // Records the indices to remove

    //! First optimize add and remove operations
    // Note: The same element cannot be removed before it is added; it can only be added first and removed later, and
    // can be removed only once, so use a nested loop and break out of the inner loop as soon as a match is found
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

    //! Filter VisiblePageActionType, keeping only the last step among repeated occurrences
    // Note: The same element may be hidden and shown repeatedly, so use a nested loop to delete the previous one and
    // record the next one as soon as a match is found, until the inner loop ends On the next outer iteration, skip
    // indices that have already been removed or are already the last
    QList<int> willNotCheckIndexs;   // Records the indices exempt from checking
    size = res.size();
    for (int i = 0; i < size; ++i) {
        if (res[i].actionType() == VisiblePageActionType) {
            // The contains used in the precondition check is also a loop; how much performance benefit does adding this
            // check bring to the first-level loop?
            if (willRemoveIndexs.contains(i) || willNotCheckIndexs.contains(i)) {
                continue;
            }
            int last = i;
            for (int j = i + 1; j < size; ++j) {
                if (res[j].actionType() == VisiblePageActionType) {
                    if (res[i].pageObjNameValue == res[j].pageObjNameValue) {
                        willRemoveIndexs << last;   // Remove the previous one and keep only the last
                        last = j;
                    }
                }
            }
            if (last != i) {   // Indicates multiple occurrences; last is already the final one and can be exempt from
                               // checking next time
                willNotCheckIndexs << last;
            }
        }
    }
    willNotCheckIndexs.clear();
    res = remove_indexs(res, willRemoveIndexs);
    willRemoveIndexs.clear();

    //! For RenamePageActionType and RenameGroupActionType, keep only the last one
    size = res.size();
    QList<int> willRenameIndexs;
    for (int i = 0; i < size; ++i) {
        if (res[i].actionType() == RenamePageActionType) {
            // Search forward; if another rename of the same page is found, add this index to the removal queue
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
            // Search forward; if another rename of the same group is found, add this index to the removal queue
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

    //! For RenamePageActionType and RenameGroupActionType, apply directly instead of leaving it until apply
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

    //! Merge consecutive ChangePageOrderActionType, ChangeGroupOrderActionType and ChangeActionOrderActionType
    size = res.size();
    for (int i = 1; i < size; ++i) {
        if ((res[i - 1].actionType() == ChangePageOrderActionType) &&
            (res[i].actionType() == ChangePageOrderActionType) &&
            (res[i - 1].pageObjNameValue == res[i].pageObjNameValue)) {
            // Indicates two consecutive order changes; add the previous indexValue to the next one and remove the
            // previous one
            res[i].indexValue += res[i - 1].indexValue;
            willRemoveIndexs << i - 1;
        } else if ((res[i - 1].actionType() == ChangeGroupOrderActionType) &&
                   (res[i].actionType() == ChangeGroupOrderActionType) &&
                   (res[i - 1].groupObjNameValue == res[i].groupObjNameValue) &&
                   (res[i - 1].pageObjNameValue == res[i].pageObjNameValue)) {
            // Indicates two consecutive order changes; add the previous indexValue to the next one and remove the
            // previous one
            res[i].indexValue += res[i - 1].indexValue;
            willRemoveIndexs << i - 1;
        } else if ((res[i - 1].actionType() == ChangeActionOrderActionType) &&
                   (res[i].actionType() == ChangeActionOrderActionType) && (res[i - 1].keyValue == res[i].keyValue) &&
                   (res[i - 1].groupObjNameValue == res[i].groupObjNameValue) &&
                   (res[i - 1].pageObjNameValue == res[i].pageObjNameValue)) {
            // Indicates two consecutive order changes; add the previous indexValue to the next one and remove the
            // previous one
            res[i].indexValue += res[i - 1].indexValue;
            willRemoveIndexs << i - 1;
        }
    }
    res = remove_indexs(res, willRemoveIndexs);
    willRemoveIndexs.clear();

    //! The previous step may produce an indexValue of 0; this step removes the entries with indexValue 0
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
