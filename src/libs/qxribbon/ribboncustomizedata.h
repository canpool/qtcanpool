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

class RibbonBar;

/**
 * @brief Data class that records all customization operations
 * @note This data depends on @ref RibbonActionsManager and must be used after RibbonActionsManager
 */
class RibbonCustomizeData
{
public:
    enum ActionType {
        UnknowActionType = 0,          ///< Unknown operation
        AddPageActionType,             ///< Add page operation
        AddGroupActionType,            ///< Add group operation
        AddActionActionType,           ///< Add action operation
        RemovePageActionType,          ///< Remove page operation
        RemoveGroupActionType,         ///< Remove group operation
        RemoveActionActionType,        ///< Remove action operation
        ChangePageOrderActionType,     ///< Change page order operation
        ChangeGroupOrderActionType,    ///< Change group order operation
        ChangeActionOrderActionType,   ///< Change action order operation
        RenamePageActionType,          ///< Rename page operation
        RenameGroupActionType,         ///< Rename group operation
        VisiblePageActionType,         ///< Hide/show page operation
    };
public:
    RibbonCustomizeData();
    RibbonCustomizeData(ActionType type, RibbonActionsManager *mgr = Q_NULLPTR);

    // Gets the action type of the CustomizeData
    ActionType actionType() const;

    // Sets the action type of the CustomizeData
    void setActionType(ActionType a);

    // Checks whether this is a valid CustomizeData
    bool isValid() const;

    // Applies the RibbonCustomizeData
    bool apply(RibbonBar *bar);

    // Gets the actions manager pointer
    RibbonActionsManager *actionsManager();

    // Sets the ActionsManager
    void setActionsManager(RibbonActionsManager *mgr);

    // Corresponds to AddPageActionType
    static RibbonCustomizeData makeAddPageCustomizeData(const QString &title, int index, const QString &objName);

    // Corresponds to AddGroupActionType
    static RibbonCustomizeData makeAddGroupCustomizeData(const QString &title, int index, const QString &pageObjName,
                                                         const QString &objName);

    // Corresponds to AddActionActionType
    static RibbonCustomizeData makeAddActionCustomizeData(const QString &key, RibbonActionsManager *mgr,
                                                          RibbonGroup::RowProportion rp, const QString &pageObjName,
                                                          const QString &groupObjName);

    // Corresponds to RenamePageActionType
    static RibbonCustomizeData makeRenamePageCustomizeData(const QString &newName, const QString &pageObjName);

    // Corresponds to RenameGroupActionType
    static RibbonCustomizeData makeRenameGroupCustomizeData(const QString &newName, const QString &pageObjName,
                                                            const QString &groupObjName);

    // Corresponds to RemovePageActionType
    static RibbonCustomizeData makeRemovePageCustomizeData(const QString &pageObjName);

    // Corresponds to ChangePageOrderActionType
    static RibbonCustomizeData makeChangePageOrderCustomizeData(const QString &pageObjName, int moveIndex);

    // Corresponds to ChangeGroupOrderActionType
    static RibbonCustomizeData makeChangeGroupOrderCustomizeData(const QString &pageObjName,
                                                                 const QString &groupObjName, int moveIndex);

    // Corresponds to ChangeActionOrderActionType
    static RibbonCustomizeData makeChangeActionOrderCustomizeData(const QString &pageObjName,
                                                                  const QString &groupObjName, const QString &key,
                                                                  RibbonActionsManager *mgr, int moveIndex);

    // Corresponds to RemoveGroupActionType
    static RibbonCustomizeData makeRemoveGroupCustomizeData(const QString &pageObjName, const QString &groupObjName);

    // Corresponds to RemoveActionActionType
    static RibbonCustomizeData makeRemoveActionCustomizeData(const QString &pageObjName, const QString &groupObjName,
                                                             const QString &key, RibbonActionsManager *mgr);

    // Corresponds to VisiblePageActionType
    static RibbonCustomizeData makeVisiblePageCustomizeData(const QString &pageObjName, bool isShow);

    // Checks whether the object can be customized; use this to prevent a specific action from being edited
    static bool isCanCustomize(QObject *obj);
    static void setCanCustomize(QObject *obj, bool canbe = true);

    // Simplifies QList<RibbonCustomizeData>
    static QList<RibbonCustomizeData> simplify(const QList<RibbonCustomizeData> &csd);
public:
    /**
     * @brief Parameter recording the order
     *
     * When actionType==AddPageActionType, this parameter records the insert position of the page;
     * when actionType==AddGroupActionType, it records the insert position of the group;
     * when actionType==AddActionActionType, it records the insert position of the group
     */
    int indexValue;

    /**
     * @brief Parameter recording the title, index, etc.
     *
     * When actionType==AddPageActionType, key is the page title;
     * when actionType==AddGroupActionType, key is the group title;
     * when actionType==AddActionActionType, key is the lookup base for the action, queried via
     * RibbonActionsManager::action
     */
    QString keyValue;

    /**
     * @brief Records pageObjName, used to locate the page
     */
    QString pageObjNameValue;

    /**
     * @brief Records groupObjName; most of the ribbon customize indices are based on objname
     */
    QString groupObjNameValue;

    ///< Row proportion; ribbon has three proportions: large, medium and small, see @ref RibbonGroup::RowProportion
    RibbonGroup::RowProportion actionRowProportionValue;
private:
    ActionType m_type;   ///< Marks whether this data is a page, a group, or an action
    RibbonActionsManager *m_actionsManager;
    bool customized;
};

typedef QList<RibbonCustomizeData> RibbonCustomizeDataList;

QX_RIBBON_END_NAMESPACE

Q_DECLARE_METATYPE(QX_RIBBON_PREPEND_NAMESPACE(RibbonCustomizeData))
