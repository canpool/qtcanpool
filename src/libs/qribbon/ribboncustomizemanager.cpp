/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include "ribboncustomizemanager.h"
#include "ribbon_def.h"
#include "ribbonbar.h"
#include "ribboncustomizemanager_p.h"
#include "ribbongroup.h"
#include "ribbonpage.h"
#include "ribbonpage_p.h"
#include "ribbonquickaccessbar.h"
#include <QBuffer>

QRIBBON_USE_NAMESPACE

#define QTC_DIC_TAGNAME_RIBBON        "RibbonStorage"
#define QTC_DIC_TAGNAME_RIBBON_QABAR  "QuickAccessBar"
#define QTC_DIC_TAGNAME_RIBBON_BAR    "RibbonBar"
#define QTC_DIC_TAGNAME_RIBBON_PAGE   "RibbonPage"
#define QTC_DIC_TAGNAME_RIBBON_GROUP  "RibbonGroup"
#define QTC_DIC_TAGNAME_RIBBON_ACTION "RibbonAction"

#define QTC_DIC_ATTRNAME_RIBBON_ID      "nameId"
#define QTC_DIC_ATTRNAME_RIBBON_TITLE   "title"
#define QTC_DIC_ATTRNAME_RIBBON_VISIBLE "visible"
#define QTC_DIC_ATTRNAME_RIBBON_TYPE    "type"

/* CustomizeEngineAbstract */
CustomizeEngineAbstract::CustomizeEngineAbstract(QObject *parent) : QObject(parent) {}

CustomizeEngineAbstract::~CustomizeEngineAbstract() {}

QString CustomizeEngineAbstract::categoryAllActions() const { return m_allCategoryName; }

QList<QAction *> CustomizeEngineAbstract::actionsAll() const { return m_allNameToActions.values(); }

QList<QAction *> CustomizeEngineAbstract::actionsByCategory(const QString &category) const
{
    QMap<QString, QList<QAction *> >::ConstIterator it = m_categoryToActions.find(category);
    if (it != m_categoryToActions.constEnd())
        return it.value();
    return QList<QAction *>();
}

QStringList CustomizeEngineAbstract::categories() const { return m_lstCategory; }

QString CustomizeEngineAbstract::actionId(QAction *action) const { return m_allActionsToName.value(action); }

void CustomizeEngineAbstract::setActionId(QAction *action, const QString &id)
{
    QString cmdName = id;

    if (action == Q_NULL || action->isSeparator())
        return;

    if (cmdName.isEmpty()) {
        // qWarning("CustomizeEngineAbstract::setActionId(): 'commandName' not set for QAction "
        //     "%p '%s', using 'text' instead", action, action->text().toLocal8Bit().constData());
        cmdName = action->text();

        if (!cmdName.isEmpty())
            cmdName += "_";

        cmdName += CustomizeEngineAbstract::generateUniqueNameIdentifier();
    }

    if (cmdName.isEmpty()) {
        Q_ASSERT(false);
        return;
    }

    if (m_allNameToActions.contains(cmdName))
        return;

    if (m_allActionsToName.contains(action))
        return;

    m_allNameToActions.insert(cmdName, action);
    m_allActionsToName.insert(action, cmdName);

    if (!m_allCategoryName.isEmpty()) {
        m_categoryToActions[m_allCategoryName].append(action);
        m_actionToCategory[action] = m_allCategoryName;
    }
}

void CustomizeEngineAbstract::addAllActionsCategory(const QString &category)
{
    if (!category.isEmpty() && !m_lstCategory.contains(category)) {
        m_allCategoryName = category;
        m_lstCategory.append(category);
    }
}

void CustomizeEngineAbstract::addToCategory(const QString &category, QAction *action)
{
    if (category.isEmpty() || action == Q_NULL || action->isSeparator())
        return;

    m_categoryToActions[category].append(action);
    m_actionToCategory[action] = category;

    if (!m_lstCategory.contains(category))
        m_lstCategory.append(category);

    setActionId(action);
}

QString CustomizeEngineAbstract::generateUniqueNameIdentifier(int hint)
{
    static QString prefix = "__qtc_Action_%1";

    if (hint >= UserNameID && hint <= MaxUserNameID && !m_setUniqueIdentifier.contains(hint)) {
        m_setUniqueIdentifier.insert(hint);
        return prefix.arg(hint);
    }

    int id = MaxUserNameID;
    while (m_setUniqueIdentifier.contains(id) && id >= UserNameID)
        --id;

    if (id >= UserNameID) {
        m_setUniqueIdentifier.insert(id);
        return prefix.arg(id);
    }
    return QString();
}

QString CustomizeEngineAbstract::generateUniqueNameIdentifierGroup(int hint /* = -1*/)
{
    static QString prefix = "__qtc_Group_%1";

    if (hint >= UserNameID && hint <= MaxUserNameID && !m_setUniqueIdentifierGroup.contains(hint)) {
        m_setUniqueIdentifierGroup.insert(hint);
        return prefix.arg(hint);
    }

    int id = MaxUserNameID;
    while (m_setUniqueIdentifierGroup.contains(id) && id >= UserNameID)
        --id;

    if (id >= UserNameID) {
        m_setUniqueIdentifierGroup.insert(id);
        return prefix.arg(id);
    }
    return QString();
}

QString CustomizeEngineAbstract::generateUniqueNameIdentifierPage(int hint /* = -1*/)
{
    static QString prefix = "__qtc_Page_%1";

    if (hint >= UserNameID && hint <= MaxUserNameID && !m_setUniqueIdentifierPage.contains(hint)) {
        m_setUniqueIdentifierPage.insert(hint);
        return prefix.arg(hint);
    }

    int id = MaxUserNameID;
    while (m_setUniqueIdentifierPage.contains(id) && id >= UserNameID)
        --id;

    if (id >= UserNameID) {
        m_setUniqueIdentifierPage.insert(id);
        return prefix.arg(id);
    }
    return QString();
}

/* ToolBarCustomizeEngine */
ToolBarCustomizeEngine::ToolBarCustomizeEngine(QObject *parent) : CustomizeEngineAbstract(parent) {}

ToolBarCustomizeEngine::~ToolBarCustomizeEngine() {}

QList<QAction *> ToolBarCustomizeEngine::actions(QToolBar *toolBar) const
{
    if (m_toolBars.contains(toolBar))
        return m_toolBars.value(toolBar);
    return QList<QAction *>();
}

QToolBar *ToolBarCustomizeEngine::toolBarWidgetAction(QAction *action) const
{
    if (m_widgetActions.contains(action))
        return m_widgetActions.value(action);
    return Q_NULL;
}

QToolBar *ToolBarCustomizeEngine::findDefaultToolBar(const QString &objectName) const
{
    QMap<QToolBar *, QList<QAction *> >::ConstIterator itToolBar = m_defaultToolBars.constBegin();
    while (itToolBar != m_defaultToolBars.constEnd()) {
        QToolBar *tb = itToolBar.key();
        if (tb->objectName() == objectName)
            return tb;

        ++itToolBar;
    }

    qWarning("ToolBarCustomizeEngine::findDefaultToolBar(): cannot find a QToolBar named "
             "'%s', trying to match using 'windowTitle' instead.",
             objectName.toLocal8Bit().constData());

    itToolBar = m_defaultToolBars.constBegin();
    while (itToolBar != m_defaultToolBars.constEnd()) {
        QToolBar *tb = itToolBar.key();
        if (tb->windowTitle() == objectName)
            return tb;
        ++itToolBar;
    }
    qWarning("ToolBarCustomizeEngine::findDefaultToolBar(): cannot find a QToolBar with "
             "matching 'windowTitle' (looking for '%s').",
             objectName.toLocal8Bit().constData());

    return Q_NULL;
}

QMap<QToolBar *, QList<QAction *> > ToolBarCustomizeEngine::defaultToolBars() const { return m_defaultToolBars; }

