/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
**/

#ifndef QGOODBYE_GLOBAL_H
#define QGOODBYE_GLOBAL_H

#include "qgood/qgood_global.h"

#if !defined(QGOODBYE_LIBRARY_DISABLE)
#if defined(QGOODBYE_LIBRARY)
#  define QGOODBYE_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define QGOODBYE_SHARED_EXPORT Q_DECL_IMPORT
#endif
#else
#  define QGOODBYE_SHARED_EXPORT
#endif

#endif // QGOODBYE_GLOBAL_H
