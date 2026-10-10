/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXEXTENSIONBUTTON_H
#define QXEXTENSIONBUTTON_H

#include "qxapp_global.h"
#include <QToolButton>

QX_APP_BEGIN_NAMESPACE

class QxExtensionButtonPrivate;

class QX_APP_EXPORT QxExtensionButton : public QToolButton
{
    Q_OBJECT
public:
    explicit QxExtensionButton(QWidget *parent = nullptr);
    ~QxExtensionButton();

    QSize sizeHint() const override;

public Q_SLOTS:
    void setOrientation(Qt::Orientation o);
protected:
    void paintEvent(QPaintEvent *) override;
    bool event(QEvent *event) override;
private:
    QX_DECLARE_PRIVATE(QxExtensionButton)
};

QX_APP_END_NAMESPACE

#endif   // QXEXTENSIONBUTTON_H
