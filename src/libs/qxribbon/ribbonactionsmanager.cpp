/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbonactionsmanager.h"
#include "ribbonactionsmanager_p.h"
#include "ribboncustomizedata.h"
#include "ribbonbar.h"

#include <QDebug>
#include <QHash>
#include <QMap>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonActionsManagerPrivate
{
    QX_DECLARE_PUBLIC(RibbonActionsManager)
public:
    RibbonActionsManagerPrivate();
    void clear();
public:
    QMap<int, QList<QAction *>> m_tagToActions;   ///< tag : QList<QAction*>
    QMap<int, QString> m_tagToName;               ///< Name corresponding to a tag
    QHash<QString, QAction *> m_keyToAction;      ///< Action corresponding to a key
    QMap<QAction *, QString> m_actionToKey;       ///< Key corresponding to an action
    QMap<int, RibbonPage *> m_tagToPage;          ///< Only used by the autoRegisteActions function
    int m_salt;   ///< Salt used to generate stable ids; when the user does not set a key,
                  ///< the id is generated based on m_salt, and as long as the registerAction call order of
                  ///< RibbonActionsManager is unchanged, the generated id stays the same because it is based on an
                  ///< auto-increment
};

RibbonActionsManagerPrivate::RibbonActionsManagerPrivate()
    : m_salt(0)
{
}

void RibbonActionsManagerPrivate::clear()
{
    m_tagToActions.clear();
    m_tagToName.clear();
    m_keyToAction.clear();
    m_actionToKey.clear();
    m_tagToPage.clear();
    m_salt = 0;
}

RibbonActionsManager::RibbonActionsManager(RibbonBar *bar)
    : QObject(bar)
{
    QX_INIT_PRIVATE(RibbonActionsManager);
    autoRegisterActions(bar);
}

RibbonActionsManager::~RibbonActionsManager()
{
    QX_FINI_PRIVATE()
}

/**
 * @brief Sets the name corresponding to a tag; this provides a mapping between tags and text
 * @param tag
 * @param name
 * @note In a multilingual environment, it must be set again on language switch to update the name
 */
void RibbonActionsManager::setTagName(int tag, const QString &name)
{
    Q_D(RibbonActionsManager);
    d->m_tagToName[tag] = name;
}

/**
 * @brief Gets the display name corresponding to a tag
 * @param tag
 * @return
 */
QString RibbonActionsManager::tagName(int tag) const
{
    Q_D(const RibbonActionsManager);
    return d->m_tagToName.value(tag, "");
}

/**
 * @brief Removes a tag
 * @note Note that this function is very time-consuming
 * @param tag
 */
void RibbonActionsManager::removeTag(int tag)
{
    Q_D(RibbonActionsManager);
    QList<QAction *> willRemoveActs = actions(tag);

    // Start removing
    d->m_tagToActions.remove(tag);
    d->m_tagToName.remove(tag);

    // Start finding the actions that must be removed from the master table
    QList<QAction *> needRemoveActs;
    QList<QAction *> remainingActs;

    for (auto i = d->m_tagToActions.begin(); i != d->m_tagToActions.end(); ++i) {
        remainingActs += i.value();
    }
    for (QAction *a : willRemoveActs) {
        if (!remainingActs.contains(a)) {
            needRemoveActs.append(a);
        }
    }
    // Remove the actions from the master table
    for (QAction *a : needRemoveActs) {
        auto i = d->m_actionToKey.find(a);
        if (i != d->m_actionToKey.end()) {
            d->m_keyToAction.remove(i.value());
            d->m_actionToKey.erase(i);
        }
    }
}

