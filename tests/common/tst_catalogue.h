/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef TST_CATALOGUE_H
#define TST_CATALOGUE_H

#include <QtCore/QString>

/*! False when lrelease was missing and no catalogue was compiled either. */
bool catalogueAvailable(const QString &catalogue);

/*!
 * Checks one library's translation catalogue against Qt's own lookup.
 *
 * The .ts is not only the translator's workbench, it is also the record of what
 * each string promised; reading it back and asking Qt for every message through
 * the very context the file records turns the catalogue into a test oracle, so
 * adding an English string to the library without translating it fails the
 * build instead of shipping.
 *
 * Skipped when lrelease was missing, because there is then no compiled
 * catalogue to check anything against - which is the same reasoning the
 * translator suite itself applies to its own fixtures.
 */
void checkCatalogue(const QString &catalogue);

#endif   // TST_CATALOGUE_H
