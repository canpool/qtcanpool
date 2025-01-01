/**
 * Copyright (C) 2020-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#include "ribbonpage.h"
#include "ribbonpage_p.h"
#include "ribbongroup.h"

#include <QHBoxLayout>
#include <QToolButton>
#include <QResizeEvent>
#include <QWheelEvent>

QCANPOOL_BEGIN_NAMESPACE

#ifdef Q_OS_LINUX
static const int widthButtonScroll = 15;
#else
static const int widthButtonScroll = 12;
#endif

/* RibbonPagePrivate */
RibbonPagePrivate::RibbonPagePrivate()
    : m_title(QString("page"))
    , m_groupXBase(0)
    , m_groupsWidth(0)
{}

RibbonPagePrivate::~RibbonPagePrivate()
{}

void RibbonPagePrivate::init()
{
    m_leftScrollB = new QToolButton(q);
    m_leftScrollB->setArrowType(Qt::LeftArrow);
    m_leftScrollB->hide();
    m_rightScrollB = new QToolButton(q);
    m_rightScrollB->setArrowType(Qt::RightArrow);
    m_rightScrollB->hide();

    QObject::connect(m_leftScrollB, SIGNAL(clicked()), this, SLOT(slotLeftScrollButton()));
    QObject::connect(m_rightScrollB, SIGNAL(clicked()), this, SLOT(slotRightScrollButton()));
}

void RibbonPagePrivate::insertGroup(int index, RibbonGroup *group)
{
    if (validateGroupIndex(index)) {
        m_listGroups.insert(index, group);
    } else {
        m_listGroups.append(group);
    }
    updateLayout();
}

void RibbonPagePrivate::removeGroup(int index)
{
    if (validateGroupIndex(index)) {
        m_listGroups.removeAt(index);
        updateLayout();
    }
}

int RibbonPagePrivate::groupIndex(RibbonGroup *group)
{
    return m_listGroups.indexOf(group);
}

int RibbonPagePrivate::calculateGroupsWidth()
{
    int w = 0;
    foreach (RibbonGroup *group, m_listGroups) {
        if (group->isHidden()) {
            continue;
        }
        w += group->sizeHint().width();
    }
    return w;
}

void RibbonPagePrivate::updateLayout()
{
    int y = 0;
    int w = q->width();
    int h = q->height();
    m_groupsWidth = calculateGroupsWidth();
    if (m_groupsWidth <= w) {
        m_groupXBase = 0;
    }
    int x = m_groupXBase;

    foreach (RibbonGroup *group, m_listGroups) {
        if (group->isHidden()) {
            continue;
        }
        QSize groupSizeHint = group->sizeHint();
        group->setGeometry(QRect(x, y, groupSizeHint.width(), q->height()));
        x += groupSizeHint.width();
    }

    m_leftScrollB->setGeometry(0, 0, widthButtonScroll, h);
    m_rightScrollB->setGeometry(w - widthButtonScroll, 0, widthButtonScroll, h);
    bool scrollLeft = false, scrollRight = false;
    if (m_groupsWidth > w) {
        if (m_groupXBase == 0) {
            scrollRight = true;
        } else if (m_groupXBase <= (w - m_groupsWidth)) {
            scrollLeft = true;
        } else {
            scrollLeft = scrollRight = true;
        }
    }
    m_leftScrollB->setVisible(scrollLeft);
    m_rightScrollB->setVisible(scrollRight);
    if (scrollLeft) {
        m_leftScrollB->raise();
    }
    if (scrollRight) {
        m_rightScrollB->raise();
    }
}

void RibbonPagePrivate::doWheelEvent(QWheelEvent *event)
{
    int w = q->width();
    if (m_groupsWidth > w) {
        QPoint numPixels = event->pixelDelta();
        QPoint numDegrees = event->angleDelta() / 8;

        int scrollpix = 0;
        if (!numPixels.isNull())
            scrollpix = numPixels.x() / 4; // FIXME
        else if (!numDegrees.isNull())
            scrollpix = numDegrees.y();
        else {
        }
        if (scrollpix > 0) {
            int tmp = m_groupXBase - scrollpix;
            if (tmp < (w - m_groupsWidth)) {
                tmp = w - m_groupsWidth;
            }
            m_groupXBase = tmp;
        } else {
            int tmp = m_groupXBase - scrollpix;
            if (tmp > 0) {
                tmp = 0;
            }
            m_groupXBase = tmp;
        }
        updateLayout();
    } else {
        event->ignore();
        m_groupXBase = 0;
    }
}

void RibbonPagePrivate::slotLeftScrollButton()
{
    int w = q->width();
    if (m_groupsWidth > w) {
        int tmp = m_groupXBase + w;
        if (tmp > 0) {
            tmp = 0;
        }
        m_groupXBase = tmp;
    } else {
        m_groupXBase = 0;
    }
    updateLayout();
}

void RibbonPagePrivate::slotRightScrollButton()
{
    int w = q->width();
    if (m_groupsWidth > w) {
        int tmp = m_groupXBase - w;
        if (tmp < (w - m_groupsWidth)) {
            tmp = w - m_groupsWidth;
        }
        m_groupXBase = tmp;
    } else {
        m_groupXBase = 0;
    }
    updateLayout();
}

RibbonPage::RibbonPage(QWidget *parent)
    : QWidget(parent)
    , d(new RibbonPagePrivate)
{
    d->q = this;
    d->init();
}

RibbonPage::~RibbonPage()
{
    delete d;
}

void RibbonPage::addGroup(RibbonGroup *group)
{
    insertGroup(-1, group);
}

RibbonGroup *RibbonPage::addGroup(const QString &title)
{
    return insertGroup(-1, title);
}

void RibbonPage::insertGroup(int index, RibbonGroup *group)
{
    d->insertGroup(index, group);
}

RibbonGroup *RibbonPage::insertGroup(int index, const QString &title)
{
    RibbonGroup *group = new RibbonGroup(title, this);
    insertGroup(index, group);
    return group;
}

void RibbonPage::removeGroup(RibbonGroup *group)
{
    removeGroup(d->groupIndex(group));
}

void RibbonPage::removeGroup(int index)
{
    d->removeGroup(index);
}

void RibbonPage::clearGroups()
{
    for (int i = d->m_listGroups.count() - 1; i >= 0; i--) {
        removeGroup(i);
    }
}

QString RibbonPage::title() const
{
    return d->m_title;
}

void RibbonPage::setTitle(const QString &title)
{
    if (d->m_title == title) {
        return;
    }
    d->m_title = title;
    Q_EMIT titleChanged(d->m_title);
}

void RibbonPage::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    d->updateLayout();
}

void RibbonPage::wheelEvent(QWheelEvent *event)
{
    d->doWheelEvent(event);
}

QCANPOOL_END_NAMESPACE
