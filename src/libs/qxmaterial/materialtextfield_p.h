/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALTEXTFIELD_P_H
#define QXMATERIALTEXTFIELD_P_H

#include "qxmaterial_global.h"
#include "materialtextfield.h"
#include <QColor>
#include <QStateMachine>
#include <QtWidgets/QWidget>

class QPropertyAnimation;

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialTextField;
class MaterialTextFieldStateMachine;
class MaterialTextFieldLabel;

class MaterialTextFieldPrivate
{
    Q_DISABLE_COPY(MaterialTextFieldPrivate)
    Q_DECLARE_PUBLIC(MaterialTextField)
public:
    MaterialTextFieldPrivate(MaterialTextField *q);
    virtual ~MaterialTextFieldPrivate();

    void init();

    MaterialTextField *const q_ptr;
    MaterialTextFieldStateMachine *stateMachine;
    MaterialTextFieldLabel *label;
    QColor textColor;
    QColor labelColor;
    QColor inkColor;
    QColor inputLineColor;
    QString labelString;
    qreal labelFontSize;
    bool showLabel;
    bool showInputLine;
    bool useThemeColors;
};

class MaterialTextFieldStateMachine : public QStateMachine
{
    Q_OBJECT

    Q_PROPERTY(qreal progress WRITE setProgress READ progress)
public:
    MaterialTextFieldStateMachine(MaterialTextField *parent);
    ~MaterialTextFieldStateMachine();

    void setLabel(MaterialTextFieldLabel *label);

    inline void setProgress(qreal progress);
    inline qreal progress() const;

public slots:
    void setupProperties();
private:
    Q_DISABLE_COPY(MaterialTextFieldStateMachine)

    MaterialTextField *const m_textField;
    QState *const m_normalState;
    QState *const m_focusedState;
    MaterialTextFieldLabel *m_label;
    QPropertyAnimation *m_offsetAnimation;
    QPropertyAnimation *m_colorAnimation;
    qreal m_progress;
};

inline void MaterialTextFieldStateMachine::setProgress(qreal progress)
{
    m_progress = progress;
    m_textField->update();
}

inline qreal MaterialTextFieldStateMachine::progress() const
{
    return m_progress;
}

class MaterialTextFieldLabel : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(qreal scale WRITE setScale READ scale)
    Q_PROPERTY(QPointF offset WRITE setOffset READ offset)
    Q_PROPERTY(QColor color WRITE setColor READ color)
public:
    MaterialTextFieldLabel(MaterialTextField *parent);
    ~MaterialTextFieldLabel();

    inline void setScale(qreal scale);
    inline qreal scale() const;

    inline void setOffset(const QPointF &pos);
    inline QPointF offset() const;

    inline void setColor(const QColor &color);
    inline QColor color() const;
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    Q_DISABLE_COPY(MaterialTextFieldLabel)

    MaterialTextField *const m_textField;
    qreal m_scale;
    qreal m_posX;
    qreal m_posY;
    QColor m_color;
};

inline void MaterialTextFieldLabel::setScale(qreal scale)
{
    m_scale = scale;
    update();
}

inline qreal MaterialTextFieldLabel::scale() const
{
    return m_scale;
}

inline void MaterialTextFieldLabel::setOffset(const QPointF &pos)
{
    m_posX = pos.x();
    m_posY = pos.y();
    update();
}

inline QPointF MaterialTextFieldLabel::offset() const
{
    return QPointF(m_posX, m_posY);
}

inline void MaterialTextFieldLabel::setColor(const QColor &color)
{
    m_color = color;
    update();
}

inline QColor MaterialTextFieldLabel::color() const
{
    return m_color;
}

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALTEXTFIELD_P_H
