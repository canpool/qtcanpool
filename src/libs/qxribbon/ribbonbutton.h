/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxribbon_global.h"
#include <QToolButton>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonButtonPrivate;
/**
 * @brief Ribbon 界面适用的 toolButton
 *
 * 相对于普通 toolbutton，主要多了两个类型设置， @ref setButtonType 和 @ref setLargeButtonType
 *
 * @note @sa setIconSize 函数不在起作用
 */
class QX_RIBBON_EXPORT RibbonButton : public QToolButton
{
    Q_OBJECT
public:
    enum RibbonButtonType {
        LargeButton,
        SmallButton
    };
    Q_ENUM(RibbonButtonType)

    enum LargeButtonType {
        Normal, ///< icon占大按钮的一半区域, 箭头在按钮最下方，文字可以换一行，对应于office的菜单按钮，下箭头在按钮最下方(默认)
        Lite    ///< icon占大按钮的60%，文字占40%，且文字不换行，对应于wps模式的菜单按钮，下箭头在文字旁边
    };
public:
    explicit RibbonButton(QWidget *parent = Q_NULLPTR);
    explicit RibbonButton(QAction *defaultAction, QWidget *parent = Q_NULLPTR);
    virtual ~RibbonButton();

    RibbonButtonType buttonType() const;
    void setButtonType(RibbonButtonType type);

    void setLargeButtonType(LargeButtonType type);
    LargeButtonType largeButtonType() const;

    virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
public:
    static void setToolButtonTextShift(bool on);
    static bool isToolButtonTextShift();
    static void setLiteStyleEnableWordWrap(bool on);
    static bool isLiteStyleEnableWordWrap();
protected:
    virtual bool event(QEvent *e) Q_DECL_OVERRIDE;
    virtual void changeEvent(QEvent *e) Q_DECL_OVERRIDE;
    virtual void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    virtual void focusOutEvent(QFocusEvent *e) Q_DECL_OVERRIDE;
    virtual void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
    virtual bool hitButton(const QPoint &pos) const Q_DECL_OVERRIDE;
    virtual void actionEvent(QActionEvent *e) Q_DECL_OVERRIDE;
protected:
    explicit RibbonButton(RibbonButtonPrivate *dd, QWidget *parent = Q_NULLPTR);
protected:
    QX_DECLARE_PRIVATE(RibbonButton)
};

QX_RIBBON_END_NAMESPACE
