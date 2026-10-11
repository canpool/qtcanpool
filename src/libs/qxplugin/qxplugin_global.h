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
#define QX_PLUGIN_VERSION_MINOR 2
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
// Every API deprecated so far stays visible. Raising this is the only way an old
// API disappears, and that is what makes the gate below usable: the default used
// to be QX_PLUGIN_VERSION (the version being built), so the gate answered false
// for every deprecation that had already happened - (0,2,0) > (0,2,0) is false -
// and flipped back to false again with every minor bump.
#define QX_PLUGIN_DISABLE_DEPRECATED_BEFORE QT_VERSION_CHECK(0, 0, 0)
#endif

/*
 QX_PLUGIN_DEPRECATED_SINCE(major, minor) is true while an API deprecated in QxPlugin
 major.minor is still part of the library, that is while its deprecation point is
 newer than QX_PLUGIN_DISABLE_DEPRECATED_BEFORE. It does not depend on the version
 being built.

 That makes it the buffer between "deprecated" and "gone" (K26): the declaration
 is guarded by it, the warning itself comes from QT_DEPRECATED / QT_DEPRECATED_X,
 and raising QX_PLUGIN_DISABLE_DEPRECATED_BEFORE to the version that removes the API
 is what compiles the declaration out.

 Example:
     #if QX_PLUGIN_DEPRECATED_SINCE(0,2)
         QT_DEPRECATED void deprecatedFunction(); // deprecated in QxPlugin 0.2
     #endif
 */
#ifdef QT_DEPRECATED
#define QX_PLUGIN_DEPRECATED_SINCE(major, minor)                                                                       \
    (QT_VERSION_CHECK(major, minor, 0) > QX_PLUGIN_DISABLE_DEPRECATED_BEFORE)
#else
#define QX_PLUGIN_DEPRECATED_SINCE(major, minor) 0
#endif
