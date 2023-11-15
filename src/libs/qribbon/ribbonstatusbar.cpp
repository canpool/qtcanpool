/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include "ribbonstatusbar.h"
#include "ribbonbar_p.h"
#include "ribbonbutton_p.h"
#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QEvent>
#include <QLayout>
#include <QPainter>
#include <QSizeGrip>
#include <QStyle>
#include <QStyleOption>
#include <QToolButton>
#include <QWidgetAction>
#include <qevent.h>

QRIBBON_USE_NAMESPACE

static QToolButton *findButtonByAction(const QWidget *parent, const QAction *action)
{
    const QObjectList &childrenGroup = parent->children();
    for (int i = 0, count = childrenGroup.count(); count > i; i++) {
        if (QToolButton *child = qobject_cast<QToolButton *>(childrenGroup.at(i))) {
            if (child->defaultAction() == action)
                return child;
        }
    }
    return Q_NULL;
}

static int findIndexByAction(const QWidget *parent, const QAction *action)
{
    const QObjectList &childrenGroup = parent->children();
    for (int i = 0, count = childrenGroup.count(); count > i; i++) {
        if (QToolButton *child = qobject_cast<QToolButton *>(childrenGroup.at(i))) {
            if (child->defaultAction() == action)
                return i;
        }
    }
    return -1;
}

static QLayout *qrnfindLayout(QLayout *lay, QWidget *wd)
{
    if (!lay)
        return Q_NULL;

    for (int i = 0, count = lay->count(); count > i; i++) {
        QLayoutItem *item = lay->itemAt(i);

        QWidget *widget = item->widget();
        if (widget == wd)
            return lay;

        if (QLayout *itemLayout = item->layout())
            return qrnfindLayout(itemLayout, wd);
    }

    return Q_NULL;
}

QRIBBON_BEGIN_NAMESPACE

class StatusButton : public QToolButton
{
public:
    StatusButton(QWidget *parent = Q_NULL);
public:
    virtual QSize sizeHint() const;
public:
    void initStyleOptionButton(QStyleOptionToolButton &option) const;
protected:
    virtual bool event(QEvent *event);
    virtual void paintEvent(QPaintEvent *);
};

class StatusSeparator : public QToolButton
{
public:
    StatusSeparator(QWidget *parent = Q_NULL);
public:
    virtual QSize sizeHint() const;
protected:
    void initStyleOption(QStyleOption *option) const;
protected:
    virtual void paintEvent(QPaintEvent *);
};

class RibbonStatusBarSwitchGroupPrivate : public QObject
{
    QRN_DECLARE_PUBLIC(RibbonStatusBarSwitchGroup)
public:
    explicit RibbonStatusBarSwitchGroupPrivate();
public:
    void init();
    StatusButton *createButton(QAction *action) const;
    void updateIndexesButtons();
public:
    QHBoxLayout *m_layout;
};

class RibbonStatusBarPrivate : public QObject
{
public:
    QRN_DECLARE_PUBLIC(RibbonStatusBar)
public:
    explicit RibbonStatusBarPrivate();
public:
    QLayout *findLayout();
    void updateLayout();
    QRect childrenPermanentRect() const;
    StatusButton *createButton(QAction *action) const;
    StatusSeparator *createSeparator(QAction *action) const;
public:
    bool m_dirty;
    bool m_addPermanentAction;
    int m_indexGroupAction;
    QSizeGrip *m_sizeGripWidget;
    QList<QWidget *> m_widgets;
    QList<QWidget *> m_permanentWidgets;
};

QRIBBON_END_NAMESPACE

/* StatusButton */
StatusButton::StatusButton(QWidget *parent) : QToolButton(parent) {}

QSize StatusButton::sizeHint() const { return QToolButton::sizeHint(); }

void StatusButton::initStyleOptionButton(QStyleOptionToolButton &opt) const
{
    QToolButton::initStyleOption(&opt);

    int actualArea = opt.rect.height() * opt.rect.width();

    int index = -1;
    int curArea = 0;
    QList<QSize> lSz = opt.icon.availableSizes();
    for (int i = 0, count = lSz.count(); count > i; i++) {
        QSize curSz = lSz[i];
        int area = curSz.height() * curSz.width();
        if (actualArea > area) {
            if (area > curArea)
                index = i;
            curArea = area;
        }
    }
    opt.iconSize = index != -1 ? opt.icon.actualSize(lSz[index]) : opt.iconSize;
}

bool StatusButton::event(QEvent *event)
{
    if (event->type() == QEvent::Enter) {
        enterEvent(event);
        return true;
    } else
        return QToolButton::event(event);
}

void StatusButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOptionToolButton opt;
    initStyleOptionButton(opt);
    style()->drawComplexControl(QStyle::CC_ToolButton, &opt, &p, this);
}

