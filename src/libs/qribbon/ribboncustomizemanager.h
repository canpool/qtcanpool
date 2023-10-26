/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "ribbon_global.h"
#include "ribbonbar.h"
#include "ribbongroup.h"
#include "ribbonpage.h"
#include <QAction>
#include <QList>
#include <QString>
#include <QToolBar>

QRIBBON_BEGIN_NAMESPACE

class RibbonCustomizeManagerPrivate;
/* RibbonCustomizeManager */
class QRIBBON_EXPORT RibbonCustomizeManager : public QObject
{
public:
    explicit RibbonCustomizeManager(RibbonBar *ribbonBar);
    virtual ~RibbonCustomizeManager();
public:
    QStringList categories() const;
    QList<QAction *> actionsAll() const;

    void addToolBar(QToolBar *toolBar);

    void addAllActionsCategory(const QString &category);

    QList<QAction *> actionsByCategory(const QString &category) const;
    void addToCategory(const QString &category, QAction *act);

    QList<QToolBar *> toolBarsByCategory(const QString &category) const;
    void addToCategory(const QString &category, QToolBar *toolBar);

    QList<RibbonPage *> pagesByCategory(const QString &category) const;
    void addToCategory(const QString &category, RibbonPage *page);

    QList<RibbonGroup *> groupsByCategory(const QString &category) const;
    void addToCategory(const QString &category, RibbonGroup *group);

    QString actionId(QAction *action) const;
    void setActionId(QAction *action, const QString &id);

    QString pageId(RibbonPage *page) const;
    void setPageId(RibbonPage *page, const QString &id);

    QString groupId(RibbonGroup *group) const;
    void setGroupId(RibbonGroup *group, const QString &id);

    void setEditMode(bool set = true);
    bool isEditMode() const;

    void reset(QToolBar *toolBar = Q_NULL);
    void commit();
    void cancel();

    QList<QAction *> actions(QToolBar *toolBar) const;
    void insertAction(QToolBar *toolBar, QAction *action, int index);
    void removeActionAt(QToolBar *toolBar, int index);
    bool containsAction(QToolBar *toolBar, QAction *action) const;

    QList<RibbonPage *> pages() const;
    RibbonPage *createPage(const QString &pageName, int index = -1);
    void deletePage(RibbonPage *page);
    int pageIndex(RibbonPage *page) const;
    void movePage(int oldIndex, int newIndex);

    QList<RibbonGroup *> pageGroups(RibbonPage *page) const;
    RibbonGroup *createGroup(RibbonPage *page, const QString &groupName, int index = -1);
    void deleteGroup(RibbonPage *page, int index);
    void moveGroup(RibbonPage *page, int oldIndex, int newIndex);

    void insertAction(RibbonGroup *group, QAction *action, int index);
    void appendActions(RibbonGroup *group, const QList<QAction *> &actions);
    void clearActions(RibbonGroup *group);
    void removeActionAt(RibbonGroup *group, int index);
    bool containsAction(RibbonGroup *group, QAction *action) const;
    QList<QAction *> actionsGroup(RibbonGroup *group) const;

    bool isPageHidden(RibbonPage *page) const;
    void setPageHidden(RibbonPage *page, bool hide);

    bool isPageVisible(RibbonPage *page) const;
    void setPageVisible(RibbonPage *page, bool visible);

    QString pageName(RibbonPage *page) const;
    void setPageName(RibbonPage *page, const QString &pageName);

    QString groupName(RibbonGroup *group) const;
    void setGroupName(RibbonGroup *group, const QString &groupName);

    bool saveStateToDevice(QIODevice *device);
    bool loadStateFromDevice(QIODevice *device);
public:
    void addDefaultStateQAccessBar();
    void addDefaultStateRibbonBar();
private:
    friend class RibbonQuickAccessBar;
    friend class RibbonBarCustomizePagePrivate;
    QRN_DECLARE_PRIVATE(RibbonCustomizeManager)
    Q_DISABLE_COPY(RibbonCustomizeManager)
};

QRIBBON_END_NAMESPACE
