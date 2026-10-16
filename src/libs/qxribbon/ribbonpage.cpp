/**
 * Copyleft (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbonpage.h"
#include "ribbonpage_p.h"
#include "ribbongroup_p.h"

#include <QApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QLinearGradient>
#include <QList>
#include <QMap>
#include <QPainter>
#include <QResizeEvent>

QX_RIBBON_BEGIN_NAMESPACE

RibbonPageScrollButton::RibbonPageScrollButton(Qt::ArrowType arr, QWidget *p)
    : QToolButton(p)
{
    setArrowType(arr);
}

RibbonPagePrivate::RibbonPagePrivate()
    : m_ribbonBar(Q_NULLPTR)
    , m_groupLayoutMode(RibbonGroup::ThreeRowMode)
    , m_leftScrollBtn(Q_NULLPTR)
    , m_rightScrollBtn(Q_NULLPTR)
    , m_sizeHint(50, 50)
    , m_contentsMargins(1, 1, 1, 1)
    , m_totalWidth(0)
    , m_XBase(0)
    , m_isRightScrollBtnShow(false)
    , m_isLeftScrollBtnShow(false)
    , m_isPageContext(false)
    , m_isCanCustomize(true)
{
}

RibbonPagePrivate::~RibbonPagePrivate()
{
}

void RibbonPagePrivate::init()
{
    Q_Q(RibbonPage);
    m_leftScrollBtn = new RibbonPageScrollButton(Qt::LeftArrow, q);
    m_rightScrollBtn = new RibbonPageScrollButton(Qt::RightArrow, q);
    m_leftScrollBtn->setVisible(false);
    m_rightScrollBtn->setVisible(false);
    connect(m_leftScrollBtn, &QToolButton::clicked, this, &RibbonPagePrivate::onLeftScrollButtonClicked);
    connect(m_rightScrollBtn, &QToolButton::clicked, this, &RibbonPagePrivate::onRightScrollButtonClicked);
}

RibbonGroup *RibbonPagePrivate::addGroup(const QString &title)
{
    return insertGroup(m_itemList.size(), title);
}

RibbonGroup *RibbonPagePrivate::insertGroup(int index, const QString &title)
{
    Q_Q(RibbonPage);
    RibbonGroup *group = new RibbonGroup(q);

    group->setWindowTitle(title);
    group->setObjectName(title);
    insertGroup(index, group);
    return group;
}

void RibbonPagePrivate::addGroup(RibbonGroup *group)
{
    insertGroup(m_itemList.size(), group);
}

void RibbonPagePrivate::insertGroup(int index, RibbonGroup *group)
{
    Q_Q(RibbonPage);
    if (Q_NULLPTR == group) {
        return;
    }
    if (group->parentWidget() != q) {
        group->setParent(q);
    }
    group->setGroupLayoutMode(m_groupLayoutMode);
    if (index < 0 || index > m_itemList.size()) {
        index = m_itemList.size();
    }
    RibbonPageItem item;

    item.m_group = group;
    item.m_separator = new RibbonSeparator(q);
    m_itemList.insert(index, item);
    updateItemGeometry();
    group->setVisible(true);
}

bool RibbonPagePrivate::takeGroup(RibbonGroup *group)
{
    RibbonPageItem item;

    for (int i = 0; i < m_itemList.size(); ++i) {
        if (group == m_itemList[i].m_group) {
            item = m_itemList.takeAt(i);
            break;
        }
    }
    if (item.isNull()) {
        return false;
    }
    if (item.m_separator) {
        item.m_separator->hide();
        item.m_separator->deleteLater();
    }
    updateItemGeometry();
    return true;
}

bool RibbonPagePrivate::removeGroup(RibbonGroup *group)
{
    if (takeGroup(group)) {
        group->hide();
        group->deleteLater();
        return true;
    }
    return false;
}

QList<RibbonGroup *> RibbonPagePrivate::groupList() const
{
    QList<RibbonGroup *> res;

    for (const RibbonPageItem &i : m_itemList) {
        if (!i.isNull()) {
            res.append(i.m_group);
        }
    }
    return res;
}

/**
 * @brief Set the group mode
 *
 * When RibbonBar calls @ref RibbonBar::setRibbonStyle it invokes this function on every RibbonPage
 * to apply the new RibbonGroup::GroupLayoutMode
 *
 * This function proxies RibbonPage::setGroupLayoutMode;
 * in the RibbonPage class it is protected and is mainly triggered by RibbonBar::setRibbonStyle
 * @param m
 */