void ToolBarCustomizeEngine::removeWidgetActions(const QMap<QToolBar *, QList<QAction *> > &actions)
{
    QMap<QToolBar *, QList<QAction *> >::ConstIterator itWidget = actions.constBegin();
    while (itWidget != actions.constEnd()) {
        QToolBar *toolBar = itWidget.key();
        QList<QAction *> newActions = m_toolBars.value(toolBar);
        QList<QAction *> newActionsWithSeparators = m_toolBarsWithSeparators.value(toolBar);

        QList<QAction *> removedActions;
        QList<QAction *> actionList = itWidget.value();
        QListIterator<QAction *> itAction(actionList);
        while (itAction.hasNext()) {
            QAction *action = itAction.next();
            if (newActions.contains(action) && toolBarWidgetAction(action) == toolBar) {
                newActions.removeAll(action);
                newActionsWithSeparators.removeAll(action);
                removedActions.append(action);
            }
        }

        m_toolBars.insert(toolBar, newActions);
        m_toolBarsWithSeparators.insert(toolBar, newActionsWithSeparators);
        QListIterator<QAction *> itRemovedAction(removedActions);
        while (itRemovedAction.hasNext()) {
            QAction *oldAction = itRemovedAction.next();
            m_widgetActions.insert(oldAction, 0);
            m_actionToToolBars[oldAction].removeAll(toolBar);
        }
        ++itWidget;
    }
}

void ToolBarCustomizeEngine::addDefaultToolBar(QToolBar *toolBar)
{
    if (!toolBar)
        return;

    if (m_toolBars.contains(toolBar))
        return;

    QList<QAction *> newActionsWithSeparators;
    QList<QAction *> newActions;
    QList<QAction *> actions = toolBar->actions();
    QListIterator<QAction *> itAction(actions);
    while (itAction.hasNext()) {
        QAction *action = itAction.next();
        if (action->isVisible() &&
            action->property(__qtc_Action_Invisible).toString() != QLatin1String("__qtc_Action_Invisible") &&
            action->property(__qtc_Quick_Access_Button).toString() != QLatin1String("__qtc_Quick_Access_Button")) {
            setActionId(action);

            if (m_widgetActions.contains(action))
                m_widgetActions.insert(action, toolBar);

            newActionsWithSeparators.append(action);
            if (action->isSeparator())
                action = 0;
            else
                m_actionToToolBars[action].append(toolBar);
            newActions.append(action);
        }
    }

    m_defaultToolBars.insert(toolBar, newActions);
    m_toolBars.insert(toolBar, newActions);
    m_toolBarsWithSeparators.insert(toolBar, newActionsWithSeparators);
}

QList<QToolBar *> ToolBarCustomizeEngine::toolBarsByCategory(const QString &category) const
{
    return m_categoryToBars.value(category);
}

void ToolBarCustomizeEngine::addToCategory(const QString &category, QToolBar *bar)
{
    if (category.isEmpty() || bar == Q_NULL)
        return;

    if (m_barToCategory.contains(bar))
        return;

    m_categoryToBars[category].append(bar);
    m_barToCategory[bar] = category;

    if (!m_lstCategory.contains(category))
        m_lstCategory.append(category);
}

void ToolBarCustomizeEngine::setToolBar(QToolBar *toolBar, const QList<QAction *> &actions)
{
    if (!toolBar)
        return;
    if (!m_toolBars.contains(toolBar))
        return;
    if (actions == m_toolBars[toolBar])
        return;

    QMap<QToolBar *, QList<QAction *> > toRemove;
    QList<QAction *> newActions;
    QListIterator<QAction *> itAction(actions);
    while (itAction.hasNext()) {
        QAction *action = itAction.next();
        if (!action || (!newActions.contains(action) && m_allNameToActions.values().contains(action)))
            newActions.append(action);

        QToolBar *oldToolBar = toolBarWidgetAction(action);
        if (oldToolBar && oldToolBar != toolBar)
            toRemove[oldToolBar].append(action);
    }

    removeWidgetActions(toRemove);

    QList<QAction *> oldActions = m_toolBarsWithSeparators.value(toolBar);
    QListIterator<QAction *> itOldAction(oldActions);
    while (itOldAction.hasNext()) {
        QAction *action = itOldAction.next();
        if (action == Q_NULL)
            continue;

        if (toolBarWidgetAction(action) == toolBar)
            m_widgetActions.insert(action, 0);
        toolBar->removeAction(action);
        if (action->isSeparator())
            delete action;
        else
            m_actionToToolBars[action].removeAll(toolBar);
    }

    QList<QAction *> newActionsWithSeparators;
    QListIterator<QAction *> itNewActions(newActions);
    while (itNewActions.hasNext()) {
        QAction *action = itNewActions.next();
        QAction *newAction = Q_NULL;
        if (!action)
            newAction = toolBar->insertSeparator(0);

        if (m_allNameToActions.values().contains(action)) {
            toolBar->insertAction(0, action);
            newAction = action;
            m_actionToToolBars[action].append(toolBar);
        }
        newActionsWithSeparators.append(newAction);
    }
    m_toolBars.insert(toolBar, newActions);
    m_toolBarsWithSeparators.insert(toolBar, newActionsWithSeparators);
}

void ToolBarCustomizeEngine::saveStateQuickAccessBar(QXmlStreamWriter &stream) const
{
    // stream.writeStartElement(QTC_DIC_TAGNAME_RIBBON_QABAR);

    QMap<QToolBar *, QList<QAction *> >::ConstIterator itToolBar = m_defaultToolBars.constBegin();
    while (itToolBar != m_defaultToolBars.constEnd()) {
        stream.writeStartElement(QTC_DIC_TAGNAME_RIBBON_QABAR);

        QToolBar *tb = itToolBar.key();
        QString title = tb->objectName();
        if (tb->objectName().isEmpty()) {
            qWarning("ToolBarCustomizeEngine::saveStateQuickAccessBar(): 'objectName' not set for QToolBar "
                     "%p '%s', using 'windowTitle' instead",
                     tb, tb->windowTitle().toLocal8Bit().constData());
            title = tb->windowTitle();
        }

        stream.writeAttribute(QTC_DIC_ATTRNAME_RIBBON_TITLE, title);

        QListIterator<QAction *> itAction(m_toolBars[tb]);
        while (itAction.hasNext()) {
            QString id;
            if (QAction *action = itAction.next()) {
                stream.writeStartElement(QTC_DIC_TAGNAME_RIBBON_ACTION);
                id = m_allActionsToName.value(action);
                if (id.isEmpty()) {
                    qWarning("ToolBarCustomizeEngine::saveStateQuickAccessBar(): 'objectName' not set for QAction "
                             "%p '%s', using 'text' instead",
                             action, action->text().toLocal8Bit().constData());
                    id = action->text();
                }
                stream.writeAttribute(QTC_DIC_ATTRNAME_RIBBON_ID, id);
                stream.writeEndElement();
            } else {
                stream.writeStartElement(QTC_DIC_TAGNAME_RIBBON_ACTION);
                stream.writeAttribute(QTC_DIC_ATTRNAME_RIBBON_ID, id);
                stream.writeEndElement();
            }
        }
        ++itToolBar;
        stream.writeEndElement();
    }

    // stream.writeEndElement();
}

bool ToolBarCustomizeEngine::restoreStateQuickAccessBar(QXmlStreamReader &stream)
{
    if (stream.readNext() != QXmlStreamReader::StartElement)
        return false;

    if (stream.name() != QTC_DIC_TAGNAME_RIBBON_QABAR)
        return false;

    while (stream.tokenType() == QXmlStreamReader::StartElement) {
        QXmlStreamAttributes attrsToolBar = stream.attributes();

        if (!attrsToolBar.hasAttribute(QTC_DIC_ATTRNAME_RIBBON_TITLE))
            return false;

        QString title = attrsToolBar.value(QTC_DIC_ATTRNAME_RIBBON_TITLE).toString();

        QList<QAction *> actions;
        if (stream.readNext() == QXmlStreamReader::StartElement) {
            if (stream.name() != QTC_DIC_TAGNAME_RIBBON_ACTION)
                return false;

            while (stream.tokenType() == QXmlStreamReader::StartElement) {
                QXmlStreamAttributes attrs = stream.attributes();

                if (!attrs.hasAttribute(QTC_DIC_ATTRNAME_RIBBON_ID))
                    return false;

                QString id = attrs.value(QTC_DIC_ATTRNAME_RIBBON_ID).toString();

                if (id.isEmpty())
                    actions.append(0);
                else if (QAction *action = m_allNameToActions.value(id))
                    actions.append(action);

                if (stream.readNext() != QXmlStreamReader::EndElement)
                    return false;

                stream.readNext();
            }
        }

        if (QToolBar *toolBar = findDefaultToolBar(title))
            setToolBar(toolBar, actions);
        /*
        QXmlStreamReader::TokenType type = stream.readNext();
        if (type != QXmlStreamReader::EndElement)
            return false;

        stream.readNext();
        */
    }

    // if (stream.readNext() != QXmlStreamReader::EndElement)
    //     return false;

    return true;
}

void ToolBarCustomizeEngine::saveState(QXmlStreamWriter &stream) const { saveStateQuickAccessBar(stream); }

bool ToolBarCustomizeEngine::restoreState(QXmlStreamReader &stream) { return restoreStateQuickAccessBar(stream); }

/* RibbonBarCustomizeEngine */
RibbonBarCustomizeEngine::RibbonBarCustomizeEngine(QObject *parent, RibbonBar *ribbonBar)
    : ToolBarCustomizeEngine(parent), m_ribbonBar(ribbonBar)
{
}

RibbonBarCustomizeEngine::~RibbonBarCustomizeEngine() {}

QString RibbonBarCustomizeEngine::pageId(RibbonPage *page) const { return m_allPagesToNameId.value(page); }

void RibbonBarCustomizeEngine::setPageId(RibbonPage *page, const QString &id)
{
    QString strPageName = id;

    if (page == Q_NULL)
        return;

    if (strPageName.isEmpty()) {
        // qWarning("RibbonBarCustomizeEngine::setPageId(): 'pageName' not set for RibbonPage "
        //     "%p '%s', using 'text' instead", page, page->title().toLocal8Bit().constData());

        strPageName = page->title();

        if (!strPageName.isEmpty())
            strPageName += "_";
        strPageName += CustomizeEngineAbstract::generateUniqueNameIdentifierPage();
    }

    if (strPageName.isEmpty()) {
        Q_ASSERT(false);
        return;
    }

    if (m_allNameIdToPages.contains(strPageName))
        return;

    if (m_allPagesToNameId.contains(page))
        return;

    m_allNameIdToPages.insert(strPageName, page);
    m_allPagesToNameId.insert(page, strPageName);
}

QString RibbonBarCustomizeEngine::groupId(RibbonGroup *group) const { return m_allGroupsToNameId.value(group); }

void RibbonBarCustomizeEngine::setGroupId(RibbonGroup *group, const QString &id)
{
    QString strGroupName = id;

    if (group == Q_NULL)
        return;

    if (strGroupName.isEmpty()) {
        // qWarning("RibbonBarCustomizeEngine::setGroupId(): 'groupName' not set for RibbonGroup "
        //     "%p '%s', using 'text' instead", group, group->title().toLocal8Bit().constData());
        strGroupName = CustomizeEngineAbstract::generateUniqueNameIdentifierGroup() + group->title();

        if (!strGroupName.isEmpty())
            strGroupName += "_";
        strGroupName += CustomizeEngineAbstract::generateUniqueNameIdentifierGroup();
    }

    if (strGroupName.isEmpty()) {
        Q_ASSERT(false);
        return;
    }

    if (m_allNameIdToGroups.contains(strGroupName))
        return;

    if (m_allGroupsToNameId.contains(group))
        return;

    m_allNameIdToGroups.insert(strGroupName, group);
    m_allGroupsToNameId.insert(group, strGroupName);
}

void RibbonBarCustomizeEngine::addToCategory(const QString &category, QAction *act)
{
    CustomizeEngineAbstract::addToCategory(category, act);
}

QList<RibbonPage *> RibbonBarCustomizeEngine::pagesByCategory(const QString &category) const
{
    return m_categoryToPages.value(category);
}

void RibbonBarCustomizeEngine::addToCategory(const QString &category, RibbonPage *page)
{
    if (category.isEmpty() || page == Q_NULL)
        return;

    if (m_pageToCategory.contains(page))
        return;

    m_categoryToPages[category].append(page);
    m_pageToCategory[page] = category;

    if (!m_lstCategory.contains(category))
        m_lstCategory.append(category);
}

QList<RibbonGroup *> RibbonBarCustomizeEngine::groupsByCategory(const QString &category) const
{
    return m_categoryToGroups.value(category);
}

void RibbonBarCustomizeEngine::addToCategory(const QString &category, RibbonGroup *group)
{
    if (category.isEmpty() || group == Q_NULL)
        return;

    QString strCategory = category;
    strCategory.remove(QChar::fromLatin1('&'));

    if (m_groupToCategory.contains(group))
        return;

    m_categoryToGroups[strCategory].append(group);
    m_groupToCategory[group] = strCategory;

    QList<QAction *> actions = group->actions();
    QListIterator<QAction *> itActions(actions);
    while (itActions.hasNext()) {
        QAction *action = itActions.next();
        if (!action->text().isEmpty()) {
            setActionId(action);
            addToCategory(strCategory, action);
        }
    }

    for (int index = 0, count = group->controlCount(); count > index; ++index) {
        RibbonControl *control = group->controlByIndex(index);
        Q_ASSERT(control != Q_NULL);
        if (RibbonToolBarControl *toolBarControl = qobject_cast<RibbonToolBarControl *>(control)) {
            QList<QAction *> actions = toolBarControl->actions();
            QListIterator<QAction *> itActions(actions);
            while (itActions.hasNext()) {
                QAction *action = itActions.next();
                if (!action->text().isEmpty()) {
                    setActionId(action);
                    addToCategory(strCategory, action);
                }
            }
        }
    }
}

QList<RibbonGroup *> RibbonBarCustomizeEngine::pageDefaultGroups(RibbonPage *page) const
{
    return m_defaultPageGroups.value(page);
}

QMap<RibbonPage *, QString> RibbonBarCustomizeEngine::defaultPagesName() const
{
    if (m_defaultPagesName.size() > 0)
        return m_defaultPagesName;
    return QMap<RibbonPage *, QString>();
}

QString RibbonBarCustomizeEngine::pageDefaultName(RibbonPage *page) const { return m_defaultPagesName.value(page); }

QList<RibbonPage *> RibbonBarCustomizeEngine::defaultHiddenPages() const
{
    if (m_defaultHiddenPages.size() > 0)
        return m_defaultHiddenPages;
    return QList<RibbonPage *>();
}

QList<RibbonPage *> RibbonBarCustomizeEngine::editHiddenPages() const
{
    if (m_regularHiddenPages.size() > 0)
        return m_regularHiddenPages;
    return QList<RibbonPage *>();
}

QList<RibbonGroup *> RibbonBarCustomizeEngine::regularGroups(RibbonPage *page) const
{
    if (m_regularPageGroups.contains(page))
        return m_regularPageGroups.value(page);
    return QList<RibbonGroup *>();
}

QMap<RibbonPage *, QList<RibbonGroup *> > RibbonBarCustomizeEngine::defaultPageGroups() const
{
    return m_defaultPageGroups;
}

QList<RibbonPage *> RibbonBarCustomizeEngine::defaultRibbonBar() const { return m_defaultPages; }

QMap<RibbonGroup *, QString> RibbonBarCustomizeEngine::defaultGroupsName() const { return m_defaultNameGroups; }

QString RibbonBarCustomizeEngine::groupDefaultName(RibbonGroup *group) const
{
    return m_defaultNameGroups.value(group);
}

void RibbonBarCustomizeEngine::addDefaultPages(RibbonBar *ribbonBar)
{
    if (!ribbonBar)
        return;

    QList<RibbonPage *> newPages;
    QList<RibbonPage *> pages = ribbonBar->pages();
    QListIterator<RibbonPage *> itPages(pages);

    while (itPages.hasNext()) {
        RibbonPage *page = itPages.next();
        // if (page->objectName() != QLatin1String("__qtc_Page_Invisible"))
        {
            setPageId(page);
            newPages.append(page);

            if (page->qtc_d()->m_associativeTab->isHidden())
                m_defaultHiddenPages.append(page);
        }
        m_defaultPagesName.insert(page, page->title());
        addDefaultGroups(page);
    }
    m_defaultPages.append(newPages);
}

void RibbonBarCustomizeEngine::addDefaultGroups(RibbonPage *page)
{
    if (!page)
        return;

    if (m_defaultPageGroups.contains(page))
        return;

    QList<RibbonGroup *> newGroups;
    QList<RibbonGroup *> groups = page->groups();
    QListIterator<RibbonGroup *> itGroups(groups);

    while (itGroups.hasNext()) {
        RibbonGroup *group = itGroups.next();
        // if (group->objectName() != QLatin1String("__qtc_Group_Invisible") )
        {
            setGroupId(group);
            newGroups.append(group);
        }
        m_defaultNameGroups.insert(group, group->title());
    }
    m_defaultPageGroups.insert(page, newGroups);
}

void RibbonBarCustomizeEngine::setQuickAccessBar()
{
    if (RibbonQuickAccessBar *quickAccessBar = m_ribbonBar->quickAccessBar())
        setToolBar(quickAccessBar, m_regularToolBars.value(quickAccessBar));
}

