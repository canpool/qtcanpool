/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "ribbonpagecontext.h"
#include "ribbonmanager.h"

#include <QDebug>
#include <QList>
#include <QVariant>

QX_RIBBON_BEGIN_NAMESPACE

/* RibbonPageData */
class RibbonPageData
{
public:
    RibbonPage *page;
};

/* RibbonPageContextPrivate */
class RibbonPageContextPrivate
{
    QX_DECLARE_PUBLIC(RibbonPageContext)
public:
    QList<RibbonPageData> pageDataList;
    QVariant contextID;
    QColor contextColor;
    QString contextTitle;
};

RibbonPageContext::RibbonPageContext(QWidget *parent)
    : QObject(parent)
{
    QX_INIT_PRIVATE(RibbonPageContext)
}

RibbonPageContext::~RibbonPageContext()
{
    QX_FINI_PRIVATE()
}

RibbonPage *RibbonPageContext::addPage(const QString &title)
{
    Q_D(RibbonPageContext);
    RibbonPageData pageData;
    RibbonPage *page = new RibbonPage(parentWidget());

    page->markIsPageContext(true);
    page->setWindowTitle(title);
    pageData.page = page;
    d->pageDataList.append(pageData);
    page->installEventFilter(this);
    emit pageAdded(page);

    return page;
}

int RibbonPageContext::pageCount() const
{
    Q_D(const RibbonPageContext);
    return d->pageDataList.count();
}

void RibbonPageContext::setId(const QVariant &id)
{
    Q_D(RibbonPageContext);
    d->contextID = id;
}

QVariant RibbonPageContext::id() const
{
    Q_D(const RibbonPageContext);
    return d->contextID;
}

void RibbonPageContext::setContextColor(const QColor color)
{
    Q_D(RibbonPageContext);
    d->contextColor = color;
}

QColor RibbonPageContext::contextColor() const
{
    Q_D(const RibbonPageContext);
    return d->contextColor;
}

QWidget *RibbonPageContext::parentWidget() const
{
    return qobject_cast<QWidget *>(parent());
}

bool RibbonPageContext::eventFilter(QObject *watched, QEvent *e)
{
    if (Q_NULLPTR == watched) {
        return false;
    }
    switch (e->type()) {
    case QEvent::Close: {
        RibbonPage *page = qobject_cast<RibbonPage *>(watched);
        if (page) {
#ifdef QX_RIBBON_DEBUG_HELP_DRAW
            qDebug() << " -----------> close event";
#endif
            takePage(page);
        }
    } break;

    default:
        break;
    }
    return false;
}

/**
 * @brief 获取上下文标签的标题
 * @return
 */
QString RibbonPageContext::contextTitle() const
{
    Q_D(const RibbonPageContext);
    return d->contextTitle;
}

/**
 * @brief 设置上下文标签的标题，标题仅在 office 模式下显示 @ref RibbonBar::RibbonStyle
 * @param title
 */
void RibbonPageContext::setContextTitle(const QString &title)
{
    Q_D(RibbonPageContext);
    d->contextTitle = title;
}

/**
 * @brief 获取对应的tab页
 * @param index
 * @return
 */
RibbonPage *RibbonPageContext::page(int index)
{
    Q_D(RibbonPageContext);
    if (index < 0 || index >= d->pageDataList.size()) {
        return Q_NULLPTR;
    }
    return d->pageDataList[index].page;
}

/**
 * @brief 获取所有的RibbonPage*
 * @return
 */
QList<RibbonPage *> RibbonPageContext::pageList() const
{
    Q_D(const RibbonPageContext);
    QList<RibbonPage *> res;

    for (const RibbonPageData &data : d->pageDataList) {
        res.append(data.page);
    }
    return res;
}

/**
 * @brief 移除这个page，这时RibbonPageContext不再管理这个page
 * @param page
 * @return 成功移除返回true
 */
bool RibbonPageContext::takePage(RibbonPage *page)
{
    Q_D(RibbonPageContext);
    for (int i = 0; i < d->pageDataList.size(); ++i) {
        if (d->pageDataList[i].page == page) {
            d->pageDataList.takeAt(i);
            return true;
        }
    }
    return false;
}

QX_RIBBON_END_NAMESPACE
