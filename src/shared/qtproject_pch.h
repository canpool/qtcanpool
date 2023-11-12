/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * Copyright (C) 2016 The Qt Company Ltd.
 * SPDX-License-Identifier: GPL-3.0
 **/

/*
 * This is a precompiled header file for use in Xcode / Mac GCC /
 * GCC >= 3.4 / VC to greatly speed the building of Qt Creator.
 *
 * The define below is checked in source code. Do not replace with #pragma once!
 */

#ifndef QTPROJECT_PCH_H
#define QTPROJECT_PCH_H

#if defined __cplusplus
#include <QtGlobal>

#ifdef Q_WS_WIN
# define _POSIX_
# include <limits.h>
# undef _POSIX_
#endif

#include <QCoreApplication>
#include <QList>
#include <QVariant>
#include <QObject>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QTextCodec>
#include <QPointer>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QDebug>

#include <stdlib.h>
#endif

#endif // QTPROJECT_PCH_H
