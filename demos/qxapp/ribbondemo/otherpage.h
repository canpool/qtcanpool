/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef OTHERPAGE_H
#define OTHERPAGE_H

#include "qxtheme/qxtheme_global.h"
#include "qxribbon/ribbonpage.h"

QX_RIBBON_USE_NAMESPACE

QX_THEME_BEGIN_NAMESPACE
class QxThemeIcons;
QX_THEME_END_NAMESPACE

/*!
 * The "Other" page of the ribbon demo: a button group, a gallery of shapes, the
 * widget menagerie (combo boxes, a calendar, ...), and a group whose title flips
 * between two languages.
 *
 * The only thing that leaves the page is the gallery - picking an item logs a
 * line in the editor that lives in the window. Everything else lands on the page
 * itself.
 */
class OtherPage : public RibbonPage
{
    Q_OBJECT
public:
    /*!
     * \a icons keeps this page's glyphs painted from the palette; the window owns
     * it and refreshes it when the theme changes. It may be null, in which case
     * the glyphs keep the colour they were drawn in.
     */
    explicit OtherPage(QX_THEME_PREPEND_NAMESPACE(QxThemeIcons) * icons, QWidget *parent = Q_NULLPTR);
Q_SIGNALS:
    /*! A gallery item was picked; the window logs it in its editor. */
    void messageRequested(const QString &message);
private:
    QAction *createAction(const QString &text, const QString &glyph, const QString &objName = QString());

    QX_THEME_PREPEND_NAMESPACE(QxThemeIcons) * m_icons;
};

#endif   // OTHERPAGE_H
