/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "qxribbon/qxribbon_global.h"
#include "qxribbon/ribbontheme.h"
#include <QMainWindow>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonAppWindowPrivate;
class RibbonBar;

class RibbonAppWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(int ribbonTheme READ ribbonTheme WRITE setRibbonTheme)
public:
    RibbonAppWindow(QWidget *parent = Q_NULLPTR);
    ~RibbonAppWindow() Q_DECL_OVERRIDE;

    RibbonBar *ribbonBar() const;

    /** theme, see to RibbonTheme::ThemeStyle */
    void setRibbonTheme(int theme);
    int ribbonTheme() const;

    void updateWindowFlags(Qt::WindowFlags flags);

    void setMenuWidget(QWidget *menuBar);
    void setMenuBar(QMenuBar *menuBar);
Q_SIGNALS:
    void windowStateChanged(Qt::WindowStates state);
protected:
    void loadTheme(const QString &themeFile);
    virtual bool eventFilter(QObject *obj, QEvent *e) Q_DECL_OVERRIDE;
    virtual bool event(QEvent *e) Q_DECL_OVERRIDE;
private:
    QX_DECLARE_PRIVATE(RibbonAppWindow)
};

QX_RIBBON_END_NAMESPACE
