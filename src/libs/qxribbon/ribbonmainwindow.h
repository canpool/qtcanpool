/**
 * Copyleft (C) 2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include <QMainWindow>
#include "ribbonbar.h"

QX_RIBBON_BEGIN_NAMESPACE

class RibbonMainWindowPrivate;

/**
 * A QMainWindow that carries a RibbonBar as its menu widget.
 *
 * The window has no opinion about looks: it never installs a style sheet, and it
 * offers no theme property. A ribbon is drawn by whatever style sheet reaches the
 * application, so picking one is the application's job - QxThemeManager in
 * qxtheme applies one for the whole application, which is the level a ribbon
 * needs it at. RibbonBar::updateRibbonTheme() is what to call after a style
 * change that the bar has to re-read (see there).
 */
class QX_RIBBON_EXPORT RibbonMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit RibbonMainWindow(QWidget *parent = Q_NULLPTR);
    virtual ~RibbonMainWindow() Q_DECL_OVERRIDE;

    RibbonBar *ribbonBar() const;

    void resizeRibbon();
    /*! Forwards to RibbonBar::resizeRibbonNow(); see there for when to use it. */
    void resizeRibbonNow();

    virtual void updateWindowFlags(Qt::WindowFlags flags);

Q_SIGNALS:
    void windowStateChanged(Qt::WindowStates state);
protected:
    void setMenuWidget(QWidget *menuBar);
    void setMenuBar(QMenuBar *menuBar);
protected:
    virtual bool event(QEvent *e) Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
private:
    QX_DECLARE_PRIVATE(RibbonMainWindow)
};

QX_RIBBON_END_NAMESPACE
