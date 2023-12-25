/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#ifndef QXMATERIALDIALOG_P_H
#define QXMATERIALDIALOG_P_H

#include "qxmaterial_global.h"
#include <QWidget>

class QStateMachine;
class QStackedLayout;

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialDialog;
class MaterialDialogWindow;
class MaterialDialogProxy;

class MaterialDialogPrivate
{
    Q_DISABLE_COPY(MaterialDialogPrivate)
    Q_DECLARE_PUBLIC(MaterialDialog)
public:
    MaterialDialogPrivate(MaterialDialog *q);
    ~MaterialDialogPrivate();

    void init();

    MaterialDialog *const q_ptr;
    MaterialDialogWindow *dialogWindow;
    QStackedLayout *proxyStack;
    QStateMachine *stateMachine;
    MaterialDialogProxy *proxy;
};

class MaterialDialogProxy : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(qreal opacity WRITE setOpacity READ opacity)

    enum TransparencyMode {
        Transparent,
        SemiTransparent,
        Opaque,
    };
public:
    MaterialDialogProxy(MaterialDialogWindow *source, QStackedLayout *layout, MaterialDialog *dialog,
                        QWidget *parent = 0);
    ~MaterialDialogProxy();

    void setOpacity(qreal opacity);
    inline qreal opacity() const;

protected slots:
    void makeOpaque();
    void makeTransparent();

    QSize sizeHint() const Q_DECL_OVERRIDE;
protected:
    bool event(QEvent *event) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    Q_DISABLE_COPY(MaterialDialogProxy)

    MaterialDialogWindow *const m_source;
    QStackedLayout *const m_layout;
    MaterialDialog *const m_dialog;
    qreal m_opacity;
    TransparencyMode m_mode;
};

inline qreal MaterialDialogProxy::opacity() const
{
    return m_opacity;
}

class MaterialDialogWindow : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(int offset WRITE setOffset READ offset)
public:
    explicit MaterialDialogWindow(MaterialDialog *dialog, QWidget *parent = 0);
    ~MaterialDialogWindow();

    void setOffset(int offset);
    int offset() const;
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    Q_DISABLE_COPY(MaterialDialogWindow)

    MaterialDialog *const m_dialog;
};

QX_MATERIAL_END_NAMESPACE

#endif   // QXMATERIALDIALOG_P_H
