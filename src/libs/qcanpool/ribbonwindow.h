/**
 * Copyright (C) 2021-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#ifndef RIBBONWINDOW_H
#define RIBBONWINDOW_H

#include "qcanpool.h"
#include <QMainWindow>

QCANPOOL_BEGIN_NAMESPACE

class RibbonBar;
class RibbonWindowPrivate;

/* RibbonWindow */
class QCANPOOL_SHARED_EXPORT RibbonWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit RibbonWindow(QWidget *parent = nullptr);
    virtual ~RibbonWindow();

    RibbonBar *ribbonBar() const;

    void setFrameless(bool frameless);

private:
    RibbonWindowPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // RIBBONWINDOW_H
