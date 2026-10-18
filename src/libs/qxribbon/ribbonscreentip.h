/**
 * Copyleft (C) 2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"

#include <QFrame>

class QLabel;

QX_RIBBON_BEGIN_NAMESPACE

/**
 * @brief Rich screen tip: an Office-style tooltip that shows an action's title, its
 *        description and its shortcut, instead of a single line of plain text.
 *
 * Internal helper for RibbonButton; deliberately not exported, so it is not part of
 * the public API surface. A single tip instance is shared (like QToolTip), because
 * only one control can be hovered at a time. Callers show it on QEvent::ToolTip and
 * hide it when the pointer or focus leaves.
 */
class RibbonScreenTip : public QFrame
{
    Q_OBJECT
public:
    /// Builds the tip and shows it just below @a anchor. description / shortcut may be empty.
    static void showTip(QWidget *anchor, const QString &title, const QString &description, const QString &shortcut);
    static void hideTip();

    QString title() const;
    QString description() const;
    QString shortcut() const;
private:
    explicit RibbonScreenTip();
    static RibbonScreenTip *instance();

    void setContent(const QString &title, const QString &description, const QString &shortcut);
    /*! Rebuilds the style sheet from the palette in force now. */
    void updateStyle();

    QLabel *m_titleLabel;
    QLabel *m_descriptionLabel;
    QLabel *m_shortcutLabel;
};

QX_RIBBON_END_NAMESPACE
