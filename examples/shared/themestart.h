/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef EXAMPLES_THEMESTART_H
#define EXAMPLES_THEMESTART_H

/*!
 * Puts a bundled theme on a sample before its window is built.
 *
 * Neither qxribbon nor qxdock ships a look of its own any more: the ribbon's
 * lives in the theme sheets, the dock's is one part of them, and both are
 * installed by the application. A sample is an application, and one that
 * registers nothing starts on whatever the platform style has - which is not
 * what the library is meant to look like and not what the sample is there to
 * show.
 *
 * So every sample of a themed library registers the bundle built into
 * res/themes.qrc and wears one of it. Which one is a command line switch
 * (--theme <id>), because a sample has no settings dialog; the id a run
 * remembers through QxSettings is honoured, so a sample that was switched once
 * comes back the way it was left.
 *
 * The bundle is enumerated from :/themes rather than listed here: the qrc is
 * already the canonical set, and a second list would be one more place to edit
 * when a family is added.
 */

/*! True once a theme is installed; false when --theme named one that is not there. */
bool installExampleTheme();

#endif   // EXAMPLES_THEMESTART_H
