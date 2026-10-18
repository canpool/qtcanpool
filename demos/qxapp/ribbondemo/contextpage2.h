/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef CONTEXTPAGE2_H
#define CONTEXTPAGE2_H

#include "qxtheme/qxtheme_global.h"
#include "qxribbon/ribbonpage.h"

QX_RIBBON_USE_NAMESPACE

QX_THEME_BEGIN_NAMESPACE
class QxThemeIcons;
QX_THEME_END_NAMESPACE

/*!
 * The second page of the ribbon demo's first context group: a page that can
 * delete itself, and a group that lays the three button sizes out next to each
 * other.
 *
 * Nothing here reaches beyond the page, so the page signs nothing - deleting a
 * page concerns the ribbon the page already belongs to.
 */
class ContextPage2 : public RibbonPage
{
    Q_OBJECT
public:
    /*!
     * \a icons keeps this page's glyphs painted from the palette; the window owns
     * it and refreshes it when the theme changes. It may be null, in which case
     * the glyphs keep the colour they were drawn in.
     */
    explicit ContextPage2(QX_THEME_PREPEND_NAMESPACE(QxThemeIcons) * icons, QWidget *parent = Q_NULLPTR);
private:
    QAction *createAction(const QString &text, const QString &glyph, const QString &objName = QString());

    QX_THEME_PREPEND_NAMESPACE(QxThemeIcons) * m_icons;
};

#endif   // CONTEXTPAGE2_H
