/**
 * Copyleft (C) 2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include <QtCore/qglobal.h>

#ifndef Q_NULLPTR
#define Q_NULLPTR 0
#endif   // Q_NULLPTR

#ifndef Q_DECL_OVERRIDE
#define Q_DECL_OVERRIDE override
#endif   // Q_DECL_OVERRIDE

#ifndef QX_VERSION_JOIN
#define QX_VERSION_JOIN2(major, minor, patch) major##.##minor##.##patch
#define QX_VERSION_JOIN(major, minor, patch)  QX_VERSION_JOIN2(major, minor, patch)
#endif   // QX_VERSION_JOIN

#ifndef QX_DECLARE_PRIVATE

#define QX_DECLARE_PRIVATE(Class)                                                                                      \
    Class##Private *d_ptr;                                                                                             \
    Q_DECLARE_PRIVATE(Class)

#define QX_DECLARE_PUBLIC(Class)                                                                                       \
    Class *q_ptr;                                                                                                      \
    inline void setPublic(Class *ptr)                                                                                  \
    {                                                                                                                  \
        q_ptr = ptr;                                                                                                   \
    }                                                                                                                  \
    Q_DECLARE_PUBLIC(Class)

#define QX_INIT_PRIVATE(Class)                                                                                         \
    d_ptr = new Class##Private();                                                                                      \
    d_ptr->setPublic(this);

#define QX_SET_PRIVATE(Dptr)                                                                                           \
    d_ptr = Dptr;                                                                                                      \
    d_ptr->setPublic(this);

#define QX_FINI_PRIVATE()                                                                                              \
    delete d_ptr;                                                                                                      \
    d_ptr = Q_NULLPTR;

#endif   // QX_DECLARE_PRIVATE

#if !defined(QX_APP_NAMESPACE_DISABLE)
#define QX_APP_NAMESPACE QxApp
#endif

#if !defined(QX_APP_NAMESPACE)
#define QX_APP_BEGIN_NAMESPACE
#define QX_APP_END_NAMESPACE
#define QX_APP_USE_NAMESPACE
#define QX_APP_PREPEND_NAMESPACE(name) name
#else
#define QX_APP_BEGIN_NAMESPACE                                                                                         \
    namespace QX_APP_NAMESPACE                                                                                         \
    {
#define QX_APP_END_NAMESPACE           }
#define QX_APP_USE_NAMESPACE           using namespace QX_APP_NAMESPACE;
#define QX_APP_PREPEND_NAMESPACE(name) QX_APP_NAMESPACE::name
#endif   // QX_APP_NAMESPACE

#ifndef QX_APP_LIBRARY_STATIC
#ifdef QX_APP_LIBRARY
#define QX_APP_EXPORT Q_DECL_EXPORT
#else
#define QX_APP_EXPORT Q_DECL_IMPORT
#endif   // QX_APP_LIBRARY
#else    // QX_APP_LIBRARY_STATIC
#define QX_APP_EXPORT
#endif   // QX_APP_LIBRARY_STATIC

#define QX_APP_VERSION_MAJOR 0
#define QX_APP_VERSION_MINOR 1
#define QX_APP_VERSION_PATCH 0
/*
   QX_APP_VERSION is (major << 16) + (minor << 8) + patch.
   can be used like #if (QX_APP_VERSION >= QT_VERSION_CHECK(0, 5, 3))
*/
#define QX_APP_VERSION       QT_VERSION_CHECK(QX_APP_VERSION_MAJOR, QX_APP_VERSION_MINOR, QX_APP_VERSION_PATCH)

#define QX_APP_VERSION_STR                                                                                             \
    QT_STRINGIFY(QX_VERSION_JOIN(QX_APP_VERSION_MAJOR, QX_APP_VERSION_MINOR, QX_APP_VERSION_PATCH))

#ifndef QX_APP_DISABLE_DEPRECATED_BEFORE
#define QX_APP_DISABLE_DEPRECATED_BEFORE QX_APP_VERSION
#endif

/*
 QX_APP_DEPRECATED_SINCE(major, minor) evaluates as true if the QxApp version is greater than
 the deprecation point specified.

 Use it to specify from which version of QxApp a function or class has been deprecated

 Example:
     #if QX_APP_DEPRECATED_SINCE(0,6)
         QT_DEPRECATED void deprecatedFunction(); // function deprecated since QxApp 0.6
     #endif
 */
#ifdef QT_DEPRECATED
#define QX_APP_DEPRECATED_SINCE(major, minor) (QT_VERSION_CHECK(major, minor, 0) > QX_APP_DISABLE_DEPRECATED_BEFORE)
#else
#define QX_APP_DEPRECATED_SINCE(major, minor) 0
#endif
