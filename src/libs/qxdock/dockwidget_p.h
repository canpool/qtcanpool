/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "dockwidget.h"
#include "dockwindow.h"
#include "dockpanel.h"
#include "docktab.h"
#include "dockmanager.h"
#include "dockutils.h"
#include "docksplitter.h"
#include "dockfloatingcontainer.h"
#include "docksidetab.h"
#include "dockautohidecontainer.h"

#include <QPointer>
#include <QBoxLayout>
#include <QAction>
#include <QAbstractScrollArea>
#include <QScrollArea>
#include <QToolBar>
#include <QXmlStreamWriter>

QX_DOCK_BEGIN_NAMESPACE

class DockWidgetPrivate
{
public:
    struct WidgetFactory {
        DockWidget::FactoryFunc createWidget;
        DockWidget::WidgetInsertMode insertMode;
    };

    QX_DECLARE_PUBLIC(DockWidget)
public:
    DockWidgetPrivate();
    void init();
    void showDockWidget();
    void hideDockWidget();
    void updateParentDockPanel();

    void closeAutoHideDockWidgetsIfNeeded();

    void setupToolBar();
    void setupScrollArea();
    bool createWidgetFromFactory();
    void setToolBarStyleFromDockWindow();
public:
    DockWindow *m_window = nullptr;
    QPointer<DockPanel> m_panel = nullptr;
    QBoxLayout *m_layout = nullptr;
    DockTab *m_tab = nullptr;
    DockWidget::DockWidgetFeatures m_features = DockWidget::DefaultDockWidgetFeatures;
    QList<QAction *> m_titleBarActions;
    QAction *m_toggleViewAction = nullptr;
    bool m_closed = false;
    bool m_isFloatingTopLevel = false;
    DockWidget::MinimumSizeHintMode m_minimumSizeHintMode = DockWidget::MinimumSizeHintFromDockWidget;
    QWidget *m_widget = nullptr;
    QScrollArea *m_scrollArea = nullptr;
    QPointer<DockSideTab> m_sideTab;
    DockWidget::ToolBarStyleSource m_toolBarStyleSource = DockWidget::ToolBarStyleFromDockWindow;
    Qt::ToolButtonStyle m_toolBarStyleDocked = Qt::ToolButtonIconOnly;
    Qt::ToolButtonStyle m_toolBarStyleFloating = Qt::ToolButtonTextUnderIcon;
    QSize m_toolBarIconSizeDocked = QSize(16, 16);
    QSize m_toolBarIconSizeFloating = QSize(24, 24);
    QToolBar *m_toolBar = nullptr;
    WidgetFactory *m_factory = nullptr;
};

QX_DOCK_END_NAMESPACE