void RibbonPagePrivate::setGroupLayoutMode(RibbonGroup::GroupLayoutMode m)
{
    if (m_groupLayoutMode == m) {
        return;
    }
    m_groupLayoutMode = m;
    QList<RibbonGroup *> groups = groupList();

    for (RibbonGroup *group : std::as_const(groups)) {
        group->setGroupLayoutMode(m);
    }
    updateItemGeometry();
}

int RibbonPagePrivate::totalSizeHintWidth() const
{
    int total = 0;
    QMargins m = m_contentsMargins;

    if (!m.isNull()) {
        total += (m.left() + m.right());
    }

    for (const RibbonPageItem &item : std::as_const(m_itemList)) {
        if (item.isEmpty()) {
            continue;
        }
        total += item.m_group->sizeHint().width();
        if (item.m_separator) {
            total += item.m_separator->sizeHint().width();
        }
    }
    return total;
}

QSize RibbonPagePrivate::pageContentSize() const
{
    Q_Q(const RibbonPage);
    QSize s = q->size();
    QMargins m = m_contentsMargins;

    if (!m.isNull()) {
        s.rheight() -= (m.top() + m.bottom());
        s.rwidth() -= (m.right() + m.left());
    }
    return s;
}

/**
 * @brief B11: keep the groups inside the page by narrowing them
 *
 * A group can be laid out in a reduced presentation, which trades rows for width by
 * packing a button that would otherwise own a whole column as a small one. This walks
 * the groups from the right - the side a shrinking window eats into first - and
 * reduces as many of them as it takes to fit, leaving the rest at their natural
 * width. Only when the narrowest form of every group still overflows does the caller
 * fall through to the page's scroll buttons.
 *
 * Nothing is reduced while the natural layout still fits, so a page with room always
 * shows every group at full width.
 *
 * @param pageWidth The width the page content has to fit into, margins included
 * @return The width the groups need once the states have been applied
 */
int RibbonPagePrivate::fitGroupsInWidth(int pageWidth)
{
    struct Entry {
        RibbonGroup *group;
        int normal;
        int reduced;
    };
    QList<Entry> entries;
    // What the groups do not contribute: the contents margins and the separators
    int fixed = 0;

    if (!m_contentsMargins.isNull()) {
        fixed += (m_contentsMargins.left() + m_contentsMargins.right());
    }
    for (const RibbonPageItem &item : std::as_const(m_itemList)) {
        if (item.isEmpty() || (Q_NULLPTR == item.m_group)) {
            continue;
        }
        Entry e;
        e.group = item.m_group;
        e.normal = item.m_group->normalWidth();
        e.reduced = item.m_group->reducedWidth();
        entries.append(e);
        if (item.m_separator) {
            fixed += item.m_separator->sizeHint().width();
        }
    }
    int natural = fixed;
    for (const Entry &e : std::as_const(entries)) {
        natural += e.normal;
    }
    QList<bool> target;
    for (int i = 0; i < entries.size(); ++i) {
        target.append(false);
    }
    if (natural > pageWidth) {
        int total = natural;
        for (int i = entries.size() - 1; (i >= 0) && (total > pageWidth); --i) {
            const int gain = entries.at(i).normal - entries.at(i).reduced;
            if (gain <= 0) {
                // This group is no narrower once reduced - a lone large button is drawn
                // wider beside its icon than under it - so it keeps its natural width
                continue;
            }
            target[i] = true;
            total -= gain;
        }
    }
    for (int i = 0; i < entries.size(); ++i) {
        entries.at(i).group->setReducedInternal(target.at(i));
    }
    return totalSizeHintWidth();
}

