/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALCHECKABLE_H
#define QXMATERIALCHECKABLE_H

#include "qxmaterial_global.h"
#include <QtWidgets/QAbstractButton>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialCheckablePrivate;

class QX_MATERIAL_EXPORT MaterialCheckable : public QAbstractButton
{
    Q_OBJECT
public:
    enum LabelPosition {
        LabelPositionLeft,
        LabelPositionRight,
    };

    explicit MaterialCheckable(QWidget *parent = 0);
    ~MaterialCheckable();

    void setLabelPosition(LabelPosition placement);
    LabelPosition labelPosition() const;

    void setUseThemeColors(bool value);
    bool useThemeColors() const;

    void setCheckedColor(const QColor &color);
    QColor checkedColor() const;

    void setUncheckedColor(const QColor &color);
    QColor uncheckedColor() const;

    void setTextColor(const QColor &color);
    QColor textColor() const;

    void setDisabledColor(const QColor &color);
    QColor disabledColor() const;

    void setCheckedIcon(const QIcon &icon);
    QIcon checkedIcon() const;

    void setUncheckedIcon(const QIcon &icon);
    QIcon uncheckedIcon() const;

    QSize sizeHint() const Q_DECL_OVERRIDE;
protected:
    MaterialCheckable(MaterialCheckablePrivate &d, QWidget *parent = 0);

    bool event(QEvent *event) Q_DECL_OVERRIDE;
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    virtual void setupProperties();

    const QScopedPointer<MaterialCheckablePrivate> d_ptr;
private:
    Q_DISABLE_COPY(MaterialCheckable)
    Q_DECLARE_PRIVATE(MaterialCheckable)
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALCHECKABLE_H
