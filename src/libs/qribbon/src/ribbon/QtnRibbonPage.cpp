/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2013 Developer Machines (http://www.devmachines.com)
** Copyright (c) 2019 MaMinJie <canpool@163.com>
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
#include <QApplication>
#include <QPainter>
#include <QBasicTimer>
#include <QStyleOption>
#include <qevent.h>

#include "QtnRibbonStyle.h"
#include "QtnRibbonPage.h"
#include "QtnRibbonGroup.h"
#include "QtnRibbonBar.h"
#include "QtnRibbonTabBar.h"
#include "QtnRibbonPrivate.h"

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif // Q_OS_WIN

QTITAN_USE_NAMESPACE


QTITAN_BEGIN_NAMESPACE
/* RibbonPagePrivate */
class RibbonPagePrivate : public QObject
{
public:
    QTN_DECLARE_PUBLIC(RibbonPage)

    struct InfoGroup
    {
        QSize maxSize;
        QSize minSize;
        RibbonGroupWrapper* group;
        bool bReduced;
        bool visible;
        InfoGroup()
        {
            bReduced = false;
            visible = true;
            group = Q_NULL;
        }
    };
public:
    explicit RibbonPagePrivate();

public:
    int calcReducedSumWidth(QList<InfoGroup>& infoGroups) const;
    void calcReducedGroups(int widthPage, QList<InfoGroup>& infoGroups);
    int calcSumWidthGroups(int* widths);
    void reduceLayoutGroups(int* pWidth, int width);
    void layoutGroups();
    bool validIndex(int index) const { return index >= 0 && index < m_listGroups.count(); }

    void removeGroup(int index);
    int getIndexGroup(RibbonGroup* page) const;

    virtual bool eventFilter(QObject*, QEvent* event);

public:
    QList<RibbonGroupWrapper*> m_listGroups;
    QList<QAction*> m_listShortcuts;
    RibbonTab* m_associativeTab;
    QBasicTimer m_timer;
    QString m_title;
    QString m_contextTitle;
    QSize m_sizeGroups;
    int m_groupsHeight;
    bool m_minimazeRibbon : 1;
    bool m_doVisiblePage  : 1;
    bool m_allowPress     : 1;
    bool m_doPopupPage    : 1;
};
QTITAN_END_NAMESPACE

RibbonPagePrivate::RibbonPagePrivate()
{
    m_minimazeRibbon = false;
    m_doVisiblePage = false;  
    m_allowPress = false;
    m_doPopupPage = false;
    m_associativeTab = Q_NULL;
    m_groupsHeight = -1;
}

int RibbonPagePrivate::calcReducedSumWidth(QList<InfoGroup>& infoGroups) const
{
    int sumWidth = 0;
    for (int i = 0, count = infoGroups.size(); count > i; i++) 
    {
        InfoGroup& itemGroup = infoGroups[i];
        if (itemGroup.visible)
        {
            sumWidth += itemGroup.bReduced ? itemGroup.minSize.width() : itemGroup.maxSize.width();
            sumWidth += 7;
        }
    }
    return sumWidth;
}

void RibbonPagePrivate::calcReducedGroups(int widthPage, QList<InfoGroup>& infoGroups)
{
    int sumWidth = calcReducedSumWidth(infoGroups);
    if (sumWidth >= widthPage)
    {
        for (int i = infoGroups.size()-1; i >= 0; i--) 
        {
            InfoGroup& itemGroup = infoGroups[i];
            if (!itemGroup.bReduced)
            {
                itemGroup.bReduced = true;
                sumWidth = calcReducedSumWidth(infoGroups);
                if (!(sumWidth < widthPage))
                    calcReducedGroups(widthPage, infoGroups);
                break;
            }
        }
    }
    else if (sumWidth <= widthPage)
    {
        for (int i = 0, count = infoGroups.size(); count > i; i++) 
        {
            InfoGroup& itemGroup = infoGroups[i];
            if (itemGroup.bReduced)
            {
                itemGroup.bReduced = false;
                sumWidth = calcReducedSumWidth(infoGroups);
                if (sumWidth > widthPage)
                    itemGroup.bReduced = true;
                else
                    calcReducedGroups(widthPage, infoGroups);
                break;
            }
        }
    }
}

int RibbonPagePrivate::calcSumWidthGroups(int* widths)
{
    int width = 0;
    for (int i = 0; i < m_listGroups.size(); i++)
    {
        width += widths[i];
        if (i != m_listGroups.size() - 1 && widths[i] != 0)
            width += 7;
    }
    return width;
}


void RibbonPagePrivate::reduceLayoutGroups(int* pWidth, int width)
{
    int resultWidth = calcSumWidthGroups(pWidth);

    if (resultWidth <= width)
    {
        if (resultWidth == width)
            return;

        for (int i = m_listGroups.size() - 1; i >= 0; i--)
        {
            RibbonGroupWrapper* group = m_listGroups.at(i);
            if (!group->isVisible())
                continue;

            if (group->extendSize(width - resultWidth))
            {
                group->updateLayout(0, false);
                pWidth[i] = group->sizeHint().width();
                break;
            }
        }
        return;
    }

    bool reduced = false;
    do
    {
        reduced = false;
        for (int level = 0; level < 4; level++)
        {
            for (int i = m_listGroups.size() - 1; i >= 0; i--)
            {
                RibbonGroupWrapper* group = m_listGroups.at(i);
                if (!group->isVisible())
                    continue;

                if (group->reduceSize(level, resultWidth - width))
                {
                    group->updateLayout(0, false);
                    int groupWidth = group->sizeHint().width();
                    resultWidth -= (pWidth[i] - groupWidth);
                    pWidth[i] = groupWidth;
                    reduced = true;
                    break;
                }
            }

            if (reduced)
                break;
        }

        if (reduced && resultWidth <= width)
            return;
    }
    while (reduced);
}

void RibbonPagePrivate::layoutGroups()
{
    QTN_P(RibbonPage);

    p.setUpdatesEnabled(false);

    int* pData = new int[m_listGroups.size()];
    for (int i = 0, count = m_listGroups.size(); count > i; i++)
    {
        RibbonGroupWrapper* group = m_listGroups.at(i);
        group->updateLayout(Q_NULL, true);
        if (group->isVisible())
        {
            group->startLayout();
            pData[i] = group->realSize().width();
        }
        else
            pData[i] = 0;
    }

    int groupsWidth = p.size().width() - 4 - 4;
    reduceLayoutGroups(pData, groupsWidth);

    QList<InfoGroup> infoGroups;
    for (int i = 0, count = m_listGroups.size(); count > i; i++)
    {
        RibbonGroupWrapper* group = m_listGroups.at(i);
        InfoGroup itemInfo;
        itemInfo.bReduced = group->isReducedGroup();
        itemInfo.visible = group->isVisible();
        itemInfo.maxSize = itemInfo.visible ? group->realSize() : QSize(0,0);
        itemInfo.minSize = itemInfo.visible ? group->reducedSize() : QSize(0,0);
        itemInfo.group = group;
        infoGroups << itemInfo;
    }
    QRect rcPage = p.geometry();
    calcReducedGroups(rcPage.width(), infoGroups);
    
    int x = 4;
    QSize size;
    for (int i = 0, count = infoGroups.size(); count > i; i++)
    {
        InfoGroup& itemGroup = infoGroups[i];
        RibbonGroupWrapper* group = itemGroup.group;

        if (itemGroup.bReduced)
        {
            if (!group->isReducedGroup())
                group->setReducedGroup(true);
        }
        else if (!itemGroup.bReduced)
        {
            if (group->isReducedGroup())
                group->resetReducedGroup();
        }

        QSize sz = itemGroup.visible ? group->sizeHint() : QSize(0, 0);
        if (m_groupsHeight != -1)
            sz.setHeight(m_groupsHeight);

        int width = sz.width();

        if (itemGroup.visible )
        {
            group->setGeometry(x, 3, width, sz.height());
            group->getGroup()->setFixedHeight(sz.height());
            x += width + 2;
        }
        QSize sizegroup = itemGroup.visible ? group->sizeHint() : QSize(0, 0);
        size = QSize(sizegroup.width() + size.width(), sizegroup.height());

        group->finalLayout();
    }
    m_sizeGroups = size;

    delete[] pData;

    p.setUpdatesEnabled(true);
}

void RibbonPagePrivate::removeGroup(int index)
{
    if (validIndex(index)) 
    {
        RibbonGroupWrapper* groupWrapper = m_listGroups[index];

        groupWrapper->getGroup()->removeEventFilter(this);
        QList<QAction*> actList = groupWrapper->getGroup()->actions();

        for (int i = actList.size()-1; i >= 0; --i) 
        {
            QAction* a = actList.at(i);
            m_listShortcuts.removeOne(a);
        }

        m_listGroups.removeAt(index);
        delete groupWrapper;
        layoutGroups();
    }
}

int RibbonPagePrivate::getIndexGroup(RibbonGroup* page) const
{
    for (int i = 0, count = m_listGroups.size(); count > i; i++) 
    {
        RibbonGroupWrapper* groupWrapper = m_listGroups.at(i);
        if (page == groupWrapper->getGroup())
            return i;
    }
    return -1;
}

bool RibbonPagePrivate::eventFilter(QObject* obj, QEvent* event)
{
    bool res = QObject::eventFilter(obj, event);

    if (!qobject_cast<RibbonGroup*>(obj))
        return res;

    switch (event->type()) 
    {
        case QEvent::ActionAdded: 
            if (QActionEvent* actEvent = static_cast<QActionEvent*>(event))
            {
                m_listShortcuts.append(actEvent->action());
                if (m_associativeTab)
                    m_associativeTab->addAction(actEvent->action());
            }
            break;
        case QEvent::ActionRemoved: 
            if (QActionEvent* actEvent = static_cast<QActionEvent*>(event))
            {
                if (m_associativeTab)
                    m_associativeTab->removeAction(actEvent->action());

                int index = m_listShortcuts.indexOf(actEvent->action());
                if (index != -1)
                    m_listShortcuts.removeAt(index);
            }
            break;
        default:
            break;
    }

    return res;
}


/* RibbonPage */
RibbonPage::RibbonPage(QWidget* parent)
    : QWidget(parent)
{
    QTN_INIT_PRIVATE(RibbonPage);
}

RibbonPage::RibbonPage(QWidget* parent, const QString& title)
    : QWidget(parent)
{
    QTN_INIT_PRIVATE(RibbonPage);
    setTitle(title);
}

RibbonPage::~RibbonPage()
{
    if (RibbonBar* ribbon = qobject_cast<RibbonBar*>(parentWidget()))
        ribbon->detachPage(this);
    QTN_FINI_PRIVATE();
}

bool RibbonPage::isVisible() const
{
    QTN_D(const RibbonPage);
    return QWidget::isVisible() || (d.m_associativeTab && d.m_associativeTab->isVisible());
}

RibbonGroup* RibbonPage::addGroup(const QString& title)
{
    return insertGroup(-1, title);
}

RibbonGroup* RibbonPage::addGroup(const QIcon& icon, const QString& title)
{
    return insertGroup(-1, icon, title);
}

RibbonGroup* RibbonPage::insertGroup(int index, const QString& title)
{
    QTN_D(RibbonPage);
    
    setUpdatesEnabled(false);

    RibbonGroupWrapper* wrapper = new RibbonGroupWrapper(this);
    RibbonGroup* group = new RibbonGroup(wrapper, title);
    wrapper->setGroup(group);
    
    if (!d.validIndex(index)) 
    {
        index = d.m_listGroups.count();
        d.m_listGroups.append(wrapper);
    } 
    else 
        d.m_listGroups.insert(index, wrapper);

    connect(group, SIGNAL(actionTriggered(QAction*)), this, SLOT(actionTriggered(QAction*)), Qt::QueuedConnection);
    connect(group, SIGNAL(released()), this, SLOT(released()));

    wrapper->show();

    group->installEventFilter(&d);

    return group;
}

RibbonGroup* RibbonPage::insertGroup(int index, const QIcon& icon, const QString& title)
{
    if (RibbonGroup* group = insertGroup(index, title))
    {
        group->setIcon(icon);
        return group;
    }
    return Q_NULL;
}

void RibbonPage::removeGroup(RibbonGroup* group)
{
    QTN_D(RibbonPage);
    removeGroup(d.getIndexGroup(group));
}

void RibbonPage::removeGroup(int index)
{
    QTN_D(RibbonPage);
    d.removeGroup(index);
}

QAction* RibbonPage::defaultAction() const
{
    QTN_D(const RibbonPage);
    if (d.m_associativeTab)
        return d.m_associativeTab->defaultAction();
    return Q_NULL;
}

int RibbonPage::groupCount() const
{
    QTN_D(const RibbonPage);
    return d.m_listGroups.count();
}

RibbonGroup* RibbonPage::getGroup(int index) const
{
    QTN_D(const RibbonPage);
    if (index < 0 || index >= d.m_listGroups.size())
        return Q_NULL;
    return d.m_listGroups[index]->getGroup();
}

void RibbonPage::clearGroups()
{
    QTN_D(RibbonPage);
    for(int i = (int)d.m_listGroups.count()-1; i >= 0; i--)
        removeGroup(i);
}

/*!
\brief Sets the predefined \a color for the contextual page.
 */ 
void RibbonPage::setContextPage(ContextColor color)
{
    QTN_D(RibbonPage);
    d.m_associativeTab->setContextTab(color);
    repaint();
    parentWidget()->repaint();
}

/*!
\brief Returns the predefined color for the contextual page.
 */ 
ContextColor RibbonPage::getContextColor() const
{
    QTN_D(const RibbonPage);
    return d.m_associativeTab ? d.m_associativeTab->getContextColor() : ContextColorNone;
}

void RibbonPage::setVisible(bool visible)
{
    QTN_D(RibbonPage);

    bool saveVisible = isVisible();

    if (d.m_minimazeRibbon && d.m_associativeTab)
    {
        if (d.m_doPopupPage)
            QWidget::setVisible(visible);

        if (!d.m_doPopupPage)
        {
            d.m_associativeTab->setVisible(visible);
            if (saveVisible != isVisible())
            {
                if (RibbonBar* ribbonBar = qobject_cast<RibbonBar*>(parentWidget()))
                {
                    QEvent ev(QEvent::LayoutRequest);
                    QApplication::sendEvent(ribbonBar, &ev);
                    ribbonBar->update();
                }
            }
        }

        if (!visible && d.m_doPopupPage)
            d.m_doPopupPage = false;
    }
    else
    {
        if (d.m_doVisiblePage)
            QWidget::setVisible(visible);
        else if (!visible)
            QWidget::setVisible(false);


        if (!d.m_doVisiblePage && d.m_associativeTab)
            d.m_associativeTab->setVisible(visible);

        if (!d.m_doVisiblePage && !visible)
        {
            if (RibbonBar* ribbon = qobject_cast<RibbonBar*>(parentWidget()))
            {
                if (ribbon->getPageCount() > 0)
                {
                    RibbonPage* page = ribbon->getPage(ribbon->currentIndexPage());
                    if (this == page)
                        ribbon->setCurrentPage(0);
                }
            }
        }

        if (saveVisible != isVisible())
        {
            if (RibbonBar* ribbonBar = qobject_cast<RibbonBar*>(parentWidget()))
            {
                QEvent ev(QEvent::LayoutRequest);
                QApplication::sendEvent(ribbonBar, &ev);
                ribbonBar->update();
            }
        }
    }
}

void RibbonPage::setTitle(const QString& title)
{
    QTN_D(RibbonPage);

    d.m_title = title;

    if (d.m_associativeTab)
        d.m_associativeTab->setTextTab(d.m_title);

    emit titleChanged(d.m_title);
}

const QString& RibbonPage::title() const
{
    QTN_D(const RibbonPage);
    return d.m_associativeTab ? d.m_associativeTab->textTab() : d.m_title;
}

void RibbonPage::setContextTitle(const QString& title)
{
    QTN_D(RibbonPage);
    d.m_contextTitle = title;

    if (d.m_associativeTab)
        d.m_associativeTab->setContextTextTab(d.m_contextTitle);
}

void RibbonPage::released()
{
    QTN_D(RibbonPage);
    if (d.m_minimazeRibbon && isVisible() && QApplication::activePopupWidget() == this)
        hide();
}

void RibbonPage::actionTriggered(QAction* action)
{
    QTN_D(RibbonPage);

    if (d.m_listShortcuts.indexOf(action) != -1)
        return;

    if (d.m_minimazeRibbon && isVisible())
    {
        if (action->menu())
            return;
        close();
    }
}

const QString& RibbonPage::contextTitle() const
{
    QTN_D(const RibbonPage);
    return d.m_associativeTab ? d.m_associativeTab->contextTextTab() : d.m_title;
}

void RibbonPage::setAssociativeTab(QWidget* widget)
{
    QTN_D(RibbonPage);
    d.m_associativeTab = qobject_cast<RibbonTab*>(widget);

    if (!d.m_associativeTab)
        return;

    for (int j = 0, count = groupCount(); count > j; j++)
    {
        if (RibbonGroup* group = getGroup(j))
        {
            QList<QAction*> actList = group->actions();

            for (int i = 0, actCount = actList.size(); actCount > i; i++) 
            {
                QAction* a = actList.at(i);
                d.m_listShortcuts.append(a);
                d.m_associativeTab->addAction(a);
            }
        }
    }
}

void RibbonPage::setPageVisible(bool visible)
{
    QTN_D(RibbonPage);
    d.m_doVisiblePage = true;
    if (visible && d.m_associativeTab && !d.m_associativeTab->isHidden())
    {
        setVisible(true);
        emit activated();
    }
    else if (d.m_associativeTab && !d.m_associativeTab->isHidden())
        setVisible(false);
    d.m_doVisiblePage = false;
}

void RibbonPage::popup()
{
    QTN_D(RibbonPage);
    d.m_doPopupPage = true;
//    ensurePolished();
//    d.layoutGroups();
    show();
}

void RibbonPage::setRibbonMinimized(bool minimized)
{
    QTN_D(RibbonPage);
    d.m_minimazeRibbon = minimized;
}

void RibbonPage::activatingPage(bool& allow)
{
    emit activating(allow);
}

void RibbonPage::setGroupsHeight(int height)
{
    QTN_D(RibbonPage);
    d.m_groupsHeight = height;
}

bool RibbonPage::event(QEvent* event)
{
    QTN_D(RibbonPage);
    switch(event->type())
    {
        case QEvent::LayoutRequest :
                d.layoutGroups();
            break;
        case QEvent::Show :
            if (d.m_minimazeRibbon && d.m_associativeTab)
            {
                d.m_associativeTab->setTrackingMode(true);
                d.m_timer.start(QApplication::doubleClickInterval(), this);
                d.m_allowPress = true;
            }
            break;
        case QEvent::Hide :
            if (d.m_minimazeRibbon && d.m_associativeTab)
            {
                d.m_associativeTab->setTrackingMode(false);
                d.m_allowPress = true;
            }
            break;
        case QEvent::Timer :
            if (d.m_minimazeRibbon && d.m_associativeTab)
            {
                QTimerEvent* pTimerEvent = (QTimerEvent*)event;
                if (d.m_timer.timerId() == pTimerEvent->timerId()) 
                {
                    d.m_allowPress = false;
                    return true;
                }
            }
            break;
        case QEvent::MouseButtonPress :
            if (d.m_minimazeRibbon && d.m_associativeTab)
            {
                QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
                if (!rect().contains(mouseEvent->pos())) 
                {
                    if (d.m_allowPress)
                    {
                        QWidget* clickedWidget = QApplication::widgetAt(mouseEvent->globalPos());

                        if (clickedWidget == d.m_associativeTab)
                        {
                            const QPoint targetPoint = clickedWidget->mapFromGlobal(mouseEvent->globalPos());

                            QMouseEvent evPress(mouseEvent->type(), targetPoint, mouseEvent->globalPos(), mouseEvent->button(), mouseEvent->buttons(), mouseEvent->modifiers());
                            QApplication::sendEvent(clickedWidget, &evPress);

                            QMouseEvent eDblClick(QEvent::MouseButtonDblClick, targetPoint, mouseEvent->globalPos(), mouseEvent->button(), mouseEvent->buttons(), mouseEvent->modifiers());
                            QApplication::sendEvent(d.m_associativeTab, &eDblClick);
                            return true;
                        }
                    }
                    else if (d.m_associativeTab && QRect(d.m_associativeTab->mapToGlobal(QPoint()), d.m_associativeTab->size()).contains(mouseEvent->globalPos()))
                        setAttribute(Qt::WA_NoMouseReplay);

                    if (QApplication::activePopupWidget() == this)
                    {
                        if (QWidget* clickedTab = qobject_cast<RibbonTab*>(QApplication::widgetAt(mouseEvent->globalPos())))
                        {
                            if (d.m_associativeTab && clickedTab != d.m_associativeTab)
                            {
                                const QPoint targetPoint = clickedTab->mapFromGlobal(mouseEvent->globalPos());

                                QMouseEvent evPress(mouseEvent->type(), targetPoint, mouseEvent->globalPos(), mouseEvent->button(), mouseEvent->buttons(), mouseEvent->modifiers());
                                QApplication::sendEvent(clickedTab, &evPress);

                                return false;
                            }
                        }
                    }
                }
            }
            break;
        default:
            break;
    }

    return QWidget::event(event);
}

QSize RibbonPage::sizeHint() const
{
    QTN_D(const RibbonPage);
    return d.m_sizeGroups;
}

void RibbonPage::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QTN_D(RibbonPage);

    QWidget* parent = this->parentWidget();
    if (parent && (d.m_minimazeRibbon || !qobject_cast<RibbonBar*>(parent)))
    {
        QPainter p(this);
        QStyleOption opt;
        opt.init(this);
        style()->drawControl((QStyle::ControlElement)RibbonStyle::CE_RibbonGroups, &opt, &p, this->parentWidget());
    }
}

