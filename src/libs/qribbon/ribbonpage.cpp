/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include <QApplication>
#include <QEvent>
#include <QPainter>
#include <QStyleOption>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    #include <QtMath>
#else
    #include <QtCore/qmath.h>
#endif

#include "ribbon_def.h"
#include "ribbonbar.h"
#include "ribbonbar_p.h"
#include "ribbongroup.h"
#include "ribbongroup_p.h"
#include "ribbonpage.h"
#include "ribbonpage_p.h"
#include "ribbonstyle.h"
#include "stylehelpers.h"

#ifdef Q_OS_WIN
    #include <qt_windows.h>
#endif   // Q_OS_WIN

QRIBBON_USE_NAMESPACE

static const int pageMarginPopup = 2;
static const int minRibbonWidth = 100;

/* RibbonPagePrivate */
RibbonPagePrivate::RibbonPagePrivate()
{
    m_animationStep = 0.0;
    m_timerElapse = 0;

    m_buttonScrollGroupLeft = Q_NULL;    // Button to draw left scroll
    m_buttonScrollGroupRight = Q_NULL;   // Button to draw right scroll

    m_title = RibbonBar::tr_compatible(RibbonUntitledString);
    m_minimazeRibbon = false;
    m_doVisiblePage = false;
    m_allowPress = false;
    m_doPopupPage = false;
    m_animation = false;
    m_contextColor = RibbonPage::ContextColorNone;
    m_layoutDirection = Qt::RightToLeft;
    m_associativeTab = Q_NULL;
    m_groupsHeight = -1;
    m_groupScrollPos = 0;
    m_pageScrollPos = 0;
    m_scrollPosTarget = 0;
}

RibbonPagePrivate::~RibbonPagePrivate() {}

void RibbonPagePrivate::init()
{
    QTC_Q(RibbonPage);

    m_buttonScrollGroupLeft = new RibbonGroupScroll(q, true);
    m_buttonScrollGroupLeft->setVisible(false);

    m_buttonScrollGroupRight = new RibbonGroupScroll(q, false);
    m_buttonScrollGroupRight->setVisible(false);

    QObject::connect(m_buttonScrollGroupLeft, SIGNAL(pressed()), this, SLOT(pressLeftScrollButton()));
    QObject::connect(m_buttonScrollGroupRight, SIGNAL(pressed()), this, SLOT(pressRightScrollButton()));

    QObject::connect(m_buttonScrollGroupLeft, SIGNAL(stopScrollTimer()), this, SLOT(forcedStopScrollTimer()));
    QObject::connect(m_buttonScrollGroupRight, SIGNAL(stopScrollTimer()), this, SLOT(forcedStopScrollTimer()));
}

bool RibbonPagePrivate::collapseGroups(int &leftOffset, int actualWidth, RibbonControlSizeDefinition::GroupSize size,
                                       bool adjust)
{
    Q_ASSERT(m_layoutDirection == Qt::RightToLeft || m_layoutDirection == Qt::LeftToRight);

    bool ret = false;
    if (leftOffset > actualWidth) {
        QListIterator<RibbonGroup *> iterator(m_listGroups);
        bool normDir = m_layoutDirection == Qt::RightToLeft;
        if (normDir)
            iterator.toBack();

        while (normDir ? iterator.hasPrevious() : iterator.hasNext()) {
            RibbonGroup *group = normDir ? iterator.previous() : iterator.next();
            ;
            if (!group->isVisible())
                continue;
            if (group->currentSize() <= size) {
                int width_0 = group->sizeHint().width();
                if (adjust)
                    group->qtc_d()->adjustCurrentSize(false);
                else
                    group->qtc_d()->reduce();
                int width_1 = group->sizeHint().width();
                int delta = width_0 - width_1;
                leftOffset -= delta;
                ret = ret || (delta != 0);
            }
            if (leftOffset <= actualWidth)
                break;
        }
    }
    return ret;
}

bool RibbonPagePrivate::expandGroups(int &leftOffset, int actualWidth, RibbonControlSizeDefinition::GroupSize size,
                                     bool adjust)
{
    Q_ASSERT(m_layoutDirection == Qt::RightToLeft || m_layoutDirection == Qt::LeftToRight);

    bool ret = false;
    bool normDir = m_layoutDirection == Qt::RightToLeft;
    QListIterator<RibbonGroup *> iterator(m_listGroups);
    if (!normDir)
        iterator.toBack();

    while (normDir ? iterator.hasNext() : iterator.hasPrevious()) {
        RibbonGroup *group = normDir ? iterator.next() : iterator.previous();
        if (!group->isVisible())
            continue;
        if (group->currentSize() >= size) {
            int width_0 = group->sizeHint().width();
            if (adjust)
                group->qtc_d()->adjustCurrentSize(true);
            else
                group->qtc_d()->expand();
            int width_1 = group->sizeHint().width();
            int delta = width_0 - width_1;
            leftOffset -= delta;
            ret = ret || (delta != 0);
        }
        if (leftOffset > actualWidth)
            break;
    }
    return ret;
}