void RibbonPagePrivate::updateItemGeometry()
{
    Q_Q(RibbonPage);
    RibbonPage *page = q;
    QSize pageSize = pageContentSize();
    int pageWidth = pageSize.width();

    int y = 0;
    int x = 0;

    if (!m_contentsMargins.isNull()) {
        y = m_contentsMargins.top();
        x = m_contentsMargins.left();
    }

    // B11: a page that has run out of room narrows its groups before it falls back
    // to the scroll buttons, so the width to start from is the one the narrowest
    // form of the groups needs rather than their natural one
    int totalWidth = fitGroupsInWidth(pageWidth);
    // Records the expandable count
    int canExpandingCount = 0;
    // Expanded width
    int expandWidth = 0;

#ifdef QX_RIBBON_DEBUG_HELP_DRAW
    qDebug() << "\r\n\r\n============================================="
             << "\r\nRibbonPagePrivate::updateItemGeometry"
             << "\r\npage name:" << page->windowTitle() << "\r\n first totalWidth:" << totalWidth << "\r\n y:" << y
             << "\r\nexpandWidth:" << expandWidth;
#endif
    // If totalWidth < pageWidth, m_XBase can be set to 0
    if (totalWidth <= pageWidth) {
        // This avoids the case where totalWidth > pageWidth initially, m_XBase was adjusted via the
        // scroll button and the widget was resized accordingly, after which totalWidth < pageWidth
        // so the page cannot be shown at its original position; m_XBase must be set to 0 here
        m_XBase = clampXBase(m_XBase, totalWidth);
        for (const RibbonPageItem &item : m_itemList) {
            if (!item.isEmpty()) {
                if (item.m_group->isExpanding()) {
                    // Group can expand
                    ++canExpandingCount;
                }
            }
        }
        // Compute the expandable width
        if (canExpandingCount > 0) {
            expandWidth = (pageWidth - totalWidth) / canExpandingCount;
        } else {
            expandWidth = 0;
        }
    }
    totalWidth = 0;   // Recompute total
    // The offset the geometry below is laid out with, kept so that a clamp
    // after the measurement can be applied as a translation of it.
    const int xBaseBeforeMeasure = m_XBase;
    x += m_XBase;

    // First set all sizes according to sizeHint
    for (RibbonPageItem &item : m_itemList) {
        if (item.isEmpty()) {
            // Skip directly if hidden
            if (item.m_separator) {
                // The separator is hidden too when the group is hidden
                item.m_separator->hide();
            }
            item.m_groupWillGeometry = QRect(0, 0, 0, 0);
            item.m_separatorWillGeometry = QRect(0, 0, 0, 0);
            continue;
        }
        RibbonGroup *group = item.m_group;
        if (Q_NULLPTR == group) {
            qDebug() << "unknow widget in RibbonPageLayout";
            continue;
        }
        QSize groupSize = group->sizeHint();
        QSize separatorSize(0, 0);
        if (item.m_separator) {
            separatorSize = item.m_separator->sizeHint();
        }
        if (group->isExpanding()) {
            // Expandable, so expand the group to the maximum
            groupSize.setWidth(groupSize.width() + expandWidth);
        }
        int w = groupSize.width();
        item.m_groupWillGeometry = QRect(x, y, w, pageSize.height());
        x += w;
        totalWidth += w;
        w = separatorSize.width();
        item.m_separatorWillGeometry = QRect(x, y, w, pageSize.height());
        x += w;
        totalWidth += w;
    }
    m_totalWidth = totalWidth;
    // The legal range of m_XBase is [pageWidth - m_totalWidth, 0]. Only the
    // "content no longer overflows" end of it is closed above: while the page
    // overflows the lower bound is never re-applied, so an offset that was
    // legal for a narrower page stays behind the bound once the page is
    // widened. The content then stops short of the right edge and leaves a gap
    // as wide as the resize, while the buttons still report "already at the
    // right end" - clicking one heals it, which makes it a transient visual
    // defect rather than corrupted state.
    // Clamping has to use the width just measured - the reason is in
    // clampXBase() - and it goes through the same range as every other path
    // that moves the content. Once the offset has moved, the geometry of this
    // pass is simply shifted by the same amount instead of being measured a
    // second time.
    m_XBase = clampXBase(m_XBase, m_totalWidth);
    const int xShift = m_XBase - xBaseBeforeMeasure;
    if (xShift != 0) {
        for (RibbonPageItem &item : m_itemList) {
            item.m_groupWillGeometry.translate(xShift, 0);
            item.m_separatorWillGeometry.translate(xShift, 0);
        }
    }
    // Determine whether the scroll buttons are shown
    if (totalWidth > pageWidth) {
        // Exceeds the total length, so the scroll buttons must be shown
        if (m_XBase == 0) {
            // Already at the leftmost, so it can move right
            m_isRightScrollBtnShow = true;
            m_isLeftScrollBtnShow = false;
        } else if (m_XBase <= (pageWidth - totalWidth)) {
            // Already at the rightmost, so it can move left
            m_isRightScrollBtnShow = false;
            m_isLeftScrollBtnShow = true;
        } else {
            // In the middle, so both directions can move
            m_isRightScrollBtnShow = true;
            m_isLeftScrollBtnShow = true;
        }
    } else {
        // totalWidth is less than pageWidth
        m_isRightScrollBtnShow = false;
        m_isLeftScrollBtnShow = false;
    }
    QWidget *par = page->parentWidget();
    int parentHeight = (Q_NULLPTR == par) ? pageSize.height() : par->height();
    int parentWidth = (Q_NULLPTR == par) ? totalWidth : par->width();
    m_sizeHint = QSize(parentWidth, parentHeight);
    doItemLayout();
}