void RibbonBarCustomizeEngine::setRibbonBar()
{
    if (!m_ribbonBar)
        return;

    m_ribbonBar->setUpdatesEnabled(false);

    QList<RibbonPage *> currentListPage = m_ribbonBar->pages();
    if (m_regularPages.isEmpty()) {
        QListIterator<RibbonPage *> itRegularPage(currentListPage);
        while (itRegularPage.hasNext()) {
            RibbonPage *currentPage = itRegularPage.next();
            if (m_defaultPagesName.contains(currentPage)) {
                QString strTitle = m_defaultPagesName.value(currentPage);
                if (!strTitle.isEmpty())
                    currentPage->setTitle(strTitle);
            }
            currentPage->setVisible(!m_defaultHiddenPages.contains(currentPage));

            if (!m_regularPages.contains(currentPage)) {
                m_ribbonBar->detachPage(currentPage);
                delete currentPage;
            }
        }
    } else {
        QListIterator<RibbonPage *> itRegularPage(currentListPage);
        while (itRegularPage.hasNext()) {
            RibbonPage *currentPage = itRegularPage.next();
            if (!m_regularPages.contains(currentPage)) {
                if (currentPage->property(__qtc_Widget_Custom).toString() == QLatin1String("__qtc_Page_Custom"))
                    m_ribbonBar->removePage(currentPage);
            }
        }
    }

    QList<RibbonPage *> newListPage;
    QListIterator<RibbonPage *> itRegularPage(m_regularPages);
    while (itRegularPage.hasNext()) {
        RibbonPage *regularPage = itRegularPage.next();
        newListPage.append(regularPage);

        if (m_regularPagesName.contains(regularPage)) {
            QString strTitle = m_regularPagesName.value(regularPage);
            if (!strTitle.isEmpty())
                regularPage->setTitle(strTitle);
        }
        m_ribbonBar->detachPage(regularPage);
    }

    QListIterator<RibbonPage *> itNewPages(newListPage);
    while (itNewPages.hasNext()) {
        RibbonPage *page = itNewPages.next();
        m_ribbonBar->addPage(page);

        if (m_regularHiddenPages.contains(page))
            page->setVisible(false);
    }
    m_ribbonBar->setUpdatesEnabled(true);
}

void RibbonBarCustomizeEngine::setRibbonGroups()
{
    for (QMap<RibbonPage *, QList<RibbonGroup *> >::ConstIterator itCurPages = m_regularPageGroups.begin();
         itCurPages != m_regularPageGroups.end(); ++itCurPages) {
        RibbonPage *page = itCurPages.key();
        Q_ASSERT(page != Q_NULL);

        QList<RibbonGroup *> groupsPage = itCurPages.value();
        QList<RibbonGroup *> realGroups = page->groups();
        if (groupsPage != realGroups || m_regularNameGroups.size() > 0 || m_regularGroupsActions.size() > 0) {
            QList<RibbonGroup *> newGroups;
            QListIterator<RibbonGroup *> itGroups(groupsPage);
            while (itGroups.hasNext()) {
                RibbonGroup *group = itGroups.next();

                if (!group || (!newGroups.contains(group)))
                    newGroups.append(group);

                if (group) {
                    // if (!realGroups.contains(group) && !m_allGroupsToNameId.keys().contains(group))
                    //     setGroupId(group);

                    QListIterator<RibbonGroup *> itRegularGroups(realGroups);
                    while (itRegularGroups.hasNext()) {
                        RibbonGroup *regularGroup = itRegularGroups.next();
                        if (!groupsPage.contains(regularGroup) &&
                            (regularGroup->property(__qtc_Widget_Custom).toString() ==
                                 QLatin1String("__qtc_Group_Custom") ||
                             regularGroup->property(__qtc_Widget_Copy).toString() ==
                                 QLatin1String("__qtc_Group_Copy"))) {
                            realGroups.removeOne(regularGroup);
                            page->removeGroup(regularGroup);
                        }
                    }
                }
            }

            QListIterator<RibbonGroup *> itOldGroup(realGroups);
            while (itOldGroup.hasNext())
                page->detachGroup(itOldGroup.next());

            QListIterator<RibbonGroup *> itNewGroups(newGroups);
            while (itNewGroups.hasNext()) {
                RibbonGroup *group = itNewGroups.next();
                page->addGroup(group);
                group->setVisible(true);

                if (m_regularNameGroups.contains(group))
                    group->setTitle(m_regularNameGroups.value(group));

                if (group->property(__qtc_Widget_Custom).toString() == QLatin1String("__qtc_Group_Custom")) {
                    group->clear();
                    QList<QAction *> actions = m_regularGroupsActions[group];
                    QListIterator<QAction *> itActions(actions);
                    while (itActions.hasNext())
                        group->addAction(itActions.next());
                }
            }
        }
    }
}

void RibbonBarCustomizeEngine::updateRibbonBar()
{
    int currentIndex = m_ribbonBar->currentPageIndex();
    if (currentIndex != -1) {
        if (RibbonPage *page = m_ribbonBar->getPage(currentIndex)) {
            if (page->isVisible()) {
                page->updateLayout();
                return;
            }

            if (!m_regularHiddenPages.contains(page))
                return;
        }
    }

    int index = 0;
    const QList<RibbonPage *> &listPage = m_ribbonBar->pages();
    for (QList<RibbonPage *>::const_iterator it = listPage.constBegin(); it != listPage.constBegin(); ++it) {
        RibbonPage *page = (*it);
        if (page->isVisible() || (!m_regularHiddenPages.isEmpty() && !m_regularHiddenPages.contains(page))) {
            m_ribbonBar->setCurrentPageIndex(index);
            page->updateLayout();
            break;
        }
        index++;
    }
}

void RibbonBarCustomizeEngine::deleteFreeGroups()
{
    for (QMap<RibbonPage *, QList<RibbonGroup *> >::ConstIterator itPages = m_regularPageGroups.begin();
         itPages != m_regularPageGroups.end(); ++itPages) {
        RibbonPage *page = itPages.key();
        QList<RibbonGroup *> listGroup = m_regularPageGroups.value(page);
        for (int i = listGroup.count() - 1; i >= 0; --i) {
            RibbonGroup *group = listGroup.at(i);
            if (group && group->parent() == Q_NULL) {
                m_regularPageGroups[page].removeOne(group);
                delete group;
            }
        }
    }
}

void RibbonBarCustomizeEngine::deleteFreePages()
{
    QListIterator<RibbonPage *> itPages(m_regularPages);
    while (itPages.hasNext()) {
        RibbonPage *page = itPages.next();
        if (page && page->parent() == Q_NULL)
            delete page;
    }
}

void RibbonBarCustomizeEngine::saveStateGroup(QXmlStreamWriter &stream, RibbonPage *page) const
{
    Q_ASSERT(page != NULL);
    if (page == NULL)
        return;

    QList<RibbonGroup *> groups = page->groups();
    QListIterator<RibbonGroup *> itGroup(groups);
    while (itGroup.hasNext()) {
        if (RibbonGroup *group = itGroup.next()) {
            stream.writeStartElement(QTC_DIC_TAGNAME_RIBBON_GROUP);

            bool custom = group->property(__qtc_Widget_Custom).toString() == QLatin1String("__qtc_Group_Custom");
            bool copy = group->property(__qtc_Widget_Copy).toString() == QLatin1String("__qtc_Group_Copy");

            QString nameId = m_allGroupsToNameId.value(group);

            if (nameId.isEmpty()) {
                if (!(custom || copy)) {
                    qWarning("RibbonBarCustomizeEngine::saveStateGroup(): 'objectName' not set for RibbonGroup "
                             "%p '%s', using 'text' instead",
                             group, group->title().toLocal8Bit().constData());
                }
                nameId = page->title();
            }

            stream.writeAttribute(QTC_DIC_ATTRNAME_RIBBON_ID, nameId);
            stream.writeAttribute(QTC_DIC_ATTRNAME_RIBBON_TITLE, group->title());

            if (custom)
                stream.writeAttribute(QTC_DIC_ATTRNAME_RIBBON_TYPE, "CustomGroup");
            else if (copy)
                stream.writeAttribute(QTC_DIC_ATTRNAME_RIBBON_TYPE, "CopyGroup");

            if (custom || copy) {
                QList<QAction *> actions = group->actions();
                QListIterator<QAction *> itActions(actions);
                while (itActions.hasNext()) {
                    QString nameAction = m_allActionsToName.value(itActions.next());
                    if (!nameAction.isEmpty()) {
                        stream.writeStartElement(QTC_DIC_TAGNAME_RIBBON_ACTION);
                        stream.writeAttribute(QTC_DIC_ATTRNAME_RIBBON_ID, nameAction);
                        stream.writeEndElement();
                    }
                }
            }
            stream.writeEndElement();
        }
    }
}

