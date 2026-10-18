/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxthemeinstaller.h"

#include "qxdockstyle.h"

#include "qxdock/dockiconprovider.h"
#include "qxtheme/qxtheme.h"
#include "qxtheme/qxthememanager.h"

#include <QtGui/QPalette>
#include <QtWidgets/QApplication>

QX_APP_BEGIN_NAMESPACE

ThemeInstaller::ThemeInstaller(QxTheme::QxThemeManager *engine, QWidget *iconRoot, QObject *parent)
    : QObject(parent)
    , m_engine(engine)
    , m_iconRoot(iconRoot)
{
    Q_ASSERT(engine);
    connect(engine, &QxTheme::QxThemeManager::themeChanged, this, &ThemeInstaller::install);
    if (qApp) {
        qApp->installEventFilter(this);
    }
    install();
}

ThemeInstaller::~ThemeInstaller()
{
    // The application object outlives every widget, so there is one here to
    // remove the filter from except during the shutdown of Qt itself.
    if (qApp) {
        qApp->removeEventFilter(this);
    }
}

void ThemeInstaller::setIconRoot(QWidget *root)
{
    m_iconRoot = root;
    installIcons();
}

QWidget *ThemeInstaller::iconRoot() const
{
    return m_iconRoot;
}

void ThemeInstaller::install()
{
    if (!qApp) {
        return;
    }

    qApp->setStyleSheet(applicationStyleSheet(m_engine));
    installIcons();
}

void ThemeInstaller::installIcons()
{
    // The palette, which the engine filled from the theme in force: the accent a
    // dock paints is QPalette::Highlight and the colour of a glyph on it is that
    // role's pair, so one source serves the sheet and the glyphs and they cannot
    // disagree.
    QX_DOCK_PREPEND_NAMESPACE(applyDockIcons)(qApp->palette(), m_iconRoot);
}

bool ThemeInstaller::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == qApp && event->type() == QEvent::ApplicationPaletteChange) {
        // The platform swapped its own palette (Windows light <-> dark). A
        // theme's sheet carries literal colours, so it is still right; the dock's
        // glyphs are pixmaps baked from the colour set and are not. A theme the
        // application never registered has no sheet at all, and the dock's
        // fragment was then rendered from the palette that just changed - that
        // case is redone whole.
        if (m_engine->themeInfo(m_engine->theme()).isValid()) {
            installIcons();
        } else {
            install();
        }
    }
    return QObject::eventFilter(watched, event);
}

QX_APP_END_NAMESPACE
