/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockpanel.h"
#include "dockcontainer.h"
#include "dockwindow.h"
#include "docktitlebar.h"

#include <QBoxLayout>

QX_DOCK_BEGIN_NAMESPACE

/**
 * Internal dock area layout mimics stack layout but only inserts the current
 * widget into the internal QLayout object.
 * \warning Only the current widget has a parent. All other widgets
 * do not have a parent. That means, a widget that is in this layout may
 * return nullptr for its parent() function if it is not the current widget.
 */
class DockAreaLayout
{
private:
    QBoxLayout *m_parentLayout;
    QList<QPointer<QWidget>> m_widgets;
    int m_currentIndex = -1;
    QWidget *m_currentWidget = nullptr;
public:
    DockAreaLayout(QBoxLayout *parentLayout) : m_parentLayout(parentLayout)
    {
    }

    int count() const
    {
        return m_widgets.count();
    }

    void insertWidget(int index, QWidget *w)
    {
        w->setParent(nullptr);
        if (index < 0) {
            index = m_widgets.count();
        }
        m_widgets.insert(index, w);
        if (m_currentIndex < 0) {
            setCurrentIndex(index);
        } else {
            if (index <= m_currentIndex) {
                ++m_currentIndex;
            }
        }
    }

    void removeWidget(QWidget *w)
    {
        if (currentWidget() == w) {
            auto layoutItem = m_parentLayout->takeAt(1);
            if (layoutItem) {
                layoutItem->widget()->setParent(nullptr);
            }
            delete layoutItem;
            m_currentWidget = nullptr;
            m_currentIndex = -1;
        } else if (indexOf(w) < m_currentIndex) {
            --m_currentIndex;
        }
        m_widgets.removeOne(w);
    }

    /**
     * Returns the current selected widget
     */
    QWidget *currentWidget() const
    {
        return m_currentWidget;
    }

    void setCurrentIndex(int index)
    {
        QWidget *prev = currentWidget();
        QWidget *next = widget(index);
        if (!next || (next == prev && !m_currentWidget)) {
            return;
        }

        bool reenableUpdates = false;
        QWidget *parent = m_parentLayout->parentWidget();

        if (parent && parent->updatesEnabled()) {
            reenableUpdates = true;
            parent->setUpdatesEnabled(false);
        }

        auto layoutItem = m_parentLayout->takeAt(1);
        if (layoutItem) {
            layoutItem->widget()->setParent(nullptr);
        }
        delete layoutItem;

        m_parentLayout->addWidget(next);
        if (prev) {
            prev->hide();
        }
        m_currentIndex = index;
        m_currentWidget = next;

        if (reenableUpdates) {
            parent->setUpdatesEnabled(true);
        }
    }

    int currentIndex() const
    {
        return m_currentIndex;
    }

    bool isEmpty() const
    {
        return m_widgets.empty();
    }

    int indexOf(QWidget *w) const
    {
        return m_widgets.indexOf(w);
    }

    QWidget *widget(int index) const
    {
        return (index < m_widgets.size()) ? m_widgets.at(index) : nullptr;
    }

    QRect geometry() const
    {
        return m_widgets.empty() ? QRect() : currentWidget()->geometry();
    }
};

class DockPanelPrivate
{
public:
    QX_DECLARE_PUBLIC(DockPanel)
public:
    DockPanelPrivate();
    ~DockPanelPrivate();
    void init();
public:
    DockWindow *m_window = nullptr;
    QBoxLayout *m_layout = nullptr;
    DockAreaLayout *m_contentsLayout = nullptr;
    DockTitleBar *m_titleBar = nullptr;
};

DockPanelPrivate::DockPanelPrivate()
{
}

DockPanelPrivate::~DockPanelPrivate()
{
    delete m_contentsLayout;
}

void DockPanelPrivate::init()
{
    Q_Q(DockPanel);

    m_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    q->setLayout(m_layout);

    m_titleBar = new DockTitleBar(q);
    m_layout->addWidget(m_titleBar);

    m_contentsLayout = new DockAreaLayout(m_layout);
}

DockPanel::DockPanel(DockWindow *window, DockContainer *parent)
    : QWidget{parent}
{
    QX_INIT_PRIVATE(DockPanel)

    Q_D(DockPanel);
    d->m_window = window;
    d->init();
}

DockPanel::~DockPanel()
{
    QX_FINI_PRIVATE()
}

DockContainer *DockPanel::dockContainer() const
{
    return nullptr;
}

void DockPanel::addDockWidget(DockWidget *w)
{
    Q_D(DockPanel);
    insertDockWidget(d->m_contentsLayout->count(), w);
}

void DockPanel::insertDockWidget(int index, DockWidget *w, bool activate)
{
    Q_D(DockPanel);
    if (index < 0 || index > d->m_contentsLayout->count()) {
        index = d->m_contentsLayout->count();
    }
    d->m_contentsLayout->insertWidget(index, w);
}

QX_DOCK_END_NAMESPACE