void RibbonPage::changeEvent(QEvent* event)
{
    switch (event->type()) 
    {
        case QEvent::FontChange: 
            if (QWidget* parent = parentWidget())
            {
                QEvent ev(QEvent::LayoutRequest);
                QApplication::sendEvent(this, &ev);
                parent->updateGeometry();
            }
            break;
        default:
            break;
    };
    return QWidget::changeEvent(event);;
}

void RibbonPage::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    QTN_D(RibbonPage);
    d.layoutGroups();
}

void RibbonPage::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    QTN_D(RibbonPage);

    QSize delta = event->size() - event->oldSize();

    if (delta.isNull())
        return;

    d.layoutGroups();
}


#ifdef Q_OS_WIN
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
bool RibbonPage::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
    bool res = QWidget::nativeEvent(eventType, message, result);
    MSG* msg = static_cast<MSG *>(message);
#else
bool RibbonPage::winEvent(MSG* message, long* result)
{
    bool res = QWidget::winEvent(message, result);
    MSG* msg = static_cast<MSG *>(message);
#endif
    if (!res)
    {
        if (msg->message == WM_LBUTTONDOWN) 
        {
            if (QWidget* activePopupWidget = QApplication::activePopupWidget())
            {
                if (activePopupWidget == this)
                {
                    POINT curPos = msg->pt;
                    QPoint globalPos(curPos.x, curPos.y);

                    QPoint pos = mapFromGlobal(globalPos);

                    QToolButton* toolButton = qobject_cast<QToolButton*>(activePopupWidget->childAt(pos));

                    if (toolButton && toolButton->isEnabled())
                    {
                        if (QAction* action = toolButton->defaultAction())
                        {
                            if (action->menu())
                            {
                                int button = Qt::LeftButton;
                                int state = 0;

                                pos = toolButton->mapFromGlobal(globalPos);

                                QMouseEvent e(QEvent::MouseButtonPress, pos, globalPos,
                                    Qt::MouseButton(button),
                                    Qt::MouseButtons(state & Qt::MouseButtonMask),
                                    Qt::KeyboardModifiers(state & Qt::KeyboardModifierMask));

                                res = QApplication::sendEvent(toolButton, &e);
                                res = res && e.isAccepted();
                            }
                        }
                    }
                }
            }
        }
    }
    return res;
}
#endif



