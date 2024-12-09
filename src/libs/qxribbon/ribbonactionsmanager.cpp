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
    QMap<int, QList<QAction *> > m_tagToActions;    ///< tag : QList<QAction*>
    QMap<int, QString> m_tagToName;                 ///< tag对应的名字
    QHash<QString, QAction *> m_keyToAction;        ///< key对应action
    QMap<QAction *, QString> m_actionToKey;         ///< action对应key
    QMap<int, RibbonPage *> m_tagToPage;   ///< 仅仅在autoRegisteActions函数会有用
    int m_salt;     ///< 盐用于生成固定的id，在用户不主动设置key时，id基于msale生成，
                    ///< 只要RibbonActionsManager的调用registerAction顺序不变，生成的id都不变，因为它是基于自增实现的
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
 * @brief 设置tag对应的名字，通过这个可以得到tag和文本的映射
 * @param tag
 * @param name
 * @note 在支持多语言的环境下，在语言切换时需要重新设置，以更新名字
 */
void RibbonActionsManager::setTagName(int tag, const QString &name)
{
    Q_D(RibbonActionsManager);
    d->m_tagToName[tag] = name;
}

/**
 * @brief 获取tag对应的中文名字
 * @param tag
 * @return
 */
QString RibbonActionsManager::tagName(int tag) const
{
    Q_D(const RibbonActionsManager);
    return d->m_tagToName.value(tag, "");
}

/**
 * @brief 移除tag
 * @note 注意，这个函数非常耗时
 * @param tag
 */
void RibbonActionsManager::removeTag(int tag)
{
    Q_D(RibbonActionsManager);
    QList<QAction *> willRemoveActs = actions(tag);

    // 开始移除
    d->m_tagToActions.remove(tag);
    d->m_tagToName.remove(tag);

    // 开始查找需要移出总表的action
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
    // 从总表移除action
    for (QAction *a : needRemoveActs) {
        auto i = d->m_actionToKey.find(a);
        if (i != d->m_actionToKey.end()) {
            d->m_keyToAction.remove(i.value());
            d->m_actionToKey.erase(i);
        }
    }
}

/**
 * @brief 把action注册到管理器中，实现action的管理
 * @param act
 * @param tag tag是可以按照位进行叠加，见 @ref ActionTag 如果
 * 要定义自己的标签，建议定义大于 @ref ActionTag::UserDefineActionTag 的值，
 * registerAction的tag是直接记录进去的，如果要多个标签并存，在registe之前先或好tag
 * @param key key是action对应的key，一个key只对应一个action，是查找action的关键
 * ,默认情况为一个QString(),这时key是QAction的objectName
 * @note 同一个action多次注册不同的tag可以通过tag索引到action，但通过action只能索引到最后一个注册的tag
 * @note tag的新增会触发actionTagChanged信号
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
    // 记录tag 对 action
    bool isneedemit = !(d->m_tagToActions.contains(tag));   // 记录是否需要发射信号

    d->m_tagToActions[tag].append(act);
    // 绑定槽
    connect(act, &QObject::destroyed, this, &RibbonActionsManager::onActionDestroyed);
    if (isneedemit && enableEmit) {
        // 说明新增tag
        emit actionTagChanged(tag, false);
    }
    return true;
}

/**
 * @brief 取消action的注册
 *
 * 如果tag对应的最后一个action被撤销，tag也将一块删除
 * @param act
 * @note tag的删除会触发actionTagChanged信号
 * @note 如果action关联了多个tag，这些tag里的action都会被删除，对应的key也同理
 */
void RibbonActionsManager::unregisterAction(QAction *act, bool enableEmit)
{
    if (Q_NULLPTR == act) {
        return;
    }
    // 绑定槽
    disconnect(act, &QObject::destroyed, this, &RibbonActionsManager::onActionDestroyed);
    removeAction(act, enableEmit);
}

/**
 * @brief 移除action
 *
 * 仅移除内存内容
 * @param act
 * @param enableEmit
 */
void RibbonActionsManager::removeAction(QAction *act, bool enableEmit)
{
    Q_D(RibbonActionsManager);
    QList<int> deletedTags;                      // 记录删除的tag，用于触发actionTagChanged
    QMap<int, QList<QAction *> > tagToActions;

    if (!d->m_actionToKey.contains(act)) {
        return;
    }

    for (auto i = d->m_tagToActions.begin(); i != d->m_tagToActions.end(); ++i) {
        // 把不是act的内容转移到tagToActions中，之后再和d里的替换
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
            // 说明这个tag没有内容
            tagToActions.erase(tmpi);
            deletedTags.append(i.key());
        }
    }
    // 删除keyToAction
    QString key = d->m_actionToKey.value(act);
    d->m_actionToKey.remove(act);
    d->m_keyToAction.remove(key);

    // 置换
    d->m_tagToActions.swap(tagToActions);
    // 发射信号
    if (enableEmit) {
        for (int delTag : deletedTags) {
            emit actionTagChanged(delTag, true);
        }
    }
}

