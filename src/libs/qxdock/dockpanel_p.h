/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "dockpanel.h"
#include "dockcontainer.h"
#include "dockwindow.h"
#include "docktitlebar.h"
#include "dockutils.h"
#include "docksplitter.h"
#include "docktabbar.h"
#include "docktab.h"
#include "dockmanager.h"
#include "dockautohidecontainer.h"
#include "docktitlebar_p.h"
#include "docklabel.h"
#include "dockfloatingcontainer.h"
#include "dockstatereader.h"

#include <QBoxLayout>
#include <QXmlStreamWriter>
#include <QPointer>
#include <QDebug>

QX_DOCK_BEGIN_NAMESPACE

inline const char *const INDEX_PROPERTY = "index";

inline bool isAutoHideFeatureEnabled()
{
    return DockManager::testAutoHideConfigFlag(DockManager::AutoHideFeatureEnabled);
}

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
    DockAreaLayout(QBoxLayout *parentLayout)
        : m_parentLayout(parentLayout)
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

inline const Qx::DockWidgetAreas s_defaultAllowedAreas = Qx::AllDockAreas;

class DockPanelPrivate
{
public:
    QX_DECLARE_PUBLIC(DockPanel)
public:
    DockPanelPrivate();
    ~DockPanelPrivate();
    void init();

    DockTabBar *tabBar() const;
    void updateTitleBarButtonStates();
    void updateTitleBarButtonVisibility(bool isTopLevel);
    void updateMinimumSizeHint();
public:
    DockWindow *m_window = nullptr;
    QBoxLayout *m_layout = nullptr;
    DockAreaLayout *m_contentsLayout = nullptr;
    DockTitleBar *m_titleBar = nullptr;
    DockPanel::DockAreaFlags m_flags{DockPanel::DefaultFlags};
    bool m_updateTitleBarButtons = false;
    Qx::DockWidgetAreas m_allowedAreas = s_defaultAllowedAreas;
    DockAutoHideContainer *m_autoHideContainer = nullptr;
    QSize m_minSizeHint;
};

QX_DOCK_END_NAMESPACE