bool RibbonBarCustomizeEngine::restoreStateGroup(QXmlStreamReader &stream, RibbonPage *page)
{
    if (stream.readNext() != QXmlStreamReader::StartElement)
        return false;
    if (stream.name() != QTC_DIC_TAGNAME_RIBBON_GROUP)
        return false;

    QList<RibbonGroup *> groups;
    QMap<RibbonGroup *, QList<QAction *> > groupActions;

    while (stream.tokenType() == QXmlStreamReader::StartElement) {
        QXmlStreamAttributes attrs = stream.attributes();
        if (!attrs.hasAttribute(QTC_DIC_ATTRNAME_RIBBON_ID))
            return false;

        QString nameId = attrs.value(QTC_DIC_ATTRNAME_RIBBON_ID).toString();

        if (!attrs.hasAttribute(QTC_DIC_ATTRNAME_RIBBON_TITLE))
            return false;

        QString title = attrs.value(QTC_DIC_ATTRNAME_RIBBON_TITLE).toString();

        bool customGroup = false;
        bool copyGroup = false;
        QString strType = attrs.value(QTC_DIC_ATTRNAME_RIBBON_TYPE).toString();
        if (!strType.isEmpty()) {
            if (strType == "CustomGroup")
                customGroup = true;
            else if (strType == "CopyGroup")
                copyGroup = true;
        }

        bool next = true;

        if (nameId.isEmpty())
            groups.append(0);
        else if (!(customGroup || copyGroup)) {
            RibbonGroup *group = m_allNameIdToGroups.value(nameId);

            QList<RibbonGroup *> listGroup = m_allNameIdToGroups.values();
            for (QList<RibbonGroup *>::iterator it = listGroup.begin(); it != listGroup.end() && group == Q_NULL;
                 ++it) {
                if ((*it)->title() == nameId || (*it)->title() == title)
                    group = (*it);
            }

            if (group) {
                groups.append(group);
                group->setTitle(title);
            }
        } else if (customGroup || copyGroup) {
            RibbonGroup *newGroup = new RibbonGroup(NULL);
            //            newGroup->setObjectName(nameId);
            newGroup->setTitle(title);
            groups.append(newGroup);

            if (customGroup)
                newGroup->setProperty(__qtc_Widget_Custom, QLatin1String("__qtc_Group_Custom"));
            else
                newGroup->setProperty(__qtc_Widget_Copy, QLatin1String("__qtc_Group_Copy"));

            next = false;

            QList<QAction *> actions;

            if (stream.readNext() == QXmlStreamReader::StartElement) {
                if (stream.name() == QTC_DIC_TAGNAME_RIBBON_ACTION) {
                    while (stream.tokenType() == QXmlStreamReader::StartElement) {
                        QXmlStreamAttributes attrsAction = stream.attributes();
                        QString idName = attrsAction.value(QTC_DIC_ATTRNAME_RIBBON_ID).toString();
                        if (!idName.isEmpty()) {
                            QAction *action = m_allNameToActions.value(idName);
                            newGroup->addAction(action);
                            actions.append(action);
                        }

                        if (stream.readNext() != QXmlStreamReader::EndElement)
                            return false;

                        stream.readNext();
                    }
                }
            }
            groupActions.insert(newGroup, actions);
        }

        if (next && stream.readNext() != QXmlStreamReader::EndElement)
            return false;

        stream.readNext();
    }
    if (m_ribbonBar) {
        m_regularPageGroups.insert(page, groups);
        m_regularGroupsActions = groupActions;
        setRibbonGroups();
        m_regularPageGroups.clear();
        m_regularGroupsActions.clear();
    }
    return true;
}

void RibbonBarCustomizeEngine::saveStatePage(QXmlStreamWriter &stream) const
{
    QListIterator<RibbonPage *> itPage(m_ribbonBar->pages());
    while (itPage.hasNext()) {
        if (RibbonPage *page = itPage.next()) {
            stream.writeStartElement(QTC_DIC_TAGNAME_RIBBON_PAGE);

            QString nameId = m_allPagesToNameId.value(page);

            if (nameId.isEmpty()) {
                if (page->property(__qtc_Widget_Custom).toString() != QLatin1String("__qtc_Page_Custom")) {
                    qWarning("RibbonBarCustomizeEngine::saveState(): 'pageId' not set for RibbonPage "
                             "%p '%s', using 'text' instead",
                             page, page->title().toLocal8Bit().constData());
                }
                nameId = page->title();
            }

            stream.writeAttribute(QTC_DIC_ATTRNAME_RIBBON_ID, nameId);
            stream.writeAttribute(QTC_DIC_ATTRNAME_RIBBON_TITLE, page->title());
            stream.writeAttribute(QTC_DIC_ATTRNAME_RIBBON_VISIBLE, QVariant(page->isVisible()).toString());

            if (page->property(__qtc_Widget_Custom).toString() == QLatin1String("__qtc_Page_Custom"))
                stream.writeAttribute(QTC_DIC_ATTRNAME_RIBBON_TYPE, "CustomPage");

            saveStateGroup(stream, page);

            stream.writeEndElement();
        }
    }
}

bool RibbonBarCustomizeEngine::restoreStatePage(QXmlStreamReader &stream)
{
    bool ok = true;

    if (stream.readNext() != QXmlStreamReader::StartElement)
        return false;
    if (stream.name() != QTC_DIC_TAGNAME_RIBBON_PAGE)
        return false;

    m_regularHiddenPages.clear();

    //    QList<RibbonPage*> pages;
    while (stream.tokenType() == QXmlStreamReader::StartElement) {
        QXmlStreamAttributes attrs = stream.attributes();
        if (!attrs.hasAttribute(QTC_DIC_ATTRNAME_RIBBON_ID))
            return false;

        QString nameId = attrs.value(QTC_DIC_ATTRNAME_RIBBON_ID).toString();

        if (!attrs.hasAttribute(QTC_DIC_ATTRNAME_RIBBON_TITLE))
            return false;

        QString title = attrs.value(QTC_DIC_ATTRNAME_RIBBON_TITLE).toString();

        if (!attrs.hasAttribute(QTC_DIC_ATTRNAME_RIBBON_VISIBLE))
            return false;

        bool visible = QVariant(attrs.value(QTC_DIC_ATTRNAME_RIBBON_VISIBLE).toString()).toBool();

        QString strType = attrs.value(QTC_DIC_ATTRNAME_RIBBON_TYPE).toString();
        bool customPage = !strType.isEmpty() && strType == "CustomPage";

        if (nameId.isEmpty())
            m_regularPages.append(0);
        else if (!customPage) {
            RibbonPage *page = m_allNameIdToPages.value(nameId);

            QList<RibbonPage *> listPage = m_allNameIdToPages.values();
            for (QList<RibbonPage *>::iterator it = listPage.begin(); it != listPage.end() && page == Q_NULL; ++it) {
                if ((*it)->title() == nameId || (*it)->title() == title)
                    page = (*it);
            }

            if (page) {
                m_regularPages.append(page);

                if (!visible)
                    m_regularHiddenPages.append(page);
                page->setTitle(title);
                restoreStateGroup(stream, page);
            } else
                stream.skipCurrentElement();
        } else if (customPage) {
            RibbonPage *newPage = new RibbonPage();
            newPage->setTitle(title);
            newPage->setObjectName(nameId);
            m_regularPages.append(newPage);
            newPage->setProperty(__qtc_Widget_Custom, QLatin1String("__qtc_Page_Custom"));

            if (!visible)
                m_regularHiddenPages.append(newPage);
            restoreStateGroup(stream, newPage);
        }
        stream.readNext();
    }

    if (stream.tokenType() != QXmlStreamReader::EndElement)
        return false;

    ((RibbonBarCustomizeEngine *)this)->setRibbonBar();
    updateRibbonBar();
    m_regularPages.clear();
    m_regularHiddenPages.clear();
    return ok;
}

void RibbonBarCustomizeEngine::saveStateRibbonBar(QXmlStreamWriter &stream) const
{
    stream.writeStartElement(QTC_DIC_TAGNAME_RIBBON_BAR);
    saveStatePage(stream);
}