RibbonControlSizeDefinition::GroupSize RibbonPagePrivate::getMinGroupSize() const
{
    RibbonControlSizeDefinition::GroupSize size = RibbonControlSizeDefinition::GroupPopup;
    for (QList<RibbonGroup *>::const_iterator it = m_listGroups.constBegin(); it != m_listGroups.constEnd(); ++it) {
        RibbonGroup *group = *it;
        size = qMin(size, group->currentSize());
        if (size == RibbonControlSizeDefinition::GroupLarge)
            break;
    }
    return size;
}

RibbonControlSizeDefinition::GroupSize RibbonPagePrivate::getMaxGroupSize() const
{
    RibbonControlSizeDefinition::GroupSize size = RibbonControlSizeDefinition::GroupLarge;
    for (QList<RibbonGroup *>::const_iterator it = m_listGroups.constBegin(); it != m_listGroups.constEnd(); ++it) {
        RibbonGroup *group = *it;
        size = qMax(size, group->currentSize());
    }
    return size;
}

bool RibbonPagePrivate::canReduce() const
{
    for (QList<RibbonGroup *>::const_iterator it = m_listGroups.constBegin(); it != m_listGroups.constEnd(); ++it) {
        RibbonGroup *group = *it;

        if (group->qtc_d()->canReduce())
            return true;
    }
    return false;
}

static RibbonControlSizeDefinition::GroupSize qtc_size_inc(RibbonControlSizeDefinition::GroupSize &size)
{
    RibbonControlSizeDefinition::GroupSize ret = size;
    size = (RibbonControlSizeDefinition::GroupSize)(size + 1);
    return ret;
}

static RibbonControlSizeDefinition::GroupSize qtc_size_dec(RibbonControlSizeDefinition::GroupSize &size)
{
    RibbonControlSizeDefinition::GroupSize ret = size;
    size = (RibbonControlSizeDefinition::GroupSize)(size - 1);
    return ret;
}

int RibbonPagePrivate::adjustGroups(int leftOffset, int actualWidth)
{
    RibbonControlSizeDefinition::GroupSize size = getMaxGroupSize();
    while (size > RibbonControlSizeDefinition::GroupLarge && leftOffset < actualWidth) {
        if (expandGroups(leftOffset, actualWidth, size, true))
            continue;
        if (!expandGroups(leftOffset, actualWidth, qtc_size_dec(size), false))
            break;
    }

    if (leftOffset > actualWidth) {
        size = getMinGroupSize();
        while (canReduce() && leftOffset > actualWidth) {
            if (collapseGroups(leftOffset, actualWidth, size, true))
                continue;
            if (!collapseGroups(leftOffset, actualWidth, qtc_size_inc(size), false))
                break;
        }
    }
    return leftOffset;
}

int RibbonPagePrivate::calculateGroupsWidth()
{
    int width = 0;
    for (int i = 0, count = m_listGroups.size(); i < count; ++i) {
        RibbonGroup *group = m_listGroups.at(i);
        if (!group->isVisible())
            continue;
        width += group->sizeHint().width();
    }
    return width;
}

void RibbonPagePrivate::updateLayout(bool updateScroll)
{
    QTC_Q(RibbonPage);

    int leftOffset = 0;
    int actualWidth = q->width();
    if (RibbonBar *ribbon = qobject_cast<RibbonBar *>(q->parentWidget())) {
        if (!(q->windowFlags() & Qt::Popup) && ribbon->qtc_d()->m_logotypeLabel->isVisible()) {
            QRect rectLogotype = ribbon->qtc_d()->m_logotypeLabel->geometry();
            if (!rectLogotype.isNull()) {
                actualWidth -= rectLogotype.width();

                if (ribbon->qtc_d()->m_logotypeLabel->alignmentLogotype() == Qt::AlignLeft)
                    leftOffset += rectLogotype.width();
            }
        }
    }

    bool IsMinimized = actualWidth < minRibbonWidth;
    if (!IsMinimized) {
        int gropsWidth = calculateGroupsWidth() + leftOffset;
        adjustGroups(gropsWidth, actualWidth);
    }

    static int margin = 4;
    for (QList<RibbonGroup *>::iterator it = m_listGroups.begin(); it != m_listGroups.end(); ++it) {
        RibbonGroup *group = *(it);
        if (!group->isVisible())
            continue;

        if (it == m_listGroups.begin())
            leftOffset += margin;

        QSize groupSizeHint = group->sizeHint();

        //        groupSizeHint.rwidth() -= 52;

        int controlsLeftOffset = 0;
        QWidget *parentWidget = group;
        if (group->isReduced()) {
            // Is the group in a popup stage then create a sub-window RibbonGroupPopup
            group->qtc_d()->reposition(QRect(QPoint(-m_groupScrollPos + leftOffset, margin - 1),
                                             QSize(groupSizeHint.width(), q->size().height())));
            Q_ASSERT(group->qtc_d()->m_groupPopup != 0);
            parentWidget = group->qtc_d()->m_groupPopup;
        }

        if (!(group->controlsAlignment() & Qt::AlignLeft)) {
            QSize contentSize = group->qtc_d()->updateControlsLayout(Q_NULL);
            QSize parentSize = parentWidget->sizeHint();
            controlsLeftOffset = QStyle::alignedRect(Qt::LeftToRight, group->controlsAlignment(), contentSize,
                                                     QRect(QPoint(0, 0), parentSize))
                                     .left();
        }

        group->qtc_d()->updateControlsLayout(parentWidget, controlsLeftOffset);

        if (!group->isReduced())
            group->qtc_d()->reposition(QRect(QPoint(-m_groupScrollPos + leftOffset, margin - 1),
                                             QSize(groupSizeHint.width(), q->size().height())));

        group->qtc_d()->updateOptionButtonLayout();
        leftOffset += groupSizeHint.width();
    }

    if (updateScroll)
        showGroupScroll(false);
}

