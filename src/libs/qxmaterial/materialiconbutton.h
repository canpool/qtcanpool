/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALICONBUTTON_H
#define QXMATERIALICONBUTTON_H

#include "qxmaterial_global.h"
#include <QtWidgets/QAbstractButton>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialIconButtonPrivate;

class QX_MATERIAL_EXPORT MaterialIconButton : public QAbstractButton
{
    Q_OBJECT
public:
    explicit MaterialIconButton(const QIcon &icon, QWidget *parent = 0);
    ~MaterialIconButton();

    QSize sizeHint() const Q_DECL_OVERRIDE;

    void setUseThemeColors(bool value);
    bool useThemeColors() const;

    void setColor(const QColor &color);
    QColor color() const;

    void setDisabledColor(const QColor &color);
    QColor disabledColor() const;
protected:
    MaterialIconButton(MaterialIconButtonPrivate &d, QWidget *parent = 0);

    bool event(QEvent *event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    const QScopedPointer<MaterialIconButtonPrivate> d_ptr;
private:
    Q_DISABLE_COPY(MaterialIconButton)
    Q_DECLARE_PRIVATE(MaterialIconButton)
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALICONBUTTON_H
