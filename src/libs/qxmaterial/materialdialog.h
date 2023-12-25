/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALDIALOG_H
#define QXMATERIALDIALOG_H

#include "materialoverlaywidget.h"
#include "qxmaterial_global.h"
#include <QScopedPointer>

class QLayout;

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialDialogPrivate;

class QX_MATERIAL_EXPORT MaterialDialog : public MaterialOverlayWidget
{
    Q_OBJECT
public:
    explicit MaterialDialog(QWidget *parent = 0);
    ~MaterialDialog();

    QLayout *windowLayout() const;
    void setWindowLayout(QLayout *layout);

public slots:
    void showDialog();
    void hideDialog();
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    const QScopedPointer<MaterialDialogPrivate> d_ptr;
private:
    Q_DISABLE_COPY(MaterialDialog)
    Q_DECLARE_PRIVATE(MaterialDialog)
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALDIALOG_H