void RibbonPagePrivate::removeGroup(int index, bool deleteGroup)
{
    if (validateGroupIndex(index)) {
        RibbonGroup *group = m_listGroups.at(index);

        group->removeEventFilter(this);
        QList<QAction *> actList = group->actions();

        for (int i = actList.size() - 1; i >= 0; --i)
            m_listShortcuts.removeOne(actList.at(i));

        m_listGroups.removeAt(index);

        if (deleteGroup)
            delete group;
        else
            group->setParent(Q_NULL);

        updateLayout();
    } else
        Q_ASSERT(false);
}

int RibbonPagePrivate::groupIndex(RibbonGroup *group) const
{
    return m_listGroups.indexOf(group);
}

int RibbonPagePrivate::calcReducedGroupsWidth() const
{
    QTC_Q(const RibbonPage);
    static int margin = 4;
    int totalWidth = 0;
    for (QList<RibbonGroup *>::const_iterator it = m_listGroups.constBegin(); it != m_listGroups.constEnd(); ++it) {
        RibbonGroup *group = *(it);
        if (!group->isVisible())
            continue;

        if (group->qtc_d()->canReduce()) {
            totalWidth = 0;
            break;
        }

        if (it == m_listGroups.begin())
            totalWidth = margin * 2;

        QRect rect = group->geometry();
        totalWidth += rect.width();
    }

    int widthLogotype = 0;
    if (RibbonBar *ribbon = qobject_cast<RibbonBar *>(q->parentWidget())) {
        if (!(q->windowFlags() & Qt::Popup) && ribbon->qtc_d()->m_logotypeLabel->isVisible()) {
            QRect rectLogotype = ribbon->qtc_d()->m_logotypeLabel->geometry();
            if (!rectLogotype.isNull())
                widthLogotype = rectLogotype.width();
        }
    }

    return totalWidth + widthLogotype;
}

void RibbonPagePrivate::enableGroupScroll(bool scrollLeft, bool scrollRight)
{
    QTC_Q(RibbonPage);

    Q_ASSERT(m_buttonScrollGroupLeft);
    Q_ASSERT(m_buttonScrollGroupRight);

    if (!m_buttonScrollGroupRight || !m_buttonScrollGroupLeft)
        return;

    QRect rcPage(q->geometry());

    if (scrollLeft) {
        m_buttonScrollGroupLeft->raise();
        m_buttonScrollGroupLeft->setVisible(true);

        int leftOffset = 0;
        if (RibbonBar *ribbon = qobject_cast<RibbonBar *>(q->parentWidget())) {
            if (!(q->windowFlags() & Qt::Popup) && ribbon->qtc_d()->m_logotypeLabel->isVisible() &&
                ribbon->qtc_d()->m_logotypeLabel->alignmentLogotype() == Qt::AlignLeft) {
                QRect rectLogotype = ribbon->qtc_d()->m_logotypeLabel->geometry();
                if (!rectLogotype.isNull())
                    leftOffset = rectLogotype.width() - 2;
            }
        }

        QRect rc(QPoint(0 + leftOffset - 1, 1), QSize(12, rcPage.height() - 1));

        if ((q->windowFlags() & Qt::Popup))
            rc.setHeight(rc.height() + pageMarginPopup);

        m_buttonScrollGroupLeft->setGeometry(rc);
    } else {
        m_buttonScrollGroupLeft->setDown(false);
        m_buttonScrollGroupLeft->setVisible(false);
    }

    if (scrollRight) {
        m_buttonScrollGroupRight->raise();
        m_buttonScrollGroupRight->setVisible(true);

        int leftOffset = 0;
        if (RibbonBar *ribbon = qobject_cast<RibbonBar *>(q->parentWidget())) {
            if (!(q->windowFlags() & Qt::Popup) && ribbon->qtc_d()->m_logotypeLabel->isVisible() &&
                ribbon->qtc_d()->m_logotypeLabel->alignmentLogotype() == Qt::AlignRight) {
                QRect rectLogotype = ribbon->qtc_d()->m_logotypeLabel->geometry();
                if (!rectLogotype.isNull())
                    leftOffset = rectLogotype.width() - 3;
            }
        }

        QRect rc(QPoint(rcPage.width() - leftOffset - 12 + 1, 1), QSize(12, rcPage.height() - 1));

        if ((q->windowFlags() & Qt::Popup))
            rc.setHeight(rc.height() + pageMarginPopup);

        m_buttonScrollGroupRight->setGeometry(rc);
    } else {
        m_buttonScrollGroupRight->setDown(false);
        m_buttonScrollGroupRight->setVisible(false);
    }
    if (!m_buttonScrollGroupLeft->isVisible() && !m_buttonScrollGroupRight->isVisible())
        m_pageScrollPos = 0;
}

