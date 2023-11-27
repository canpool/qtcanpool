/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
**/

#ifndef QGOOD_GLOBAL_H
#define QGOOD_GLOBAL_H

#include <QtCore/qglobal.h>

#if !defined(QGOOD_LIBRARY_DISABLE)
#if defined(QGOOD_LIBRARY)
#  define QGOOD_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define QGOOD_SHARED_EXPORT Q_DECL_IMPORT
#endif
#else
#  define QGOOD_SHARED_EXPORT
#endif

#ifndef QGOOD_WINDOW_ENABLE
#define QGOOD_WINDOW_ENABLE
#endif // QGOOD_WINDOW_ENABLE

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#ifndef QT_VERSION_QT5
#define QT_VERSION_QT5
#endif // QT_VERSION_QT5
#elif QT_VERSION >= QT_VERSION_CHECK(6, 0, 0) && QT_VERSION < QT_VERSION_CHECK(7, 0, 0)
#ifndef QT_VERSION_QT6
#define QT_VERSION_QT6
#endif // QT_VERSION_QT6
#endif

#endif // QGOOD_GLOBAL_H
