/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALFLATBUTTON_H
#define QXMATERIALFLATBUTTON_H

#include "materialnamespace.h"
#include "qxmaterial_global.h"
#include <QScopedPointer>
#include <QtWidgets/QPushButton>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialFlatButtonPrivate;

class QX_MATERIAL_EXPORT MaterialFlatButton : public QPushButton
{
    Q_OBJECT

    Q_PROPERTY(QColor foregroundColor WRITE setForegroundColor READ foregroundColor)
    Q_PROPERTY(QColor backgroundColor WRITE setBackgroundColor READ backgroundColor)
    Q_PROPERTY(QColor overlayColor WRITE setOverlayColor READ overlayColor)
    Q_PROPERTY(QColor disabledForegroundColor WRITE setDisabledForegroundColor READ disabledForegroundColor)
    Q_PROPERTY(QColor disabledBackgroundColor WRITE setDisabledBackgroundColor READ disabledBackgroundColor)
    Q_PROPERTY(qreal fontSize WRITE setFontSize READ fontSize)
public:
    explicit MaterialFlatButton(QWidget *parent = 0, QxMaterial::ButtonPreset preset = QxMaterial::FlatPreset);
    explicit MaterialFlatButton(const QString &text, QWidget *parent = 0,
                                QxMaterial::ButtonPreset preset = QxMaterial::FlatPreset);
    MaterialFlatButton(const QString &text, QxMaterial::Role role, QWidget *parent = 0,
                       QxMaterial::ButtonPreset preset = QxMaterial::FlatPreset);
    ~MaterialFlatButton();

    void applyPreset(QxMaterial::ButtonPreset preset);

    void setUseThemeColors(bool value);
    bool useThemeColors() const;

    void setRole(QxMaterial::Role role);
    QxMaterial::Role role() const;

    void setForegroundColor(const QColor &color);
    QColor foregroundColor() const;

    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;

    void setOverlayColor(const QColor &color);
    QColor overlayColor() const;

    void setDisabledForegroundColor(const QColor &color);
    QColor disabledForegroundColor() const;

    void setDisabledBackgroundColor(const QColor &color);
    QColor disabledBackgroundColor() const;

    void setFontSize(qreal size);
    qreal fontSize() const;

    void setHaloVisible(bool visible);
    bool isHaloVisible() const;

    void setOverlayStyle(QxMaterial::OverlayStyle style);
    QxMaterial::OverlayStyle overlayStyle() const;

    void setRippleStyle(QxMaterial::RippleStyle style);
    QxMaterial::RippleStyle rippleStyle() const;

    void setIconPlacement(QxMaterial::ButtonIconPlacement placement);
    QxMaterial::ButtonIconPlacement iconPlacement() const;

    void setCornerRadius(qreal radius);
    qreal cornerRadius() const;

    void setBackgroundMode(Qt::BGMode mode);
    Qt::BGMode backgroundMode() const;

    void setBaseOpacity(qreal opacity);
    qreal baseOpacity() const;

    void setCheckable(bool value);

    void setHasFixedRippleRadius(bool value);
    bool hasFixedRippleRadius() const;

    void setFixedRippleRadius(qreal radius);

    void setTextAlignment(Qt::Alignment alignment);
    Qt::Alignment textAlignment() const;

    QSize sizeHint() const Q_DECL_OVERRIDE;
protected:
    enum {
        IconPadding = 12
    };

    MaterialFlatButton(MaterialFlatButtonPrivate &d, QWidget *parent = 0,
                       QxMaterial::ButtonPreset preset = QxMaterial::FlatPreset);

    void checkStateSet() Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    virtual void paintBackground(QPainter *painter);
    virtual void paintHalo(QPainter *painter);
    virtual void paintForeground(QPainter *painter);

    virtual void updateClipPath();

    const QScopedPointer<MaterialFlatButtonPrivate> d_ptr;
private:
    Q_DISABLE_COPY(MaterialFlatButton)
    Q_DECLARE_PRIVATE(MaterialFlatButton)
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALFLATBUTTON_H
