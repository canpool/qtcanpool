/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxribbon_global.h"
#include "ribbonstyleoption.h"

#include <QScopedPointer>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonControlButton;

class QX_RIBBON_EXPORT RibbonFactory
{
public:
    RibbonFactory();
    virtual ~RibbonFactory();

    virtual RibbonControlButton *createHideGroupButton(QWidget *parent);

    RibbonStyleOption &getRibbonStyleOption();
    void setRibbonStyleOption(RibbonStyleOption *opt);

private:
    QScopedPointer<RibbonStyleOption> m_opt;
};

QX_RIBBON_END_NAMESPACE
