/**
 * Copyleft (C) 2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "qxribbon_global.h"
#include <QMainWindow>
#include "ribbonbar.h"
#include "ribbontheme.h"

QX_RIBBON_BEGIN_NAMESPACE

class RibbonMainWindowPrivate;

class QX_RIBBON_EXPORT RibbonMainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(int ribbonTheme READ ribbonTheme WRITE setRibbonTheme)
public:
    explicit RibbonMainWindow(QWidget *parent = Q_NULLPTR);
    virtual ~RibbonMainWindow() Q_DECL_OVERRIDE;

    RibbonBar *ribbonBar() const;

    void resizeRibbon();

    /** theme, see to RibbonTheme::ThemeStyle */
    void setRibbonTheme(int theme);
    int ribbonTheme() const;
    void loadTheme(const QString &themeFile);

    virtual void updateWindowFlags(Qt::WindowFlags flags);

Q_SIGNALS:
    void windowStateChanged(Qt::WindowStates state);
protected:
    void setMenuWidget(QWidget *menuBar);
    void setMenuBar(QMenuBar *menuBar);
protected:
    virtual bool event(QEvent *e) Q_DECL_OVERRIDE;
private:
    QX_DECLARE_PRIVATE(RibbonMainWindow)
};

QX_RIBBON_END_NAMESPACE
