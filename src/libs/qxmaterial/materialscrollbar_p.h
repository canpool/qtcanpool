/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALSCROLLBAR_P_H
#define QXMATERIALSCROLLBAR_P_H

#include "qxmaterial_global.h"
#include <QColor>
#include <QStateMachine>
#include "materialscrollbar.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialScrollBar;
class MaterialScrollBarStateMachine;

class MaterialScrollBarPrivate
{
    Q_DISABLE_COPY(MaterialScrollBarPrivate)
    Q_DECLARE_PUBLIC(MaterialScrollBar)
public:
    MaterialScrollBarPrivate(MaterialScrollBar *q);
    ~MaterialScrollBarPrivate();

    void init();

    MaterialScrollBar *const q_ptr;
    MaterialScrollBarStateMachine *stateMachine;
    QColor backgroundColor;
    QColor sliderColor;
    QColor canvasColor;
    bool hideOnMouseOut;
    bool useThemeColors;
};

class MaterialScrollBarStateMachine : public QStateMachine
{
    Q_OBJECT

    Q_PROPERTY(qreal opacity WRITE setOpacity READ opacity)
public:
    MaterialScrollBarStateMachine(MaterialScrollBar *parent);
    ~MaterialScrollBarStateMachine();

    inline void setOpacity(qreal opacity);
    inline qreal opacity() const;
private:
    Q_DISABLE_COPY(MaterialScrollBarStateMachine)

    MaterialScrollBar *const m_scrollBar;
    QState *const m_focusState;
    QState *const m_blurState;
    qreal m_opacity;
};

inline void MaterialScrollBarStateMachine::setOpacity(qreal opacity)
{
    m_opacity = opacity;
    m_scrollBar->update();
}

inline qreal MaterialScrollBarStateMachine::opacity() const
{
    return m_opacity;
}

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALSCROLLBAR_P_H
