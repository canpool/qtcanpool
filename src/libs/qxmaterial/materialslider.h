/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALSLIDER_H
#define QXMATERIALSLIDER_H

#include "qxmaterial_global.h"
#include <QScopedPointer>
#include <QtWidgets/QAbstractSlider>

QX_MATERIAL_BEGIN_NAMESPACE

#define QT_MATERIAL_SLIDER_MARGIN 30

class MaterialSliderPrivate;

class QX_MATERIAL_EXPORT MaterialSlider : public QAbstractSlider
{
    Q_OBJECT

    Q_PROPERTY(QColor thumbColor WRITE setThumbColor READ thumbColor)
    Q_PROPERTY(QColor trackColor WRITE setTrackColor READ trackColor)
    Q_PROPERTY(QColor disabledColor WRITE setDisabledColor READ disabledColor)
public:
    explicit MaterialSlider(QWidget *parent = 0);
    ~MaterialSlider();

    void setUseThemeColors(bool value);
    bool useThemeColors() const;

    void setThumbColor(const QColor &color);
    QColor thumbColor() const;

    void setTrackColor(const QColor &color);
    QColor trackColor() const;

    void setDisabledColor(const QColor &color);
    QColor disabledColor() const;

    void setPageStepMode(bool pageStep);
    bool pageStepMode() const;

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;

    void setInvertedAppearance(bool value);
protected:
    void sliderChange(SliderChange change) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;

    void updateThumbOffset();

    const QScopedPointer<MaterialSliderPrivate> d_ptr;
private:
    Q_DISABLE_COPY(MaterialSlider)
    Q_DECLARE_PRIVATE(MaterialSlider)
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALSLIDER_H