void RibbonPagePrivate::doItemLayout()
{
    Q_Q(RibbonPage);
    RibbonPage *page = q;

    // The positions of the two scroll buttons never change
    m_leftScrollBtn->setGeometry(0, 0, 12, page->height());
    m_rightScrollBtn->setGeometry(page->width() - 12, 0, 12, page->height());
    QList<QWidget *> showWidgets, hideWidgets;

    for (const RibbonPageItem &item : m_itemList) {
        if (item.isNull()) {
            continue;
        }
        if (item.isEmpty()) {
            hideWidgets << item.m_group;
            if (item.m_separator) {
                hideWidgets << item.m_separator;
            }
        } else {
            // item.m_group->setFixedSize(item.m_groupWillGeometry.size());
            // item.m_group->move(item.m_groupWillGeometry.topLeft());
            item.m_group->setGeometry(item.m_groupWillGeometry);
            showWidgets << item.m_group;
            if (item.m_separator) {
                item.m_separator->setGeometry(item.m_separatorWillGeometry);
                showWidgets << item.m_separator;
            }
#ifdef QX_RIBBON_DEBUG_HELP_DRAW
            qDebug() << "RibbonPagePrivate::doItemLayout() =";
            qDebug() << "\r\n     group:" << item.m_group->windowTitle()
                     << "\r\n     group geo:" << item.m_groupWillGeometry
                     << "\r\n     sep geo:" << item.m_separatorWillGeometry;
#endif
        }
    }

    m_rightScrollBtn->setVisible(m_isRightScrollBtnShow);
    m_leftScrollBtn->setVisible(m_isLeftScrollBtnShow);
    if (m_isRightScrollBtnShow) {
        m_rightScrollBtn->raise();
    }
    if (m_isLeftScrollBtnShow) {
        m_leftScrollBtn->raise();
    }
    // Show and hide are not done above because that would trigger a repaint of
    // RibbonGroupLayout, causing recursive painting and hurting efficiency
    for (QWidget *w : std::as_const(showWidgets)) {
        w->show();
    }
    for (QWidget *w : std::as_const(hideWidgets)) {
        w->hide();
    }
}