/* StatusSeparator */
StatusSeparator::StatusSeparator(QWidget *parent) : QToolButton(parent) {}

QSize StatusSeparator::sizeHint() const
{
    QSize sz = QToolButton::sizeHint();
    QStyleOption opt;
    initStyleOption(&opt);
    const int extent = style()->pixelMetric(QStyle::PM_ToolBarSeparatorExtent, &opt, parentWidget());
    sz.setWidth(extent);
    return sz;
}

void StatusSeparator::initStyleOption(QStyleOption *option) const
{
    option->initFrom(this);
    option->state |= QStyle::State_Horizontal;
}

void StatusSeparator::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOption opt;
    initStyleOption(&opt);
    style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &opt, &p, parentWidget());
}

/* RibbonStatusBarSwitchGroupPrivate */
RibbonStatusBarSwitchGroupPrivate::RibbonStatusBarSwitchGroupPrivate() {}

void RibbonStatusBarSwitchGroupPrivate::init()
{
    Q_Q(RibbonStatusBarSwitchGroup);
    m_layout = new QHBoxLayout();
    m_layout->setMargin(0);
    m_layout->setSpacing(0);

    //    m_layout->addWidget(m_slider);
    q->setLayout(m_layout);
}

StatusButton *RibbonStatusBarSwitchGroupPrivate::createButton(QAction *action) const
{
    StatusButton *button = new StatusButton();
    button->setAutoRaise(true);
    button->setFocusPolicy(Qt::NoFocus);
    button->setDefaultAction(action);
    return button;
}

void RibbonStatusBarSwitchGroupPrivate::updateIndexesButtons()
{
    Q_Q(RibbonStatusBarSwitchGroup);
    int index = 0;
    int switchCount = 0;
    const QObjectList &children = q->children();
    for (int i = 0, count = children.count(); count > i; i++) {
        if (QWidget *child = qobject_cast<QWidget *>(children.at(i))) {
            switchCount++;
            child->setProperty(_qrn_Index_Group, index);
            index++;
        }
    }
    q->setProperty(_qrn_Switch_Count, switchCount);
}

/* RibbonStatusBarSwitchGroup */
RibbonStatusBarSwitchGroup::RibbonStatusBarSwitchGroup()
{
    QRN_INIT_PRIVATE(RibbonStatusBarSwitchGroup);
    Q_D(RibbonStatusBarSwitchGroup);
    d->init();
}

RibbonStatusBarSwitchGroup::~RibbonStatusBarSwitchGroup() { QRN_FINI_PRIVATE(); }

void RibbonStatusBarSwitchGroup::actionEvent(QActionEvent *event)
{
    Q_D(RibbonStatusBarSwitchGroup);

    QAction *action = event->action();
    if (event->type() == QEvent::ActionAdded) {
        Q_ASSERT_X(::findButtonByAction(this, action) == Q_NULL, "RibbonStatusBarSwitchGroup",
                   "widgets cannot be inserted multiple times");

        StatusButton *newButton = d->createButton(action);

        int index = layout()->count();
        if (event->before()) {
            int ind = ::findIndexByAction(this, event->before());
            if (ind != -1)
                index = ind;
            Q_ASSERT_X(index != -1, "RibbonStatusBarSwitchGroup::insertAction", "internal error");
        }

        d->m_layout->insertWidget(index, newButton);
        d->updateIndexesButtons();
    } else if (event->type() == QEvent::ActionRemoved) {
        if (QToolButton *button = ::findButtonByAction(this, action)) {
            d->m_layout->removeWidget(button);
            delete button;
            d->updateIndexesButtons();
        }
    }
}

void RibbonStatusBarSwitchGroup::clear()
{
    QList<QAction *> acts = actions();
    for (int i = acts.size() - 1; i >= 0; --i)
        removeAction(acts.at(i));
}

/* RibbonStatusBarPrivate */
RibbonStatusBarPrivate::RibbonStatusBarPrivate()
{
    m_dirty = false;
    m_addPermanentAction = false;
    m_indexGroupAction = 0;
    m_sizeGripWidget = Q_NULL;
}

QLayout *RibbonStatusBarPrivate::findLayout()
{
    Q_Q(RibbonStatusBar);

    QLayout *l = q->layout();
    const QObjectList &children = q->children();
    for (int i = 0; i < children.size(); ++i) {
        if (QWidget *child = qobject_cast<QWidget *>(children.at(i))) {
            if (QLayout *internalLayout = ::qrnfindLayout(l, child))
                return internalLayout;
            else if (QSizeGrip *grip = qobject_cast<QSizeGrip *>(child))
                m_sizeGripWidget = grip;
        }
    }
    return Q_NULL;
}

