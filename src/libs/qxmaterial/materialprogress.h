/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALPROGRESS_H
#define QXMATERIALPROGRESS_H

#include "materialnamespace.h"
#include "qxmaterial_global.h"
#include <QtWidgets/QProgressBar>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialProgressPrivate;

class QX_MATERIAL_EXPORT MaterialProgress : public QProgressBar
{
    Q_OBJECT

    Q_PROPERTY(QColor progressColor WRITE setProgressColor READ progressColor)
    Q_PROPERTY(QColor backgroundColor WRITE setProgressColor READ backgroundColor)
public:
    explicit MaterialProgress(QWidget *parent = 0);
    ~MaterialProgress();

    void setProgressType(QxMaterial::ProgressType type);
    QxMaterial::ProgressType progressType() const;

    void setUseThemeColors(bool state);
    bool useThemeColors() const;

    void setProgressColor(const QColor &color);
    QColor progressColor() const;

    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    const QScopedPointer<MaterialProgressPrivate> d_ptr;
private:
    Q_DISABLE_COPY(MaterialProgress)
    Q_DECLARE_PRIVATE(MaterialProgress)
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALPROGRESS_H
