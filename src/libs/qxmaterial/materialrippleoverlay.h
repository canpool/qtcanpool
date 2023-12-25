/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALRIPPLEOVERLAY_H
#define QXMATERIALRIPPLEOVERLAY_H

#include "materialoverlaywidget.h"
#include "qxmaterial_global.h"
#include <QPainterPath>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialRipple;

class QX_MATERIAL_EXPORT MaterialRippleOverlay : public MaterialOverlayWidget
{
    Q_OBJECT
public:
    explicit MaterialRippleOverlay(QWidget *parent = 0);
    ~MaterialRippleOverlay();

    void addRipple(MaterialRipple *ripple);
    void addRipple(const QPoint &position, qreal radius = 300);

    void removeRipple(MaterialRipple *ripple);

    inline void setClipping(bool enable);
    inline bool hasClipping() const;

    inline void setClipPath(const QPainterPath &path);
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    inline QList<MaterialRipple *> ripples() const;
private:
    Q_DISABLE_COPY(MaterialRippleOverlay)

    void paintRipple(QPainter *painter, MaterialRipple *ripple);

    QList<MaterialRipple *> m_ripples;
    QPainterPath m_clipPath;
    bool m_useClip;
};

inline void MaterialRippleOverlay::setClipping(bool enable)
{
    m_useClip = enable;
    update();
}

inline bool MaterialRippleOverlay::hasClipping() const
{
    return m_useClip;
}

inline void MaterialRippleOverlay::setClipPath(const QPainterPath &path)
{
    m_clipPath = path;
    update();
}

inline QList<MaterialRipple *> MaterialRippleOverlay::ripples() const
{
    return m_ripples;
}

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALRIPPLEOVERLAY_H
