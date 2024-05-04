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
// RibbonActionsModel 特有
class RibbonActionsManagerModelPrivate;

/**
 * @brief 用于管理 Ribbon 的所有 Action
 *
 * RibbonActionsManager 维护着两个表，一个是 tag（标签）对应的 Action list，
 * 一个是所有接受 RibbonActionsManager 管理的 action list。
 *
 * RibbonActionsManager 的标签对应一组 actions，每个标签对应的 action 可以重复出现，
 * 但 RibbonActionsManager 维护的 action list 里只有一份 action，不会重复出现。
 *
 * tag 用于对 action list 分组，每个 tag 的实体名字通过 @ref setTagName 进行设置，在语言变化时需要及时调用
 * setTagName 设置新的标签对应的文本。
 *
 * RibbonActionsManager 默认预设了6个常用标签见 @ref RibbonActionsManager::ActionTag ，用户自定义标签需要在
 * RibbonActionsManager::UserDefineActionTag 值的基础上进行累加。
 *
 * @ref filter （等同 @ref actions ）函数用于提取标签管理的 action list， @ref allActions 函数返回 RibbonActionsManager
 * 管理的所有标签。
 *
 * 通过 @ref autoRegisteActions 函数可以快速的建立 action 的管理，此函数会遍历 @ref RibbonBar 下的所有子 object，
 * 同时遍历 RibbonBar 下所有 @ref RibbonGroup
 * 添加的 action, 并给予 Page 建立 tag，正常使用用户仅需关注此 autoRegisteActions 函数即可
 *
 */
class QX_RIBBON_EXPORT RibbonActionsManager : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 定义action的标签
     */
    enum ActionTag {
        UnknowActionTag = 0,                    ///< 未知的tag
        CommonlyUsedActionTag = 0x01,           ///< 预设tag-常用命令
        NotInFunctionalAreaActionTag = 0x02,    ///< 预设tag-不在功能区命令
        AutoPageDistinguishBeginTag = 0x1000,   ///< 自动按Page划分的标签起始，在 @ref autoRegisteActions 函数会用到
        AutoPageDistinguishEndTag = 0x2000,     ///< 自动按Page划分的标签结束，在 @ref autoRegisteActions 函数会用到
        NotInRibbonPageTag = 0x2001,            ///< 不在功能区的标签@ref autoRegisteActions
                                                ///< 函数会遍历所有page的action以及RibbonBar下的action，如果两个
        UserDefineActionTag = 0x8000,           ///< 自定义标签，所有用户自定义tag要大于此tag
    };
public:
    RibbonActionsManager(RibbonBar *p);
    ~RibbonActionsManager();

    // 设置tag对应的名字
    void setTagName(int tag, const QString &name);

    // 获取tag对应的名字
    QString tagName(int tag) const;

    // 移除tag，注意，这个函数非常耗时
    void removeTag(int tag);

    // 注册action
    bool registeAction(QAction *act, int tag, const QString &key = QString(), bool enableEmit = true);

    // 取消action的注册
    void unregisteAction(QAction *act, bool enableEmit = true);

    // 过滤得到actions对应的引用，实际是一个迭代器
    QList<QAction *> &filter(int tag);

    // 通过tag筛选出系列action
    QList<QAction *> &actions(int tag);
    const QList<QAction *> actions(int tag) const;

    // 获取所有的标签
    QList<int> actionTags() const;

    // 通过key获取action
    QAction *action(const QString &key) const;

    // 通过action找到key
    QString key(QAction *act) const;

    // 返回所有管理的action数
    int count() const;

    // 返回所有管理的actions
    QList<QAction *> allActions() const;

    // 自动加载action,返回tag对应的Page指针
    QMap<int, RibbonPage *> autoRegisteActions(RibbonBar *bar);

    // 自动加载widget下的actions函数返回的action,返回加载的数量，这些
    QSet<QAction *> autoRegisteWidgetActions(QWidget *w, int tag, bool enableEmit = false);

    // 根据标题查找action
    QList<QAction *> search(const QString &text);

    // 清除
    void clear();

    // 获取ribbonbar
    RibbonBar *ribbonBar() const;

    // 设置所有管理的action都可以编辑
    void setAllActionCanCustomize(bool on = true);

signals:
    /**
     * @brief 标签变化触发的信号，变化包括新增和删除
     */
    void actionTagChanged(int tag, bool isdelete);

private slots:
    void onActionDestroyed(QObject *o);
    void onPageTitleChanged(const QString &title);
private:
    void removeAction(QAction *act, bool enableEmit = true);
private:
    QX_DECLARE_PRIVATE(RibbonActionsManager)
    friend class RibbonActionsManagerModel;
};

/**
 * @brief RibbonActionsManager 对应的model
 */
class QX_RIBBON_EXPORT RibbonActionsManagerModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit RibbonActionsManagerModel(QObject *p = Q_NULLPTR);
    explicit RibbonActionsManagerModel(RibbonActionsManager *m, QObject *p = Q_NULLPTR);
    ~RibbonActionsManagerModel();

    virtual int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    void setFilter(int tag);
    void update();
    void setupActionsManager(RibbonActionsManager *m);
    void uninstallActionsManager();
    QAction *indexToAction(QModelIndex index) const;
    void search(const QString &text);

private slots:
    void onActionTagChanged(int tag, bool isdelete);
private:
    QX_DECLARE_PRIVATE(RibbonActionsManagerModel)
};

QX_RIBBON_END_NAMESPACE
