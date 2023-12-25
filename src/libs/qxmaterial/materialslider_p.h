/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALSLIDER_P_H
#define QXMATERIALSLIDER_P_H

#include "qxmaterial_global.h"
#include <QColor>
#include <QRectF>
#include <QStateMachine>
#include "materialoverlaywidget.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialSlider;
class MaterialSliderThumb;
class MaterialSliderTrack;
class MaterialSliderStateMachine;

class MaterialSliderPrivate
{
    Q_DISABLE_COPY(MaterialSliderPrivate)
    Q_DECLARE_PUBLIC(MaterialSlider)
public:
    MaterialSliderPrivate(MaterialSlider *q);
    ~MaterialSliderPrivate();

    void init();

    QRectF trackBoundingRect() const;
    QRectF thumbBoundingRect() const;

    int valueFromPosition(const QPoint &pos) const;

    void setHovered(bool status);

    MaterialSlider *const q_ptr;
    MaterialSliderThumb *thumb;
    MaterialSliderTrack *track;
    MaterialSliderStateMachine *stateMachine;
    QColor thumbColor;
    QColor trackColor;
    QColor disabledColor;
    int stepTo;
    int oldValue;
    int trackWidth;
    bool hoverTrack;
    bool hoverThumb;
    bool hover;
    bool step;
    bool pageStepMode;
    bool useThemeColors;
};

class MaterialSliderStateMachine : public QStateMachine
{
    Q_OBJECT
public:
    MaterialSliderStateMachine(MaterialSlider *slider, MaterialSliderThumb *thumb, MaterialSliderTrack *track);
    ~MaterialSliderStateMachine();

    void setupProperties();
private:
    Q_DISABLE_COPY(MaterialSliderStateMachine)

    MaterialSlider *const m_slider;
    MaterialSliderThumb *const m_thumb;
    MaterialSliderTrack *const m_track;
    QState *const m_topState;
    QState *const m_fstState;
    QState *const m_sndState;
    QState *const m_inactiveState;
    QState *const m_focusState;
    QState *const m_slidingState;
    QState *const m_pulseOutState;
    QState *const m_pulseInState;
    QState *const m_minState;
    QState *const m_normalState;
};

class MaterialSliderThumb : public MaterialOverlayWidget
{
    Q_OBJECT

    Q_PROPERTY(qreal diameter WRITE setDiameter READ diameter)
    Q_PROPERTY(qreal borderWidth WRITE setBorderWidth READ borderWidth)
    Q_PROPERTY(QColor borderColor WRITE setBorderColor READ borderColor)
    Q_PROPERTY(QColor fillColor WRITE setFillColor READ fillColor)
    Q_PROPERTY(qreal haloSize WRITE setHaloSize READ haloSize)
    Q_PROPERTY(QColor haloColor WRITE setHaloColor READ haloColor)
public:
    explicit MaterialSliderThumb(MaterialSlider *slider);
    ~MaterialSliderThumb();

    inline void setDiameter(qreal diameter);
    inline qreal diameter() const;

    inline void setBorderWidth(qreal width);
    inline qreal borderWidth() const;

    inline void setBorderColor(const QColor &color);
    inline QColor borderColor() const;

    inline void setFillColor(const QColor &color);
    inline QColor fillColor() const;

    inline void setHaloSize(qreal size);
    inline qreal haloSize() const;

    inline void setHaloColor(const QColor &color);
    inline QColor haloColor() const;

    inline void setOffset(int offset);
    inline int offset() const;
protected:
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    Q_DISABLE_COPY(MaterialSliderThumb)

    const MaterialSlider *const m_slider;
    QColor m_borderColor;
    QColor m_fillColor;
    QColor m_haloColor;
    qreal m_diameter;
    qreal m_borderWidth;
    qreal m_haloSize;
    int m_offset;
};

inline void MaterialSliderThumb::setDiameter(qreal diameter)
{
    m_diameter = diameter;
    update();
}

inline qreal MaterialSliderThumb::diameter() const
{
    return m_diameter;
}

inline void MaterialSliderThumb::setBorderWidth(qreal width)
{
    m_borderWidth = width;
    update();
}

inline qreal MaterialSliderThumb::borderWidth() const
{
    return m_borderWidth;
}

inline void MaterialSliderThumb::setBorderColor(const QColor &color)
{
    m_borderColor = color;
    update();
}

inline QColor MaterialSliderThumb::borderColor() const
{
    return m_borderColor;
}

inline void MaterialSliderThumb::setFillColor(const QColor &color)
{
    m_fillColor = color;
    update();
}

inline QColor MaterialSliderThumb::fillColor() const
{
    return m_fillColor;
}

inline void MaterialSliderThumb::setHaloSize(qreal size)
{
    m_haloSize = size;
    update();
}

inline qreal MaterialSliderThumb::haloSize() const
{
    return m_haloSize;
}

inline void MaterialSliderThumb::setHaloColor(const QColor &color)
{
    m_haloColor = color;
    update();
}

inline QColor MaterialSliderThumb::haloColor() const
{
    return m_haloColor;
}

inline void MaterialSliderThumb::setOffset(int offset)
{
    m_offset = offset;
    update();
}

inline int MaterialSliderThumb::offset() const
{
    return m_offset;
}

class MaterialSliderTrack : public MaterialOverlayWidget
{
    Q_OBJECT

    Q_PROPERTY(QColor fillColor WRITE setFillColor READ fillColor)
public:
    explicit MaterialSliderTrack(MaterialSliderThumb *thumb, MaterialSlider *slider);
    ~MaterialSliderTrack();

    inline void setFillColor(const QColor &color);
    inline QColor fillColor() const;

    inline void setTrackWidth(int width);
    inline int trackWidth() const;
protected:
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    Q_DISABLE_COPY(MaterialSliderTrack)

    const MaterialSlider *const m_slider;
    MaterialSliderThumb *const m_thumb;
    QColor m_fillColor;
    int m_trackWidth;
};

inline void MaterialSliderTrack::setFillColor(const QColor &color)
{
    m_fillColor = color;
    update();
}

inline QColor MaterialSliderTrack::fillColor() const
{
    return m_fillColor;
}

void MaterialSliderTrack::setTrackWidth(int width)
{
    m_trackWidth = width;
    update();
}

int MaterialSliderTrack::trackWidth() const
{
    return m_trackWidth;
}

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALSLIDER_P_H
