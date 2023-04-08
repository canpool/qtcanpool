/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/

#ifndef QTC_GLOBAL_H
#define QTC_GLOBAL_H

#include <QtCore/qglobal.h>

#define QTC_QUOTE_(x) #x
#define QTC_QUOTE(x) QTC_QUOTE_(x)

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

#define QTC_DECLARE_PRIVATE(Class) \
    Class##Private *qtc_d_ptr; \
    Class##Private* qtc_d() { return qtc_d_ptr; } \
    const Class##Private* qtc_d() const { return qtc_d_ptr; } \
    friend class Class##Private;

#define QTC_DECLARE_PUBLIC(Class) \
    Class *qtc_q_ptr; \
    inline void setPublic(Class *ptr) { qtc_q_ptr = ptr; } \
    Class* qtc_q() { return qtc_q_ptr; } \
    const Class* qtc_q() const { return qtc_q_ptr; } \
    friend class Class;

#define QTC_INIT_PRIVATE(Class) \
    qtc_d_ptr = new Class##Private(); \
    qtc_d_ptr->setPublic(this);

#define QTC_FINI_PRIVATE() \
    delete qtc_d_ptr; \
    qtc_d_ptr = Q_NULL;

#define QTC_INIT_EX_PRIVATE(Class, __d_ptr) \
    qtc_d_ptr = reinterpret_cast<Class##Private *>(__d_ptr); \
    qtc_d_ptr->setPublic(this);

#define QTC_DECLARE_EX_PRIVATE(Class) \
    inline Class##Private* qtc_d() \
    { return reinterpret_cast<Class##Private *>(qtc_d_ptr); } \
    inline const Class##Private* qtc_d() const \
    { return reinterpret_cast<const Class##Private *>(qtc_d_ptr); } \
    friend class Class##Private;

#define QTC_DECLARE_EX_PUBLIC(Class) \
    Class* qtc_q() \
    { return static_cast<Class *>(qtc_q_ptr); } \
    const Class* qtc_q() const \
    { return static_cast<Class *>(qtc_q_ptr); } \
    friend class Class;

#define QTC_D(Class) Class##Private * const d = qtc_d();
#define QTC_Q(Class) Class * const q = qtc_q();

#endif // QTC_GLOBAL_H
