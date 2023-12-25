/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALBADGE_H
#define QXMATERIALBADGE_H

#include "materialoverlaywidget.h"
#include "qxmaterial_global.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialBadgePrivate;

class QX_MATERIAL_EXPORT MaterialBadge : public MaterialOverlayWidget
{
    Q_OBJECT

    Q_PROPERTY(QColor textColor WRITE setTextColor READ textColor)
    Q_PROPERTY(QColor backgroundColor WRITE setBackgroundColor READ backgroundColor)
    Q_PROPERTY(QPointF relativePosition WRITE setRelativePosition READ relativePosition)
public:
    explicit MaterialBadge(QWidget *parent = 0);
    explicit MaterialBadge(const QIcon &icon, QWidget *parent = 0);
    explicit MaterialBadge(const QString &text, QWidget *parent = 0);
    ~MaterialBadge();

    void setUseThemeColors(bool value);
    bool useThemeColors() const;

    void setTextColor(const QColor &color);
    QColor textColor() const;

    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;

    void setRelativePosition(const QPointF &pos);
    void setRelativePosition(qreal x, qreal y);
    QPointF relativePosition() const;

    void setRelativeXPosition(qreal x);
    qreal relativeXPosition() const;

    void setRelativeYPosition(qreal y);
    qreal relativeYPosition() const;

    QSize sizeHint() const Q_DECL_OVERRIDE;

    void setIcon(const QIcon &icon);
    QIcon icon() const;

    void setText(const QString &text);
    QString text() const;
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    int getDiameter() const;

    const QScopedPointer<MaterialBadgePrivate> d_ptr;
private:
    Q_DISABLE_COPY(MaterialBadge)
    Q_DECLARE_PRIVATE(MaterialBadge)
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALBADGE_H
