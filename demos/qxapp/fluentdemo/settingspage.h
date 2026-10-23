/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include "demopage.h"

// The material enum is what the page asks for, so its class comes in whole
// rather than as a forward declaration: an enum inside a class cannot be named
// through one.
#include "qxfluent/fluentwindowmaterial.h"

class QLabel;

QX_FLUENT_BEGIN_NAMESPACE
class FluentSwitch;
QX_FLUENT_END_NAMESPACE

QX_FLUENT_USE_NAMESPACE

/*! The screen where the application is configured.
 *
 *  What the page asks for is the window's, not its own: a pane to fold and a
 *  window material are things only the window can do, so the page says what was
 *  asked for and leaves the doing to it - which is also why it can be lifted
 *  into another application without being rewritten.
 */
class SettingsPage : public DemoPage
{
    Q_OBJECT
public:
    explicit SettingsPage(QWidget *parent = Q_NULLPTR);

    /*! True while the pane is meant to be showing its words as well as its
     *  icons; asked for by the page, kept by the window. */
    bool paneOpen() const;

    /*! Says whether the pane is open, because the window folded it somewhere
     *  else - the caption's own button is the other way to ask for the same
     *  thing. The switch is told rather than asked, so that it says what is in
     *  force rather than what was last clicked here. */
    void setPaneOpen(bool open);

    /*! Says whether the material that was asked for was applied.
     *
     *  The window calls this, because the window is the only thing that can
     *  ask - and because the honest answer on a platform with no window
     *  materials is no.
     */
    void reportMaterial(bool applied);
Q_SIGNALS:
    void paneOpenChanged(bool open);
    void materialRequested(FluentWindowMaterial::Material material);
private:
    bool m_paneOpen = true;
    QX_FLUENT_PREPEND_NAMESPACE(FluentSwitch) *m_paneSwitch = Q_NULLPTR;
    QLabel *m_materialState = Q_NULLPTR;
};

#endif   // SETTINGSPAGE_H
