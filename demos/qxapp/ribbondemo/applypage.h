/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef APPLYPAGE_H
#define APPLYPAGE_H

#include "qxtheme/qxtheme_global.h"
#include "qxribbon/ribbonpage.h"

class QFontComboBox;

QX_RIBBON_USE_NAMESPACE

QX_THEME_BEGIN_NAMESPACE
class QxThemeIcons;
QX_THEME_END_NAMESPACE

/*!
 * The "Apply" page of the ribbon demo: what a page looks like once it owns its
 * own glyphs and actions instead of being a slice of the main window.
 *
 * The main window builds no widgets here - it hands this page the icon set and
 * connects the three things it wants to be told about. That is what makes the
 * pages replaceable one at a time: everything a page needs is in the page.
 */
class ApplyPage : public RibbonPage
{
    Q_OBJECT
public:
    /*!
     * \a icons keeps this page's glyphs painted from the palette; the window owns
     * it and refreshes it when the theme changes. It may be null, in which case
     * the glyphs keep the colour they were drawn in.
     */
    explicit ApplyPage(QX_THEME_PREPEND_NAMESPACE(QxThemeIcons) * icons, QWidget *parent = Q_NULLPTR);

Q_SIGNALS:
    /*! The font picker's choice, relayed so the window can apply it to the editor. */
    void fontSelected(const QFont &font);
    /*! The "Larger" glyph was pressed. */
    void largerFontRequested();
    /*! The "Smaller" glyph was pressed. */
    void smallerFontRequested();
private:
    QAction *createAction(const QString &text, const QString &glyph);

    QX_THEME_PREPEND_NAMESPACE(QxThemeIcons) * m_icons;
};

#endif   // APPLYPAGE_H
