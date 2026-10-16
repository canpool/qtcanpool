/**
 * Copyleft (C) 2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"

#include <QStatusBar>

class QAction;

QX_RIBBON_BEGIN_NAMESPACE

class RibbonStatusBarPrivate;

/**
 * @brief A ribbon-styled status bar for ribbon windows.
 *
 * RibbonStatusBar is a QStatusBar with the extra structure Office's ribbon puts in
 * its status bar: a left section of plain text segments - each one a QAction, so the
 * host updates a segment by setting the action's text - and a right, permanent
 * section for view switches and the like. A theme styles it the way it styles the
 * rest of the ribbon: the dark themes already carry a QStatusBar rule, and the bar
 * draws a 1px top divider that matches the window background so it reads as part of
 * the ribbon rather than a separate strip.
 */
class QX_RIBBON_EXPORT RibbonStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit RibbonStatusBar(QWidget *parent = nullptr);
    ~RibbonStatusBar();

    /**
     * @brief Adds a text segment to the left section.
     *
     * The returned action is owned by the bar; set its text to change what the
     * segment shows. The segment sits in the temporary area, so a call to
     * showMessage() temporarily hides it, as with any QStatusBar widget.
     */
    QAction *addAction(const QString &text);

    /// Adds a thin vertical separator between two left-section segments.
    QAction *addSeparator();

    /**
     * @brief Adds a segment to the right, permanent section.
     *
     * The action's text (and icon) are shown; the segment is never pushed out by
     * showMessage(). Connect to the action's triggered() to run something.
     */
    void addPermanentAction(QAction *action);

    // Keep QWidget::addAction(QAction *) / addAction(QIcon, QString) reachable;
    // the signature above hides them.
    using QWidget::addAction;
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    Q_DISABLE_COPY(RibbonStatusBar)
    friend class RibbonStatusBarPrivate;

    QX_DECLARE_PRIVATE(RibbonStatusBar)
};

QX_RIBBON_END_NAMESPACE
