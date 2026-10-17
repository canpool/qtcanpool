/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include <QToolButton>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonButtonPrivate;
/**
 * @brief A toolButton suitable for the Ribbon UI
 *
 * Compared with a normal toolbutton, it mainly adds two type settings, @ref setButtonType and @ref setLargeButtonType
 *
 * @note @sa setIconSize no longer has any effect
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
        Normal,   ///< the icon takes half of the large button,
                  ///< the arrow is at the very bottom of the button and the text may wrap to one extra line (default)
        Lite      ///< the icon takes 60% and the text 40% of the large button with no word wrap; the arrow sits
                  ///< beside the text; what a group asks for in RibbonGroup::TwoRowMode
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
protected:
    explicit RibbonButton(RibbonButtonPrivate *d, QWidget *parent = Q_NULLPTR);
protected:
    QX_DECLARE_PRIVATE(RibbonButton)
};

QX_RIBBON_END_NAMESPACE
