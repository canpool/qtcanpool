/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALSCROLLBAR_H
#define QXMATERIALSCROLLBAR_H

#include "qxmaterial_global.h"
#include <QtWidgets/QScrollBar>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialScrollBarPrivate;

class QX_MATERIAL_EXPORT MaterialScrollBar : public QScrollBar
{
    Q_OBJECT

    Q_PROPERTY(QColor canvasColor WRITE setCanvasColor READ canvasColor)
    Q_PROPERTY(QColor backgroundColor WRITE setBackgroundColor READ backgroundColor)
    Q_PROPERTY(QColor sliderColor WRITE setSliderColor READ sliderColor)
public:
    explicit MaterialScrollBar(QWidget *parent = 0);
    ~MaterialScrollBar();

    QSize sizeHint() const Q_DECL_OVERRIDE;

    void setUseThemeColors(bool value);
    bool useThemeColors() const;

    void setCanvasColor(const QColor &color);
    QColor canvasColor() const;

    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;

    void setSliderColor(const QColor &color);
    QColor sliderColor() const;

    void setHideOnMouseOut(bool value);
    bool hideOnMouseOut() const;
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    const QScopedPointer<MaterialScrollBarPrivate> d_ptr;
private:
    Q_DISABLE_COPY(MaterialScrollBar)
    Q_DECLARE_PRIVATE(MaterialScrollBar)
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALSCROLLBAR_H
