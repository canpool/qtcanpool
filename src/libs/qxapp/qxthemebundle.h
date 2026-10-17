/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxapp_global.h"

#include "qxtheme/qxtheme.h"
#include "qxtheme/qxthememanager.h"

#include <QtCore/QString>

QX_APP_BEGIN_NAMESPACE

/*!
 * Reads the style sheet at \a fileName from the application's resource tree
 * (\c{:/themes/<fileName>}) and returns it as a QxThemeInfo named \a id,
 * displayed as \a name, drawing on a dark background when \a dark is true.
 *
 * An application that bundles its themes as \c{:/themes/*.css} reuses this
 * instead of writing its own file reader; registerBundledThemes() is the
 * one-call version for the canonical four-family set.
 */
QX_APP_EXPORT QxTheme::QxThemeInfo readBundledTheme(const QString &id, const QString &name, bool dark,
                                                    const QString &fileName);

/*!
 * Registers the four bundled theme families - soft, classic, flat and slate -
 * each in its light and dark variant, reading their style sheets from
 * \c{:/themes/<family>_<light|dark>.css}. The ids and file names follow the
 * canonical set in \c{res/themes.qrc}, so an application that ships those
 * files gets all eight themes from this single call, made once before the
 * first setTheme().
 *
 * A registration whose file is missing is simply skipped: the engine ignores
 * a QxThemeInfo without an id, so a half-populated resource tree never throws.
 */
QX_APP_EXPORT void registerBundledThemes(QxTheme::QxThemeManager *engine);

QX_APP_END_NAMESPACE
