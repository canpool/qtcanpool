/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALTOGGLE_H
#define QXMATERIALTOGGLE_H

#include "qxmaterial_global.h"
#include <QtWidgets/QAbstractButton>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialTogglePrivate;

class QX_MATERIAL_EXPORT MaterialToggle : public QAbstractButton
{
    Q_OBJECT

    Q_PROPERTY(QColor disabledColor WRITE setDisabledColor READ disabledColor)
    Q_PROPERTY(QColor activeColor WRITE setActiveColor READ activeColor)
    Q_PROPERTY(QColor inactiveColor WRITE setInactiveColor READ inactiveColor)
    Q_PROPERTY(QColor trackColor WRITE setTrackColor READ trackColor)
public:
    explicit MaterialToggle(QWidget *parent = 0);
    ~MaterialToggle();

    void setUseThemeColors(bool value);
    bool useThemeColors() const;

    void setDisabledColor(const QColor &color);
    QColor disabledColor() const;

    void setActiveColor(const QColor &color);
    QColor activeColor() const;

    void setInactiveColor(const QColor &color);
    QColor inactiveColor() const;

    void setTrackColor(const QColor &color);
    QColor trackColor() const;

    void setOrientation(Qt::Orientation orientation);
    Qt::Orientation orientation() const;

    QSize sizeHint() const Q_DECL_OVERRIDE;
protected:
    bool event(QEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    const QScopedPointer<MaterialTogglePrivate> d_ptr;
private:
    Q_DISABLE_COPY(MaterialToggle)
    Q_DECLARE_PRIVATE(MaterialToggle)
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALTOGGLE_H
