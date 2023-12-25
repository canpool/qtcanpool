/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALTEXTFIELD_H
#define QXMATERIALTEXTFIELD_H

#include "qxmaterial_global.h"
#include <QColor>
#include <QtWidgets/QLineEdit>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialTextFieldPrivate;

class QX_MATERIAL_EXPORT MaterialTextField : public QLineEdit
{
    Q_OBJECT

    Q_PROPERTY(QColor textColor WRITE setTextColor READ textColor)
    Q_PROPERTY(QColor inkColor WRITE setInkColor READ inkColor)
    Q_PROPERTY(QColor inputLineColor WRITE setInputLineColor READ inputLineColor)
public:
    explicit MaterialTextField(QWidget *parent = 0);
    ~MaterialTextField();

    void setUseThemeColors(bool value);
    bool useThemeColors() const;

    void setShowLabel(bool value);
    bool hasLabel() const;

    void setLabelFontSize(qreal size);
    qreal labelFontSize() const;

    void setLabel(const QString &label);
    QString label() const;

    void setTextColor(const QColor &color);
    QColor textColor() const;

    void setLabelColor(const QColor &color);
    QColor labelColor() const;

    void setInkColor(const QColor &color);
    QColor inkColor() const;

    void setInputLineColor(const QColor &color);
    QColor inputLineColor() const;

    void setShowInputLine(bool value);
    bool hasInputLine() const;
protected:
    MaterialTextField(MaterialTextFieldPrivate &d, QWidget *parent = 0);

    bool event(QEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    const QScopedPointer<MaterialTextFieldPrivate> d_ptr;
private:
    Q_DISABLE_COPY(MaterialTextField)
    Q_DECLARE_PRIVATE(MaterialTextField)
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALTEXTFIELD_H
