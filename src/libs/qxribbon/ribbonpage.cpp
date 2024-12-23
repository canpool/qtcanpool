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
 * @brief 设置group的模式
 *
 * 在@ref RibbonBar调用@ref RibbonBar::setRibbonStyle 函数时，会对所有的RibbonPage调用此函数
 * 把新的RibbonGroup::GroupLayoutMode设置进去
 *
 * 此函数为RibbonPage::setGroupLayoutMode的代理，
 * 在RibbonPage类中，此函数为protected，主要在RibbonBar::setRibbonStyle时才会触发
 * @param m
 */
void RibbonPagePrivate::setGroupLayoutMode(RibbonGroup::GroupLayoutMode m)
{
    if (m_groupLayoutMode == m) {
        return;
    }
    m_groupLayoutMode = m;
    QList<RibbonGroup *> groups = groupList();

    for (RibbonGroup *group : qAsConst(groups)) {
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

    for (const RibbonPageItem &item : qAsConst(m_itemList)) {
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

    int totalWidth = totalSizeHintWidth();
    // 记录可以扩展的数量
    int canExpandingCount = 0;
    // 扩展的宽度
    int expandWidth = 0;

#ifdef QX_RIBBON_DEBUG_HELP_DRAW
    qDebug() << "\r\n\r\n============================================="
             << "\r\nRibbonPagePrivate::updateItemGeometry"
             << "\r\npage name:" << page->windowTitle() << "\r\n first totalWidth:" << totalWidth << "\r\n y:" << y
             << "\r\nexpandWidth:" << expandWidth;
#endif
    // 如果 totalWidth < pageWidth, m_XBase 可以设置为 0
    if (totalWidth <= pageWidth) {
        // 这个是避免一开始 totalWidth > pageWidth，通过滚动按钮调整了 m_XBase，随之调整了窗体尺寸，
        // 调整后 totalWidth < pageWidth 导致 page 在原来位置无法显示，必须这里把 m_XBase 设置为 0
        m_XBase = 0;
        for (const RibbonPageItem &item : m_itemList) {
            if (!item.isEmpty()) {
                if (item.m_group->isExpanding()) {
                    // group可扩展
                    ++canExpandingCount;
                }
            }
        }
        // 计算可扩展的宽度
        if (canExpandingCount > 0) {
            expandWidth = (pageWidth - totalWidth) / canExpandingCount;
        } else {
            expandWidth = 0;
        }
    }
    totalWidth = 0;   // total重新计算
    x += m_XBase;

    // 先按照sizeHint设置所有的尺寸
    for (RibbonPageItem &item : m_itemList) {
        if (item.isEmpty()) {
            // 如果是hide就直接跳过
            if (item.m_separator) {
                // group hide分割线也要hide
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
            // 可扩展，就把group扩展到最大
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
    // 判断滚动按钮是否显示
    if (totalWidth > pageWidth) {
        // 超过总长度，需要显示滚动按钮
        if (m_XBase == 0) {
            // 已经移动到最左，需要可以向右移动
            m_isRightScrollBtnShow = true;
            m_isLeftScrollBtnShow = false;
        } else if (m_XBase <= (pageWidth - totalWidth)) {
            // 已经移动到最右，需要可以向左移动
            m_isRightScrollBtnShow = false;
            m_isLeftScrollBtnShow = true;
        } else {
            // 移动到中间两边都可以动
            m_isRightScrollBtnShow = true;
            m_isLeftScrollBtnShow = true;
        }
    } else {
        // 说明 totalWidth 小于 pageWidth
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

    // 两个滚动按钮的位置永远不变
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
    // 不在上面那里进行show和hide因为这会触发RibbonGroupLayout的重绘，导致循环绘制，非常影响效率
    for (QWidget *w : qAsConst(showWidgets)) {
        w->show();
    }
    for (QWidget *w : qAsConst(hideWidgets)) {
        w->hide();
    }
}

void RibbonPagePrivate::doWheelEvent(QWheelEvent *event)
{
    int pageWidth = pageContentSize().width();
    int totalWidth = m_totalWidth;

    if (totalWidth > pageWidth) {
        // 这个时候滚动有效
        QPoint numPixels = event->pixelDelta();
        QPoint numDegrees = event->angleDelta() / 8;

        // Linux GNOME 平台上测试，event->pixelDelta() 和 event->angleDelta() 取值都为 (0, +/-120)
        // Windows 10 平台上测试，event->pixelDelta() 和 event->angleDelta() 取值分别为 (0, 0) 和 (0, +/-120)
        int scrollpix = 0;
        if (!numPixels.isNull()) {
            // Linux GNOME 平台上，一次 scrollpix 为 30
            scrollpix = numPixels.y() / 4;
        } else if (!numDegrees.isNull()) {
            // Windows 10 平台上, 一次 scrollpix 为 15
            scrollpix = numDegrees.y();
        }
        if (scrollpix > 0) {   // 当滚轮向上滑，RibbonPage向左走
            int tmp = m_XBase - scrollpix;
            if (tmp < (pageWidth - totalWidth)) {
                tmp = pageWidth - totalWidth;
            }
            m_XBase = tmp;
        } else {                             // 当滚轮向下滑，RibbonPage向右走
            int tmp = m_XBase - scrollpix;   // 此时numDegrees为负数
            if (tmp > 0) {
                tmp = 0;
            }
            m_XBase = tmp;
        }
        updateItemGeometry();
    } else {
        // 这时候无需处理事件，把滚动事件上发让父级也能接收
        event->ignore();
        m_XBase = 0;
    }
}

void RibbonPagePrivate::onLeftScrollButtonClicked()
{
    int pageWidth = pageContentSize().width();
    int totalWidth = m_totalWidth;

    if (totalWidth > pageWidth) {
        // 点击左侧，相当于向右滚动一页
        //
        //    |---- pageWidth ----|
        //    |-------- totalWidth --------|
        //    `-> m_xBase (max pos is 0)
        //
        int tmp = m_XBase + pageWidth;
        if (tmp > 0) {
            tmp = 0;
        }
        m_XBase = tmp;
    } else {
        m_XBase = 0;
    }
    updateItemGeometry();
}

void RibbonPagePrivate::onRightScrollButtonClicked()
{
    int pageWidth = pageContentSize().width();
    int totalWidth = m_totalWidth;

    if (totalWidth > pageWidth) {
        // 点击右侧，相当于向左滚动一页
        //
        //             |---- pageWidth ----|
        //    |-------- totalWidth --------|
        //    `-> m_xBase (min pos is pageWidth - totalWidth)
        //
        int tmp = m_XBase - pageWidth;
        if (tmp < (pageWidth - totalWidth)) {
            tmp = pageWidth - totalWidth;   // m_XBase 最小值
        }
        m_XBase = tmp;
    } else {
        m_XBase = 0;
    }
    updateItemGeometry();
}

RibbonPage::RibbonPage(QWidget *parent)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(RibbonPage);
    Q_D(RibbonPage);
    d->init();
    setAutoFillBackground(true);
    setBackgroundBrush(Qt::white);
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
 * @brief 添加group
 *
 * @note group的所有权由RibbonPage来管理，请不要在外部对其进行销毁
 * @param title group的标题，在office/wps的三行模式下会显示在group的下方
 * @return 返回生成的@ref RibbonGroup 指针
 * @see 对RibbonGroup的其他操作，参考 @ref RibbonPage::takeGroup
 */
RibbonGroup *RibbonPage::addGroup(const QString &title)
{
    Q_D(RibbonPage);
    return d->addGroup(title);
}

/**
 * @brief 添加group
 * @param group group的所有权RibbonPage来管理
 */
void RibbonPage::addGroup(RibbonGroup *group)
{
    Q_D(RibbonPage);
    d->addGroup(group);
}

/**
 * @brief 新建一个group，并插入到index位置
 * @param title group的title
 * @param index 插入的位置，如果index超出page里group的个数，将插入到最后
 * @return 返回生成的@ref RibbonGroup 指针
 */
RibbonGroup *RibbonPage::insertGroup(int index, const QString &title)
{
    Q_D(RibbonPage);
    return d->insertGroup(index, title);
}

/**
 * @brief 通过索引找到group，如果超过索引范围，会返回Q_NULLPTR
 * @param index
 * @return 如果超过索引范围，会返回Q_NULLPTR
 */
RibbonGroup *RibbonPage::group(int index) const
{
    Q_D(const RibbonPage);
    return d->m_itemList.value(index).m_group;
}

/**
 * @brief 通过名字查找group
 * @param title
 * @return 如果有重名，只会返回第一个符合条件的
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
 * @brief 通过ObjectName查找group
 * @param objname
 * @return 如果有重名，只会返回第一个符合条件的
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
 * @brief 查找group对应的索引
 * @param group
 * @return 如果找不到，返回-1
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
 * @brief 移动一个Group从from index到to index
 * @param from 要移动group的index
 * @param to 要移动到的位置
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
 * @brief 移除Group，Page会直接回收RibbonGroup内存
 * @param group 需要移除的group
 * @note 移除后group为野指针，一般操作完建议把group指针设置为Q_NULLPTR
 *
 * 此操作等同于：
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
 * @brief 移除group
 * @param index group的索引，如果超出会返回false
 * @return 成功返回true
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
 * @brief 如果是ContextPage，此函数返回true
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
 * @brief 刷新page的布局，适用于改变ribbon的模式之后调用
 */
void RibbonPage::updateItemGeometry()
{
    Q_D(RibbonPage);
    QList<RibbonGroup *> groups = groupList();
    for (RibbonGroup *group : qAsConst(groups)) {
        group->updateItemGeometry();
    }
    d->updateItemGeometry();
}

/**
 * @brief 标记这个是上下文标签
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
