/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef EXTENSIONBUTTON_H
#define EXTENSIONBUTTON_H

#include "qxwidget_global.h"
#include <QToolButton>

QX_WIDGET_BEGIN_NAMESPACE

class ExtensionButtonPrivate;

class QX_WIDGET_EXPORT ExtensionButton : public QToolButton
{
    Q_OBJECT
public:
    explicit ExtensionButton(QWidget *parent = nullptr);
    ~ExtensionButton();

    QSize sizeHint() const override;

public Q_SLOTS:
    void setOrientation(Qt::Orientation o);

protected:
    void paintEvent(QPaintEvent *) override;
    bool event(QEvent *event) override;
private:
    QX_DECLARE_PRIVATE(ExtensionButton)
};

QX_WIDGET_END_NAMESPACE

#endif // EXTENSIONBUTTON_H
