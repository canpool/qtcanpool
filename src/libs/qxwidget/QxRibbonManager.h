/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "QxGlobal.h"
#include "QxRibbonFactory.h"

QX_BEGIN_NAMESPACE

class QX_WIDGET_EXPORT RibbonManager
{
protected:
    RibbonManager();
public:
    virtual ~RibbonManager();
    static RibbonManager *instance();
    RibbonFactory *factory();
    void setupFactory(RibbonFactory *factory);
private:
    RibbonFactory *m_factory;
};

#ifndef RibbonElementMgr
#define RibbonElementMgr RibbonManager::instance()
#endif

#ifndef RibbonElementFactory
#define RibbonElementFactory RibbonElementMgr->factory()
#endif

#ifndef RibbonElementStyleOpt
#define RibbonElementStyleOpt RibbonElementFactory->getRibbonStyleOption()
#endif

QX_END_NAMESPACE
