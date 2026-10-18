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
 * An application that bundles its themes under \c{:/themes/} reuses this
 * instead of writing its own file reader; registerBundledThemes() does the
 * same for every theme the bundle ships.
 */
QX_APP_EXPORT QxTheme::QxThemeInfo readBundledTheme(const QString &id, const QString &name, bool dark,
                                                    const QString &fileName);

/*!
 * Registers every theme bundled in \c{res/themes.qrc}, each family in its
 * light and dark variant, reading their style sheets from
 * \c{:/themes/<family>_<light|dark>.css}. The ids and file names follow that
 * canonical set, so an application that ships those files gets the whole
 * bundle from this single call, made once before the first setTheme().
 *
 * A registration whose file is missing is simply skipped: the engine ignores
 * a QxThemeInfo without an id, so a half-populated resource tree never throws.
 */
QX_APP_EXPORT void registerBundledThemes(QxTheme::QxThemeManager *engine);

QX_APP_END_NAMESPACE