void RibbonPagePrivate::showGroupScroll(bool onlyCalc)
{
    QTC_Q(RibbonPage);
    int totalWidth = calcReducedGroupsWidth();
    if (totalWidth == 0 && !m_buttonScrollGroupLeft->isVisible() && !m_buttonScrollGroupRight->isVisible())
        return;

    int groupsLength = q->width();
    int scrollPos = m_groupScrollPos;

    if (totalWidth > groupsLength) {
        if (scrollPos > totalWidth - groupsLength)
            scrollPos = totalWidth - groupsLength;
    } else
        scrollPos = 0;

    if (scrollPos < 0)
        scrollPos = 0;

    m_groupScrollPos = scrollPos;
    if (!onlyCalc)
        enableGroupScroll(scrollPos > 0, totalWidth - groupsLength - scrollPos > 0);
}

void RibbonPagePrivate::scrollGroupAnimate()
{
    if (qAbs(m_groupScrollPos - m_scrollPosTarget) > qFabs(m_animationStep)) {
        m_groupScrollPos = int((double)m_groupScrollPos + m_animationStep);
        updateLayout(false);
    } else {
        m_animation = false;
        m_groupScrollPos = m_scrollPosTarget;
        m_timerElapse = 0;
        m_scrollTimer.stop();
        showGroupScroll(false);
    }
}

void RibbonPagePrivate::startScrollGropsAnimate(int groupScrollPos, int scrollPosTarget)
{
    QTC_Q(RibbonPage)
    m_animation = true;
    m_scrollPosTarget = scrollPosTarget;
    m_groupScrollPos = groupScrollPos;
    m_timerElapse = QApplication::doubleClickInterval() * 4 / 5;
    m_animationStep = double(m_scrollPosTarget - m_groupScrollPos) / (m_timerElapse > 200 ? 8.0 : 3.0);

    if (m_animationStep > 0 && m_animationStep < 1)
        m_animationStep = 1;
    if (m_animationStep < 0 && m_animationStep > -1)
        m_animationStep = -1;

    int nms = 40;
    m_scrollTimer.start(nms, q);
    scrollGroupAnimate();
}

static void listPageWidth(int totalWidth, int realWidth, QList<int> &pagesWidth)
{
    if (totalWidth > realWidth) {
        pagesWidth << realWidth;
        listPageWidth(totalWidth - realWidth, realWidth, pagesWidth);
    } else
        pagesWidth << totalWidth;
    return;
}

void RibbonPagePrivate::pressLeftScrollButton()
{
    QTC_Q(RibbonPage)
    QList<int> pagesWidth;
    listPageWidth(calcReducedGroupsWidth(), q->width(), pagesWidth);

    if (0 < m_pageScrollPos)
        m_pageScrollPos--;

    int scrollPos = m_groupScrollPos;
    m_groupScrollPos -= pagesWidth[m_pageScrollPos];
    showGroupScroll(true);
    startScrollGropsAnimate(scrollPos, m_groupScrollPos);
}

void RibbonPagePrivate::pressRightScrollButton()
{
    QTC_Q(RibbonPage)
    QList<int> pagesWidth;
    listPageWidth(calcReducedGroupsWidth(), q->width(), pagesWidth);

    m_groupScrollPos += pagesWidth[m_pageScrollPos];
    showGroupScroll(true);

    if (pagesWidth.size() - 1 > m_pageScrollPos)
        m_pageScrollPos++;

    startScrollGropsAnimate(0, m_groupScrollPos);
}

void RibbonPagePrivate::forcedStopScrollTimer()
{
    m_scrollTimer.stop();
    showGroupScroll(false);
}

