/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
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

#define QX_FINI_PRIVATE()                                                                                              \
    delete d_ptr;                                                                                                      \
    d_ptr = Q_NULLPTR;

#endif   // QX_DECLARE_PRIVATE

#if !defined(QX_PLUGIN_NAMESPACE_DISABLE)
#define QX_PLUGIN_NAMESPACE QxPlugin
#endif

#if !defined(QX_PLUGIN_NAMESPACE)
#define QX_PLUGIN_BEGIN_NAMESPACE
#define QX_PLUGIN_END_NAMESPACE
#define QX_PLUGIN_USE_NAMESPACE
#define QX_PLUGIN_PREPEND_NAMESPACE(name) name
#else
#define QX_PLUGIN_BEGIN_NAMESPACE                                                                                      \
    namespace QX_PLUGIN_NAMESPACE                                                                                      \
    {
#define QX_PLUGIN_END_NAMESPACE           }
#define QX_PLUGIN_USE_NAMESPACE           using namespace QX_PLUGIN_NAMESPACE;
#define QX_PLUGIN_PREPEND_NAMESPACE(name) QX_PLUGIN_NAMESPACE::name
#endif   // QX_PLUGIN_NAMESPACE

#ifndef QX_PLUGIN_LIBRARY_STATIC
#ifdef QX_PLUGIN_LIBRARY
#define QX_PLUGIN_EXPORT Q_DECL_EXPORT
#else
#define QX_PLUGIN_EXPORT Q_DECL_IMPORT
#endif   // QX_PLUGIN_LIBRARY
#else    // QX_PLUGIN_LIBRARY_STATIC
#define QX_PLUGIN_EXPORT
#endif   // QX_PLUGIN_LIBRARY_STATIC

#define QX_PLUGIN_VERSION_MAJOR 0
#define QX_PLUGIN_VERSION_MINOR 1
#define QX_PLUGIN_VERSION_PATCH 0
/*
   QX_PLUGIN_VERSION is (major << 16) + (minor << 8) + patch.
   can be used like #if (QX_PLUGIN_VERSION >= QT_VERSION_CHECK(0, 5, 3))
*/
#define QX_PLUGIN_VERSION       QT_VERSION_CHECK(QX_PLUGIN_VERSION_MAJOR, QX_PLUGIN_VERSION_MINOR, QX_PLUGIN_VERSION_PATCH)

#define QX_PLUGIN_VERSION_STR                                                                                          \
    QT_STRINGIFY(QX_VERSION_JOIN(QX_PLUGIN_VERSION_MAJOR, QX_PLUGIN_VERSION_MINOR, QX_PLUGIN_VERSION_PATCH))

/*
   The Qt plugin IID every QxPlugin implementation must carry in its
   Q_PLUGIN_METADATA, so QPluginLoader can recognise it as one of ours and the
   manager can reject anything that is not.
*/
#ifndef QX_PLUGIN_IID
#define QX_PLUGIN_IID "org.qtcanpool.QtCanpool.QxPlugin/1.0"
#endif   // QX_PLUGIN_IID

#ifndef QX_PLUGIN_DISABLE_DEPRECATED_BEFORE
#define QX_PLUGIN_DISABLE_DEPRECATED_BEFORE QX_PLUGIN_VERSION
#endif

/*
 QX_PLUGIN_DEPRECATED_SINCE(major, minor) evaluates as true if the QxPlugin version is greater than
 the deprecation point specified.

 Use it to specify from which version of QxPlugin a function or class has been deprecated

 Example:
     #if QX_PLUGIN_DEPRECATED_SINCE(0,3)
         QT_DEPRECATED void deprecatedFunction(); // function deprecated since QxPlugin 0.3
     #endif
 */
#ifdef QT_DEPRECATED
#define QX_PLUGIN_DEPRECATED_SINCE(major, minor)                                                                       \
    (QT_VERSION_CHECK(major, minor, 0) > QX_PLUGIN_DISABLE_DEPRECATED_BEFORE)
#else
#define QX_PLUGIN_DEPRECATED_SINCE(major, minor) 0
#endif
