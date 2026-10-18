/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxapp_global.h"

#include <QtCore/QObject>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

namespace QxTheme
{
class QxThemeManager;
}

QX_APP_BEGIN_NAMESPACE

/*!
 * Keeps the whole theme in force on the application: the theme's sheet with the
 * dock's appended to it, and the dock's glyphs painted from the palette the theme
 * installed - once when it is constructed and again on every change.
 *
 * Why a class rather than a pair of calls: the theme has two sources of change
 * and only one of them is a signal. A theme switch arrives as
 * QxThemeManager::themeChanged; the platform swapping its own light/dark palette
 * under us arrives as QEvent::ApplicationPaletteChange on the application object
 * and has no signal at all. An application that wires up the first and forgets
 * the second looks right until the desktop changes, at which point the sheet
 * still agrees with itself and the dock's glyphs no longer agree with it. That
 * is the failure this class exists to make impossible.
 *
 * One installer per application: it is the owner of QApplication::setStyleSheet()
 * for as long as it lives. An application that has a sheet of its own composes it
 * into the string applicationStyleSheet() returns instead of calling
 * setStyleSheet() behind the installer's back.
 */
class QX_APP_EXPORT ThemeInstaller : public QObject
{
    Q_OBJECT
public:
    /*!
     * Installs the theme in force now and follows it from here on.
     *
     * \a iconRoot is the widget whose existing dock buttons are repainted when
     * the colours change - normally the top-level window. Docks built after this
     * call need no repaint: they take their icons from the set registered here.
     * Passing no root still registers that set, which is the right thing for a
     * window that has no docks yet - a shell that receives them from plugins is
     * built that way.
     */
    explicit ThemeInstaller(QxTheme::QxThemeManager *engine, QWidget *iconRoot = Q_NULLPTR,
                            QObject *parent = Q_NULLPTR);
    ~ThemeInstaller() override;

    /*! The widget whose dock buttons are repainted on a colour change. */
    void setIconRoot(QWidget *root);
    QWidget *iconRoot() const;

    /*!
     * Puts the sheet and the icons in place for the theme in force.
     *
     * Called by the constructor, by a theme switch and by a platform palette
     * change, so an application only needs it for a change none of those
     * describe. Turning DockManager::FocusHighlighting on is the one that
     * exists: the dock's focus fragment is rendered from that flag, and the flag
     * has no signal either.
     */
    void install();
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    void installIcons();

    QxTheme::QxThemeManager *m_engine = Q_NULLPTR;
    QWidget *m_iconRoot = Q_NULLPTR;
};

QX_APP_END_NAMESPACE
