/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef EXTENSIONBUTTON_H
#define EXTENSIONBUTTON_H

#include "qcanpool.h"
#include <QToolButton>

QCANPOOL_BEGIN_NAMESPACE

class ExtensionButtonPrivate;

class QCANPOOL_SHARED_EXPORT ExtensionButton : public QToolButton
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
    QCP_DECLARE_PRIVATE(ExtensionButton)
};

QCANPOOL_END_NAMESPACE

#endif // EXTENSIONBUTTON_H
