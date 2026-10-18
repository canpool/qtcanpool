/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXTHEMEICON_H
#define QXTHEMEICON_H

#include "qxtheme.h"

#include <QtCore/QObject>
#include <QtCore/QSize>
#include <QtCore/QString>
#include <QtGui/QIcon>
#include <QtGui/QPalette>
#include <QtGui/QPixmap>

class QAction;

QX_THEME_BEGIN_NAMESPACE

/*!
 * Single-colour icons, painted with the palette the theme installed.
 *
 * A glyph shipped in one colour has to be repainted when the theme changes:
 * black on a light surface is unreadable on a dark one. These four functions are
 * the entry point an application - and QxApp, which is the only library that
 * sees the whole picture - asks for that; they read the colour from the palette
 * currently installed on QGuiApplication, which is the one QxThemeManager put
 * there.
 *
 * The icon is rasterised and then repainted through its alpha only, so what is
 * painted over is the shape and nothing else: the colour the file was drawn in
 * is irrelevant here, and a glyph with more than one colour comes out as a
 * silhouette.
 *
 * QxRibbon and QxDock each carry a routine of their own for the same job, and
 * neither of them is going anywhere: both libraries are usable without this one,
 * and making them call into qxtheme would turn two independent libraries into
 * two that depend on a third. What keeps the three honest is not shared code but
 * a shared rule - what a tintable icon may look like, and a gate that checks it.
 * See doc/pages/icons.md.
 */

/*!
 * \a filePath rasterised at \a size and repainted in \a color; a null pixmap
 * when the file cannot be loaded, and the untouched raster when \a color is
 * invalid - without an application there is no palette to ask, and a glyph in
 * its own colour is still a glyph.
 *
 * An empty \a size asks for the asset's own size.
 */
QX_THEME_EXPORT QPixmap tintedPixmap(const QString &filePath, const QColor &color, const QSize &size = QSize());
/*!
 * tintedPixmap() at several sizes, as one icon.
 *
 * Each size is rasterised separately, which is what keeps a vector glyph sharp:
 * one raster scaled up is blurry, a raster made at the size it is drawn at is
 * not. An empty \a size asks for the asset's own size, scaled for the common
 * device pixel ratios.
 */
QX_THEME_EXPORT QIcon tintedIcon(const QString &filePath, const QColor &color, const QSize &size = QSize());

/*!
 * tintedPixmap() in the colour \a role names in the palette currently installed.
 *
 * Asking the palette rather than naming a colour is the whole point: the same
 * call answers differently on a light theme and a dark one, and an application
 * that hands its icons through this function needs no code of its own to follow
 * a theme change beyond re-asking.
 */
QX_THEME_EXPORT QPixmap themedPixmap(const QString &filePath, QPalette::ColorRole role = QPalette::WindowText,
                                     const QSize &size = QSize());
/*! themedPixmap() at several sizes, as one icon. */
QX_THEME_EXPORT QIcon themedIcon(const QString &filePath, QPalette::ColorRole role = QPalette::WindowText,
                                 const QSize &size = QSize());

class QxThemeIconsPrivate;

/*!
 * The other half of the job: painting the icons again once the palette moved.
 *
 * themedIcon() answers with the colour in force at the moment it is called, and
 * an answer given once is a fixed colour from then on - which is no better than
 * shipping a black glyph when the theme can change under it. This object keeps
 * what it would need to answer again: it takes the actions an application hands
 * it, paints each one's icon from the palette, and repaints them when the
 * application palette changes, which is what a theme switch does.
 *
 * Nothing is repainted unless an application asks for it to be, so a program
 * that never changes theme pays nothing. Watching for the change is automatic -
 * QApplication::setPalette() announces it, and the application object is where
 * this object listens - while refresh() is there for a palette replaced in a way
 * Qt does not announce, or for an icon file that changed underneath.
 *
 * @code
 * auto *icons = new QxThemeIcons(this);
 * icons->add(saveAction, QStringLiteral(":/icons/save.svg"), QPalette::ButtonText);
 * @endcode
 */
class QX_THEME_EXPORT QxThemeIcons : public QObject
{
    Q_OBJECT
public:
    /*! Listens for palette changes on the application object, which has to exist. */
    explicit QxThemeIcons(QObject *parent = Q_NULLPTR);
    ~QxThemeIcons() override;

    /*!
     * Paints \a action's icon from \a filePath in the colour \a role names now,
     * and remembers all of it so refresh() can do the same again.
     *
     * Adding an action that is already known replaces what was remembered for it
     * rather than binding it twice. A null \a action is ignored.
     */
    void add(QAction *action, const QString &filePath, QPalette::ColorRole role = QPalette::WindowText,
             const QSize &size = QSize());
    /*!
     * Stops keeping \a action up to date; its icon is left as it is. True when
     * it was bound.
     */
    bool remove(QAction *action);
    /*! How many actions are being kept up to date. */
    int count() const;
    /*! Forgets every action; the icons are left as they are. */
    void clear();

public Q_SLOTS:
    /*! Repaints every remembered action's icon from the palette now in force. */
    void refresh();
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    Q_DISABLE_COPY(QxThemeIcons)
    QX_DECLARE_PRIVATE(QxThemeIcons)
};

QX_THEME_END_NAMESPACE

#endif   // QXTHEMEICON_H
