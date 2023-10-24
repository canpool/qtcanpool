/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include <QtCore/qglobal.h>

#ifndef Q_NULLPTR
#define Q_NULLPTR 0
#endif

#ifndef Q_DECL_OVERRIDE
#define Q_DECL_OVERRIDE override
#endif

#define QX_VERSION_JOIN2(major, minor, patch) major##.##minor##.##patch
#define QX_VERSION_JOIN(major, minor, patch) QX_VERSION_JOIN2(major, minor, patch)

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

#if !defined(QX_NAMESPACE_DISABLE)
#define QX_NAMESPACE  Qx
#endif

#if !defined(QX_NAMESPACE)
#define QX_BEGIN_NAMESPACE
#define QX_END_NAMESPACE
#define QX_USE_NAMESPACE
#define QX_PREPEND_NAMESPACE(name) name
#else
#define QX_BEGIN_NAMESPACE namespace QX_NAMESPACE {
#define QX_END_NAMESPACE }
#define QX_USE_NAMESPACE using namespace QX_NAMESPACE;
#define QX_PREPEND_NAMESPACE(name) QX_NAMESPACE::name
#endif // QX_NAMESPACE

#ifndef QX_WIDGET_LIBRARY_STATIC
#ifdef  QX_WIDGET_LIBRARY
#define QX_WIDGET_EXPORT Q_DECL_EXPORT
#else
#define QX_WIDGET_EXPORT Q_DECL_IMPORT
#endif // QX_WIDGET_LIBRARY
#else  // QX_WIDGET_LIBRARY_STATIC
#define QX_WIDGET_EXPORT
#endif // QX_WIDGET_LIBRARY_STATIC

#define QX_VERSION_MAJOR 0
#define QX_VERSION_MINOR 6
#define QX_VERSION_PATCH 0
/*
   QX_VERSION is (major << 16) + (minor << 8) + patch.
   can be used like #if (QX_VERSION >= QT_VERSION_CHECK(0, 5, 3))
*/
#define QX_VERSION \
    QT_VERSION_CHECK(QX_VERSION_MAJOR, QX_VERSION_MINOR, QX_VERSION_PATCH)

#define QX_VERSION_STR \
    QT_STRINGIFY(QX_VERSION_JOIN(QX_VERSION_MAJOR, QX_VERSION_MINOR, QX_VERSION_PATCH))


#ifndef QX_DISABLE_DEPRECATED_BEFORE
#define QX_DISABLE_DEPRECATED_BEFORE QT_VERSION_CHECK(0, 6, 0)
#endif

/*
 QX_DEPRECATED_SINCE(major, minor) evaluates as true if the QxRibbon version is greater than
 the deprecation point specified.

 Use it to specify from which version of QxRibbon a function or class has been deprecated

 Example:
     #if QX_DEPRECATED_SINCE(0,6)
         QT_DEPRECATED void deprecatedFunction(); // function deprecated since QxRibbon 0.6
     #endif

 */
#ifdef QT_DEPRECATED
#define QX_DEPRECATED_SINCE(major, minor) (QT_VERSION_CHECK(major, minor, 0) > QX_DISABLE_DEPRECATED_BEFORE)
#else
#define QX_DEPRECATED_SINCE(major, minor) 0
#endif

QX_BEGIN_NAMESPACE
// nothing
QX_END_NAMESPACE
