/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALAPPBAR_H
#define QXMATERIALAPPBAR_H

#include "qxmaterial_global.h"
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QWidget>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialAppBarPrivate;

class QX_MATERIAL_EXPORT MaterialAppBar : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QColor foregroundColor WRITE setForegroundColor READ foregroundColor)
    Q_PROPERTY(QColor backgroundColor WRITE setBackgroundColor READ backgroundColor)
public:
    explicit MaterialAppBar(QWidget *parent = 0);
    ~MaterialAppBar();

    QSize sizeHint() const Q_DECL_OVERRIDE;

    void setUseThemeColors(bool value);
    bool useThemeColors() const;

    void setForegroundColor(const QColor &color);
    QColor foregroundColor() const;

    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;

    inline QHBoxLayout *appBarLayout() const;
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    const QScopedPointer<MaterialAppBarPrivate> d_ptr;
private:
    Q_DISABLE_COPY(MaterialAppBar)
    Q_DECLARE_PRIVATE(MaterialAppBar)
};

inline QHBoxLayout *MaterialAppBar::appBarLayout() const
{
    return static_cast<QHBoxLayout *>(layout());
}

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALAPPBAR_H
