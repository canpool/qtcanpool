/**
 * Copyleft (C) 2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"

QX_DOCK_BEGIN_NAMESPACE

// Accessibility (B2): register the dock's QAccessible factory. Idempotent.
void installDockAccessibility();

QX_DOCK_END_NAMESPACE