/**
 * @brief 根据tag得到actions
 * @param tag
 * @return
 */
QList<QAction *> RibbonActionsManager::actions(int tag) const
{
    Q_D(const RibbonActionsManager);
    return d->m_tagToActions.value(tag);
}

/**
 * @brief 获取所有的标签
 * @return
 */
QList<int> RibbonActionsManager::actionTags() const
{
    Q_D(const RibbonActionsManager);
    return d->m_tagToActions.keys();
}

/**
 * @brief 通过key获取action
 * @param key
 * @return 如果没有key，返回Q_NULLPTR
 */
QAction *RibbonActionsManager::action(const QString &key) const
{
    Q_D(const RibbonActionsManager);
    return d->m_keyToAction.value(key, Q_NULLPTR);
}

/**
 * @brief 通过action找到key
 * @param act
 * @return 如果找不到，返回QString()
 */
QString RibbonActionsManager::key(QAction *act) const
{
    Q_D(const RibbonActionsManager);
    return d->m_actionToKey.value(act, QString());
}

/**
 * @brief 返回所有管理的action数
 * @return
 */
int RibbonActionsManager::count() const
{
    Q_D(const RibbonActionsManager);
    return d->m_keyToAction.size();
}

/**
 * @brief 返回所有管理的actions
 * @return
 */
QList<QAction *> RibbonActionsManager::allActions() const
{
    Q_D(const RibbonActionsManager);
    return d->m_keyToAction.values();
}

/**
 * @brief 自动加载RibbonBar的action
 * 此函数会遍历@ref RibbonBar 下的所有子object，找到action注册，
 * 并会遍历所有@ref RibbonPage,把RibbonPage下的action按RibbonPage的title name进行分类
 *
 * 此函数会把所有page下的action生成tag并注册，返回的QMap<int, RibbonPage *>是记录了page对应的tag
 *
 * 此函数还会把RibbonBar下面的action，但不在任何一个page下的作为NotInRibbonPageTag标签注册，默认名字会赋予not
 * in ribbon， 可以通过@ref setTagName 改变
 *
 * @param bar
 * @return
 * @note 此函数的调用最好在page设置了标题后调用，因为会以page的标题作为标签的命名
 */
QMap<int, RibbonPage *> RibbonActionsManager::autoRegisterActions(RibbonBar *bar)
{
    Q_D(RibbonActionsManager);
    QMap<int, RibbonPage *> res;
    if (Q_NULLPTR == bar) {
        return res;
    }
    // 先遍历RibbonBar下的所有子对象，把所有action找到
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
    // 找到不在功能区的actions
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
 * @brief 自动加载action
 * @param w
 * @param tag
 * @param enableEmit
 * @return 返回成功加入RibbonActionsManager管理的action
 */
QSet<QAction *> RibbonActionsManager::autoRegisterWidgetActions(QWidget *w, int tag, bool enableEmit)
{
    QSet<QAction *> res;
    QList<QAction *> was = w->actions();

    for (QAction *a : was) {
        if (res.contains(a) || a->objectName().isEmpty()) {
            // 重复内容不重复加入，没有object name不加入
            continue;
        }
        if (registerAction(a, tag, a->objectName(), enableEmit)) {
            res.insert(a);
        }
    }
    return res;
}

/**
 * @brief 根据标题查找action
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
 * @brief 此函数用于设置所有管理的action都可以自定义编辑
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
 * @brief action 被delete时，将触发此槽把管理的action删除
 * @param o
 * @note 这个函数不会触发actionTagChanged信号
 */
void RibbonActionsManager::onActionDestroyed(QObject *o)
{
    QAction *act = static_cast<QAction *>(o);

    removeAction(act, false);
}

/**
 * @brief autoRegisteActions函数会关联此槽，在标签内容改变时改变tag 对应 文本
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
    QString m_seatchText;
    QList<QAction *> m_actions;
};

RibbonActionsManagerModelPrivate::RibbonActionsManagerModelPrivate()
    : m_mgr(Q_NULLPTR)
    , m_tag(RibbonActionsManager::CommonlyUsedActionTag)
    , m_seatchText(QString())
{
}

void RibbonActionsManagerModelPrivate::updateRef()
{
    if (isNull()) {
        return;
    }
    if (!m_seatchText.isEmpty()) {
        m_actions = m_mgr->search(m_seatchText);
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
    : QAbstractListModel(p)
{
    QX_INIT_PRIVATE(RibbonActionsManagerModel)
}

RibbonActionsManagerModel::RibbonActionsManagerModel(RibbonActionsManager *m, QObject *p)
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
    if (parent.isValid()) {   // 非顶层
        return 0;
    }
    // 顶层
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
 * @brief 查找
 * @param text
 */
void RibbonActionsManagerModel::search(const QString &text)
{
    Q_D(RibbonActionsManagerModel);
    d->m_seatchText = text;
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