bool RibbonBarCustomizeEngine::restoreStateRibbonBar(QXmlStreamReader &stream)
{
    if (stream.readNext() != QXmlStreamReader::StartElement)
        return false;

    if (stream.name() != QTC_DIC_TAGNAME_RIBBON_BAR)
        return false;

    bool ok = restoreStatePage(stream);
    Q_ASSERT(ok);

    if (stream.readNext() != QXmlStreamReader::EndElement)
        return false;

    return ok;
}

void RibbonBarCustomizeEngine::saveState(QXmlStreamWriter &stream) const
{
    ToolBarCustomizeEngine::saveState(stream);
    saveStateRibbonBar(stream);
}

bool RibbonBarCustomizeEngine::restoreState(QXmlStreamReader &stream)
{
    bool ok = ToolBarCustomizeEngine::restoreState(stream);
    Q_ASSERT(ok);
    ok = restoreStateRibbonBar(stream);
    Q_ASSERT(ok);
    return ok;
}

/* RibbonCustomizeManagerPrivate */
RibbonCustomizeManagerPrivate::RibbonCustomizeManagerPrivate()
{
    m_editMode = false;
    m_ribbonManager = Q_NULL;
    m_ribbonBar = Q_NULL;
}

RibbonCustomizeManagerPrivate::~RibbonCustomizeManagerPrivate() {}

void RibbonCustomizeManagerPrivate::init(RibbonBar *ribbonBar)
{
    QTC_Q(RibbonCustomizeManager)
    m_ribbonBar = ribbonBar;
    m_ribbonManager = new RibbonBarCustomizeEngine(q, ribbonBar);
}

/*!
\class RibbonCustomizeManager
\inmodule ribbon
RibbonCustomizeManager is a base class to customize actions on the RibbonBar
*/
RibbonCustomizeManager::RibbonCustomizeManager(RibbonBar *ribbonBar) : QObject(ribbonBar)
{
    QTC_INIT_PRIVATE(RibbonCustomizeManager);
    QTC_D(RibbonCustomizeManager);
    d->init(ribbonBar);
}

/*!
Destructor.
*/
RibbonCustomizeManager::~RibbonCustomizeManager() { QTC_FINI_PRIVATE(); }

/*!
Returns list of all categories.
*/
QStringList RibbonCustomizeManager::categories() const
{
    QTC_D(const RibbonCustomizeManager)
    return d->m_ribbonManager->categories();
}

/*!
Returns a list of all actions which are subject to customize.
*/
QList<QAction *> RibbonCustomizeManager::actionsAll() const
{
    QTC_D(const RibbonCustomizeManager)
    return d->m_ribbonManager->actionsAll();
}

/*!
Adds QToolBar \a toolBar to customize manager.
*/
void RibbonCustomizeManager::addToolBar(QToolBar *toolBar)
{
    QTC_D(RibbonCustomizeManager)
    d->m_ribbonManager->addDefaultToolBar(toolBar);
}

/*!
Adds new \a category to customize manager.
*/
void RibbonCustomizeManager::addAllActionsCategory(const QString &category)
{
    QTC_D(RibbonCustomizeManager)
    d->m_ribbonManager->addAllActionsCategory(category);
}

/*!
Returns all actions from the specific \a category.
*/
QList<QAction *> RibbonCustomizeManager::actionsByCategory(const QString &category) const
{
    QTC_D(const RibbonCustomizeManager)
    return d->m_ribbonManager->actionsByCategory(category);
}

/*!
Adds action to the specific \a category.
*/
void RibbonCustomizeManager::addToCategory(const QString &category, QAction *action)
{
    QTC_D(RibbonCustomizeManager)
    d->m_ribbonManager->addToCategory(category, action);
}

/*!
Returns all toolbars from the specific \a category.
*/
QList<QToolBar *> RibbonCustomizeManager::toolBarsByCategory(const QString &category) const
{
    QTC_D(const RibbonCustomizeManager)
    return d->m_ribbonManager->toolBarsByCategory(category);
}

/*!
Adds \a toolBar to the specific \a category.
*/
void RibbonCustomizeManager::addToCategory(const QString &category, QToolBar *toolBar)
{
    QTC_D(RibbonCustomizeManager)
    d->m_ribbonManager->ToolBarCustomizeEngine::addToCategory(category, toolBar);
}

/*!
Returns all ribbon pages from the specific \a category.
*/
QList<RibbonPage *> RibbonCustomizeManager::pagesByCategory(const QString &category) const
{
    QTC_D(const RibbonCustomizeManager)
    return d->m_ribbonManager->pagesByCategory(category);
}

/*!
Adds ribbon \a page to the specific \a category.
*/
void RibbonCustomizeManager::addToCategory(const QString &category, RibbonPage *page)
{
    QTC_D(RibbonCustomizeManager)
    return d->m_ribbonManager->addToCategory(category, page);
}

/*!
Returns all ribbon groups from the specific \a category.
*/
QList<RibbonGroup *> RibbonCustomizeManager::groupsByCategory(const QString &category) const
{
    QTC_D(const RibbonCustomizeManager)
    return d->m_ribbonManager->groupsByCategory(category);
}

/*!
Adds ribbon \a group to the specific \a category.
*/
void RibbonCustomizeManager::addToCategory(const QString &category, RibbonGroup *group)
{
    QTC_D(RibbonCustomizeManager)
    d->m_ribbonManager->addToCategory(category, group);
}

/*!
Returns unique identifier for \a action.
*/
QString RibbonCustomizeManager::actionId(QAction *action) const
{
    QTC_D(const RibbonCustomizeManager);
    return d->m_ribbonManager->actionId(action);
}

/*!
Assigns the unique identifier \a id for \a action.
*/
void RibbonCustomizeManager::setActionId(QAction *action, const QString &id)
{
    QTC_D(RibbonCustomizeManager);
    d->m_ribbonManager->setActionId(action, id);
}

/*!
Returns unique identifier for \a page.
*/
QString RibbonCustomizeManager::pageId(RibbonPage *page) const
{
    QTC_D(const RibbonCustomizeManager)
    return d->m_ribbonManager->pageId(page);
}

/*!
Assign the unique identifier \a id for \a page.
*/
void RibbonCustomizeManager::setPageId(RibbonPage *page, const QString &id)
{
    QTC_D(RibbonCustomizeManager)
    d->m_ribbonManager->setPageId(page, id);
}

/*!
Returns unique identifier for \a group.
*/
QString RibbonCustomizeManager::groupId(RibbonGroup *group) const
{
    QTC_D(const RibbonCustomizeManager)
    return d->m_ribbonManager->groupId(group);
}

/*!
Assign the unique identifier \a id for \a group.
*/
void RibbonCustomizeManager::setGroupId(RibbonGroup *group, const QString &id)
{
    QTC_D(RibbonCustomizeManager)
    d->m_ribbonManager->setGroupId(group, id);
}

/*!
Sets customize manager to the edit mode.
*/
void RibbonCustomizeManager::setEditMode(bool set)
{
    QTC_D(RibbonCustomizeManager)
    if (d->m_editMode == set)
        return;

    d->m_editMode = set;
    if (d->m_editMode) {
        d->m_ribbonManager->m_regularToolBars.clear();
        d->m_ribbonManager->m_regularHiddenPages.clear();
        d->m_ribbonManager->m_regularPagesName.clear();
        d->m_ribbonManager->m_regularPages.clear();
        d->m_ribbonManager->m_regularPageGroups.clear();
        d->m_ribbonManager->m_regularGroupsActions.clear();

        if (RibbonQuickAccessBar *toolBar = d->m_ribbonManager->m_ribbonBar->quickAccessBar()) {
            QList<QAction *> listAction = toolBar->actions();
            for (QList<QAction *>::iterator it = listAction.begin(); it != listAction.end(); ++it) {
                if (d->m_ribbonManager->m_allActionsToName.contains(*it))
                    d->m_ribbonManager->m_regularToolBars[toolBar].append(*it);
                else if ((*it)->isSeparator())
                    d->m_ribbonManager->m_regularToolBars[toolBar].append(0);
            }
        }

        d->m_ribbonManager->m_regularPages = d->m_ribbonManager->m_ribbonBar->pages();
        for (QList<RibbonPage *>::iterator it = d->m_ribbonManager->m_regularPages.begin();
             it != d->m_ribbonManager->m_regularPages.end(); ++it) {
            RibbonPage *page = *it;
            setPageName(page, page->title());
            setPageHidden(page, !page->isVisible());

            QList<RibbonGroup *> listGrops = page->groups();
            d->m_ribbonManager->m_regularPageGroups.insert(page, listGrops);

            for (QList<RibbonGroup *>::iterator iGroup = listGrops.begin(); iGroup != listGrops.end(); ++iGroup) {
                RibbonGroup *group = *iGroup;
                QList<QAction *> actions = group->actions();
                if (!actions.isEmpty())
                    d->m_ribbonManager->m_regularGroupsActions.insert(group, actions);

                for (int index = 0, count = group->controlCount(); count > index; ++index) {
                    RibbonControl *control = group->controlByIndex(index);
                    Q_ASSERT(control != Q_NULL);
                    if (RibbonToolBarControl *toolBarControl = qobject_cast<RibbonToolBarControl *>(control)) {
                        QList<QAction *> actions = toolBarControl->actions();
                        d->m_ribbonManager->m_regularGroupsActions.insert(group, actions);
                    }
                }
            }
        }
    } else
        cancel();
}

