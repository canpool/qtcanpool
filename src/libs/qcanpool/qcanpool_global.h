/***************************************************************************
 **
 **  Copyright (C) 2018 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **
 **  This program is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  This program is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with this program.  If not, see http://www.gnu.org/licenses/.
 **
****************************************************************************/
#ifndef QCANPOOL_GLOBAL_H
#define QCANPOOL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QCANPOOL_LIBRARY)
#  define QCANPOOL_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define QCANPOOL_SHARED_EXPORT Q_DECL_IMPORT
#endif

#define QCANPOOL_NAMESPACE  QCanpool

#if !defined(QCANPOOL_NAMESPACE)
#define QCANPOOL_BEGIN_NAMESPACE
#define QCANPOOL_END_NAMESPACE
#define QCANPOOL_USE_NAMESPACE
#define QCANPOOL_PREPEND_NAMESPACE(name) name
#else
#define QCANPOOL_BEGIN_NAMESPACE namespace QCANPOOL_NAMESPACE {
#define QCANPOOL_END_NAMESPACE }
#define QCANPOOL_USE_NAMESPACE using namespace QCANPOOL_NAMESPACE;
#define QCANPOOL_PREPEND_NAMESPACE(name) QCANPOOL_NAMESPACE::name
#endif // QCANPOOL_NAMESPACE

QCANPOOL_BEGIN_NAMESPACE
// nothing
QCANPOOL_END_NAMESPACE

#endif // QCANPOOL_GLOBAL_H
