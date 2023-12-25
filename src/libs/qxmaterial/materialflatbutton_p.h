/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALFLATBUTTON_P_H
#define QXMATERIALFLATBUTTON_P_H

#include "materialnamespace.h"
#include "qxmaterial_global.h"
#include <QColor>
#include <QStateMachine>

class QSequentialAnimationGroup;

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialFlatButton;
class MaterialRippleOverlay;
class MaterialFlatButtonStateMachine;

class MaterialFlatButtonPrivate
{
    Q_DISABLE_COPY(MaterialFlatButtonPrivate)
    Q_DECLARE_PUBLIC(MaterialFlatButton)
public:
    MaterialFlatButtonPrivate(MaterialFlatButton *q);
    virtual ~MaterialFlatButtonPrivate();

    void init();

    MaterialFlatButton *const q_ptr;
    MaterialRippleOverlay *rippleOverlay;
    MaterialFlatButtonStateMachine *stateMachine;
    QxMaterial::Role role;
    QxMaterial::RippleStyle rippleStyle;
    QxMaterial::ButtonIconPlacement iconPlacement;
    QxMaterial::OverlayStyle overlayStyle;
    Qt::BGMode bgMode;
    Qt::Alignment textAlignment;
    QColor backgroundColor;
    QColor foregroundColor;
    QColor overlayColor;
    QColor disabledColor;
    QColor disabledBackgroundColor;
    qreal fixedRippleRadius;
    qreal cornerRadius;
    qreal baseOpacity;
    qreal fontSize;
    bool useThemeColors;
    bool useFixedRippleRadius;
    bool haloVisible;
};

class MaterialFlatButtonStateMachine : public QStateMachine
{
    Q_OBJECT

    Q_PROPERTY(qreal overlayOpacity WRITE setOverlayOpacity READ overlayOpacity)
    Q_PROPERTY(qreal checkedOverlayProgress WRITE setCheckedOverlayProgress READ checkedOverlayProgress)
    Q_PROPERTY(qreal haloOpacity WRITE setHaloOpacity READ haloOpacity)
    Q_PROPERTY(qreal haloSize WRITE setHaloSize READ haloSize)
    Q_PROPERTY(qreal haloScaleFactor WRITE setHaloScaleFactor READ haloScaleFactor)
public:
    explicit MaterialFlatButtonStateMachine(MaterialFlatButton *parent);
    ~MaterialFlatButtonStateMachine();

    void setOverlayOpacity(qreal opacity);
    inline qreal overlayOpacity() const;

    void setCheckedOverlayProgress(qreal progress);
    inline qreal checkedOverlayProgress() const;

    void setHaloOpacity(qreal opacity);
    inline qreal haloOpacity() const;

    void setHaloSize(qreal size);
    inline qreal haloSize() const;

    void setHaloScaleFactor(qreal factor);
    inline qreal haloScaleFactor() const;

    void startAnimations();
    void setupProperties();
    void updateCheckedStatus();

signals:
    void buttonPressed();
    void buttonChecked();
    void buttonUnchecked();
protected:
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;
private:
    Q_DISABLE_COPY(MaterialFlatButtonStateMachine)

    void addTransition(QObject *object, QEvent::Type eventType, QState *fromState, QState *toState);
    void addTransition(QAbstractTransition *transition, QState *fromState, QState *toState);

    MaterialFlatButton *const m_button;
    QState *const m_topLevelState;
    QState *const m_configState;
    QState *const m_checkableState;
    QState *const m_checkedState;
    QState *const m_uncheckedState;
    QState *const m_neutralState;
    QState *const m_neutralFocusedState;
    QState *const m_hoveredState;
    QState *const m_hoveredFocusedState;
    QState *const m_pressedState;
    QSequentialAnimationGroup *const m_haloAnimation;
    qreal m_overlayOpacity;
    qreal m_checkedOverlayProgress;
    qreal m_haloOpacity;
    qreal m_haloSize;
    qreal m_haloScaleFactor;
    bool m_wasChecked;
};

inline qreal MaterialFlatButtonStateMachine::overlayOpacity() const
{
    return m_overlayOpacity;
}

inline qreal MaterialFlatButtonStateMachine::checkedOverlayProgress() const
{
    return m_checkedOverlayProgress;
}

inline qreal MaterialFlatButtonStateMachine::haloOpacity() const
{
    return m_haloOpacity;
}

inline qreal MaterialFlatButtonStateMachine::haloSize() const
{
    return m_haloSize;
}

inline qreal MaterialFlatButtonStateMachine::haloScaleFactor() const
{
    return m_haloScaleFactor;
}

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALFLATBUTTON_P_H
