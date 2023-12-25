/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALSNACKBAR_P_H
#define QXMATERIALSNACKBAR_P_H

#include "qxmaterial_global.h"
#include <QColor>
#include <QObject>
#include <QStateMachine>
#include <QTimer>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialSnackbar;
class MaterialSnackbarStateMachine;

class MaterialSnackbarPrivate
{
    Q_DISABLE_COPY(MaterialSnackbarPrivate)
    Q_DECLARE_PUBLIC(MaterialSnackbar)
public:
    MaterialSnackbarPrivate(MaterialSnackbar *q);
    ~MaterialSnackbarPrivate();

    void init();

    MaterialSnackbar *const q_ptr;
    MaterialSnackbarStateMachine *stateMachine;
    QColor backgroundColor;
    QColor textColor;
    qreal bgOpacity;
    QList<QString> messages;
    int duration;
    int boxWidth;
    bool clickDismiss;
    bool useThemeColors;
};

class MaterialSnackbarStateMachine : public QStateMachine
{
    Q_OBJECT

    Q_PROPERTY(qreal offset WRITE setOffset READ offset)
public:
    MaterialSnackbarStateMachine(MaterialSnackbar *parent);
    ~MaterialSnackbarStateMachine();

    void setOffset(qreal offset);
    inline qreal offset() const;

public slots:
    void progress();

protected slots:
    void snackbarShown();
protected:
    bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;
private:
    Q_DISABLE_COPY(MaterialSnackbarStateMachine)

    MaterialSnackbar *const m_snackbar;
    QTimer m_timer;
    qreal m_offset;
};

inline qreal MaterialSnackbarStateMachine::offset() const
{
    return m_offset;
}

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALSNACKBAR_P_H
