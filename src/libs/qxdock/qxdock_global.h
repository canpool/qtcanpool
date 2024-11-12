/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include <QtCore/qglobal.h>

#ifndef Q_NULLPTR
#define Q_NULLPTR 0
#endif // Q_NULLPTR

#ifndef Q_DECL_OVERRIDE
#define Q_DECL_OVERRIDE override
#endif // Q_DECL_OVERRIDE

#ifndef QX_VERSION_JOIN
#define QX_VERSION_JOIN2(major, minor, patch) major##.##minor##.##patch
#define QX_VERSION_JOIN(major, minor, patch) QX_VERSION_JOIN2(major, minor, patch)
#endif // QX_VERSION_JOIN

#ifndef QX_DECLARE_PRIVATE

#define QX_DECLARE_PRIVATE(Class) \
    Class##Private *d_ptr; \
    Q_DECLARE_PRIVATE(Class)

#define QX_DECLARE_PUBLIC(Class) \
    Class *q_ptr; \
    inline void setPublic(Class *ptr) { q_ptr = ptr; } \
    Q_DECLARE_PUBLIC(Class)

#define QX_INIT_PRIVATE(Class) \
    d_ptr = new Class##Private(); \
    d_ptr->setPublic(this);

#define QX_SET_PRIVATE(Dptr) \
    d_ptr = Dptr; \
    d_ptr->setPublic(this);

#define QX_FINI_PRIVATE() \
    delete d_ptr; d_ptr = Q_NULLPTR;

#endif // QX_DECLARE_PRIVATE

#if !defined(QX_DOCK_NAMESPACE_DISABLE)
#define QX_DOCK_NAMESPACE  QxDock
#endif

#if !defined(QX_DOCK_NAMESPACE)
#define QX_DOCK_BEGIN_NAMESPACE
#define QX_DOCK_END_NAMESPACE
#define QX_DOCK_USE_NAMESPACE
#define QX_DOCK_PREPEND_NAMESPACE(name) name
#else
#define QX_DOCK_BEGIN_NAMESPACE namespace QX_DOCK_NAMESPACE {
#define QX_DOCK_END_NAMESPACE }
#define QX_DOCK_USE_NAMESPACE using namespace QX_DOCK_NAMESPACE;
#define QX_DOCK_PREPEND_NAMESPACE(name) QX_DOCK_NAMESPACE::name
#endif // QX_DOCK_NAMESPACE

#ifndef QX_DOCK_LIBRARY_STATIC
#ifdef  QX_DOCK_LIBRARY
#define QX_DOCK_EXPORT Q_DECL_EXPORT
#else
#define QX_DOCK_EXPORT Q_DECL_IMPORT
#endif // QX_DOCK_LIBRARY
#else  // QX_DOCK_LIBRARY_STATIC
#define QX_DOCK_EXPORT
#endif // QX_DOCK_LIBRARY_STATIC

#define QX_DOCK_VERSION_MAJOR 0
#define QX_DOCK_VERSION_MINOR 8
#define QX_DOCK_VERSION_PATCH 1
/*
   QX_DOCK_VERSION is (major << 16) + (minor << 8) + patch.
   can be used like #if (QX_DOCK_VERSION >= QT_VERSION_CHECK(0, 5, 3))
*/
#define QX_DOCK_VERSION \
    QT_VERSION_CHECK(QX_DOCK_VERSION_MAJOR, QX_DOCK_VERSION_MINOR, QX_DOCK_VERSION_PATCH)

#define QX_DOCK_VERSION_STR \
    QT_STRINGIFY(QX_VERSION_JOIN(QX_DOCK_VERSION_MAJOR, QX_DOCK_VERSION_MINOR, QX_DOCK_VERSION_PATCH))


#ifndef QX_DOCK_DISABLE_DEPRECATED_BEFORE
#define QX_DOCK_DISABLE_DEPRECATED_BEFORE QT_VERSION_CHECK(0, 0, 1)
#endif

/*
 QX_DOCK_DEPRECATED_SINCE(major, minor) evaluates as true if the QxDock version is greater than
 the deprecation point specified.

 Use it to specify from which version of QxDock a function or class has been deprecated

 Example:
     #if QX_DOCK_DEPRECATED_SINCE(0,6)
         QT_DEPRECATED void deprecatedFunction(); // function deprecated since QxDock 0.6
     #endif
 */
#ifdef QT_DEPRECATED
#define QX_DOCK_DEPRECATED_SINCE(major, minor) \
    (QT_VERSION_CHECK(major, minor, 0) > QX_DOCK_DISABLE_DEPRECATED_BEFORE)
#else
#define QX_DOCK_DEPRECATED_SINCE(major, minor) 0
#endif

namespace Qx {

enum DockWidgetArea {
    NoDockWidgetArea = 0x00,
    LeftDockWidgetArea = 0x1,
    RightDockWidgetArea = 0x2,
    TopDockWidgetArea = 0x4,
    BottomDockWidgetArea = 0x8,
    CenterDockWidgetArea = 0x10,

    InvalidDockWidgetArea = NoDockWidgetArea,
    DockWidgetArea_Mask = 0x1f,
    AllDockWidgetAreas = DockWidgetArea_Mask,
    OuterDockAreas = TopDockWidgetArea | LeftDockWidgetArea | RightDockWidgetArea | BottomDockWidgetArea,
    AllDockAreas = OuterDockAreas | CenterDockWidgetArea
};
enum DockWidgetAreaSizes {
    NDockWidgetAreas = 5
};

Q_DECLARE_FLAGS(DockWidgetAreas, DockWidgetArea)
Q_DECLARE_OPERATORS_FOR_FLAGS(DockWidgetAreas)

enum DockTabIndex {
    DockTabDefaultInsertIndex = -1,
    DockTabInvalidIndex = -2
};

enum DockTitleBarButton {
    TitleBarButtonTabsMenu,
    TitleBarButtonUndock,
    TitleBarButtonClose,
    TitleBarButtonAutoHide,
    TitleBarButtonMinimize
};

enum DockDragState {
    DockDraggingInactive,
    DockDraggingMousePressed,
    DockDraggingTab,
    DockDraggingFloatingWidget
};

enum DockIcon {
    DockTabCloseIcon,
    DockAutoHideIcon,
    DockPanelMenuIcon,
    DockPanelUndockIcon,
    DockPanelCloseIcon,
    DockPanelMinimizeIcon
};
enum DockIconSizes {
    NDockIconSizes = 6
};

enum DockBitwiseOperator {
    DockBitwiseAnd,
    DockBitwiseOr
};

enum DockSideBarArea {
    DockSideBarTop,
    DockSideBarLeft,
    DockSideBarRight,
    DockSideBarBottom,
    DockSideBarNone
};

}
