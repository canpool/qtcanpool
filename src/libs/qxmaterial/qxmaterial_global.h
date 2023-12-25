/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
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

#if !defined(QX_MATERIAL_NAMESPACE_DISABLE)
#define QX_MATERIAL_NAMESPACE  QxMaterial
#endif

#if !defined(QX_MATERIAL_NAMESPACE)
#define QX_MATERIAL_BEGIN_NAMESPACE
#define QX_MATERIAL_END_NAMESPACE
#define QX_MATERIAL_USE_NAMESPACE
#define QX_MATERIAL_PREPEND_NAMESPACE(name) name
#else
#define QX_MATERIAL_BEGIN_NAMESPACE namespace QX_MATERIAL_NAMESPACE {
#define QX_MATERIAL_END_NAMESPACE }
#define QX_MATERIAL_USE_NAMESPACE using namespace QX_MATERIAL_NAMESPACE;
#define QX_MATERIAL_PREPEND_NAMESPACE(name) QX_MATERIAL_NAMESPACE::name
#endif // QX_MATERIAL_NAMESPACE

#ifndef QX_MATERIAL_LIBRARY_STATIC
#ifdef  QX_MATERIAL_LIBRARY
#define QX_MATERIAL_EXPORT Q_DECL_EXPORT
#else
#define QX_MATERIAL_EXPORT Q_DECL_IMPORT
#endif // QX_MATERIAL_LIBRARY
#else  // QX_MATERIAL_LIBRARY_STATIC
#define QX_MATERIAL_EXPORT
#endif // QX_MATERIAL_LIBRARY_STATIC

#define QX_MATERIAL_VERSION_MAJOR 0
#define QX_MATERIAL_VERSION_MINOR 1
#define QX_MATERIAL_VERSION_PATCH 0
/*
   QX_MATERIAL_VERSION is (major << 16) + (minor << 8) + patch.
   can be used like #if (QX_MATERIAL_VERSION >= QT_VERSION_CHECK(0, 5, 3))
*/
#define QX_MATERIAL_VERSION \
    QT_VERSION_CHECK(QX_MATERIAL_VERSION_MAJOR, QX_MATERIAL_VERSION_MINOR, QX_MATERIAL_VERSION_PATCH)

#define QX_MATERIAL_VERSION_STR \
    QT_STRINGIFY(QX_VERSION_JOIN(QX_MATERIAL_VERSION_MAJOR, QX_MATERIAL_VERSION_MINOR, QX_MATERIAL_VERSION_PATCH))


#ifndef QX_MATERIAL_DISABLE_DEPRECATED_BEFORE
#define QX_MATERIAL_DISABLE_DEPRECATED_BEFORE QT_VERSION_CHECK(0, 1, 0)
#endif

/*
 QX_MATERIAL_DEPRECATED_SINCE(major, minor) evaluates as true if the QxMaterial version is greater than
 the deprecation point specified.

 Use it to specify from which version of QxMaterial a function or class has been deprecated

 Example:
     #if QX_MATERIAL_DEPRECATED_SINCE(0,6)
         QT_DEPRECATED void deprecatedFunction(); // function deprecated since QxMaterial 0.6
     #endif
 */
#ifdef QT_DEPRECATED
#define QX_MATERIAL_DEPRECATED_SINCE(major, minor) \
    (QT_VERSION_CHECK(major, minor, 0) > QX_MATERIAL_DISABLE_DEPRECATED_BEFORE)
#else
#define QX_MATERIAL_DEPRECATED_SINCE(major, minor) 0
#endif