/**
 * @brief Registers an action into the manager to enable action management
 * @param act
 * @param tag The tag can be combined bitwise, see @ref ActionTag. If
 * you want to define your own tag, define a value greater than @ref ActionTag::UserDefineActionTag.
 * The tag passed to registerAction is recorded directly; to keep multiple tags simultaneously, OR them before
 * registering
 * @param key The key corresponding to the action; one key maps to only one action and is the key for looking up an
 * action. By default it is an empty QString(), in which case the key is the objectName of the QAction
 * @param enableEmit Whether to emit the actionTagChanged signal
 * @note Registering the same action with different tags allows indexing the action by tag, but indexing by action only
 * yields the last registered tag
 * @note Adding a tag triggers the actionTagChanged signal
 */
bool RibbonActionsManager::registerAction(QAction *act, int tag, const QString &key, bool enableEmit)
{
    Q_D(RibbonActionsManager);
    if (Q_NULLPTR == act) {
        return false;
    }
    QString k = key;

    if (k.isEmpty()) {
        k = QString("id_%1_%2").arg(d->m_salt++).arg(act->objectName());
    }
    if (d->m_keyToAction.contains(k)) {
        qWarning()
            << "key: " << k
            << " have been exist,you can set key in an unique value when use RibbonActionsManager::registerAction";
        return false;
    }
    d->m_keyToAction[k] = act;
    d->m_actionToKey[act] = k;
    // Records the mapping from tag to action
    bool isneedemit = !(d->m_tagToActions.contains(tag));   // Records whether a signal must be emitted

    d->m_tagToActions[tag].append(act);
    // Binds the slot
    connect(act, &QObject::destroyed, this, &RibbonActionsManager::onActionDestroyed);
    if (isneedemit && enableEmit) {
        // Indicates a newly added tag
        Q_EMIT actionTagChanged(tag, false);
    }
    return true;
}

/**
 * @brief Unregisters an action
 *
 * If the last action of a tag is unregistered, the tag is removed as well
 * @param act
 * @param enableEmit Whether to emit the actionTagChanged signal
 * @note Removing a tag triggers the actionTagChanged signal
 * @note If an action is associated with multiple tags, removing these tags removes the action from all of them, and the
 * same applies to the corresponding keys
 */
void RibbonActionsManager::unregisterAction(QAction *act, bool enableEmit)
{
    if (Q_NULLPTR == act) {
        return;
    }
    // Binds the slot
    disconnect(act, &QObject::destroyed, this, &RibbonActionsManager::onActionDestroyed);
    removeAction(act, enableEmit);
}

/**
 * @brief Removes an action
 *
 * Only removes in-memory content
 * @param act
 * @param enableEmit
 */
void RibbonActionsManager::removeAction(QAction *act, bool enableEmit)
{
    Q_D(RibbonActionsManager);
    QList<int> deletedTags;   // Records the deleted tags, used to trigger actionTagChanged
    QMap<int, QList<QAction *>> tagToActions;

    if (!d->m_actionToKey.contains(act)) {
        return;
    }

    for (auto i = d->m_tagToActions.begin(); i != d->m_tagToActions.end(); ++i) {
        // Moves the content that is not act into tagToActions, then swaps it with the one in d
        auto tmpi = tagToActions.insert(i.key(), QList<QAction *>());
        int count = 0;
        for (int j = 0; j < i.value().size(); ++j) {
            QAction *tmpAct = i.value()[j];
            if (tmpAct != act) {
                tmpi.value().append(tmpAct);
                ++count;
            }
        }
        if (0 == count) {
            // Indicates this tag has no content
            tagToActions.erase(tmpi);
            deletedTags.append(i.key());
        }
    }
    // Removes from keyToAction
    QString key = d->m_actionToKey.value(act);
    d->m_actionToKey.remove(act);
    d->m_keyToAction.remove(key);

    // Swap
    d->m_tagToActions.swap(tagToActions);
    // Emit signals
    if (enableEmit) {
        for (int delTag : deletedTags) {
            Q_EMIT actionTagChanged(delTag, true);
        }
    }
}

/**
 * @brief Gets the actions by tag
 * @param tag
 * @return
 */
