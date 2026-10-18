/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef CUSTOMPAGE_H
#define CUSTOMPAGE_H

#include "qxtheme/qxtheme_global.h"
#include "qxribbon/ribbonpage.h"

QX_RIBBON_USE_NAMESPACE

QX_THEME_BEGIN_NAMESPACE
class QxThemeIcons;
QX_THEME_END_NAMESPACE

/*!
 * The "Custom" page of the ribbon demo: the knobs that change the ribbon
 * itself rather than the document.
 *
 * The quick access bar group is handled here because it only moves this page's
 * own action in and out; anything that changes the application as a whole is
 * signalled instead.
 */
class CustomPage : public RibbonPage
{
    Q_OBJECT
public:
    /*!
     * \a icons keeps this page's glyphs painted from the palette; the window owns
     * it and refreshes it when the theme changes. It may be null, in which case
     * the glyphs keep the colour they were drawn in.
     */
    explicit CustomPage(QX_THEME_PREPEND_NAMESPACE(QxThemeIcons) * icons, QWidget *parent = Q_NULLPTR);

Q_SIGNALS:
    /*! The floating "use qss" switch, whose effect outlives this window. */
    void useQssRequested();
    /*! The customization file should be loaded from disk. */
    void loadCustomizeRequested();
private:
    QAction *createAction(const QString &text, const QString &glyph);

    QX_THEME_PREPEND_NAMESPACE(QxThemeIcons) * m_icons;
};

#endif   // CUSTOMPAGE_H
