/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALTABS_H
#define QXMATERIALTABS_H

#include "materialnamespace.h"
#include "qxmaterial_global.h"
#include <QIcon>
#include <QtWidgets/QWidget>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialTabBarPrivate;
class MaterialTab;

class QX_MATERIAL_EXPORT MaterialTabBar : public QWidget
{
    Q_OBJECT
public:
    explicit MaterialTabBar(QWidget *parent = 0);
    ~MaterialTabBar();

    void setUseThemeColors(bool value);
    bool useThemeColors() const;

    void setHaloVisible(bool value);
    bool isHaloVisible() const;

    void setRippleStyle(QxMaterial::RippleStyle style);
    QxMaterial::RippleStyle rippleStyle() const;

    void setInkColor(const QColor &color);
    QColor inkColor() const;

    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;

    void setTextColor(const QColor &color);
    QColor textColor() const;

    void addTab(const QString &text, const QIcon &icon = QIcon());

    void setCurrentTab(MaterialTab *tab);
    void setCurrentTab(int index);

    int currentIndex() const;

signals:
    void currentChanged(int);
protected:
    void setTabActive(int index, bool active = true);
    void updateTabs();

    const QScopedPointer<MaterialTabBarPrivate> d_ptr;
private:
    Q_DISABLE_COPY(MaterialTabBar)
    Q_DECLARE_PRIVATE(MaterialTabBar)
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALTABS_H