/*!
Returns true if customize manager in edit mode. Otherwise, returns false.
*/
bool RibbonCustomizeManager::isEditMode() const
{
    QTC_D(const RibbonCustomizeManager)
    return d->m_editMode;
}

/*!
Reset all changes to the default for additional \a toolBar. If \a toolBar is NULL then reset changes for RibbonBar.
*/
void RibbonCustomizeManager::reset(QToolBar *toolBar)
{
    QTC_D(RibbonCustomizeManager)
    if (toolBar == Q_NULL) {
        d->m_ribbonManager->m_regularHiddenPages.clear();
        d->m_ribbonManager->m_regularHiddenPages = d->m_ribbonManager->defaultHiddenPages();
        d->m_ribbonManager->m_regularPagesName.clear();
        d->m_ribbonManager->m_regularPagesName = d->m_ribbonManager->defaultPagesName();
        d->m_ribbonManager->m_regularNameGroups.clear();
        d->m_ribbonManager->m_regularNameGroups = d->m_ribbonManager->defaultGroupsName();

        d->m_ribbonManager->m_regularPages.clear();
        d->m_ribbonManager->m_regularPages = d->m_ribbonManager->defaultRibbonBar();
        d->m_ribbonManager->m_regularPageGroups.clear();
        d->m_ribbonManager->m_regularPageGroups = d->m_ribbonManager->defaultPageGroups();
        d->m_ribbonManager->m_regularGroupsActions.clear();

        for (QMap<RibbonPage *, QList<RibbonGroup *> >::iterator it = d->m_ribbonManager->m_regularPageGroups.begin();
             it != d->m_ribbonManager->m_regularPageGroups.end(); ++it) {
            QList<RibbonGroup *> listGrops = it.value();
            for (QList<RibbonGroup *>::iterator iGroup = listGrops.begin(); iGroup != listGrops.end(); ++iGroup) {
                RibbonGroup *group = *iGroup;
                QList<QAction *> actions = group->actions();
                if (!actions.isEmpty())
                    d->m_ribbonManager->m_regularGroupsActions.insert(group, actions);

                for (int index = 0, count = group->controlCount(); count > index; ++index) {
                    RibbonControl *control = group->controlByIndex(index);
                    Q_ASSERT(control != Q_NULL);
                    if (RibbonToolBarControl *toolBarControl = qobject_cast<RibbonToolBarControl *>(control)) {
                        QList<QAction *> actions = toolBarControl->actions();
                        d->m_ribbonManager->m_regularGroupsActions.insert(group, actions);
                    }
                }
            }
        }
    } else {
        d->m_ribbonManager->m_regularToolBars.clear();
        QList<QAction *> listAction = d->m_ribbonManager->defaultToolBars().value(toolBar);
        for (QList<QAction *>::iterator it = listAction.begin(); it != listAction.end(); ++it) {
            if (d->m_ribbonManager->m_allActionsToName.contains(*it))
                d->m_ribbonManager->m_regularToolBars[toolBar].append(*it);
            else if ((*it)->isSeparator())
                d->m_ribbonManager->m_regularToolBars[toolBar].append(0);
        }
    }
}

/*!
Cancel all changes that were made.
*/
void RibbonCustomizeManager::cancel()
{
    QTC_D(RibbonCustomizeManager)
    d->m_ribbonManager->m_regularToolBars.clear();
    d->m_ribbonManager->m_regularHiddenPages.clear();
    d->m_ribbonManager->m_regularPagesName.clear();
    d->m_ribbonManager->m_regularNameGroups.clear();
    d->m_ribbonManager->deleteFreePages();
    d->m_ribbonManager->m_regularPages.clear();
    d->m_ribbonManager->deleteFreeGroups();
    d->m_ribbonManager->m_regularPageGroups.clear();
    d->m_ribbonManager->m_regularGroupsActions.clear();
}

/*!
Apply all changes to the toolBars.
*/
void RibbonCustomizeManager::commit()
{
    QTC_D(RibbonCustomizeManager)
    if (isEditMode()) {
        d->m_ribbonManager->setQuickAccessBar();
        d->m_ribbonManager->setRibbonBar();
        d->m_ribbonManager->setRibbonGroups();
        d->m_ribbonManager->updateRibbonBar();
    }
}

/*!
Returns all actions from \a toolBar.
*/
QList<QAction *> RibbonCustomizeManager::actions(QToolBar *toolBar) const
{
    QTC_D(const RibbonCustomizeManager)
    if (qobject_cast<RibbonQuickAccessBar *>(toolBar))
        return d->m_ribbonManager->m_regularToolBars.value(toolBar);
    else
        return d->m_ribbonManager->actions(toolBar);
}

/*!
Insert \a action to \a toolBar in the position \a index.
*/
void RibbonCustomizeManager::insertAction(QToolBar *toolBar, QAction *action, int index)
{
    QTC_D(RibbonCustomizeManager)
    if (!isEditMode())
        setEditMode(true);

    d->m_ribbonManager->m_regularToolBars[toolBar].insert(index, action);
}

/*!
Remove an action from \a toolBar in the position \a index.
*/
void RibbonCustomizeManager::removeActionAt(QToolBar *toolBar, int index)
{
    QTC_D(RibbonCustomizeManager)
    if (!isEditMode())
        setEditMode(true);

    d->m_ribbonManager->m_regularToolBars[toolBar].removeAt(index);
}

/*!
Returns true if \a toolBar contains the \a action.
*/
bool RibbonCustomizeManager::containsAction(QToolBar *toolBar, QAction *action) const
{
    QTC_D(const RibbonCustomizeManager)
    return d->m_ribbonManager->m_regularToolBars[toolBar].contains(action);
}

/*!
Returns all pages that were added to customize manager.
*/
QList<RibbonPage *> RibbonCustomizeManager::pages() const
{
    QTC_D(const RibbonCustomizeManager)
    return d->m_ribbonManager->m_regularPages;
}

/*!
Creates page with name \a pageName and moves it to the position \a index. If \a index is -1 then page will be added to
the last position.
*/
RibbonPage *RibbonCustomizeManager::createPage(const QString &pageName, int index)
{
    QTC_D(RibbonCustomizeManager)

    if (!isEditMode())
        setEditMode(true);

    RibbonPage *newPage = new RibbonPage();
    newPage->setTitle(pageName);
    newPage->setProperty(__qtc_Widget_Custom, QLatin1String("__qtc_Page_Custom"));

    if (index == -1)
        d->m_ribbonManager->m_regularPages.append(newPage);
    else
        d->m_ribbonManager->m_regularPages.insert(index, newPage);
    return newPage;
}

/*!
Deletes the \a page.
*/
void RibbonCustomizeManager::deletePage(RibbonPage *page)
{
    QTC_D(RibbonCustomizeManager)
    if (!isEditMode())
        setEditMode(true);

    d->m_ribbonManager->deleteFreeGroups();
    d->m_ribbonManager->m_regularPages.removeOne(page);
    d->m_ribbonManager->m_regularPageGroups.remove(page);

    if (page->parentWidget() == Q_NULL)
        delete page;
}

/*!
Returns an index of the \a page.
*/
int RibbonCustomizeManager::pageIndex(RibbonPage *page) const
{
    QTC_D(const RibbonCustomizeManager)
    return d->m_ribbonManager->m_regularPages.indexOf(page);
}

