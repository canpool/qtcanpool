/***************************************************************************
 **
 **  Copyright (C) 2019-2021 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **           https://gitee.com/icanpool
 **
 **  GNU Lesser General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU Lesser
 **  General Public License version 3 as published by the Free Software
 **  Foundation and appearing in the file LICENSE.LGPL3 included in the
 **  packaging of this file. Please review the following information to
 **  ensure the GNU Lesser General Public License version 3 requirements
 **  will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
 **
 **  GNU General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU
 **  General Public License version 2.0 or (at your option) the GNU General
 **  Public license version 3 or any later version approved by the KDE Free
 **  Qt Foundation. The licenses are as published by the Free Software
 **  Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
 **  included in the packaging of this file. Please review the following
 **  information to ensure the GNU General Public License requirements will
 **  be met: https://www.gnu.org/licenses/gpl-2.0.html and
 **  https://www.gnu.org/licenses/gpl-3.0.html.
 **
****************************************************************************/
#ifndef QLITE_GLOBAL_H
#define QLITE_GLOBAL_H

#include <QtCore/qglobal.h>

#if !defined(QLITE_LIBRARY_DISABLE)
#if defined(QLITE_LIBRARY)
#  define QLITE_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define QLITE_SHARED_EXPORT Q_DECL_IMPORT
#endif
#else
#  define QLITE_SHARED_EXPORT
#endif

#if !defined(QLITE_NAMESPACE_DISABLE)
#define QLITE_NAMESPACE QLite
#endif

#if !defined(QLITE_NAMESPACE)
#define QLITE_BEGIN_NAMESPACE
#define QLITE_END_NAMESPACE
#define QLITE_USE_NAMESPACE
#define QLITE_PREPEND_NAMESPACE(name) name
#else
#define QLITE_BEGIN_NAMESPACE namespace QLITE_NAMESPACE {
#define QLITE_END_NAMESPACE }
#define QLITE_USE_NAMESPACE using namespace QLITE_NAMESPACE;
#define QLITE_PREPEND_NAMESPACE(name) QLITE_NAMESPACE::name
#endif // QLITE_NAMESPACE

QLITE_BEGIN_NAMESPACE
// nothing
QLITE_END_NAMESPACE

#endif // QLITE_GLOBAL_H
