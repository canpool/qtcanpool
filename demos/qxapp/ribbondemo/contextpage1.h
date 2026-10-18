/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef CONTEXTPAGE1_H
#define CONTEXTPAGE1_H

#include "qxtheme/qxtheme_global.h"
#include "qxribbon/ribbonpage.h"

QX_RIBBON_USE_NAMESPACE

QX_THEME_BEGIN_NAMESPACE
class QxThemeIcons;
QX_THEME_END_NAMESPACE

/*!
 * The first page of the ribbon demo's first context group: the show/hide
 * playground - groups that hide, actions that hide each other, and the widget
 * containers a group can host.
 *
 * Everything the page does lands on the page itself (it reaches the ribbon bar
 * through RibbonPage::ribbonBar() when it needs a repaint), so the page signs
 * nothing.
 */
class ContextPage1 : public RibbonPage
{
    Q_OBJECT
public:
    /*!
     * \a icons keeps this page's glyphs painted from the palette; the window owns
     * it and refreshes it when the theme changes. It may be null, in which case
     * the glyphs keep the colour they were drawn in.
     */
    explicit ContextPage1(QX_THEME_PREPEND_NAMESPACE(QxThemeIcons) * icons, QWidget *parent = Q_NULLPTR);
private:
    QAction *createAction(const QString &text, const QString &glyph);

    QX_THEME_PREPEND_NAMESPACE(QxThemeIcons) * m_icons;
};

#endif   // CONTEXTPAGE1_H
