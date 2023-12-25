/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALDRAWER_P_H
#define QXMATERIALDRAWER_P_H

#include "qxmaterial_global.h"
#include <QObject>
#include <QStateMachine>
#include "materialoverlaywidget.h"

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialDrawer;
class MaterialDrawerWidget;
class MaterialDrawerStateMachine;

class MaterialDrawerPrivate
{
    Q_DISABLE_COPY(MaterialDrawerPrivate)
    Q_DECLARE_PUBLIC(MaterialDrawer)
public:
    MaterialDrawerPrivate(MaterialDrawer *q);
    ~MaterialDrawerPrivate();

    void init();
    void setClosed(bool value = true);

    MaterialDrawer *const q_ptr;
    MaterialDrawerWidget *widget;
    MaterialDrawerStateMachine *stateMachine;
    QWidget *window;
    int width;
    bool clickToClose;
    bool autoRaise;
    bool closed;
    bool overlay;
};

class MaterialDrawerStateMachine : public QStateMachine
{
    Q_OBJECT

    Q_PROPERTY(qreal opacity WRITE setOpacity READ opacity)
public:
    explicit MaterialDrawerStateMachine(MaterialDrawerWidget *drawer, MaterialDrawer *parent);
    ~MaterialDrawerStateMachine();

    void setOpacity(qreal opacity);
    inline qreal opacity() const;

    bool isInClosedState() const;

    void updatePropertyAssignments();

signals:
    void signalOpen();
    void signalClose();
private:
    Q_DISABLE_COPY(MaterialDrawerStateMachine)

    MaterialDrawerWidget *const m_drawer;
    MaterialDrawer *const m_main;
    QState *const m_openingState;
    QState *const m_openedState;
    QState *const m_closingState;
    QState *const m_closedState;
    qreal m_opacity;
};

inline qreal MaterialDrawerStateMachine::opacity() const
{
    return m_opacity;
}

class MaterialDrawerWidget : public MaterialOverlayWidget
{
    Q_OBJECT

    Q_PROPERTY(int offset WRITE setOffset READ offset)
public:
    explicit MaterialDrawerWidget(QWidget *parent = 0);
    ~MaterialDrawerWidget();

    void setOffset(int offset);
    inline int offset() const;
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    QRect overlayGeometry() const Q_DECL_OVERRIDE;
private:
    int m_offset;
};

inline int MaterialDrawerWidget::offset() const
{
    return m_offset;
}

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALDRAWER_P_H
