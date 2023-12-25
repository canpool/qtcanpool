/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALAUTOCOMPLETE_H
#define QXMATERIALAUTOCOMPLETE_H

#include "materialtextfield.h"
#include "qxmaterial_global.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialAutoCompletePrivate;

class QX_MATERIAL_EXPORT MaterialAutoComplete : public MaterialTextField
{
    Q_OBJECT
public:
    explicit MaterialAutoComplete(QWidget *parent = 0);
    ~MaterialAutoComplete();

    void setDataSource(const QStringList &data);

signals:
    void itemSelected(QString);

protected slots:
    void updateResults(QString text);
protected:
    bool event(QEvent *event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;
private:
    Q_DISABLE_COPY(MaterialAutoComplete)
    Q_DECLARE_PRIVATE(MaterialAutoComplete)
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALAUTOCOMPLETE_H
