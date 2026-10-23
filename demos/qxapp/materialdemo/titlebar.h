/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QtCore/QPoint>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QWidget>

#include "qxmaterial/qxmaterial_global.h"
#include "qxwindow/qxwindow_global.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialRipple;
class MaterialTopAppBar;
QX_MATERIAL_END_NAMESPACE

QX_WINDOW_BEGIN_NAMESPACE
class WindowAgentWidget;
QX_WINDOW_END_NAMESPACE

/*!
 * One caption button of a Material window, drawn here rather than skinned.
 *
 * The window draws no system frame, so its caption buttons are its own. A
 * Material caption button is the M3 icon button at the top of a screen: a glyph
 * in the middle of a 40 pixel circle that only appears under the pointer, and a
 * close button whose circle takes the error colour. Painting it from the
 * palette is what makes a change of theme repaint the caption with the rest of
 * the window.
 */
class CaptionButton : public QAbstractButton
{
    Q_OBJECT
public:
    enum class Glyph {
        Minimize,
        Maximize,
        Restore,
        Close,
    };
    Q_ENUM(Glyph)

    explicit CaptionButton(Glyph glyph, QWidget *parent = Q_NULLPTR);

    Glyph glyph() const;
    /*! A maximize button draws Restore while the window is maximized. */
    void setGlyph(Glyph glyph);

    QSize sizeHint() const override;
protected:
    bool event(QEvent *event) override;
    bool hitButton(const QPoint &pos) const override;
    void paintEvent(QPaintEvent *event) override;
private:
    Glyph m_glyph;
    /*! The press answer: a circle from where the pointer landed, which is what
     *  an M3 icon button gives and a caption button on a Material window is
     *  one of. */
    QX_MATERIAL_PREPEND_NAMESPACE(MaterialRipple) *m_ripple = Q_NULLPTR;
};

/*!
 * The demo's caption: the material app bar, and the window's buttons beside it.
 *
 * A Material application's top app bar is the screen's name and the things that
 * can be done to the screen, and a window with no system frame has to draw that
 * itself - so the bar the library ships is the caption here, and the rail opens
 * the screens under it. The wrapper is thin on purpose: the four shapes an app
 * bar has, and what a taller one does to the name, stay the bar's business, and
 * the app bar keeps covering all of them because it is still the app bar.
 *
 * The window's buttons stand beside the bar rather than over it. Over it would
 * mean two things fighting for one corner - the bar's trailing actions are
 * there, and a large bar puts its name far below the top of its rectangle - and
 * MUI's own bars carry no application mark, so the corner the mark would have
 * taken is free for the buttons instead.
 */
class TitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit TitleBar(QWidget *parent = Q_NULLPTR);

    /*! The agent that took the window over, which is what still knows how to
     *  show the system menu the caption used to have. */
    void setWindowAgent(QX_WINDOW_PREPEND_NAMESPACE(WindowAgentWidget) * agent);

    /*! The app bar the window fills: what the screen is called, and what can be
     *  done to it. */
    QX_MATERIAL_PREPEND_NAMESPACE(MaterialTopAppBar) * appBar() const;

    CaptionButton *minimizeButton() const;
    CaptionButton *maximizeButton() const;
    CaptionButton *closeButton() const;
protected:
    /*! Watches the app bar to give back what the hit test no longer can: a
     *  press on the bar that turns into a drag moves the window, a double click
     *  on it maximizes the window, and a right click on it asks the window for
     *  its system menu. All three are what a caption does, and the bar answers
     *  as client area so that its own icons stay clickable. */
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    QX_MATERIAL_PREPEND_NAMESPACE(MaterialTopAppBar) *m_bar = Q_NULLPTR;
    QX_WINDOW_PREPEND_NAMESPACE(WindowAgentWidget) *m_agent = Q_NULLPTR;
    CaptionButton *m_minimize = Q_NULLPTR;
    CaptionButton *m_maximize = Q_NULLPTR;
    CaptionButton *m_close = Q_NULLPTR;
    /*! Where the press on the bar landed, in the bar's coordinates, and whether
     *  the move it may have started has begun. */
    QPoint m_pressPos;
    bool m_moving = false;
};

#endif   // TITLEBAR_H