QList<QAction *> RibbonActionsManager::actions(int tag) const
{
    Q_D(const RibbonActionsManager);
    return d->m_tagToActions.value(tag);
}

/**
 * @brief Gets all tags
 * @return
 */
QList<int> RibbonActionsManager::actionTags() const
{
    Q_D(const RibbonActionsManager);
    return d->m_tagToActions.keys();
}

/**
 * @brief Gets an action by key
 * @param key
 * @return Returns Q_NULLPTR if there is no such key
 */
QAction *RibbonActionsManager::action(const QString &key) const
{
    Q_D(const RibbonActionsManager);
    return d->m_keyToAction.value(key, Q_NULLPTR);
}

/**
 * @brief Finds the key by action
 * @param act
 * @return Returns QString() if not found
 */
QString RibbonActionsManager::key(QAction *act) const
{
    Q_D(const RibbonActionsManager);
    return d->m_actionToKey.value(act, QString());
}

/**
 * @brief Returns the number of all managed actions
 * @return
 */
int RibbonActionsManager::count() const
{
    Q_D(const RibbonActionsManager);
    return d->m_keyToAction.size();
}

/**
 * @brief Returns all managed actions
 * @return
 */
QList<QAction *> RibbonActionsManager::allActions() const
{
    Q_D(const RibbonActionsManager);
    return d->m_keyToAction.values();
}

/**
 * @brief Automatically loads the actions of the RibbonBar
 * This function traverses all child objects under @ref RibbonBar, finds and registers the actions,
 * and traverses all @ref RibbonPage, classifying the actions under each RibbonPage by the RibbonPage's title name
 *
 * This function generates and registers a tag for the actions under every page; the returned QMap<int, RibbonPage *>
 * records the tag corresponding to each page
 *
 * This function also registers the actions under the RibbonBar that are not under any page with the NotInRibbonPageTag
 * tag; the default name assigned is "not in ribbon", which can be changed via @ref setTagName
 *
 * @param bar
 * @return
 * @note It is best to call this function after the page titles are set, because the page title is used as the tag name
 */
QMap<int, RibbonPage *> RibbonActionsManager::autoRegisterActions(RibbonBar *bar)
{
    Q_D(RibbonActionsManager);
    QMap<int, RibbonPage *> res;
    if (Q_NULLPTR == bar) {
        return res;
    }
    // First traverse all child objects under the RibbonBar and find all actions
    QSet<QAction *> ribbonBarActions;

    for (QObject *o : bar->children()) {
        if (QAction *a = qobject_cast<QAction *>(o)) {
            if (!a->objectName().isEmpty()) {
                ribbonBarActions.insert(a);
            }
        }
    }

    QSet<QAction *> pageActions;
    QList<RibbonPage *> pages = bar->pages();
    int tag = AutoPageDistinguishBeginTag;

    for (RibbonPage *page : pages) {
        QList<RibbonGroup *> groups = page->groupList();
        for (RibbonGroup *group : groups) {
            pageActions += autoRegisterWidgetActions(group, tag, false);
        }
        setTagName(tag, page->windowTitle());
        res[tag] = page;
        ++tag;
    }
    // Find the actions that are not in the functional area
    QSet<QAction *> notInPageActions = ribbonBarActions - pageActions;

    for (QAction *a : notInPageActions) {
        if (!a->objectName().isEmpty()) {
            registerAction(a, NotInRibbonPageTag, a->objectName(), false);
        }
    }
    if (notInPageActions.size() > 0) {
        setTagName(NotInRibbonPageTag, tr("not in ribbon"));
    }
    for (auto i = res.begin(); i != res.end(); ++i) {
        connect(i.value(), &RibbonPage::windowTitleChanged, this, &RibbonActionsManager::onPageTitleChanged);
    }
    d->m_tagToPage = res;
    return res;
}