void RibbonStatusBarPrivate::updateLayout()
{
    if (m_dirty) {
        if (QLayout *layoutFind = findLayout()) {
            layoutFind->setSpacing(0);
            layoutFind->setMargin(0);
            int permanentIndex = 0;
            for (int i = layoutFind->count() - 1; i >= 0; --i) {
                QLayoutItem *item = layoutFind->itemAt(i);
                if (dynamic_cast<QSpacerItem *>(item))
                    permanentIndex++;

                if (QWidget *widget = item->widget()) {
                    if (permanentIndex != 2)
                        m_permanentWidgets.append(widget);
                    else
                        m_widgets.append(widget);
                }
            }
        }
        m_dirty = false;
    }
}

QRect RibbonStatusBarPrivate::childrenPermanentRect() const
{
    QRect permanentRect;
    for (int i = 0, count = m_permanentWidgets.count(); count > i; i++)
        permanentRect |= m_permanentWidgets.at(i)->geometry();

    if (m_sizeGripWidget && m_sizeGripWidget->isVisible())
        permanentRect |= m_sizeGripWidget->geometry();
    return permanentRect;
}

StatusButton *RibbonStatusBarPrivate::createButton(QAction *action) const
{
    StatusButton *button = new StatusButton();
    button->setAutoRaise(true);
    button->setFocusPolicy(Qt::NoFocus);
    button->setDefaultAction(action);
    return button;
}

StatusSeparator *RibbonStatusBarPrivate::createSeparator(QAction *action) const
{
    Q_UNUSED(action);
    StatusSeparator *sep = new StatusSeparator();
    return sep;
}

/* RibbonStatusBar */
RibbonStatusBar::RibbonStatusBar(QWidget *parent) : QStatusBar(parent) { QRN_INIT_PRIVATE(RibbonStatusBar); }

RibbonStatusBar::~RibbonStatusBar() { QRN_FINI_PRIVATE(); }

QAction *RibbonStatusBar::addAction(const QString &text)
{
    QAction *action = new QAction(text, this);
    QStatusBar::addAction(action);
    return action;
}

QAction *RibbonStatusBar::addSeparator()
{
    QAction *action = new QAction(this);
    action->setSeparator(true);
    QStatusBar::addAction(action);
    return action;
}

void RibbonStatusBar::addPermanentAction(QAction *action)
{
    Q_D(RibbonStatusBar);
    d->m_addPermanentAction = true;
    QStatusBar::addAction(action);
    d->m_addPermanentAction = false;
}

QRect RibbonStatusBar::childrenPermanentRect() const
{
    Q_D(const RibbonStatusBar);
    return d->childrenPermanentRect();
}

bool RibbonStatusBar::event(QEvent *event)
{
    Q_D(RibbonStatusBar);
    bool result = QStatusBar::event(event);

    if (!d->m_dirty && QEvent::ChildRemoved == event->type()) {
        d->m_dirty = true;
        d->m_permanentWidgets.clear();
        d->m_widgets.clear();
        d->m_sizeGripWidget = Q_NULL;
    }

    return result;
}

void RibbonStatusBar::paintEvent(QPaintEvent *event) { QStatusBar::paintEvent(event); }

void RibbonStatusBar::actionEvent(QActionEvent *event)
{
    Q_D(RibbonStatusBar);
    QAction *action = event->action();
    if (event->type() == QEvent::ActionAdded) {
        Q_ASSERT_X(::findButtonByAction(this, action) == Q_NULL, "RibbonStatusBar",
                   "widgets cannot be inserted multiple times");

        int index = d->m_addPermanentAction ? d->m_permanentWidgets.count() : d->m_widgets.count();
        if (event->before()) {
            int ind = ::findIndexByAction(this, event->before());
            if (ind != -1)
                index = ind;
            Q_ASSERT_X(index != -1, "RibbonStatusBar::insertAction", "internal error");
        }

        if (action->isSeparator()) {
            StatusSeparator *sep = d->createSeparator(action);
            if (!d->m_addPermanentAction)
                insertWidget(index, sep);
            else
                insertPermanentWidget(index, sep);
        } else {
            StatusButton *button = d->createButton(action);
            if (!d->m_addPermanentAction)
                insertWidget(index, button);
            else
                insertPermanentWidget(index, button);
        }
    } else if (event->type() == QEvent::ActionRemoved) {
        if (QToolButton *button = ::findButtonByAction(this, action)) {
            removeWidget(button);
            delete button;
            d->updateLayout();
        }
    }
}

void RibbonStatusBar::resizeEvent(QResizeEvent *event)
{
    Q_D(RibbonStatusBar);
    QStatusBar::resizeEvent(event);
    d->updateLayout();
}
