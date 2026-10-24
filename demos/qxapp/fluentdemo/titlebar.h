/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QFrame>

#include "qxfluent/qxfluent_global.h"

class QLabel;

QX_FLUENT_BEGIN_NAMESPACE
class FluentCommandBar;
class FluentTextBox;
QX_FLUENT_END_NAMESPACE

/*!
 * One button of the demo's title bar, drawn rather than taken from a style.
 *
 * The window draws no system frame - it is handed over to qxwindow's agent, so
 * the caption is a widget of ours - which means the caption buttons are ours
 * too: a QToolButton would arrive with whatever the style draws, and a Fluent
 * caption button is a flat rectangle that only shows itself on hover or while
 * pressed. So the glyphs are a handful of lines here, painted from the palette
 * so that a change of theme repaints the bar like everything else.
 *
 * The icon is the exception: it is the application's, and it opens the system
 * menu on a click and closes the window on a double click, the way the icon of
 * a framed window does on Windows. Both of those are the window's business
 * (see MainWindow), so this reports the double click and leaves the menu alone.
 */
class CaptionButton : public QAbstractButton
{
    Q_OBJECT
public:
    /*! What the button draws. App is the application mark in the corner. */
    enum class Glyph {
        App,
        Minimize,
        Maximize,
        Restore,
        Close,
    };
    Q_ENUM(Glyph)

    explicit CaptionButton(Glyph glyph, QWidget *parent = Q_NULLPTR);

    Glyph glyph() const;
    /*! Changes what the button draws: a maximize button draws Restore while the
     *  window is maximized, the way the system's own does. */
    void setGlyph(Glyph glyph);

    QSize sizeHint() const override;
Q_SIGNALS:
    void doubleClicked();
protected:
    bool event(QEvent *event) override;
    bool hitButton(const QPoint &pos) const override;
    void paintEvent(QPaintEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
private:
    Glyph m_glyph;
};

/*!
 * The demo's title bar: the application mark, the name, a search field that
 * narrows the pane, the window's commands, and the caption buttons.
 *
 * It is an ordinary widget put in place of a menu bar; the agent is told which
 * widget it is and nothing else. What the bar owns is the row and its look -
 * what the commands are and what a search does belong to the window, which
 * fills the empty command bar and connects to the field.
 *
 * A Fluent application has no menu bar, so the row that would have been one is
 * where its commands live instead: the same commands the ribbon demo puts in a
 * tab, in one row, next to the name of the application they act on.
 */
class TitleBar : public QFrame
{
    Q_OBJECT
public:
    explicit TitleBar(QWidget *parent = Q_NULLPTR);

    /*! The empty command bar the window fills. The bar owns it. */
    QX_FLUENT_PREPEND_NAMESPACE(FluentCommandBar) * commandBar() const;
    /*! The field the pane is narrowed with; empty means "all of it". */
    QX_FLUENT_PREPEND_NAMESPACE(FluentTextBox) * searchField() const;

    CaptionButton *iconButton() const;
    CaptionButton *minimizeButton() const;
    CaptionButton *maximizeButton() const;
    CaptionButton *closeButton() const;

    /*! The name shown beside the application mark. */
    void setTitle(const QString &title);
Q_SIGNALS:
    void searchTextChanged(const QString &text);
private:
    QX_FLUENT_PREPEND_NAMESPACE(FluentCommandBar) *m_bar = Q_NULLPTR;
    QX_FLUENT_PREPEND_NAMESPACE(FluentTextBox) *m_search = Q_NULLPTR;
    QLabel *m_title = Q_NULLPTR;
    CaptionButton *m_icon = Q_NULLPTR;
    CaptionButton *m_minimize = Q_NULLPTR;
    CaptionButton *m_maximize = Q_NULLPTR;
    CaptionButton *m_close = Q_NULLPTR;
};

#endif   // TITLEBAR_H
