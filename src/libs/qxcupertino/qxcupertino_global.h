/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXCUPERTINO_GLOBAL_H
#define QXCUPERTINO_GLOBAL_H

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

#if !defined(QX_CUPERTINO_NAMESPACE_DISABLE)
#define QX_CUPERTINO_NAMESPACE QxCupertino
#endif

#if !defined(QX_CUPERTINO_NAMESPACE)
#define QX_CUPERTINO_BEGIN_NAMESPACE
#define QX_CUPERTINO_END_NAMESPACE
#define QX_CUPERTINO_USE_NAMESPACE
#define QX_CUPERTINO_PREPEND_NAMESPACE(name) name
#else
#define QX_CUPERTINO_BEGIN_NAMESPACE                                                                                   \
    namespace QX_CUPERTINO_NAMESPACE                                                                                   \
    {
#define QX_CUPERTINO_END_NAMESPACE           }
#define QX_CUPERTINO_USE_NAMESPACE           using namespace QX_CUPERTINO_NAMESPACE;
#define QX_CUPERTINO_PREPEND_NAMESPACE(name) QX_CUPERTINO_NAMESPACE::name
#endif   // QX_CUPERTINO_NAMESPACE

#ifndef QX_CUPERTINO_LIBRARY_STATIC
#ifdef QX_CUPERTINO_LIBRARY
#define QX_CUPERTINO_EXPORT Q_DECL_EXPORT
#else
#define QX_CUPERTINO_EXPORT Q_DECL_IMPORT
#endif   // QX_CUPERTINO_LIBRARY
#else    // QX_CUPERTINO_LIBRARY_STATIC
#define QX_CUPERTINO_EXPORT
#endif   // QX_CUPERTINO_LIBRARY_STATIC

#define QX_CUPERTINO_VERSION_MAJOR 0
#define QX_CUPERTINO_VERSION_MINOR 1
#define QX_CUPERTINO_VERSION_PATCH 0
/*
   QX_CUPERTINO_VERSION is (major << 16) + (minor << 8) + patch.
   can be used like #if (QX_CUPERTINO_VERSION >= QT_VERSION_CHECK(0, 5, 3))
*/
#define QX_CUPERTINO_VERSION                                                                                           \
    QT_VERSION_CHECK(QX_CUPERTINO_VERSION_MAJOR, QX_CUPERTINO_VERSION_MINOR, QX_CUPERTINO_VERSION_PATCH)

#define QX_CUPERTINO_VERSION_STR                                                                                       \
    QT_STRINGIFY(QX_VERSION_JOIN(QX_CUPERTINO_VERSION_MAJOR, QX_CUPERTINO_VERSION_MINOR, QX_CUPERTINO_VERSION_PATCH))

#ifndef QX_CUPERTINO_DISABLE_DEPRECATED_BEFORE
// Every API deprecated so far stays visible. Raising this is the only way an old
// API disappears, and that is what makes the gate below usable: with the default
// at the version being built, the gate answers false for every deprecation that
// has already happened - (0,2,0) > (0,2,0) is false - and flips back with every
// bump.
#define QX_CUPERTINO_DISABLE_DEPRECATED_BEFORE QT_VERSION_CHECK(0, 0, 0)
#endif

/*
 QX_CUPERTINO_DEPRECATED_SINCE(major, minor) is true while an API deprecated in QxCupertino
 major.minor is still part of the library, that is while its deprecation point is
 newer than QX_CUPERTINO_DISABLE_DEPRECATED_BEFORE. It does not depend on the version
 being built.

 That makes it the buffer between "deprecated" and "gone" (K26): the declaration
 is guarded by it, the warning itself comes from QT_DEPRECATED / QT_DEPRECATED_X,
 and raising QX_CUPERTINO_DISABLE_DEPRECATED_BEFORE to the version that removes the API
 is what compiles the declaration out.

 Example:
     #if QX_CUPERTINO_DEPRECATED_SINCE(0,2)
         QT_DEPRECATED void deprecatedFunction(); // deprecated in QxCupertino 0.2
     #endif
 */
#ifdef QT_DEPRECATED
#define QX_CUPERTINO_DEPRECATED_SINCE(major, minor)                                                                    \
    (QT_VERSION_CHECK(major, minor, 0) > QX_CUPERTINO_DISABLE_DEPRECATED_BEFORE)
#else
#define QX_CUPERTINO_DEPRECATED_SINCE(major, minor) 0
#endif

#endif   // QXCUPERTINO_GLOBAL_H
