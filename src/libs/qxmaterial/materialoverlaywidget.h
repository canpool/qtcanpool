/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALOVERLAYWIDGET_H
#define QXMATERIALOVERLAYWIDGET_H

#include "qxmaterial_global.h"
#include <QtWidgets/QWidget>

QX_MATERIAL_BEGIN_NAMESPACE

class QX_MATERIAL_EXPORT MaterialOverlayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MaterialOverlayWidget(QWidget *parent = 0);
    ~MaterialOverlayWidget();
protected:
    bool event(QEvent *event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;

    virtual QRect overlayGeometry() const;
private:
    Q_DISABLE_COPY(MaterialOverlayWidget)
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALOVERLAYWIDGET_H
