/***************************************************************************
 **
 **  Copyright (C) 2018-2021 MaMinJie <canpool@163.com>
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
#ifndef QCANPOOL_GLOBAL_H
#define QCANPOOL_GLOBAL_H

#include <QtCore/qglobal.h>

#if !defined(QCANPOOL_LIBRARY_DISABLE)
#if defined(QCANPOOL_LIBRARY)
#  define QCANPOOL_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define QCANPOOL_SHARED_EXPORT Q_DECL_IMPORT
#endif
#else
#  define QCANPOOL_SHARED_EXPORT
#endif

#if !defined(QCANPOOL_NAMESPACE_DISABLE)
#define QCANPOOL_NAMESPACE  QCanpool
#endif

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