/**
 * @brief Automatically loads actions
 * @param w
 * @param tag
 * @param enableEmit
 * @return Returns the actions successfully added to RibbonActionsManager management
 */
QSet<QAction *> RibbonActionsManager::autoRegisterWidgetActions(QWidget *w, int tag, bool enableEmit)
{
    QSet<QAction *> res;
    QList<QAction *> was = w->actions();

    for (QAction *a : was) {
        if (res.contains(a) || a->objectName().isEmpty()) {
            // Skip duplicate content and entries without an object name
            continue;
        }
        if (registerAction(a, tag, a->objectName(), enableEmit)) {
            res.insert(a);
        }
    }
    return res;
}

/**
 * @brief Finds actions by title
 * @param text
 * @return
 */
QList<QAction *> RibbonActionsManager::search(const QString &text)
{
    Q_D(const RibbonActionsManager);
    QList<QAction *> res;

    if (text.isEmpty()) {
        return res;
    }
    QStringList kws = text.split(" ");

    if (kws.isEmpty()) {
        kws.append(text);
    }

    for (const QString &k : kws) {
        for (auto i = d->m_actionToKey.begin(); i != d->m_actionToKey.end(); ++i) {
            if (i.key()->text().contains(k, Qt::CaseInsensitive)) {
                res.append(i.key());
            }
        }
    }
    return res;
}

void RibbonActionsManager::clear()
{
    Q_D(RibbonActionsManager);
    d->clear();
}

RibbonBar *RibbonActionsManager::ribbonBar() const
{
    return qobject_cast<RibbonBar *>(parent());
}

/**
 * @brief This function makes all managed actions customizable
 * @param on
 */
void RibbonActionsManager::setAllActionCanCustomize(bool on)
{
    Q_D(RibbonActionsManager);
    for (auto i = d->m_actionToKey.begin(); i != d->m_actionToKey.end(); ++i) {
        RibbonCustomizeData::setCanCustomize(i.key(), on);
    }
}

/**
 * @brief Triggered when an action is deleted; this slot removes the managed action
 * @param o
 * @note This function does not trigger the actionTagChanged signal
 */
void RibbonActionsManager::onActionDestroyed(QObject *o)
{
    QAction *act = static_cast<QAction *>(o);

    removeAction(act, false);
}

/**
 * @brief The autoRegisteActions function connects this slot to update the tag-to-text mapping when the tag content
 * changes
 * @param title
 */
void RibbonActionsManager::onPageTitleChanged(const QString &title)
{
    Q_D(RibbonActionsManager);
    RibbonPage *page = qobject_cast<RibbonPage *>(sender());

    if (Q_NULLPTR == page) {
        return;
    }
    int tag = d->m_tagToPage.key(page, -1);

    if (tag == -1) {
        return;
    }
    setTagName(tag, title);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/// RibbonActionsModel
////////////////////////////////////////////////////////////////////////////////////////////////////////
class RibbonActionsManagerModelPrivate
{
    QX_DECLARE_PUBLIC(RibbonActionsManagerModel)
public:
    RibbonActionsManagerModelPrivate();

    void updateRef();
    int count() const;
    QAction *at(int index) const;
    bool isNull() const;
public:
    RibbonActionsManager *m_mgr;
    int m_tag;
    QString m_searchText;
    QList<QAction *> m_actions;
};

RibbonActionsManagerModelPrivate::RibbonActionsManagerModelPrivate()
    : m_mgr(Q_NULLPTR)
    , m_tag(RibbonActionsManager::CommonlyUsedActionTag)
    , m_searchText(QString())
{
}

void RibbonActionsManagerModelPrivate::updateRef()
{
    if (isNull()) {
        return;
    }
    if (!m_searchText.isEmpty()) {
        m_actions = m_mgr->search(m_searchText);
    } else {
        m_actions = m_mgr->actions(m_tag);
    }
}

int RibbonActionsManagerModelPrivate::count() const
{
    if (isNull()) {
        return 0;
    }
    return m_actions.size();
}

QAction *RibbonActionsManagerModelPrivate::at(int index) const
{
    if (isNull()) {
        return Q_NULLPTR;
    }
    if (index >= count()) {
        return Q_NULLPTR;
    }
    return m_actions.at(index);
}

bool RibbonActionsManagerModelPrivate::isNull() const
{
    return (m_mgr == Q_NULLPTR);
}

RibbonActionsManagerModel::RibbonActionsManagerModel(QObject *p)
    : QAbstractListModel(p){QX_INIT_PRIVATE(RibbonActionsManagerModel)}

    RibbonActionsManagerModel::RibbonActionsManagerModel(RibbonActionsManager * m, QObject * p)
    : RibbonActionsManagerModel(p)
{
    setupActionsManager(m);
}

RibbonActionsManagerModel::~RibbonActionsManagerModel()
{
    QX_FINI_PRIVATE()
}

int RibbonActionsManagerModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const RibbonActionsManagerModel);
    if (parent.isValid()) {   // Not top-level
        return 0;
    }
    // Top-level
    return d->count();
}

