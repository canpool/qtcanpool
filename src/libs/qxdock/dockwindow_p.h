/**
 * Copyleft (C) 2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include "dockwindow.h"
#include "dockpanel.h"
#include "dockwidget.h"
#include "dockfloatingcontainer.h"
#include "dockoverlay.h"
#include "dockfocuscontroller.h"
#include "dockmanager.h"
#include "dockautohidecontainer.h"
#include "dockdrag.h"
#include "dockutils.h"
#include "dockstatereader.h"
#include "docksplitter.h"

#include <QPointer>

QX_DOCK_BEGIN_NAMESPACE

// Accessibility (B2): register the dock's QAccessible factory. Idempotent.
void installDockAccessibility();

class DockWindowPrivate
{
public:
    QX_DECLARE_PUBLIC(DockWindow)
public:
    DockWindowPrivate();

    void init();

    void mapDockWidget(DockWidget *w);

    bool checkFormat(const QByteArray &state, int version);
    bool restoreStateFromXml(const QByteArray &state, int version, bool testing = internal::Restore);
    bool restoreContainer(int index, DockStateReader &stream, bool testing);
    bool restoreState(const QByteArray &state, int version);

    void restoreDockWidgetsOpenState();
    void restoreDockAreasIndices();
    void emitTopLevelEvents();
    void hideFloatingWidgets();
    void markDockWidgetsDirty();
public:
    QList<DockContainer *> m_containers;
    QMap<QString, DockWidget *> m_dockWidgetsMap;
    QList<QPointer<DockFloatingContainer>> m_floatingContainers;
    QList<QPointer<DockFloatingContainer>> m_hiddenFloatingContainers;
    QVector<DockFloatingContainer *> m_uninitializedFloatingWidgets;
    QMap<QString, QByteArray> m_perspectives;
    DockWidget *m_centralWidget = nullptr;
    DockOverlay *m_containerOverlay;
    DockOverlay *m_panelOverlay;
    DockFocusController *m_focusController = nullptr;
    bool m_isLeavingMinimized = false;
    Qt::ToolButtonStyle m_toolBarStyleDocked = Qt::ToolButtonIconOnly;
    Qt::ToolButtonStyle m_toolBarStyleFloating = Qt::ToolButtonTextUnderIcon;
    QSize m_toolBarIconSizeDocked = QSize(16, 16);
    QSize m_toolBarIconSizeFloating = QSize(24, 24);
    DockWidget::DockWidgetFeatures m_lockedDockWidgetFeatures = DockWidget::NoDockWidgetFeatures;
    bool m_restoringState = false;
};

QX_DOCK_END_NAMESPACE
