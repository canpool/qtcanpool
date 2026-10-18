/**
 * Copyleft (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"

#include <QtCore/QString>
#include <QtGui/QColor>
#include <QtGui/QPalette>

QX_DOCK_BEGIN_NAMESPACE

/*!
 * The colours the dock sheets are painted with.
 *
 * The sheets say which widget owns which border and when; they no longer say
 * what colour anything is. Every declaration that used to name a palette role
 * or a literal now carries a placeholder, and dockStyleSheet() substitutes one
 * of these fields for it. That is what lets a theme decide the dock's colours
 * without restating the dock's three hundred rules: the application maps its
 * theme's roles onto this set (qxapp::dockStyleColors() does it for the bundled
 * themes) and hands the result in.
 *
 * Two kinds of field look alike but are not:
 *
 * - \c surface, \c divider, \c text and friends describe a *place* in the dock
 *   (the fill a tab sits on). They are what the old sheets expressed with
 *   palette(window), palette(dark) and palette(light).
 * - \c onAccent, \c onAccentHover and \c onAccentPressed describe what goes on
 *   top of \c accent. The style sheet cannot ask the palette for that colour -
 *   palette(highlightedtext) is not among the names its palette() lookup
 *   accepts - which is why it used to be a #ffffff literal.
 *
 * A field left invalid falls back to the value fromPalette() gives for it, so a
 * caller that maps only the fields it cares about still gets a complete sheet
 * rather than an empty declaration (Qt drops those without saying so).
 */
class QX_DOCK_EXPORT DockStyleColors
{
public:
    QColor surface;         /*!< Containers, panels, tabs, side bars, resize handles. */
    QColor surfaceRaised;   /*!< A dock widget's body, one step above \c surface. */
    QColor divider;         /*!< Every 1px/2px separator, in whichever direction reads. */
    QColor text;            /*!< The active tab's label. */
    QColor mutedText;       /*!< An inactive tab's label. */
    QColor accent;          /*!< The focused tab, the auto hide title, the active mark. */
    QColor accentText;      /*!< Accent used as text on a neutral fill. */
    QColor onAccent;        /*!< Text and glyphs on \c accent. */
    QColor onAccentHover;   /*!< A wash of \c onAccent, for hover. */
    QColor onAccentPressed; /*!< The deeper wash, for press. */
    QColor subtle;          /*!< A low contrast step above \c surface (the side tab marks). */
    QColor hover;           /*!< Hover fill on a neutral surface. */
    QColor pressed;         /*!< Press fill on a neutral surface. */
    QColor tabActiveTop;    /*!< Start of the active tab's gradient. */
    QColor tabActiveBottom; /*!< End of it. */

    /*!
     * The colours the old palette()-based sheets resolved to, for an application
     * that does not want to map a theme of its own.
     *
     * This is also the fallback the renderer uses per field. Two values are
     * deliberately not a straight role lookup: \c onAccent comes from
     * QPalette::HighlightedText (askable in C++, unlike through a style sheet),
     * and \c subtle flips from a black wash to a white one on a dark window,
     * because a black wash on a dark surface is invisible.
     */
    static DockStyleColors fromPalette(const QPalette &palette);

    /*!
     * A copy with every invalid field filled from fromPalette() of the current
     * application palette. The renderer and the icon tinting both start here,
     * so a partial mapping never produces an empty declaration or an invisible
     * glyph.
     */
    DockStyleColors resolved() const;
};

/*!
 * Reads the fragments that make up the dock's style sheet and substitutes
 * \a colors into them.
 *
 * The sheets behind this used to live in the library and were applied by
 * DockWindow's own constructor. That put the dock's appearance beyond the reach
 * of the application: a widget-level style sheet outranks the application-level
 * one, so an application that set a dark theme still got a DockWindow the
 * library had already styled. The sheets moved out with B12's move of the
 * ribbon themes and are now built in by the applications that want them - see
 * res/dockstyle.qrc, which is to the dock what res/themes.qrc is to the ribbon.
 *
 * What comes back is three fragments joined: base.css always, focus.css when
 * \a focusHighlighting is set, and x11.css on an X11 build. They are fragments
 * rather than four complete sheets because the two modifiers are independent
 * and the four hand-written combinations had already drifted apart.
 *
 * They are handed out rather than applied on purpose. A sheet can only be read
 * from the executable that carries it, so which fragments are right is a
 * question only the application can answer, and it answers it by building them
 * in and asking here for their text.
 *
 * The install order is the application's to keep: the theme's sheet first, this
 * one after it. Both are application-level, Qt keeps the earlier of two equally
 * specific rules, and the dock's rules are single type selectors - the same
 * specificity as a theme's QWidget rule - so the dock has to come last to win.
 *
 * \a focusHighlighting picks whether the focused dock widget gets a frame and
 * its tab the accent fill; it is the same DockManager::FocusHighlighting the
 * library used to read for itself. QX_DOCK_X11 picks the x11 fragment, which
 * carries the extra rules an X11 desktop needs and is the only thing in the
 * library that knows which desktop that is.
 *
 * Returns an empty string when a fragment is missing or a placeholder is left
 * unresolved - a correct build always has both - and says so on stderr. An
 * empty string is deliberately not applied to anything: the caller is about to
 * hand it to setStyleSheet(), and doing that with nothing would wipe whatever
 * the application had already set.
 */
QX_DOCK_EXPORT QString dockStyleSheet(const DockStyleColors &colors, bool focusHighlighting);

/*!
 * The same sheet, rendered with DockStyleColors::fromPalette() of the current
 * application palette. Kept for callers written before DockStyleColors existed.
 */
QX_DOCK_EXPORT QString dockStyleSheet(bool focusHighlighting);

QX_DOCK_END_NAMESPACE