/**
 * @brief The legal range of m_XBase for content that is contentWidth wide
 *
 * The range is [pageWidth - contentWidth, 0]:
 *
 *    |---- pageWidth ----|
 *    |-------- contentWidth --------|
 *    `-> m_XBase (0 is the rightmost, pageWidth - contentWidth is the leftmost)
 *
 * While the content fits, the range collapses to the single point 0: there is
 * nothing to scroll to, and "scrolled" stops having a meaning.
 *
 * Every path that moves the content - the wheel, a scroll button, a resize that
 * re-clamps - lands here, because the range is one rule and not three. The
 * content width is always the caller's, because the caller is the one that
 * knows which measurement it is allowed to use: the total from the size hints
 * carries the contents margins but none of the expanding width, so clamping
 * with it would land on the wrong pixel.
 */
int RibbonPagePrivate::clampXBase(int xBase, int contentWidth) const
{
    const int pageWidth = pageContentSize().width();
    if (contentWidth <= pageWidth) {
        return 0;
    }
    return qBound(pageWidth - contentWidth, xBase, 0);
}

void RibbonPagePrivate::doWheelEvent(QWheelEvent *event)
{
    int pageWidth = pageContentSize().width();
    int totalWidth = m_totalWidth;

    if (totalWidth > pageWidth) {
        // Scrolling is effective at this point
        QPoint numPixels = event->pixelDelta();
        QPoint numDegrees = event->angleDelta() / 8;

        // Tested on the Linux GNOME platform: both event->pixelDelta() and event->angleDelta() are (0, +/-120)
        // Tested on the Windows 10 platform: event->pixelDelta() and event->angleDelta() are (0, 0) and (0, +/-120)
        int scrollpix = 0;
        if (!numPixels.isNull()) {
            // On the Linux GNOME platform, one scrollpix is 30
            scrollpix = numPixels.y() / 4;
        } else if (!numDegrees.isNull()) {
            // On the Windows 10 platform, one scrollpix is 15
            scrollpix = numDegrees.y();
        }
        // The sign of the step says which way the wheel went; the direction it
        // moves the content in is the opposite one, and both ends come from the
        // single range clampXBase() holds.
        m_XBase = clampXBase(m_XBase - scrollpix, totalWidth);
        updateItemGeometry();
    } else {
        // No handling is needed here; forward the scroll event so the parent can also receive it
        event->ignore();
        m_XBase = clampXBase(m_XBase, totalWidth);
    }
}

/**
 * @brief Scroll horizontally by page
 *
 * pages > 0 means the left button was clicked and the content moves right by one page; pages < 0
 * means the right button.
 */
void RibbonPagePrivate::scrollByPage(int pages)
{
    int pageWidth = pageContentSize().width();
    int totalWidth = m_totalWidth;

    m_XBase = clampXBase(m_XBase + pages * pageWidth, totalWidth);
    updateItemGeometry();
}

void RibbonPagePrivate::onLeftScrollButtonClicked()
{
    scrollByPage(1);
}

void RibbonPagePrivate::onRightScrollButtonClicked()
{
    scrollByPage(-1);
}

RibbonPage::RibbonPage(QWidget *parent)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(RibbonPage);
    Q_D(RibbonPage);
    d->init();
    setAutoFillBackground(true);
    setBackgroundBrush(Qt::white);
    // Accessibility (B2): expose the page title (sans the '&' mnemonic) as the accessible
    // name, so assistive tools can announce the page.
    connect(this, &QWidget::windowTitleChanged, this, [this](const QString &title) {
        setAccessibleName(QString(title).remove(QLatin1Char('&')));
    });
}

