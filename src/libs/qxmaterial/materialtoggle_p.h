/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALTOGGLE_P_H
#define QXMATERIALTOGGLE_P_H

#include "qxmaterial_global.h"
#include <QColor>
#include <Qt>
#include <QtWidgets/QWidget>
#include "materialrippleoverlay.h"

class QStateMachine;
class QState;

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialToggle;
class MaterialToggleTrack;
class MaterialToggleThumb;
class MaterialToggleRippleOverlay;

class MaterialTogglePrivate
{
    Q_DISABLE_COPY(MaterialTogglePrivate)
    Q_DECLARE_PUBLIC(MaterialToggle)
public:
    MaterialTogglePrivate(MaterialToggle *q);
    ~MaterialTogglePrivate();

    void init();
    void setupProperties();

    MaterialToggle *const q_ptr;
    MaterialToggleTrack *track;
    MaterialToggleThumb *thumb;
    MaterialToggleRippleOverlay *rippleOverlay;
    QStateMachine *stateMachine;
    QState *offState;
    QState *onState;
    Qt::Orientation orientation;
    QColor disabledColor;
    QColor activeColor;
    QColor inactiveColor;
    QColor trackColor;
    bool useThemeColors;
};

class MaterialToggleRippleOverlay : public MaterialRippleOverlay
{
    Q_OBJECT
public:
    MaterialToggleRippleOverlay(MaterialToggleThumb *thumb, MaterialToggleTrack *track, MaterialToggle *parent);
    ~MaterialToggleRippleOverlay();

protected slots:
    void addToggleRipple();
protected:
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    QRect overlayGeometry() const Q_DECL_OVERRIDE;
private:
    Q_DISABLE_COPY(MaterialToggleRippleOverlay)

    MaterialToggle *const m_toggle;
    MaterialToggleThumb *const m_thumb;
    MaterialToggleTrack *const m_track;
};

class MaterialToggleThumb : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(qreal shift WRITE setShift READ shift)
    Q_PROPERTY(QColor thumbColor WRITE setThumbColor READ thumbColor)
public:
    MaterialToggleThumb(MaterialToggle *parent);
    ~MaterialToggleThumb();

    void setShift(qreal shift);
    inline qreal shift() const;

    inline qreal offset() const;

    inline void setThumbColor(const QColor &color);
    inline QColor thumbColor() const;
protected:
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    Q_DISABLE_COPY(MaterialToggleThumb)

    void updateOffset();

    MaterialToggle *const m_toggle;
    QColor m_thumbColor;
    qreal m_shift;
    qreal m_offset;
};

inline qreal MaterialToggleThumb::shift() const
{
    return m_shift;
}

inline qreal MaterialToggleThumb::offset() const
{
    return m_offset;
}

inline void MaterialToggleThumb::setThumbColor(const QColor &color)
{
    m_thumbColor = color;
    update();
}

inline QColor MaterialToggleThumb::thumbColor() const
{
    return m_thumbColor;
}

class MaterialToggleTrack : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QColor trackColor WRITE setTrackColor READ trackColor)
public:
    MaterialToggleTrack(MaterialToggle *parent);
    ~MaterialToggleTrack();

    void setTrackColor(const QColor &color);
    inline QColor trackColor() const;
protected:
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    Q_DISABLE_COPY(MaterialToggleTrack)

    MaterialToggle *const m_toggle;
    QColor m_trackColor;
};

inline QColor MaterialToggleTrack::trackColor() const
{
    return m_trackColor;
}

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALTOGGLE_P_H
