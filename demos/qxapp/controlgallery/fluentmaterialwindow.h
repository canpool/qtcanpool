/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef FLUENTMATERIALWINDOW_H
#define FLUENTMATERIALWINDOW_H

#include <QtWidgets/QWidget>

#include "qxwindow/qxwindow_global.h"

class QAbstractButton;
class QLabel;

QX_WINDOW_BEGIN_NAMESPACE
class WindowAgentWidget;
QX_WINDOW_END_NAMESPACE

/*!
 * The window a window material is shown on.
 *
 * A material is not paint: it is what the window system makes the window out
 * of, and asking for one takes the system frame away - qxwindow's agent does
 * that, because the windows it is used on draw the caption themselves. A
 * material asked of a window that kept the system's title bar therefore leaves
 * that window with no title bar at all, which is not the material being wrong
 * but the window not being the kind of window a material is for.
 *
 * Hence this window: small, frameless, and with a caption of its own - a name
 * and one button - so that a material has somewhere to be seen and the caption
 * is still there after the window has been resized. It is the only thing the
 * gallery's material row opens; the gallery itself is never asked for one.
 */
class FluentMaterialWindow : public QWidget
{
    Q_OBJECT
public:
    explicit FluentMaterialWindow(QWidget *parent = Q_NULLPTR);

    /*! What the body says about the material: its name and whether this
     *  platform gave it. */
    void setMaterialText(const QString &text);
private:
    void installWindowAgent();

    QX_WINDOW_PREPEND_NAMESPACE(WindowAgentWidget) *m_agent = Q_NULLPTR;
    QWidget *m_caption = Q_NULLPTR;
    QAbstractButton *m_close = Q_NULLPTR;
    QLabel *m_material = Q_NULLPTR;
};

#endif   // FLUENTMATERIALWINDOW_H