RibbonPage::RibbonPage(const QString &name, QWidget *parent)
    : RibbonPage(parent)
{
    setPageName(name);
}

RibbonPage::~RibbonPage()
{
    QX_FINI_PRIVATE();
}

QString RibbonPage::pageName() const
{
    return windowTitle();
}

void RibbonPage::setPageName(const QString &title)
{
    setWindowTitle(title);
}

QString RibbonPage::title() const
{
    return pageName();
}

RibbonGroup::GroupLayoutMode RibbonPage::groupLayoutMode() const
{
    Q_D(const RibbonPage);
    return d->m_groupLayoutMode;
}

void RibbonPage::setGroupLayoutMode(RibbonGroup::GroupLayoutMode m)
{
    Q_D(RibbonPage);
    d->setGroupLayoutMode(m);
}

/**
 * @brief Add a group
 *
 * @note Ownership of the group is managed by RibbonPage; do not destroy it externally
 * @param title Group title; shown below the group in Office/WPS three-row mode
 * @return The created @ref RibbonGroup pointer
 * @see For other operations on RibbonGroup, refer to @ref RibbonPage::takeGroup
 */
RibbonGroup *RibbonPage::addGroup(const QString &title)
{
    Q_D(RibbonPage);
    return d->addGroup(title);
}

/**
 * @brief Add a group
 * @param group Ownership of the group is managed by RibbonPage
 */
void RibbonPage::addGroup(RibbonGroup *group)
{
    Q_D(RibbonPage);
    d->addGroup(group);
}

/**
 * @brief Create a group and insert it at the given index
 * @param title Group title
 * @param index Insert position; if it exceeds the number of groups in the page, it is inserted last
 * @return The created @ref RibbonGroup pointer
 */
RibbonGroup *RibbonPage::insertGroup(int index, const QString &title)
{
    Q_D(RibbonPage);
    return d->insertGroup(index, title);
}

/**
 * @brief Find a group by index; returns Q_NULLPTR if the index is out of range
 * @param index
 * @return Returns Q_NULLPTR if the index is out of range
 */
RibbonGroup *RibbonPage::group(int index) const
{
    Q_D(const RibbonPage);
    return d->m_itemList.value(index).m_group;
}

/**
 * @brief Find a group by name
 * @param title
 * @return If there are duplicate names, only the first match is returned
 */
RibbonGroup *RibbonPage::groupByName(const QString &title) const
{
    Q_D(const RibbonPage);
    for (const RibbonPageItem &i : d->m_itemList) {
        if (i.m_group) {
            if (i.m_group->windowTitle() == title) {
                return i.m_group;
            }
        }
    }
    return Q_NULLPTR;
}

/**
 * @brief Find a group by object name
 * @param objname
 * @return If there are duplicates, only the first match is returned
 */
RibbonGroup *RibbonPage::groupByObjectName(const QString &objname) const
{
    Q_D(const RibbonPage);
    for (const RibbonPageItem &i : d->m_itemList) {
        if (i.m_group) {
            if (i.m_group->objectName() == objname) {
                return i.m_group;
            }
        }
    }
    return Q_NULLPTR;
}

/**
 * @brief Find the index corresponding to a group
 * @param group
 * @return Returns -1 if not found
 */
