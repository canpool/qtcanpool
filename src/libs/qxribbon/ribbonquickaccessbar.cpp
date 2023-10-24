/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#include "ribbonquickaccessbar.h"
#include "ribbonbuttongroup.h"
#include "quickaccessbar.h"

QX_RIBBON_BEGIN_NAMESPACE

class RibbonQuickAccessBarPrivate
{
    QX_DECLARE_PUBLIC(RibbonQuickAccessBar)
public:
    QuickAccessBar *m_accessBar;
};

RibbonQuickAccessBar::RibbonQuickAccessBar(QWidget *parent)
    : RibbonCtrlContainer(parent)
{
    QX_INIT_PRIVATE(RibbonQuickAccessBar)
    Q_D(RibbonQuickAccessBar);
    d->m_accessBar = new QuickAccessBar(this);
    d->m_accessBar->setObjectName(QStringLiteral("qx_QuickAccessBar"));
    setWidget(d->m_accessBar);
    setTitleVisible(false);
}

RibbonQuickAccessBar::~RibbonQuickAccessBar()
{
    QX_FINI_PRIVATE()
}

QuickAccessBar *RibbonQuickAccessBar::accessBar() const
{
    Q_D(const RibbonQuickAccessBar);
    return d->m_accessBar;
}

QX_RIBBON_END_NAMESPACE
