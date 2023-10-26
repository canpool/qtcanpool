/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
**/
#pragma once

#include <QtCore/qglobal.h>

#define QRN_QUOTE_(x) #x
#define QRN_QUOTE(x) QRN_QUOTE_(x)

#ifndef Q_NULL
#ifdef Q_COMPILER_NULLPTR
#define Q_NULL    nullptr
#else
#define Q_NULL    NULL
#endif // Q_COMPILER_NULLPTR
#endif // Q_NULL

#define Q_MAXINT INT_MAX
#define Q_DELETE_AND_NULL(x) delete x; x = Q_NULL;

#ifndef Q_DECL_NOTHROW
#define Q_DECL_NOTHROW throw()
#endif

#define QRN_DECLARE_PRIVATE(Class) \
    Class##Private *d_ptr; \
    Q_DECLARE_PRIVATE(Class)

#define QRN_DECLARE_PUBLIC(Class) \
    Class *q_ptr; \
    inline void setPublic(Class *ptr) { q_ptr = ptr; } \
    Q_DECLARE_PUBLIC(Class)

#define QRN_INIT_PRIVATE(Class) \
    d_ptr = new Class##Private(); \
    d_func()->setPublic(this);

#define QRN_FINI_PRIVATE() \
    delete d_ptr; d_ptr = Q_NULL;

#define QRN_SET_PRIVATE(Dptr) \
    d_ptr = Dptr; \
    d_func()->setPublic(this);


#define QRIBBON_NAMESPACE QRibbon

#if !defined(QRIBBON_NAMESPACE)
#  define QRIBBON_BEGIN_NAMESPACE
#  define QRIBBON_END_NAMESPACE
#  define QRIBBON_USE_NAMESPACE
#  define QRIBBON_PREPEND_NAMESPACE(name) name
#else
#  define QRIBBON_BEGIN_NAMESPACE namespace QRIBBON_NAMESPACE {
#  define QRIBBON_END_NAMESPACE }
#  define QRIBBON_USE_NAMESPACE using namespace QRIBBON_NAMESPACE;
#  define QRIBBON_PREPEND_NAMESPACE(name) QRIBBON_NAMESPACE::name
#endif


QRIBBON_BEGIN_NAMESPACE

#ifndef QRIBBON_LIBRARY_STATIC
#ifdef  QRIBBON_LIBRARY
#define QRIBBON_EXPORT Q_DECL_EXPORT
#else
#define QRIBBON_EXPORT Q_DECL_IMPORT
#endif
#else
#define QRIBBON_EXPORT
#endif

QRIBBON_END_NAMESPACE
