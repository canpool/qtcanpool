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

#ifndef QTPROJECT_GUI_PCH_H
#define QTPROJECT_GUI_PCH_H

#include "qtproject_pch.h"

#if defined __cplusplus

#include <QEvent>
#include <QTimer>
#include <QApplication>
#include <QBitmap>
#include <QCursor>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QDesktopWidget>
#endif
#include <QImage>
#include <QLayout>
#include <QPainter>
#include <QPixmap>
#include <QStyle>
#include <QWidget>

#endif

#endif // QTPROJECT_GUI_PCH_H
