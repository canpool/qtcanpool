/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALCIRCULARPROGRESS_H
#define QXMATERIALCIRCULARPROGRESS_H

#include "materialnamespace.h"
#include "qxmaterial_global.h"
#include <QtWidgets/QProgressBar>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialCircularProgressPrivate;

class QX_MATERIAL_EXPORT MaterialCircularProgress : public QProgressBar
{
    Q_OBJECT

    Q_PROPERTY(qreal lineWidth WRITE setLineWidth READ lineWidth)
    Q_PROPERTY(qreal size WRITE setSize READ size)
    Q_PROPERTY(QColor color WRITE setColor READ color)
public:
    explicit MaterialCircularProgress(QWidget *parent = 0);
    ~MaterialCircularProgress();

    void setProgressType(QxMaterial::ProgressType type);
    QxMaterial::ProgressType progressType() const;

    void setUseThemeColors(bool value);
    bool useThemeColors() const;

    void setLineWidth(qreal width);
    qreal lineWidth() const;

    void setSize(int size);
    int size() const;

    void setColor(const QColor &color);
    QColor color() const;

    QSize sizeHint() const Q_DECL_OVERRIDE;
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    const QScopedPointer<MaterialCircularProgressPrivate> d_ptr;
private:
    Q_DISABLE_COPY(MaterialCircularProgress)
    Q_DECLARE_PRIVATE(MaterialCircularProgress)
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALCIRCULARPROGRESS_H
