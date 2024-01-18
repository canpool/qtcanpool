/**
 * Copyright (C) 2018-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#ifndef QCANPOOL_H
#define QCANPOOL_H

#include <QtCore/qglobal.h>

#ifndef Q_NULLPTR
#define Q_NULLPTR 0
#endif

#ifndef Q_DECL_OVERRIDE
#define Q_DECL_OVERRIDE override
#endif

#define QCP_VERSION_JOIN2(major, minor, patch) major##.##minor##.##patch
#define QCP_VERSION_JOIN(major, minor, patch) QCP_VERSION_JOIN2(major, minor, patch)

#define QCP_DECLARE_PRIVATE(Class) \
    Class##Private *d_ptr; \
    Q_DECLARE_PRIVATE(Class)

#define QCP_DECLARE_PUBLIC(Class) \
    Class *q_ptr; \
    inline void setPublic(Class *ptr) { q_ptr = ptr; } \
    Q_DECLARE_PUBLIC(Class)

#define QCP_INIT_PRIVATE(Class) \
    d_ptr = new Class##Private(); \
    d_ptr->setPublic(this);

#define QCP_SET_PRIVATE(Dptr) \
    d_ptr = Dptr; \
    d_ptr->setPublic(this);

#define QCP_FINI_PRIVATE() \
    delete d_ptr; d_ptr = Q_NULLPTR;


#if !defined(QCANPOOL_LIBRARY_DISABLE)
#if defined(QCANPOOL_LIBRARY)
#  define QCANPOOL_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define QCANPOOL_SHARED_EXPORT Q_DECL_IMPORT
#endif
#else
#  define QCANPOOL_SHARED_EXPORT
#endif

#if !defined(QCANPOOL_NAMESPACE_DISABLE)
#define QCANPOOL_NAMESPACE  QCanpool
#endif

#if !defined(QCANPOOL_NAMESPACE)
#define QCANPOOL_BEGIN_NAMESPACE
#define QCANPOOL_END_NAMESPACE
#define QCANPOOL_USE_NAMESPACE
#define QCANPOOL_PREPEND_NAMESPACE(name) name
#else
#define QCANPOOL_BEGIN_NAMESPACE namespace QCANPOOL_NAMESPACE {
#define QCANPOOL_END_NAMESPACE }
#define QCANPOOL_USE_NAMESPACE using namespace QCANPOOL_NAMESPACE;
#define QCANPOOL_PREPEND_NAMESPACE(name) QCANPOOL_NAMESPACE::name
#endif // QCANPOOL_NAMESPACE


#define QCANPOOL_VERSION_MAJOR 2
#define QCANPOOL_VERSION_MINOR 0
#define QCANPOOL_VERSION_PATCH 1
/*
   QCANPOOL_VERSION is (major << 16) + (minor << 8) + patch.
   can be used like #if (QCANPOOL_VERSION >= QT_VERSION_CHECK(0, 5, 3))
*/
#define QCANPOOL_VERSION \
QT_VERSION_CHECK(QCANPOOL_VERSION_MAJOR, QCANPOOL_VERSION_MINOR, QCANPOOL_VERSION_PATCH)

#define QCANPOOL_VERSION_STR \
QT_STRINGIFY(QCANPOOL_VERSION_JOIN(QCANPOOL_VERSION_MAJOR, QCANPOOL_VERSION_MINOR, QCANPOOL_VERSION_PATCH))


#ifndef QCANPOOL_DISABLE_DEPRECATED_BEFORE
    #define QCANPOOL_DISABLE_DEPRECATED_BEFORE QT_VERSION_CHECK(2, 0, 0)
#endif

/*
 QCANPOOL_DEPRECATED_SINCE(major, minor) evaluates as true if the qcanpool version is greater than
 the deprecation point specified.

Use it to specify from which version of qcanpool a function or class has been deprecated

Example:
  #if QCANPOOL_DEPRECATED_SINCE(2, 0)
      QT_DEPRECATED void deprecatedFunction(); // function deprecated since qcanpool 2.0
  #endif

 */
#ifdef QT_DEPRECATED
#define QCANPOOL_DEPRECATED_SINCE(major, minor) \
    (QT_VERSION_CHECK(major, minor, 0) > QCANPOOL_DISABLE_DEPRECATED_BEFORE)
#else
#define QCANPOOL_DEPRECATED_SINCE(major, minor) 0
#endif

#endif // QCANPOOL_H
