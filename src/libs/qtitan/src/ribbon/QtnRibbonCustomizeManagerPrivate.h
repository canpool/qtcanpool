/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2015 Developer Machines (http://www.devmachines.com)
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/
#ifndef QTN_RIBBONCUSTOMIZEMANAGERPRIVATE_H
#define QTN_RIBBONCUSTOMIZEMANAGERPRIVATE_H

#include <QMap>
#include <QSet>
#include <QList>
#include <QXmlStreamWriter>
#include <QAction>
#include <QString>
#include <QToolBar>

#include "QtitanDef.h"



QTITAN_BEGIN_NAMESPACE
#define UserNameID     1000  // first user name id
#define MaxUserNameID  65535 // last user name id

#define __qtn_Widget_Custom       "WidgetCustom"
#define __qtn_Widget_Copy         "WidgetCopy"
#define __qtn_Action_Invisible    "__qtn_Action_Invisible"
#define __qtn_Quick_Access_Button "__qtn_Quick_Access_Button"

/* CustomizeEngineAbstract */
class CustomizeEngineAbstract : public QObject
{
public:
    explicit CustomizeEngineAbstract(QObject* parent);
    virtual ~CustomizeEngineAbstract();

public:
    QString categoryAllActions() const;
    QList<QAction*> actionsAll() const;

    QList<QAction*> actionsByCategory(const QString& category) const;
    QStringList categories() const;

    QString actionId(QAction* action) const;
    void setActionId(QAction* action, const QString& id = QString());

    void addToCategory(const QString& category, QAction* action);
    void addAllActionsCategory(const QString& category);

    virtual void saveState(QXmlStreamWriter& stream) const = 0;
    virtual bool restoreState(QXmlStreamReader&stream) = 0;

protected:
    QString generateUniqueNameIdentifier(int hint = -1);
    QString generateUniqueNameIdentifierGroup(int hint = -1);
    QString generateUniqueNameIdentifierPage(int hint = -1);

protected:
    QSet< int > m_setUniqueIdentifier;
    QSet< int > m_setUniqueIdentifierGroup;
    QSet< int > m_setUniqueIdentifierPage;

    QString m_allCategoryName;
    QList< QString > m_lstCategory;

    QMap< QString, QAction* > m_allNameToActions;
    QMap< QAction*, QString > m_allActionsToName;

    QMap< QString, QList<QAction*> > m_categoryToActions;
    QMap< QAction*, QString > m_actionToCategory;

private:
    Q_DISABLE_COPY(CustomizeEngineAbstract)
};

/* ToolBarCustomizeEngine */
class ToolBarCustomizeEngine : public CustomizeEngineAbstract
{
public:
    explicit ToolBarCustomizeEngine(QObject* parent);
    virtual ~ToolBarCustomizeEngine();

public:
    QList<QAction*> actions(QToolBar* toolBar) const;
    QToolBar* toolBarWidgetAction(QAction* action) const;
    QToolBar* findDefaultToolBar(const QString& objectName) const;
    QMap< QToolBar*, QList<QAction*> > defaultToolBars() const;

    void removeWidgetActions(const QMap< QToolBar*, QList<QAction*> >& actions);
    void addDefaultToolBar(QToolBar* toolBar);

    QList<QToolBar *> toolBarsByCategory(const QString& category) const;
    void addToCategory(const QString& category, QToolBar* bar); 

    void setToolBar(QToolBar* toolBar, const QList<QAction*>& actions);

    void saveStateQuickAccessBar(QXmlStreamWriter& stream) const;
    bool restoreStateQuickAccessBar(QXmlStreamReader& stream);

    void saveState(QXmlStreamWriter& stream) const;
    bool restoreState(QXmlStreamReader& stream);

protected:
    QMap< QAction*, QToolBar* > m_widgetActions;
    QMap< QAction*, QList<QToolBar*> > m_actionToToolBars;

    QMap< QString, QList<QToolBar*> > m_categoryToBars;
    QMap< QToolBar*, QString > m_barToCategory;

    QMap< QToolBar*, QList<QAction*> > m_defaultToolBars;
    QMap< QToolBar*, QList<QAction*> > m_toolBarsWithSeparators;
    QMap< QToolBar*, QList<QAction*> > m_toolBars;
    QMap< QToolBar*, QList<QAction*> > m_regularToolBars;

private:
    Q_DISABLE_COPY(ToolBarCustomizeEngine)
};

class RibbonPage;
class RibbonGroup;
/* RibbonBarCustomizeEngine */
class RibbonBarCustomizeEngine : public ToolBarCustomizeEngine
{
public:
    explicit RibbonBarCustomizeEngine(QObject* parent, RibbonBar* ribbonBar);
    virtual ~RibbonBarCustomizeEngine();

public:
    QString pageId(RibbonPage* page) const;
    void setPageId(RibbonPage* page, const QString& id = QString());

    QString groupId(RibbonGroup* group) const;
    void setGroupId(RibbonGroup* group, const QString& id = QString());

    void addToCategory(const QString& category, QAction* act);

    QList< RibbonPage* > pagesByCategory(const QString& category) const;
    void addToCategory(const QString& category, RibbonPage* page);

    QList< RibbonGroup* > groupsByCategory(const QString& category) const;
    void addToCategory(const QString& category, RibbonGroup* group);

    QList<RibbonGroup*> pageDefaultGroups(RibbonPage* page) const;
    QList< RibbonPage* > defaultHiddenPages() const;
    QList< RibbonPage* > editHiddenPages() const;

    QMap< RibbonPage*, QString > defaultPagesName() const;
    QString pageDefaultName(RibbonPage* page) const;

    QList< RibbonGroup* > regularGroups(RibbonPage* page) const;
    QMap< RibbonPage*,  QList<RibbonGroup*> > defaultPageGroups() const;
    QMap< RibbonPage*,  QList<RibbonGroup*> > regularPageGroups() const;
    QMap< RibbonGroup*, QString > defaultGroupsName() const;
    QString groupDefaultName(RibbonGroup* group) const;

    QList<RibbonPage*> defaultRibbonBar() const;

    void addDefaultPages(RibbonBar* ribbonBar);
    void addDefaultGroups(RibbonPage* page);
    void addNonContextCategories();

    void setQuickAccessBar();
    void setRibbonBar();
    void setRibbonGroups();
    void updateRibbonBar();
    void deleteFreeGroups();
    void deleteFreePages();

    void saveStateGroup(QXmlStreamWriter& stream, RibbonPage* page) const;
    bool restoreStateGroup(QXmlStreamReader& stream, RibbonPage* page);

    void saveStatePage(QXmlStreamWriter& stream) const;
    bool restoreStatePage(QXmlStreamReader& stream);

    void saveStateRibbonBar(QXmlStreamWriter& stream) const;
    bool restoreStateRibbonBar(QXmlStreamReader& stream);

    void saveState(QXmlStreamWriter& stream) const;
    bool restoreState(QXmlStreamReader& stream);

protected:
    RibbonBar* m_ribbonBar;

    QMap< QString, QList<RibbonPage*> > m_categoryToPages;
    QMap< RibbonPage*, QString > m_pageToCategory;

    QMap< QString, QList<RibbonGroup*> > m_categoryToGroups;
    QMap< RibbonGroup*, QString > m_groupToCategory;

    QList< RibbonPage* > m_defaultPages;
    QList< RibbonPage* > m_regularPages;
    QList< RibbonPage* > m_defaultHiddenPages;
    QList< RibbonPage* > m_regularHiddenPages;

    QMap< RibbonPage*, QString > m_defaultPagesName;
    QMap< RibbonPage*, QString > m_regularPagesName;

    QMap< QString, RibbonPage* > m_allNameIdToPages;
    QMap< RibbonPage*, QString > m_allPagesToNameId;

    QMap< QString, RibbonGroup* > m_allNameIdToGroups;
    QMap< RibbonGroup*, QString > m_allGroupsToNameId;

    QMap< RibbonPage*,  QList<RibbonGroup*> > m_defaultPageGroups;
    QMap< RibbonPage*,  QList<RibbonGroup*> > m_regularPageGroups;
    QMap< RibbonGroup*, QString > m_defaultNameGroups;
    QMap< RibbonGroup*, QString > m_regularNameGroups;

    QMap< RibbonGroup*, QList<QAction*> > m_regularGroupsActions;

private:
    friend class RibbonCustomizeManager;
    Q_DISABLE_COPY(RibbonBarCustomizeEngine)
};


/* RibbonCustomizeManagerPrivate */
class RibbonCustomizeManagerPrivate : public QObject
{
public:
    QTN_DECLARE_PUBLIC(RibbonCustomizeManager)

public:
    explicit RibbonCustomizeManagerPrivate();
    virtual ~RibbonCustomizeManagerPrivate();

public:
    void init(RibbonBar* ribbonBar);

public:
    bool m_editMode;
    RibbonBar* m_ribbonBar;
    RibbonBarCustomizeEngine* m_ribbonManager;

private:
    Q_DISABLE_COPY(RibbonCustomizeManagerPrivate)
};

QTITAN_END_NAMESPACE

#endif // QTN_RIBBONCUSTOMIZEMANAGERPRIVATE_H
