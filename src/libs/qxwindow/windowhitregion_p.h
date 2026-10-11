/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
 **/
#ifndef WINDOWHITREGION_P_H
#define WINDOWHITREGION_P_H

//
//  W A R N I N G !!!
//  -----------------
//
// This file is not part of the QxWindow API. It is used purely as an
// implementation detail. This header file may change from version to
// version without notice, or may even be removed.
//

#include "qxwindow_global.h"

#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QtCore/QString>
#include <QtCore/QStringList>

QX_WINDOW_BEGIN_NAMESPACE

/*!
 * The title bar / system button bookkeeping that WindowContext does, written as
 * plain functions over rectangles and names.
 *
 * Why it is here: these two questions are asked on every hit test, so they sit in
 * front of the whole resize-and-drag decision, and until now the only way to
 * reach them was to build a real frameless window and move a real mouse over it -
 * they lived in windowcontext.cpp, whose symbols a Windows DLL does not export.
 * Pulling the geometry out means the interesting part is a header, which tests can
 * include on any platform, and windowcontext.cpp is left with the part that
 * genuinely needs a window.
 */

/*!
 * Whether \a pos sits inside one of the rectangles in \a buttons, and if so which
 * one by index. \a visible reports whether the rectangle at an index should be
 * considered at all - a hidden or disabled button must not claim the point, or a
 * click meant for whatever sits underneath would be swallowed by an invisible
 * button.
 *
 * The first match wins, which is the order windowcontext.cpp iterates the system
 * buttons in.
 */
inline int systemButtonAt(const QPoint &pos, const QList<QRect> &buttons, const QList<bool> &visible)
{
    const int count = qMin(buttons.size(), visible.size());
    for (int i = 0; i < count; ++i) {
        if (visible.at(i) && buttons.at(i).contains(pos)) {
            return i;
        }
    }
    return -1;
}

/*!
 * Whether \a name is one of the nominated caption class names. The comparison is
 * exact: a name that merely starts the same does not match, so nominating
 * "QLabel" does not silently capture every subclass of it.
 */
inline bool isCaptionClassName(const char *name, const QStringList &captionClassNames)
{
    for (const QString &cn : captionClassNames) {
        if (cn.compare(QLatin1String(name)) == 0) {
            return true;
        }
    }
    return false;
}

/*!
 * Whether \a pos is in the title bar's draggable area: inside \a titleBarRect,
 * but not inside any of \a buttons or \a hitTestItems, and - when
 * \a captionClassNames is not empty - not sitting over a child of the title bar
 * whose class name is absent from it.
 *
 * Every exclusion answers the same question from a different angle: something
 * that should receive the press itself does, so the title bar must not also start
 * a window drag from that point. A system button that loses to the title bar
 * would turn a click into a move, a hit-test-visible control would stop being
 * hoverable or clickable, and a non-caption child in the title bar - a label, an
 * icon - would be impossible to press at all.
 *
 * \a captionClassNames being empty means "no rule": any child claims the point.
 * That asymmetry is deliberate and matches windowcontext.cpp, where the child
 * check is guarded by !m_captionClassNameList.isEmpty().
 *
 * \a childCaptionName is the class name of the child under \a pos, or an empty
 * string when there is none. Naming a class in \a captionClassNames is what lets
 * that child stay draggable - the caller decides what to pass, so the rule itself
 * is testable without a widget hierarchy.
 */
inline bool isInTitleBarDraggableArea(const QPoint &pos, const QRect &windowRect, const QRect &titleBarRect,
                                      const QList<QRect> &buttons, const QList<bool> &visible,
                                      const QList<QRect> &hitTestItems, const QList<bool> &hitTestItemVisible,
                                      const QString &childCaptionName, const QStringList &captionClassNames)
{
    // A title bar entirely outside the window is treated as no title bar, which is
    // what windowcontext.cpp does before anything else.
    if (!titleBarRect.intersects(windowRect)) {
        return false;
    }
    if (!titleBarRect.contains(pos)) {
        return false;
    }
    if (systemButtonAt(pos, buttons, visible) >= 0) {
        return false;
    }
    for (int i = 0; i < hitTestItems.size() && i < hitTestItemVisible.size(); ++i) {
        if (hitTestItemVisible.at(i) && hitTestItems.at(i).contains(pos)) {
            return false;
        }
    }
    // A child of the title bar claims the point unless its class was nominated as a
    // caption. Only applied when there is at least one nomination, matching the
    // isEmpty() guard in windowcontext.cpp.
    if (!childCaptionName.isEmpty() && !captionClassNames.isEmpty() &&
        !isCaptionClassName(childCaptionName.toLatin1().constData(), captionClassNames)) {
        return false;
    }
    return true;
}

QX_WINDOW_END_NAMESPACE

#endif   // WINDOWHITREGION_P_H