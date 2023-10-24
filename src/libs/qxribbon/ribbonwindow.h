/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxribbon_global.h"
#include <QMainWindow>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonWindowPrivate;
class RibbonBar;

/**
 * @brief 如果要使用 RibbonBar，必须使用 RibbonWindow 代替 QMainWindow
 *
 * 由于 ribbon 的风格和传统的 MenuBar/Toolbar 风格差异较大，RibbonBar 使用需要把原有的 QMainWindow 替换为 RibbonWindow,
 * RibbonWindow 默认是个无边框窗体，继承自 QMainWindow, 其构造函数的参数 useRibbon, 用于指定是否使用 ribbon 风格，默认为 true
 *
 * @code
 * RibbonWindow(QWidget *parent = Q_NULLPTR, bool useRibbon = true);
 * @endcode
 *
 * 如果想使用非 ribbon 风格，只需要把 useRibbon 设置为 false 即可, 成员函数 isUseRibbon 用于判断当前是否为 ribbon 模式，
 * 这个函数在兼容传统 Toolbar 风格和 ribbon 风格时非常有用。
 *
 * @code
 * bool isUseRibbon() const;
 * @endcode
 *
 * 由于无边框窗体存在一些缺陷，所以提供 setFrameless 接口来设置是否采用无边框。如果不采用无边框的话，window 原生标题栏加上
 * RibbonBar 的高度比较高，可以考虑将 RibbonBar 设置为 wps 模式 @sa RibbonBar::setRibbonStyle
 *
 * @code
 * void setFrameless(bool frameless);
 * @endcode
 *
 * @ref RibbonWindow 提供了几种常用的 ribbon 样式表，样式可见 @ref RibbonTheme
 * 通过 @ref setRibbonTheme 可改变 ribbon 的样式，用户也可通过 qss 自己定义自己的样式。
 *
 * 常用方法：在 RibbonWindow 继承类中自定义 Theme 枚举类型，如: RibbonThemeCustom, 然后首个类型值定义为 CustomTheme + 1
 *
 */
class QX_RIBBON_EXPORT RibbonWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(int ribbonTheme READ ribbonTheme WRITE setRibbonTheme)
public:
    enum RibbonTheme {
        NormalTheme,
        Office2013Theme,
        WpsdarkTheme,
        CustomTheme,
    };
    Q_ENUM(RibbonTheme)
public:
    RibbonWindow(QWidget *parent = Q_NULLPTR, bool useRibbon = true);
    ~RibbonWindow() Q_DECL_OVERRIDE;

    RibbonBar *ribbonBar() const;

    void setRibbonTheme(int theme);
    int ribbonTheme() const;

    bool isUseRibbon() const;

    bool isFrameless() const;
    void setFrameless(bool frameless);

    void updateWindowFlags(Qt::WindowFlags flags);

    void setMenuWidget(QWidget *menuBar);
    void setMenuBar(QMenuBar *menuBar);
protected:
    void loadTheme(const QString &themeFile);
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject *obj, QEvent *e) Q_DECL_OVERRIDE;
    virtual bool event(QEvent *e) Q_DECL_OVERRIDE;
private:
    QX_DECLARE_PRIVATE(RibbonWindow)
};

QX_RIBBON_END_NAMESPACE