QVariant RibbonActionsManagerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (Qt::Horizontal == orientation) {
        return tr("action name");
    }
    return QVariant();
}

Qt::ItemFlags RibbonActionsManagerModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    return (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

QVariant RibbonActionsManagerModel::data(const QModelIndex &index, int role) const
{
    QAction *act = indexToAction(index);

    if (Q_NULLPTR == act) {
        return QVariant();
    }

    switch (role) {
    case Qt::DisplayRole:
        return act->text();

    case Qt::DecorationRole:
        return act->icon();

    default:
        break;
    }
    return QVariant();
}

void RibbonActionsManagerModel::setFilter(int tag)
{
    Q_D(RibbonActionsManagerModel);
    d->m_tag = tag;
    update();
}

void RibbonActionsManagerModel::update()
{
    Q_D(RibbonActionsManagerModel);
    beginResetModel();
    d->updateRef();
    endResetModel();
}

void RibbonActionsManagerModel::setupActionsManager(RibbonActionsManager *m)
{
    Q_D(RibbonActionsManagerModel);
    d->m_mgr = m;
    d->m_tag = RibbonActionsManager::CommonlyUsedActionTag;
    d->m_actions = m->actions(d->m_tag);
    connect(m, &RibbonActionsManager::actionTagChanged, this, &RibbonActionsManagerModel::onActionTagChanged);
    update();
}

void RibbonActionsManagerModel::uninstallActionsManager()
{
    Q_D(RibbonActionsManagerModel);
    if (!d->isNull()) {
        disconnect(d->m_mgr, &RibbonActionsManager::actionTagChanged, this,
                   &RibbonActionsManagerModel::onActionTagChanged);
        d->m_mgr = Q_NULLPTR;
        d->m_tag = RibbonActionsManager::CommonlyUsedActionTag;
    }
    update();
}

QAction *RibbonActionsManagerModel::indexToAction(QModelIndex index) const
{
    Q_D(const RibbonActionsManagerModel);
    if (!index.isValid()) {
        return Q_NULLPTR;
    }
    if (index.row() >= d->count()) {
        return Q_NULLPTR;
    }
    return d->at(index.row());
}

/**
 * @brief Searches
 * @param text
 */
void RibbonActionsManagerModel::search(const QString &text)
{
    Q_D(RibbonActionsManagerModel);
    d->m_searchText = text;
    update();
}

void RibbonActionsManagerModel::onActionTagChanged(int tag, bool isdelete)
{
    Q_D(RibbonActionsManagerModel);
    if (isdelete && (tag == d->m_tag)) {
        d->m_tag = RibbonActionsManager::UnknowActionTag;
        update();
    } else {
        if (tag == d->m_tag) {
            update();
        }
    }
}

QX_RIBBON_END_NAMESPACE
