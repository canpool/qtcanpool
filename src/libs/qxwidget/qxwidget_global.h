/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxglobal.h"

#if !defined(QX_WIDGET_LIBRARY_DISABLE)
#if defined(QX_WIDGET_LIBRARY)
#  define QX_WIDGET_EXPORT Q_DECL_EXPORT
#else
#  define QX_WIDGET_EXPORT Q_DECL_IMPORT
#endif
#else
#  define QX_WIDGET_EXPORT
#endif

#if !defined(QX_WIDGET_NAMESPACE_DISABLE)
#define QX_WIDGET_NAMESPACE  QxWidget
#endif

#if !defined(QX_WIDGET_NAMESPACE)
#define QX_WIDGET_BEGIN_NAMESPACE
#define QX_WIDGET_END_NAMESPACE
#define QX_WIDGET_USE_NAMESPACE
#define QX_WIDGET_PREPEND_NAMESPACE(name) name
#else
#define QX_WIDGET_BEGIN_NAMESPACE namespace QX_WIDGET_NAMESPACE {
#define QX_WIDGET_END_NAMESPACE }
#define QX_WIDGET_USE_NAMESPACE using namespace QX_WIDGET_NAMESPACE;
#define QX_WIDGET_PREPEND_NAMESPACE(name) QX_WIDGET_NAMESPACE::name
#endif // QX_WIDGET_NAMESPACE

#define QX_WIDGET_VERSION_MAJOR 0
#define QX_WIDGET_VERSION_MINOR 1
#define QX_WIDGET_VERSION_PATCH 0
/*
   QX_WIDGET_VERSION is (major << 16) + (minor << 8) + patch.
   can be used like #if (QX_WIDGET_VERSION >= QT_VERSION_CHECK(0, 5, 3))
*/
#define QX_WIDGET_VERSION \
    QT_VERSION_CHECK(QX_WIDGET_VERSION_MAJOR, QX_WIDGET_VERSION_MINOR, QX_WIDGET_VERSION_PATCH)

#define QX_WIDGET_VERSION_STR \
    QT_STRINGIFY(QX_VERSION_JOIN(QX_WIDGET_VERSION_MAJOR, QX_WIDGET_VERSION_MINOR, QX_WIDGET_VERSION_PATCH))


#ifndef QX_WIDGET_DISABLE_DEPRECATED_BEFORE
#define QX_WIDGET_DISABLE_DEPRECATED_BEFORE QT_VERSION_CHECK(0, 1, 0)
#endif

/*
 QX_WIDGET_DEPRECATED_SINCE(major, minor) evaluates as true if the qxwidget version is greater than
 the deprecation point specified.

 Use it to specify from which version of qxwidget a function or class has been deprecated

 Example:
     #if QX_WIDGET_DEPRECATED_SINCE(0, 1)
         QT_DEPRECATED void deprecatedFunction(); // function deprecated since qxwidget 0.1
     #endif

 */
#ifdef QT_DEPRECATED
#define QX_WIDGET_DEPRECATED_SINCE(major, minor) \
    (QT_VERSION_CHECK(major, minor, 0) > QX_WIDGET_DISABLE_DEPRECATED_BEFORE)
#else
#define QX_WIDGET_DEPRECATED_SINCE(major, minor) 0
#endif
