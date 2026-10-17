/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"

#include <QAbstractListModel>
#include <QAction>
#include <QMap>
#include <QObject>
#include <QSet>
#include <QString>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonBar;
class RibbonPage;
class RibbonActionsManagerPrivate;

/**
 * @brief Manages all Actions of the Ribbon
 *
 * RibbonActionsManager maintains two tables: one mapping tags to action lists,
 * and one list of all actions managed by RibbonActionsManager.
 *
 * A RibbonActionsManager tag corresponds to a group of actions; the same action may appear under multiple tags,
 * but the action list maintained by RibbonActionsManager contains only one copy of each action, without duplicates.
 *
 * A tag groups the action list; the display name of each tag is set via @ref setTagName, which must be called
 * in time when the language changes to set the text for the new tag.
 *
 * RibbonActionsManager provides six common preset tags by default, see @ref RibbonActionsManager::ActionTag;
 * user-defined tags must be accumulated on top of the RibbonActionsManager::UserDefineActionTag value.
 *
 * The @ref RibbonActionsManager::actions function extracts the action list managed by a tag, and the @ref allActions
 * function returns all tags managed by RibbonActionsManager.
 *
 * The @ref autoRegisterActions function quickly sets up action management; it traverses all child objects under
 * @ref RibbonBar, as well as the actions added by all @ref RibbonGroup under the RibbonBar, and builds tags for
 * each Page. For normal usage, the user only needs to care about this autoRegisterActions function.
 *
 */
class QX_RIBBON_EXPORT RibbonActionsManager : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Defines the tag of an action
     */
    enum ActionTag {
        UnknowActionTag = 0,                   ///< Unknown tag
        CommonlyUsedActionTag = 0x01,          ///< Preset tag - commonly used commands
        NotInFunctionalAreaActionTag = 0x02,   ///< Preset tag - commands not in the functional area
        AutoPageDistinguishBeginTag =
            0x1000,   ///< Start of tags automatically divided by Page, used by the @ref autoRegisterActions function
        AutoPageDistinguishEndTag =
            0x2000,   ///< End of tags automatically divided by Page, used by the @ref autoRegisterActions function
        NotInRibbonPageTag =
            0x2001,   ///< Tag for commands not in the functional area @ref autoRegisterActions
                      ///< The function traverses the actions of all pages and the actions under the RibbonBar; if two
        UserDefineActionTag = 0x8000,   ///< Custom tag; all user-defined tags must be greater than this tag
    };
public:
    RibbonActionsManager(RibbonBar *bar);
    ~RibbonActionsManager();

    // Sets the name corresponding to a tag
    void setTagName(int tag, const QString &name);

    // Gets the name corresponding to a tag
    QString tagName(int tag) const;

    // Removes a tag; note that this function is very time-consuming
    void removeTag(int tag);

    // Registers an action
    bool registerAction(QAction *act, int tag, const QString &key = QString(), bool enableEmit = true);

    // Unregisters an action
    void unregisterAction(QAction *act, bool enableEmit = true);

    // Filters the actions of a series by tag
    QList<QAction *> actions(int tag) const;

    // Gets all tags
    QList<int> actionTags() const;

    // Gets an action by key
    QAction *action(const QString &key) const;

    // Finds the key by action
    QString key(QAction *act) const;

    // Returns the number of all managed actions
    int count() const;

    // Returns all managed actions
    QList<QAction *> allActions() const;

    // Automatically loads the actions under the RibbonBar
    QMap<int, RibbonPage *> autoRegisterActions(RibbonBar *bar);

    // Automatically loads the actions under the widget
    QSet<QAction *> autoRegisterWidgetActions(QWidget *w, int tag, bool enableEmit = false);

    // Finds actions by title
    QList<QAction *> search(const QString &text);

    // Clears
    void clear();

    // Gets the ribbon bar
    RibbonBar *ribbonBar() const;

    // Makes all managed actions customizable
    void setAllActionCanCustomize(bool on = true);

Q_SIGNALS:
    /**
     * @brief Signal emitted when a tag changes; changes include addition and removal
     */
    void actionTagChanged(int tag, bool isDeleted);

private Q_SLOTS:
    void onActionDestroyed(QObject *o);
    void onPageTitleChanged(const QString &title);
private:
    void removeAction(QAction *act, bool enableEmit = true);
private:
    QX_DECLARE_PRIVATE(RibbonActionsManager)
    friend class RibbonActionsManagerModel;
};

QX_RIBBON_END_NAMESPACE
