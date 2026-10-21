/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTWINDOWMATERIAL_H
#define FLUENTWINDOWMATERIAL_H

#include "qxfluent_global.h"

#include <QtCore/QObject>

class QWidget;

QX_FLUENT_BEGIN_NAMESPACE

class FluentWindowMaterialPrivate;

/*!
 * Fluent's window materials: what the window itself is made of.
 *
 * Fluent names three of them - Mica, Mica Alt and Acrylic - and they are not
 * colours an application paints: they are the window system's, asked for by
 * name and answered with yes or no, because a Windows 10 cannot give Mica and
 * a Linux has none of them at all. A Fluent application that runs anywhere has
 * to be able to ask without being told the answer first, which is why asking
 * returns a bool rather than assuming.
 *
 * The three are one slot on a window, not three switches: asking for one turns
 * the others off, because that is what the window system has.
 *
 * What does the asking is qxwindow's window agent, which already knows how to
 * reach the window system on each platform; this is the Fluent spelling of it
 * and nothing more.
 */
class QX_FLUENT_EXPORT FluentWindowMaterial : public QObject
{
    Q_OBJECT
public:
    /*! What the window is made of. */
    enum class Material {
        None,    /*!< The window system's own: no material asked for. */
        Mica,    /*!< Mica - the one a Fluent application's main window wears. */
        MicaAlt, /*!< Mica Alt - the denser one, for a window with tabs in it. */
        Acrylic  /*!< Acrylic - the blurred one, for a window that is passing
                      through: a flyout, transient window. */
    };
    Q_ENUM(Material)

    explicit FluentWindowMaterial(QWidget *window);
    ~FluentWindowMaterial() override;

    QWidget *window() const;
    /*! What was asked for last, whatever the window system made of it. */
    Material material() const;
    /*! Whether the window system took the last thing that was asked for. */
    bool isApplied() const;

public Q_SLOTS:
    /*! Asks the window for \a material, and answers whether it was given. */
    bool apply(Material material);
private:
    Q_DISABLE_COPY(FluentWindowMaterial)
    QX_DECLARE_PRIVATE(FluentWindowMaterial)
};

/*! The one-call form: asks \a window for \a material, keeping the thing that
 *  does the asking as the window's own - a window has one material, so asking
 *  twice does not mean two of them. */
bool QX_FLUENT_EXPORT applyWindowMaterial(QWidget *window, FluentWindowMaterial::Material material);

QX_FLUENT_END_NAMESPACE

#endif   // FLUENTWINDOWMATERIAL_H
