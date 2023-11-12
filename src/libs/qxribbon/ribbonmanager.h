/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "qxribbon_global.h"
#include "ribbonfactory.h"

QX_RIBBON_BEGIN_NAMESPACE

class QX_RIBBON_EXPORT RibbonManager
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

QX_RIBBON_END_NAMESPACE