bool RibbonPagePrivate::eventFilter(QObject *obj, QEvent *event)
{
    bool res = QObject::eventFilter(obj, event);

    if (!qobject_cast<RibbonGroup *>(obj))
        return res;

    switch (event->type()) {
    case QEvent::ActionAdded:
        if (QActionEvent *actEvent = static_cast<QActionEvent *>(event)) {
            m_listShortcuts.append(actEvent->action());
            if (m_associativeTab)
                m_associativeTab->addAction(actEvent->action());
        }
        break;
    case QEvent::ActionRemoved:
        if (QActionEvent *actEvent = static_cast<QActionEvent *>(event)) {
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
RibbonPage::RibbonPage(QWidget *parent) : QWidget(parent)
{
    QTC_INIT_PRIVATE(RibbonPage);
    QTC_D(RibbonPage);
    d->init();
}

RibbonPage::RibbonPage(RibbonBar *ribbonBar, const QString &title) : QWidget(ribbonBar)
{
    QTC_INIT_PRIVATE(RibbonPage);
    QTC_D(RibbonPage);
    d->init();
    setTitle(title);
}

RibbonPage::~RibbonPage()
{
    QTC_D(RibbonPage);
    if (d->m_associativeTab)
        d->m_associativeTab->setPage(Q_NULL);

    if (RibbonBar *ribbon = qobject_cast<RibbonBar *>(parentWidget()))
        ribbon->detachPage(this);
    QTC_FINI_PRIVATE();
}

bool RibbonPage::isVisible() const
{
    QTC_D(const RibbonPage);
    return QWidget::isVisible() || (d->m_associativeTab && d->m_associativeTab->isVisible());
}

void RibbonPage::addGroup(RibbonGroup *group)
{
    insertGroup(-1, group);
}

RibbonGroup *RibbonPage::addGroup(const QString &title)
{
    return insertGroup(-1, title);
}

RibbonGroup *RibbonPage::addGroup(const QIcon &icon, const QString &title)
{
    return insertGroup(-1, icon, title);
}

void RibbonPage::insertGroup(int index, RibbonGroup *group)
{
    QTC_D(RibbonPage);

    group->setParent(this);

    if (RibbonBar *ribbonBar = qobject_cast<RibbonBar *>(parentWidget()))
        group->setProperty(_qtc_TitleGroupsVisible, ribbonBar->isTitleGroupsVisible());

    if (!d->validateGroupIndex(index)) {
        index = d->m_listGroups.count();
        d->m_listGroups.append(group);
    } else
        d->m_listGroups.insert(index, group);

    connect(group, SIGNAL(actionTriggered(QAction *)), this, SLOT(actionTriggered(QAction *)), Qt::QueuedConnection);
    connect(group, SIGNAL(released()), this, SLOT(released()));

    group->show();
    d->updateLayout();
}

RibbonGroup *RibbonPage::insertGroup(int index, const QString &title)
{
    RibbonGroup *group = new RibbonGroup(Q_NULL, title);
    insertGroup(index, group);
    return group;
}

RibbonGroup *RibbonPage::insertGroup(int index, const QIcon &icon, const QString &title)
{
    if (RibbonGroup *group = insertGroup(index, title)) {
        group->setIcon(icon);
        return group;
    }
    return Q_NULL;
}

void RibbonPage::removeGroup(RibbonGroup *group)
{
    QTC_D(RibbonPage);
    removeGroup(d->groupIndex(group));
}

void RibbonPage::removeGroup(int index)
{
    QTC_D(RibbonPage);
    d->removeGroup(index, true);
}

void RibbonPage::detachGroup(RibbonGroup *group)
{
    QTC_D(RibbonPage);
    detachGroup(d->groupIndex(group));
}

void RibbonPage::detachGroup(int index)
{
    QTC_D(RibbonPage);
    d->removeGroup(index, false);
}

void RibbonPage::clearGroups()
{
    QTC_D(RibbonPage);
    for (int i = (int)d->m_listGroups.count() - 1; i >= 0; i--)
        removeGroup(i);
}

QAction *RibbonPage::defaultAction() const
{
    QTC_D(const RibbonPage);
    if (d->m_associativeTab)
        return d->m_associativeTab->defaultAction();
    return Q_NULL;
}

int RibbonPage::groupCount() const
{
    QTC_D(const RibbonPage);
    return d->m_listGroups.count();
}

RibbonGroup *RibbonPage::getGroup(int index) const
{
    QTC_D(const RibbonPage);
    if (index < 0 || index >= d->m_listGroups.size())
        return Q_NULL;
    return d->m_listGroups[index];
}

int RibbonPage::groupIndex(RibbonGroup *group) const
{
    QTC_D(const RibbonPage);
    Q_ASSERT(group != Q_NULL);
    if (group && d->m_listGroups.contains(group))
        return d->m_listGroups.indexOf(group);
    return -1;
}

QList<RibbonGroup *> RibbonPage::groups() const
{
    QTC_D(const RibbonPage);
    return d->m_listGroups;
}

/*!
\brief Sets the predefined \a color for the contextual page.
 */
void RibbonPage::setContextColor(ContextColor color)
{
    QTC_D(RibbonPage);
    d->m_contextColor = color;

    if (d->m_associativeTab) {
        d->m_associativeTab->setContextTab(color);
        update();
        if (parentWidget())
            parentWidget()->update();
#ifdef Q_OS_WIN
        if (RibbonBar *ribbonBar = qobject_cast<RibbonBar *>(parentWidget()))
            ribbonBar->updateWindowTitle();
#endif   // Q_OS_WIN
    }
}

/*!
\brief Returns the predefined color for the contextual page.
 */
RibbonPage::ContextColor RibbonPage::contextColor() const
{
    QTC_D(const RibbonPage);
    return d->m_contextColor;
}

void RibbonPage::setVisible(bool visible)
{
    QTC_D(RibbonPage);
    if (d->m_associativeTab) {
        d->m_associativeTab->setContextTab(d->m_contextColor);
        d->m_associativeTab->setContextTextTab(d->m_contextTitle);
    }

    bool saveVisible = isVisible();

    if (d->m_minimazeRibbon && d->m_associativeTab) {
        if (d->m_doPopupPage)
            QWidget::setVisible(visible);

        if (!d->m_doPopupPage) {
            d->m_associativeTab->setVisible(visible);
            if (saveVisible != isVisible()) {
#ifdef Q_OS_WIN
                if (RibbonBar *ribbonBar = qobject_cast<RibbonBar *>(parentWidget()))
                    ribbonBar->updateWindowTitle();
#endif   // Q_OS_WIN
            }
        }

        if (!visible && d->m_doPopupPage)
            d->m_doPopupPage = false;
    } else {
        if (d->m_doVisiblePage)
            QWidget::setVisible(visible);
        else if (!visible)
            QWidget::setVisible(false);

        if (!d->m_doVisiblePage && d->m_associativeTab)
            d->m_associativeTab->setVisible(visible);

        if (RibbonBar *ribbonBar = qobject_cast<RibbonBar *>(parentWidget())) {
            if (!d->m_doVisiblePage && !visible) {
                bool setCurrentPage = false;
                int index = ribbonBar->currentPageIndex();
                if (index >= 0 && index < ribbonBar->getPageCount()) {
                    RibbonPage *page = ribbonBar->getPage(index);
                    if (page->isVisible()) {
                        ribbonBar->setCurrentPageIndex(index);
                        setCurrentPage = true;
                    }
                }

                for (int i = 0, count = ribbonBar->getPageCount(); count > i && !setCurrentPage; ++i) {
                    RibbonPage *page = ribbonBar->getPage(i);
                    if (page->qtc_d()->m_associativeTab && page->qtc_d()->m_associativeTab->isVisible()) {
                        ribbonBar->setCurrentPageIndex(i);
                        break;
                    }
                }
            } else if (visible && saveVisible != isVisible()) {
                for (int i = 0, count = ribbonBar->getPageCount(); count > i; ++i) {
                    if (i == ribbonBar->currentPageIndex()) {
                        RibbonPage *page = ribbonBar->getPage(i);
                        if (page == this && page->qtc_d()->m_associativeTab->isVisible()) {
                            QWidget::setVisible(visible);
                            break;
                        }
                    }
                }
                ribbonBar->updateLayout();
            }

            if (d->m_associativeTab && d->m_contextColor != RibbonPage::ContextColorNone && saveVisible != visible) {
#ifdef Q_OS_WIN
                if (RibbonBar *ribbonBar = qobject_cast<RibbonBar *>(parentWidget()))
                    ribbonBar->updateWindowTitle();
#endif   // Q_OS_WIN
            }
        }
    }
}

void RibbonPage::setTitle(const QString &title)
{
    QTC_D(RibbonPage);

    if (d->m_title == title)
        return;

    d->m_title = title;

    if (d->m_associativeTab)
        d->m_associativeTab->setTextTab(d->m_title);

    emit titleChanged(d->m_title);
}

/*!
\property RibbonPage::title
Sets the title of the page.
\sa RibbonPage::contextTitle
\inmodule ribbon
*/
const QString &RibbonPage::title() const
{
    QTC_D(const RibbonPage);
    return d->m_associativeTab ? d->m_associativeTab->textTab() : d->m_title;
}

void RibbonPage::setContextTitle(const QString &title)
{
    QTC_D(RibbonPage);
    d->m_contextTitle = title;

    if (d->m_associativeTab) {
        d->m_associativeTab->setContextTextTab(d->m_contextTitle);
#ifdef Q_OS_WIN
        if (RibbonBar *ribbonBar = qobject_cast<RibbonBar *>(parentWidget()))
            ribbonBar->updateWindowTitle();
#endif   // Q_OS_WIN
    }
}

void RibbonPage::released()
{
    QTC_D(RibbonPage);
    if (d->m_minimazeRibbon && isVisible() && QApplication::activePopupWidget() == this)
        hide();
}

void RibbonPage::actionTriggered(QAction *action)
{
    QTC_D(RibbonPage);

    if (d->m_listShortcuts.indexOf(action) != -1)
        return;

    if (d->m_minimazeRibbon && isVisible()) {
        if (action->menu())
            return;
        close();
    }
}

/*!
\property RibbonPage::contextTitle
Sets the context title of the page. Context title is located on main window frame if page is contextual.
\sa RibbonPage::title
\inmodule ribbon
*/

const QString &RibbonPage::contextTitle() const
{
    QTC_D(const RibbonPage);
    return d->m_associativeTab ? d->m_associativeTab->contextTextTab() : d->m_title;
}

void RibbonPage::setContextGroupName(const QString &groupName)
{
    QTC_D(RibbonPage);
    d->m_contextGroupName = groupName;

    if (d->m_associativeTab) {
        d->m_associativeTab->setContextGroupName(d->m_contextGroupName);
#ifdef Q_OS_WIN
        if (RibbonBar *ribbonBar = qobject_cast<RibbonBar *>(parentWidget()))
            ribbonBar->updateWindowTitle();
#endif   // Q_OS_WIN
    }
}

/*!
\property RibbonPage::tabWidth
Sets the width of the tab in pixels. By default the width is calculated automaticaly relative to the font of this page.
\inmodule ribbon
*/
void RibbonPage::setTabWidth(int width)
{
    QTC_D(RibbonPage);
    if (d->m_associativeTab == Q_NULL)
        return;
    d->m_associativeTab->setTabWidth(width);
}

int RibbonPage::tabWidth() const
{
    QTC_D(const RibbonPage);
    if (d->m_associativeTab == Q_NULL)
        return -1;
    return d->m_associativeTab->tabWidth();
}

void RibbonPage::updateLayout()
{
    QTC_D(RibbonPage);
    d->updateLayout();
}

void RibbonPage::setAssociativeTab(QWidget *widget)
{
    QTC_D(RibbonPage);
    d->m_associativeTab = qobject_cast<RibbonTab *>(widget);

    if (!d->m_associativeTab)
        return;

    d->m_associativeTab->setPage(this);
    d->m_associativeTab->setContextTab(d->m_contextColor);
    d->m_associativeTab->setContextTextTab(d->m_contextTitle);

    for (int j = 0, count = groupCount(); count > j; ++j) {
        if (RibbonGroup *group = getGroup(j)) {
            QList<QAction *> actList = group->actions();

            for (int i = 0, actCount = actList.size(); actCount > i; ++i) {
                QAction *a = actList.at(i);
                d->m_listShortcuts.append(a);
                d->m_associativeTab->addAction(a);
            }
        }
    }
}

QWidget *RibbonPage::associativeTab() const
{
    QTC_D(const RibbonPage);
    return d->m_associativeTab;
}

void RibbonPage::setPageVisible(bool visible)
{
    QTC_D(RibbonPage);
    d->m_doVisiblePage = true;
    if (visible && d->m_associativeTab && !d->m_associativeTab->isHidden()) {
        setVisible(true);
        emit activated();
    } else if (d->m_associativeTab && !d->m_associativeTab->isHidden())
        setVisible(false);
    d->m_doVisiblePage = false;
}

void RibbonPage::popup()
{
    QTC_D(RibbonPage);
    d->m_doPopupPage = true;
    setVisible(true);
}

void RibbonPage::setRibbonMinimized(bool minimized)
{
    QTC_D(RibbonPage);
    if (d->m_minimazeRibbon != minimized) {
        d->m_minimazeRibbon = minimized;
    }
}

void RibbonPage::activatingPage(bool &allow)
{
    emit activating(allow);
}

void RibbonPage::setGroupsHeight(int height)
{
    QTC_D(RibbonPage);
    d->m_groupsHeight = height;
}

bool RibbonPage::event(QEvent *event)
{
    QTC_D(RibbonPage);
    switch (event->type()) {
    case QEvent::LayoutRequest:
        d->updateLayout();
        break;
    case QEvent::Show:
        if (d->m_minimazeRibbon && d->m_associativeTab) {
            d->m_associativeTab->setTrackingMode(true);
            d->m_timer.start(QApplication::doubleClickInterval(), this);
            d->m_allowPress = true;
        }
        break;
    case QEvent::Hide:
        if (d->m_minimazeRibbon && d->m_associativeTab) {
            d->m_associativeTab->setTrackingMode(false);
            d->m_allowPress = true;
        }
        break;
    case QEvent::Timer: {
        QTimerEvent *timerEvent = static_cast<QTimerEvent *>(event);
        if (d->m_minimazeRibbon && d->m_associativeTab) {
            if (d->m_timer.timerId() == timerEvent->timerId()) {
                d->m_allowPress = false;
                return true;
            }
        }
        if (d->m_scrollTimer.timerId() == timerEvent->timerId()) {
            d->scrollGroupAnimate();
            event->accept();
            return true;
        }
    } break;
    case QEvent::StyleChange:
    case QEvent::FontChange: {
        qtc_set_font_to_ribbon_children(this, font());
    } break;
    case QEvent::ChildAdded: {
        QChildEvent *childEvent = static_cast<QChildEvent *>(event);
        if (RibbonGroup *group = qobject_cast<RibbonGroup *>(childEvent->child()))
            group->installEventFilter(d);

        if (childEvent->added()) {
            QWidget *widget = qobject_cast<QWidget *>(childEvent->child());
            if (widget != Q_NULL) {
                if (qobject_cast<RibbonGroup *>(widget) != Q_NULL)
                    qobject_cast<RibbonGroup *>(widget)->setFont(font());
                else {
                    widget->setFont(font());
                    qtc_set_font_to_ribbon_children(widget, font());
                }
            }
        }
    } break;
    case QEvent::ChildRemoved: {
        QChildEvent *childEvent = static_cast<QChildEvent *>(event);
        if (RibbonGroup *group = qobject_cast<RibbonGroup *>(childEvent->child()))
            group->removeEventFilter(d);
    } break;
    case QEvent::MouseButtonPress:
        if (d->m_minimazeRibbon && d->m_associativeTab) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            if (!rect().contains(mouseEvent->pos())) {
                if (d->m_allowPress) {
                    QWidget *clickedWidget = QApplication::widgetAt(mouseEvent->globalPos());

                    if (clickedWidget == d->m_associativeTab) {
                        const QPoint targetPoint = clickedWidget->mapFromGlobal(mouseEvent->globalPos());

                        QMouseEvent evPress(mouseEvent->type(), targetPoint, mouseEvent->globalPos(),
                                            mouseEvent->button(), mouseEvent->buttons(), mouseEvent->modifiers());
                        QApplication::sendEvent(clickedWidget, &evPress);

                        QMouseEvent eDblClick(QEvent::MouseButtonDblClick, targetPoint, mouseEvent->globalPos(),
                                              mouseEvent->button(), mouseEvent->buttons(), mouseEvent->modifiers());
                        QApplication::sendEvent(d->m_associativeTab, &eDblClick);
                        return true;
                    }
                } else if (d->m_associativeTab &&
                           QRect(d->m_associativeTab->mapToGlobal(QPoint()), d->m_associativeTab->size())
                               .contains(mouseEvent->globalPos()))
                    setAttribute(Qt::WA_NoMouseReplay);

                if (QApplication::activePopupWidget() == this) {
                    if (QWidget *clickedTab =
                            qobject_cast<RibbonTab *>(QApplication::widgetAt(mouseEvent->globalPos()))) {
                        if (d->m_associativeTab && clickedTab != d->m_associativeTab) {
                            const QPoint targetPoint = clickedTab->mapFromGlobal(mouseEvent->globalPos());

                            QMouseEvent evPress(mouseEvent->type(), targetPoint, mouseEvent->globalPos(),
                                                mouseEvent->button(), mouseEvent->buttons(), mouseEvent->modifiers());
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
    QSize resultSize = QWidget::sizeHint();

    // Calculate a max height of the group. Group may contain a different number of rows(3 - rows, 5 - rows)
    int maxHeight = 0;
    for (int i = 0, count = groupCount(); count > i; i++) {
        if (RibbonGroup *group = getGroup(i))
            maxHeight = qMax(maxHeight, group->sizeHint().height());
    }
    resultSize.setHeight(maxHeight);

    if ((windowFlags() & Qt::Popup))
        resultSize.setHeight(resultSize.height() - pageMarginPopup);

    return resultSize;
}

void RibbonPage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if ((windowFlags() & Qt::Popup)) {
        QPainter p(this);
        StyleOptionRibbon opt;
        opt.init(this);
        opt.rect.adjust(0, 0, 0, pageMarginPopup);
        style()->drawControl((QStyle::ControlElement)RibbonStyle::CE_RibbonGroups, &opt, &p, this->parentWidget());
    }
}

void RibbonPage::changeEvent(QEvent *event)
{
    QTC_D(RibbonPage);
    switch (event->type()) {
    case QEvent::StyleChange:
        d->updateLayout();
        break;
    case QEvent::FontChange: {
        for (int i = 0; i < d->m_listGroups.size(); i++) {
            RibbonGroup *group = d->m_listGroups.at(i);
            group->setFont(font());
        }
        d->updateLayout();
    } break;
    default:
        break;
    };
    return QWidget::changeEvent(event);
}

void RibbonPage::resizeEvent(QResizeEvent *event)
{
    QTC_D(RibbonPage);
    QWidget::resizeEvent(event);

    if ((windowFlags() & Qt::Popup)) {
        QSize delta = event->size() - event->oldSize();
        if (delta.isNull())
            return;
        d->updateLayout();
    }
}

#ifdef Q_OS_WIN
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
bool RibbonPage::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    bool res = QWidget::nativeEvent(eventType, message, result);
    MSG *msg = static_cast<MSG *>(message);
    #else
bool RibbonPage::winEvent(MSG *message, long *result)
{
    bool res = QWidget::winEvent(message, result);
    MSG *msg = static_cast<MSG *>(message);
    #endif
    if (!res) {
        if (msg->message == WM_LBUTTONDOWN) {
            if (QWidget *activePopupWidget = QApplication::activePopupWidget()) {
                if (activePopupWidget == this) {
                    POINT curPos = msg->pt;
                    QPoint globalPos(curPos.x, curPos.y);

                    QPoint pos = mapFromGlobal(globalPos);

                    QToolButton *toolButton = qobject_cast<QToolButton *>(activePopupWidget->childAt(pos));

                    if (toolButton && toolButton->isEnabled()) {
                        if (QAction *action = toolButton->defaultAction()) {
                            if (action->menu()) {
                                int button = Qt::LeftButton;
                                int state = 0;

                                pos = toolButton->mapFromGlobal(globalPos);

                                QMouseEvent e(QEvent::MouseButtonPress, pos, globalPos, Qt::MouseButton(button),
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
