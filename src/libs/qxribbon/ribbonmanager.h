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

    /*!
     * The process-wide instance, created on first use.
     */
    static RibbonManager *instance();

    /*!
     * Returns the factory every ribbon element is created from, never null.
     *
     * A default factory is installed at construction and put back whenever
     * setupFactory() is passed null, so the RibbonElementFactory macro - which
     * walks through here - always lands on a usable object.
     */
    RibbonFactory *factory();

    /*!
     * Installs \a factory in place of the current one, taking ownership of it
     * and deleting the factory it replaces.
     *
     * Passing the factory that is already installed does nothing rather than
     * deleting it, and passing null puts the default factory back: the manager
     * never leaves itself without a factory, because every use of
     * RibbonElementFactory dereferences what factory() returns.
     */
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
