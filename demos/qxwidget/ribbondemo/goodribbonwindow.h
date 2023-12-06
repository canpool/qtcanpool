/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "qxribbon/qxribbon_global.h"
#include "qxribbon/ribbontheme.h"
#include "qgood/qgoodwindow.h"

QX_RIBBON_BEGIN_NAMESPACE

class GoodRibbonWindowPrivate;
class RibbonBar;

class GoodRibbonWindow : public QGoodWindow
{
    Q_OBJECT
    Q_PROPERTY(int ribbonTheme READ ribbonTheme WRITE setRibbonTheme)
public:
    GoodRibbonWindow(QWidget *parent = Q_NULLPTR);
    ~GoodRibbonWindow() Q_DECL_OVERRIDE;

    RibbonBar *ribbonBar() const;

    /** theme, see to RibbonTheme::ThemeStyle */
    void setRibbonTheme(int theme);
    int ribbonTheme() const;

    void updateWindowFlags(Qt::WindowFlags flags);

    void setMenuWidget(QWidget *menuBar);
    void setMenuBar(QMenuBar *menuBar);
signals:
    void windowStateChanged(Qt::WindowStates state);
protected:
    void loadTheme(const QString &themeFile);
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject *obj, QEvent *e) Q_DECL_OVERRIDE;
    virtual bool event(QEvent *e) Q_DECL_OVERRIDE;
private:
    QX_DECLARE_PRIVATE(GoodRibbonWindow)
};

QX_RIBBON_END_NAMESPACE
