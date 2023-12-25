/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALAUTOCOMPLETE_P_H
#define QXMATERIALAUTOCOMPLETE_P_H

#include "materialautocomplete.h"
#include "materialtextfield_p.h"
#include <QStateMachine>

class QWidget;
class QVBoxLayout;

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialAutoCompleteOverlay;
class MaterialAutoCompleteStateMachine;

class MaterialAutoCompletePrivate : public MaterialTextFieldPrivate
{
    Q_DISABLE_COPY(MaterialAutoCompletePrivate)
    Q_DECLARE_PUBLIC(MaterialAutoComplete)
public:
    MaterialAutoCompletePrivate(MaterialAutoComplete *q);
    virtual ~MaterialAutoCompletePrivate();

    void init();

    QWidget *menu;
    QWidget *frame;
    MaterialAutoCompleteStateMachine *stateMachine;
    QVBoxLayout *menuLayout;
    QStringList dataSource;
    int maxWidth;
};

class MaterialAutoCompleteStateMachine : public QStateMachine
{
    Q_OBJECT
public:
    explicit MaterialAutoCompleteStateMachine(QWidget *menu);
    ~MaterialAutoCompleteStateMachine();

signals:
    void shouldOpen();
    void shouldClose();
    void shouldFade();
private:
    Q_DISABLE_COPY(MaterialAutoCompleteStateMachine)

    QWidget *const m_menu;
    QState *const m_closedState;
    QState *const m_openState;
    QState *const m_closingState;
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALAUTOCOMPLETE_P_H
