/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
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

#if !defined(QX_WINDOW_NAMESPACE_DISABLE)
#define QX_WINDOW_NAMESPACE  QxWindow
#endif

#if !defined(QX_WINDOW_NAMESPACE)
#define QX_WINDOW_BEGIN_NAMESPACE
#define QX_WINDOW_END_NAMESPACE
#define QX_WINDOW_USE_NAMESPACE
#define QX_WINDOW_PREPEND_NAMESPACE(name) name
#else
#define QX_WINDOW_BEGIN_NAMESPACE namespace QX_WINDOW_NAMESPACE {
#define QX_WINDOW_END_NAMESPACE }
#define QX_WINDOW_USE_NAMESPACE using namespace QX_WINDOW_NAMESPACE;
#define QX_WINDOW_PREPEND_NAMESPACE(name) QX_WINDOW_NAMESPACE::name
#endif // QX_WINDOW_NAMESPACE

#ifndef QX_WINDOW_LIBRARY_STATIC
#ifdef  QX_WINDOW_LIBRARY
#define QX_WINDOW_EXPORT Q_DECL_EXPORT
#else
#define QX_WINDOW_EXPORT Q_DECL_IMPORT
#endif // QX_WINDOW_LIBRARY
#else  // QX_WINDOW_LIBRARY_STATIC
#define QX_WINDOW_EXPORT
#endif // QX_WINDOW_LIBRARY_STATIC

#define QX_WINDOW_VERSION_MAJOR 0
#define QX_WINDOW_VERSION_MINOR 1
#define QX_WINDOW_VERSION_PATCH 1
/*
   QX_WINDOW_VERSION is (major << 16) + (minor << 8) + patch.
   can be used like #if (QX_WINDOW_VERSION >= QT_VERSION_CHECK(0, 5, 3))
*/
#define QX_WINDOW_VERSION \
    QT_VERSION_CHECK(QX_WINDOW_VERSION_MAJOR, QX_WINDOW_VERSION_MINOR, QX_WINDOW_VERSION_PATCH)

#define QX_WINDOW_VERSION_STR \
    QT_STRINGIFY(QX_VERSION_JOIN(QX_WINDOW_VERSION_MAJOR, QX_WINDOW_VERSION_MINOR, QX_WINDOW_VERSION_PATCH))


#ifndef QX_WINDOW_DISABLE_DEPRECATED_BEFORE
#define QX_WINDOW_DISABLE_DEPRECATED_BEFORE QX_WINDOW_VERSION
#endif

/*
 QX_WINDOW_DEPRECATED_SINCE(major, minor) evaluates as true if the QxWindow version is greater than
 the deprecation point specified.

 Use it to specify from which version of QxWindow a function or class has been deprecated

 Example:
     #if QX_WINDOW_DEPRECATED_SINCE(0,6)
         QT_DEPRECATED void deprecatedFunction(); // function deprecated since QxWindow 0.6
     #endif
 */
#ifdef QT_DEPRECATED
#define QX_WINDOW_DEPRECATED_SINCE(major, minor) \
    (QT_VERSION_CHECK(major, minor, 0) > QX_WINDOW_DISABLE_DEPRECATED_BEFORE)
#else
#define QX_WINDOW_DEPRECATED_SINCE(major, minor) 0
#endif

#include <QRect>
#include <functional>

QX_WINDOW_BEGIN_NAMESPACE

// QRect func_ptr(const QSize &size)
using ScreenRectCallback = std::function<QRect(const QSize &)>;

QX_WINDOW_END_NAMESPACE
