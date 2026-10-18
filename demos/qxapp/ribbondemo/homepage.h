/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include "qxtheme/qxtheme_global.h"
#include "qxribbon/ribbonpage.h"

QX_RIBBON_USE_NAMESPACE

class QCheckBox;
class QButtonGroup;

QX_THEME_BEGIN_NAMESPACE
class QxThemeIcons;
QX_THEME_END_NAMESPACE

/*!
 * The "Home" page of the ribbon demo: the ribbon's own control panel. It plays
 * with the bar's look and the window's chrome, so almost everything it does lands
 * on the ribbon bar it belongs to or on its top-level window - both reachable from
 * a RibbonPage (ribbonBar() and window()). The only things that leave the page are
 * the context tabs, which the window owns, and the lines logged into the editor.
 */
class HomePage : public RibbonPage
{
    Q_OBJECT
public:
    /*!
     * \a icons keeps this page's glyphs painted from the palette; the window owns
     * it and refreshes it when the theme changes. It may be null, in which case
     * the glyphs keep the colour they were drawn in.
     */
    explicit HomePage(QX_THEME_PREPEND_NAMESPACE(QxThemeIcons) * icons, QWidget *parent = Q_NULLPTR);
Q_SIGNALS:
    /*! Show or hide the first context tab - the window owns that tab. */
    void contextPageVisibleRequested(bool on);
    /*! Show or hide the second context tab. */
    void context2VisibleRequested(bool on);
    /*! Tear down the first context tab. */
    void contextPageDestroyRequested();
    /*! A popup action was exercised; the window logs it in its editor. */
    void messageRequested(const QString &message);
protected:
    bool event(QEvent *e) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *watched, QEvent *e) Q_DECL_OVERRIDE;
private:
    QAction *createAction(const QString &text, const QString &glyph, const QString &objName = QString());

    /*! Finish the wiring that needs a live ribbon bar; runs once, on adoption. */
    void attachToBar();

    QX_THEME_PREPEND_NAMESPACE(QxThemeIcons) * m_icons;
    QAction *m_hideRibbonAction;
    QAction *m_showMinimumButtonAction;
    QButtonGroup *m_styleGroup;
    QCheckBox *m_fullScreenCheck;
    bool m_barAttached;
};

#endif   // HOMEPAGE_H
