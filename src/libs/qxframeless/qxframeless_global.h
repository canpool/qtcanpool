/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
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

#if !defined(QX_FRAMELESS_NAMESPACE_DISABLE)
#define QX_FRAMELESS_NAMESPACE  QxFrameless
#endif

#if !defined(QX_FRAMELESS_NAMESPACE)
#define QX_FRAMELESS_BEGIN_NAMESPACE
#define QX_FRAMELESS_END_NAMESPACE
#define QX_FRAMELESS_USE_NAMESPACE
#define QX_FRAMELESS_PREPEND_NAMESPACE(name) name
#else
#define QX_FRAMELESS_BEGIN_NAMESPACE namespace QX_FRAMELESS_NAMESPACE {
#define QX_FRAMELESS_END_NAMESPACE }
#define QX_FRAMELESS_USE_NAMESPACE using namespace QX_FRAMELESS_NAMESPACE;
#define QX_FRAMELESS_PREPEND_NAMESPACE(name) QX_FRAMELESS_NAMESPACE::name
#endif // QX_FRAMELESS_NAMESPACE

#ifndef QX_FRAMELESS_LIBRARY_STATIC
#ifdef  QX_FRAMELESS_LIBRARY
#define QX_FRAMELESS_EXPORT Q_DECL_EXPORT
#else
#define QX_FRAMELESS_EXPORT Q_DECL_IMPORT
#endif // QX_FRAMELESS_LIBRARY
#else  // QX_FRAMELESS_LIBRARY_STATIC
#define QX_FRAMELESS_EXPORT
#endif // QX_FRAMELESS_LIBRARY_STATIC

#define QX_FRAMELESS_VERSION_MAJOR 0
#define QX_FRAMELESS_VERSION_MINOR 1
#define QX_FRAMELESS_VERSION_PATCH 0
/*
   QX_FRAMELESS_VERSION is (major << 16) + (minor << 8) + patch.
   can be used like #if (QX_FRAMELESS_VERSION >= QT_VERSION_CHECK(0, 5, 3))
*/
#define QX_FRAMELESS_VERSION \
    QT_VERSION_CHECK(QX_FRAMELESS_VERSION_MAJOR, QX_FRAMELESS_VERSION_MINOR, QX_FRAMELESS_VERSION_PATCH)

#define QX_FRAMELESS_VERSION_STR \
    QT_STRINGIFY(QX_VERSION_JOIN(QX_FRAMELESS_VERSION_MAJOR, QX_FRAMELESS_VERSION_MINOR, QX_FRAMELESS_VERSION_PATCH))


#ifndef QX_FRAMELESS_DISABLE_DEPRECATED_BEFORE
#define QX_FRAMELESS_DISABLE_DEPRECATED_BEFORE QT_VERSION_CHECK(0, 1, 0)
#endif

/*
 QX_FRAMELESS_DEPRECATED_SINCE(major, minor) evaluates as true if the QxFrameless version is greater than
 the deprecation point specified.

 Use it to specify from which version of QxFrameless a function or class has been deprecated

 Example:
     #if QX_FRAMELESS_DEPRECATED_SINCE(0,6)
         QT_DEPRECATED void deprecatedFunction(); // function deprecated since QxFrameless 0.6
     #endif
 */
#ifdef QT_DEPRECATED
#define QX_FRAMELESS_DEPRECATED_SINCE(major, minor) \
    (QT_VERSION_CHECK(major, minor, 0) > QX_FRAMELESS_DISABLE_DEPRECATED_BEFORE)
#else
#define QX_FRAMELESS_DEPRECATED_SINCE(major, minor) 0
#endif
