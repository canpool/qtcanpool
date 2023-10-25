/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxglobal.h"

#if !defined(QX_RIBBON_NAMESPACE_DISABLE)
#define QX_RIBBON_NAMESPACE  QxRibbon
#endif

#if !defined(QX_RIBBON_NAMESPACE)
#define QX_RIBBON_BEGIN_NAMESPACE
#define QX_RIBBON_END_NAMESPACE
#define QX_RIBBON_USE_NAMESPACE
#define QX_RIBBON_PREPEND_NAMESPACE(name) name
#else
#define QX_RIBBON_BEGIN_NAMESPACE namespace QX_RIBBON_NAMESPACE {
#define QX_RIBBON_END_NAMESPACE }
#define QX_RIBBON_USE_NAMESPACE using namespace QX_RIBBON_NAMESPACE;
#define QX_RIBBON_PREPEND_NAMESPACE(name) QX_RIBBON_NAMESPACE::name
#endif // QX_RIBBON_NAMESPACE

#ifndef QX_RIBBON_LIBRARY_STATIC
#ifdef  QX_RIBBON_LIBRARY
#define QX_RIBBON_EXPORT Q_DECL_EXPORT
#else
#define QX_RIBBON_EXPORT Q_DECL_IMPORT
#endif // QX_RIBBON_LIBRARY
#else  // QX_RIBBON_LIBRARY_STATIC
#define QX_RIBBON_EXPORT
#endif // QX_RIBBON_LIBRARY_STATIC

#define QX_RIBBON_VERSION_MAJOR 0
#define QX_RIBBON_VERSION_MINOR 7
#define QX_RIBBON_VERSION_PATCH 0
/*
   QX_RIBBON_VERSION is (major << 16) + (minor << 8) + patch.
   can be used like #if (QX_RIBBON_VERSION >= QT_VERSION_CHECK(0, 5, 3))
*/
#define QX_RIBBON_VERSION \
    QT_VERSION_CHECK(QX_RIBBON_VERSION_MAJOR, QX_RIBBON_VERSION_MINOR, QX_RIBBON_VERSION_PATCH)

#define QX_RIBBON_VERSION_STR \
    QT_STRINGIFY(QX_VERSION_JOIN(QX_RIBBON_VERSION_MAJOR, QX_RIBBON_VERSION_MINOR, QX_RIBBON_VERSION_PATCH))


#ifndef QX_RIBBON_DISABLE_DEPRECATED_BEFORE
#define QX_RIBBON_DISABLE_DEPRECATED_BEFORE QT_VERSION_CHECK(0, 6, 0)
#endif

/*
 QX_RIBBON_DEPRECATED_SINCE(major, minor) evaluates as true if the QxRibbon version is greater than
 the deprecation point specified.

 Use it to specify from which version of QxRibbon a function or class has been deprecated

 Example:
     #if QX_RIBBON_DEPRECATED_SINCE(0,6)
         QT_DEPRECATED void deprecatedFunction(); // function deprecated since QxRibbon 0.6
     #endif
 */
#ifdef QT_DEPRECATED
#define QX_RIBBON_DEPRECATED_SINCE(major, minor) \
    (QT_VERSION_CHECK(major, minor, 0) > QX_RIBBON_DISABLE_DEPRECATED_BEFORE)
#else
#define QX_RIBBON_DEPRECATED_SINCE(major, minor) 0
#endif
