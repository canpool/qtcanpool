/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALTABS_P_H
#define QXMATERIALTABS_P_H

#include "materialnamespace.h"
#include "qxmaterial_global.h"
#include <QColor>
#include "materialoverlaywidget.h"
#include "materialflatbutton.h"

class QHBoxLayout;
class QPropertyAnimation;

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialTabBar;
class MaterialTabBarInkBar;

class MaterialTabBarPrivate
{
    Q_DISABLE_COPY(MaterialTabBarPrivate)
    Q_DECLARE_PUBLIC(MaterialTabBar)
public:
    MaterialTabBarPrivate(MaterialTabBar *q);
    ~MaterialTabBarPrivate();

    void init();

    MaterialTabBar *const q_ptr;
    MaterialTabBarInkBar *inkBar;
    QHBoxLayout *tabLayout;
    QxMaterial::RippleStyle rippleStyle;
    QColor inkColor;
    QColor backgroundColor;
    QColor textColor;
    int tab;
    bool showHalo;
    bool useThemeColors;
};

class MaterialTabBarInkBar : public MaterialOverlayWidget
{
    Q_OBJECT

    Q_PROPERTY(qreal tweenValue WRITE setTweenValue READ tweenValue)
public:
    MaterialTabBarInkBar(MaterialTabBar *parent);
    ~MaterialTabBarInkBar();

    inline void setTweenValue(qreal value);
    inline qreal tweenValue() const;

    void refreshGeometry();
    void animate();
protected:
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    Q_DISABLE_COPY(MaterialTabBarInkBar)

    MaterialTabBar *const m_tabBar;
    QPropertyAnimation *const m_animation;
    QRect m_geometry;
    QRect m_previousGeometry;
    qreal m_tween;
};

inline void MaterialTabBarInkBar::setTweenValue(qreal value)
{
    m_tween = value;
    refreshGeometry();
}

inline qreal MaterialTabBarInkBar::tweenValue() const
{
    return m_tween;
}

class MaterialTab : public MaterialFlatButton
{
    Q_OBJECT
public:
    explicit MaterialTab(MaterialTabBar *parent);
    ~MaterialTab();

    inline void setActive(bool state);
    inline bool isActive() const;

    QSize sizeHint() const Q_DECL_OVERRIDE;

protected slots:
    void activateTab();
protected:
    void paintForeground(QPainter *painter) Q_DECL_OVERRIDE;
private:
    Q_DISABLE_COPY(MaterialTab)

    MaterialTabBar *const m_tabBar;
    bool m_active;
};

inline void MaterialTab::setActive(bool state)
{
    m_active = state;
    update();
}

inline bool MaterialTab::isActive() const
{
    return m_active;
}

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALTABS_P_H