int RibbonPage::groupIndex(RibbonGroup *group) const
{
    Q_D(const RibbonPage);
    int c = groupCount();

    for (int i = 0; i < c; ++i) {
        if (d->m_itemList[i].m_group == group) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Move a Group from one index to another
 * @param from Index of the group to move
 * @param to Destination position
 */
void RibbonPage::moveGroup(int from, int to)
{
    Q_D(RibbonPage);
    if (from == to) {
        return;
    }
    int c = groupCount();
    if (c <= 0) {
        return;
    }
    if (from < 0 || from >= c || to < 0 || to >= c) {
        return;
    }
    // This is the same as insert(to, takeAt(from))
    d->m_itemList.move(from, to);
    d->updateItemGeometry();
}

bool RibbonPage::takeGroup(RibbonGroup *group)
{
    Q_D(RibbonPage);
    return d->takeGroup(group);
}

/**
 * @brief Remove a Group; the Page reclaims the RibbonGroup memory directly
 * @param group The group to remove
 * @note After removal the group is a dangling pointer; it is recommended to set the group pointer
 * to Q_NULLPTR after the operation
 *
 * This operation is equivalent to:
 *
 * @code
 * RibbonGroup *group;
 * ...
 * page->takeGroup(group);
 * group->hide();
 * group->deleteLater();
 * @endcode
 */
bool RibbonPage::removeGroup(RibbonGroup *group)
{
    Q_D(RibbonPage);
    return d->removeGroup(group);
}

/**
 * @brief Remove a group
 * @param index Group index; returns false if out of range
 * @return Returns true on success
 */
bool RibbonPage::removeGroup(int index)
{
    RibbonGroup *grp = group(index);

    if (Q_NULLPTR == grp) {
        return false;
    }
    return removeGroup(grp);
}

void RibbonPage::clearGroups()
{
    for (RibbonGroup *group : groupList()) {
        if (group->parent() == this) {
            removeGroup(group);
        }
    }
}

QList<RibbonGroup *> RibbonPage::groupList() const
{
    Q_D(const RibbonPage);
    return d->groupList();
}

QList<RibbonGroup *> RibbonPage::groups() const
{
    return groupList();
}

void RibbonPage::setBackgroundBrush(const QBrush &brush)
{
    QPalette p = palette();
    p.setBrush(QPalette::Window, brush);
    setPalette(p);
}

QSize RibbonPage::sizeHint() const
{
    Q_D(const RibbonPage);
    return d->m_sizeHint;
}

/**
 * @brief Returns true if this is a ContextPage
 * @return
 */
bool RibbonPage::isPageContext() const
{
    Q_D(const RibbonPage);
    return d->m_isPageContext;
}

int RibbonPage::groupCount() const
{
    Q_D(const RibbonPage);
    return d->m_itemList.size();
}

bool RibbonPage::isCanCustomize() const
{
    Q_D(const RibbonPage);
    return d->m_isCanCustomize;
}

void RibbonPage::setCanCustomize(bool b)
{
    Q_D(RibbonPage);
    d->m_isCanCustomize = b;
}

RibbonBar *RibbonPage::ribbonBar() const
{
    Q_D(const RibbonPage);
    return d->m_ribbonBar;
}

/**
 * @brief Refresh the page layout; suitable to call after changing the ribbon mode
 */
void RibbonPage::updateItemGeometry()
{
    Q_D(RibbonPage);
    QList<RibbonGroup *> groups = groupList();
    for (RibbonGroup *group : std::as_const(groups)) {
        group->updateItemGeometry();
    }
    d->updateItemGeometry();
}

void RibbonPage::setTitle(const QString &title)
{
    setPageName(title);
}

/**
 * @brief Mark this as a context tab
 * @param isPageContext
 */
void RibbonPage::markIsPageContext(bool isPageContext)
{
    Q_D(RibbonPage);
    d->m_isPageContext = isPageContext;
}

bool RibbonPage::event(QEvent *e)
{
    Q_D(RibbonPage);
    switch (e->type()) {
    case QEvent::LayoutRequest: {
        d->updateItemGeometry();
    } break;

    default:
        break;
    }
    return QWidget::event(e);
}

void RibbonPage::resizeEvent(QResizeEvent *e)
{
    Q_D(RibbonPage);
    QWidget::resizeEvent(e);
    d->updateItemGeometry();
}

void RibbonPage::wheelEvent(QWheelEvent *event)
{
    Q_D(RibbonPage);
    d->doWheelEvent(event);
}

void RibbonPage::setRibbonBar(RibbonBar *bar)
{
    Q_D(RibbonPage);
    d->m_ribbonBar = bar;
}

QX_RIBBON_END_NAMESPACE
