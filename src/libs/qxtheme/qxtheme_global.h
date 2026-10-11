/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXTHEME_GLOBAL_H
#define QXTHEME_GLOBAL_H

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

/*!
 * The d-pointer helpers are shared with qxcore: whichever of the two headers
 * is included first wins, and the second one keeps the definitions it finds.
 */
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

#if !defined(QX_THEME_NAMESPACE_DISABLE)
#define QX_THEME_NAMESPACE QxTheme
#endif

#if !defined(QX_THEME_NAMESPACE)
#define QX_THEME_BEGIN_NAMESPACE
#define QX_THEME_END_NAMESPACE
#define QX_THEME_USE_NAMESPACE
#define QX_THEME_PREPEND_NAMESPACE(name) name
#else
#define QX_THEME_BEGIN_NAMESPACE                                                                                       \
    namespace QX_THEME_NAMESPACE                                                                                       \
    {
#define QX_THEME_END_NAMESPACE           }
#define QX_THEME_USE_NAMESPACE           using namespace QX_THEME_NAMESPACE;
#define QX_THEME_PREPEND_NAMESPACE(name) QX_THEME_NAMESPACE::name
#endif   // QX_THEME_NAMESPACE

#ifndef QX_THEME_LIBRARY_STATIC
#ifdef QX_THEME_LIBRARY
#define QX_THEME_EXPORT Q_DECL_EXPORT
#else
#define QX_THEME_EXPORT Q_DECL_IMPORT
#endif   // QX_THEME_LIBRARY
#else    // QX_THEME_LIBRARY_STATIC
#define QX_THEME_EXPORT
#endif   // QX_THEME_LIBRARY_STATIC

#define QX_THEME_VERSION_MAJOR 0
#define QX_THEME_VERSION_MINOR 1
#define QX_THEME_VERSION_PATCH 0
/*
   QX_THEME_VERSION is (major << 16) + (minor << 8) + patch.
   can be used like #if (QX_THEME_VERSION >= QT_VERSION_CHECK(0, 5, 3))
*/
#define QX_THEME_VERSION       QT_VERSION_CHECK(QX_THEME_VERSION_MAJOR, QX_THEME_VERSION_MINOR, QX_THEME_VERSION_PATCH)

#define QX_THEME_VERSION_STR                                                                                           \
    QT_STRINGIFY(QX_VERSION_JOIN(QX_THEME_VERSION_MAJOR, QX_THEME_VERSION_MINOR, QX_THEME_VERSION_PATCH))

#ifndef QX_THEME_DISABLE_DEPRECATED_BEFORE
// Every API deprecated so far stays visible. Raising this is the only way an old
// API disappears, and that is what makes the gate below usable: the default used
// to be QX_THEME_VERSION (the version being built), so the gate answered false
// for every deprecation that had already happened - (0,1,0) > (0,1,0) is false -
// and flipped back to false again with every minor bump.
#define QX_THEME_DISABLE_DEPRECATED_BEFORE QT_VERSION_CHECK(0, 0, 0)
#endif

/*
 QX_THEME_DEPRECATED_SINCE(major, minor) is true while an API deprecated in QxTheme
 major.minor is still part of the library, that is while its deprecation point is
 newer than QX_THEME_DISABLE_DEPRECATED_BEFORE. It does not depend on the version
 being built.

 That makes it the buffer between "deprecated" and "gone" (K26): the declaration
 is guarded by it, the warning itself comes from QT_DEPRECATED / QT_DEPRECATED_X,
 and raising QX_THEME_DISABLE_DEPRECATED_BEFORE to the version that removes the API
 is what compiles the declaration out.

 Example:
     #if QX_THEME_DEPRECATED_SINCE(0,2)
         QT_DEPRECATED void deprecatedFunction(); // deprecated in QxTheme 0.2
     #endif
 */
#ifdef QT_DEPRECATED
#define QX_THEME_DEPRECATED_SINCE(major, minor) (QT_VERSION_CHECK(major, minor, 0) > QX_THEME_DISABLE_DEPRECATED_BEFORE)
#else
#define QX_THEME_DEPRECATED_SINCE(major, minor) 0
#endif

#endif   // QXTHEME_GLOBAL_H