/*!
Moves page from position \a oldIndex to \a newIndex.
*/
void RibbonCustomizeManager::movePage(int oldIndex, int newIndex)
{
    QTC_D(RibbonCustomizeManager)

    RibbonPage *page = d->m_ribbonManager->m_regularPages.at(oldIndex);
    d->m_ribbonManager->m_regularPages.removeAt(oldIndex);
    d->m_ribbonManager->m_regularPages.insert(newIndex, page);
}

/*!
Returns all groups in \a page.
*/
QList<RibbonGroup *> RibbonCustomizeManager::pageGroups(RibbonPage *page) const
{
    QTC_D(const RibbonCustomizeManager)
    return d->m_ribbonManager->m_regularPageGroups.value(page);
}

/*!
Creates a new group in \a page with name \a groupName in the position \a index. If \a index is -1 then group will be
added to the last position.
*/
RibbonGroup *RibbonCustomizeManager::createGroup(RibbonPage *page, const QString &groupName, int index)
{
    QTC_D(RibbonCustomizeManager)
    if (!isEditMode())
        setEditMode(true);

    RibbonGroup *newGroup = new RibbonGroup();
    newGroup->setTitle(groupName);

    if (index == -1)
        d->m_ribbonManager->m_regularPageGroups[page].append(newGroup);
    else
        d->m_ribbonManager->m_regularPageGroups[page].insert(index, newGroup);
    return newGroup;
}

/*!
Deletes group by \a index from \a page.
*/
void RibbonCustomizeManager::deleteGroup(RibbonPage *page, int index)
{
    QTC_D(RibbonCustomizeManager)
    if (!isEditMode())
        setEditMode(true);

    RibbonGroup *group = d->m_ribbonManager->m_regularPageGroups[page].takeAt(index);
    Q_ASSERT(group != Q_NULL);
    if (group->parent() == Q_NULL)
        delete group;
}

/*!
Moves group from position \a oldIndex to \a newIndex in \a page.
*/
void RibbonCustomizeManager::moveGroup(RibbonPage *page, int oldIndex, int newIndex)
{
    QTC_D(RibbonCustomizeManager)
    if (!isEditMode())
        setEditMode(true);

    RibbonGroup *group = d->m_ribbonManager->m_regularPageGroups[page].at(oldIndex);
    d->m_ribbonManager->m_regularPageGroups[page].removeAt(oldIndex);
    d->m_ribbonManager->m_regularPageGroups[page].insert(newIndex, group);
}

/*!
Inserts \a action to the \a group in the position \a index.
*/
void RibbonCustomizeManager::insertAction(RibbonGroup *group, QAction *action, int index)
{
    QTC_D(RibbonCustomizeManager)
    if (!isEditMode())
        setEditMode(true);
    d->m_ribbonManager->m_regularGroupsActions[group].insert(index, action);
}

/*!
Inserts a list of \a actions to the \a group in last postion.
*/
void RibbonCustomizeManager::appendActions(RibbonGroup *group, const QList<QAction *> &actions)
{
    QTC_D(RibbonCustomizeManager)
    if (!isEditMode())
        setEditMode(true);
    d->m_ribbonManager->m_regularGroupsActions[group] = actions;
}

/*!
Clear all actions in \a group.
*/
void RibbonCustomizeManager::clearActions(RibbonGroup *group)
{
    QTC_D(RibbonCustomizeManager)
    if (!isEditMode())
        setEditMode(true);
    d->m_ribbonManager->m_regularGroupsActions.remove(group);
}

/*!
Removes an action from \a group in the position \a index.
*/
void RibbonCustomizeManager::removeActionAt(RibbonGroup *group, int index)
{
    QTC_D(RibbonCustomizeManager)
    if (!isEditMode())
        setEditMode(true);
    d->m_ribbonManager->m_regularGroupsActions[group].removeAt(index);
}

/*!
Returns true if \a group contains the \a action.
*/
bool RibbonCustomizeManager::containsAction(RibbonGroup *group, QAction *action) const
{
    QTC_D(const RibbonCustomizeManager)
    return d->m_ribbonManager->m_regularGroupsActions.value(group).contains(action);
}

/*!
Returns all actions from \a group.
*/
QList<QAction *> RibbonCustomizeManager::actionsGroup(RibbonGroup *group) const
{
    QTC_D(const RibbonCustomizeManager)
    return d->m_ribbonManager->m_regularGroupsActions.value(group);
}

/*!
Returns true if page is hidden.
*/
bool RibbonCustomizeManager::isPageHidden(RibbonPage *page) const
{
    QTC_D(const RibbonCustomizeManager)
    return d->m_ribbonManager->m_regularHiddenPages.contains(page);
}

/*!
Assigns \a page hiddenness to \a hide.
*/
void RibbonCustomizeManager::setPageHidden(RibbonPage *page, bool hide)
{
    if (!isEditMode())
        setEditMode(true);

    QTC_D(RibbonCustomizeManager)
    if (hide)
        d->m_ribbonManager->m_regularHiddenPages.append(page);
    else
        d->m_ribbonManager->m_regularHiddenPages.removeOne(page);
}

/*!
Returns true if page is visible.
\sa isPageHidden()
*/
bool RibbonCustomizeManager::isPageVisible(RibbonPage *page) const { return !isPageHidden(page); }

/*!
Assigns \a page visibility to \a visible.
*/
void RibbonCustomizeManager::setPageVisible(RibbonPage *page, bool visible) { setPageHidden(page, !visible); }

/*!
Returns the name of the \a page.
*/
QString RibbonCustomizeManager::pageName(RibbonPage *page) const
{
    QTC_D(const RibbonCustomizeManager)
    return d->m_ribbonManager->m_regularPagesName.value(page);
}

/*!
Assigns the name \a pageName for the \a page.
*/
void RibbonCustomizeManager::setPageName(RibbonPage *page, const QString &pageName)
{
    QTC_D(const RibbonCustomizeManager)
    d->m_ribbonManager->m_regularPagesName.insert(page, pageName);
}

/*!
Returns the name of the \a group.
*/
QString RibbonCustomizeManager::groupName(RibbonGroup *group) const
{
    QTC_D(const RibbonCustomizeManager)
    return d->m_ribbonManager->m_regularNameGroups.value(group);
}

/*!
Assigns the name \a groupName for the \a group.
*/
void RibbonCustomizeManager::setGroupName(RibbonGroup *group, const QString &groupName)
{
    QTC_D(const RibbonCustomizeManager)
    d->m_ribbonManager->m_regularNameGroups.insert(group, groupName);
}

/*!
 */
void RibbonCustomizeManager::addDefaultStateQAccessBar()
{
    QTC_D(RibbonCustomizeManager)
    if (RibbonQuickAccessBar *quickAccessBar = d->m_ribbonBar->quickAccessBar())
        d->m_ribbonManager->addDefaultToolBar(quickAccessBar);
}

/*!
 */
void RibbonCustomizeManager::addDefaultStateRibbonBar()
{
    QTC_D(RibbonCustomizeManager)
    d->m_ribbonManager->addDefaultPages(d->m_ribbonBar);
}

/*!
Save the toolbars state to the \a device.
*/
bool RibbonCustomizeManager::saveStateToDevice(QIODevice *device)
{
    QTC_D(const RibbonCustomizeManager)

    QXmlStreamWriter xmlwriter(device);

    xmlwriter.writeStartDocument("1.0");
    xmlwriter.writeNamespace("http://gitee.com/icanpool/qtcanpool", "QRibbon");
    xmlwriter.writeStartElement(QTC_DIC_TAGNAME_RIBBON);
    xmlwriter.writeAttribute("type", "customize");
    xmlwriter.writeAttribute("version", "1.0");

    d->m_ribbonManager->saveState(xmlwriter);

    xmlwriter.writeEndElement();
    xmlwriter.writeEndDocument();

    return true;
}

/*!
Restore the toolbars state from the \a device.
*/
bool RibbonCustomizeManager::loadStateFromDevice(QIODevice *device)
{
    QTC_D(const RibbonCustomizeManager)

    QXmlStreamReader xmlreader(device);
    if (xmlreader.readNext() != QXmlStreamReader::StartDocument)
        return false;
    if (xmlreader.readNext() != QXmlStreamReader::StartElement)
        return false;

    if (xmlreader.name() != QTC_DIC_TAGNAME_RIBBON)
        return false;

    d->m_ribbonManager->restoreState(xmlreader);

    if (xmlreader.readNext() != QXmlStreamReader::EndElement)
        return false;
    if (xmlreader.readNext() != QXmlStreamReader::EndDocument)
        return false;

    return true;
}
