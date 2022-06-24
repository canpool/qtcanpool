/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2015 Developer Machines (http://www.devmachines.com)
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/
#include "QtnRibbonDsgnPlugins.h"

#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QStyleFactory>
#include <QStyle>
#include <QtPlugin>

#include "QtnRibbonMainWindowDsgnPlugin.h"
#include "QtnRibbonBarDsgnPlugin.h"
#include "QtnRibbonPageDsgnPlugin.h"
#include "QtnRibbonGroupDsgnPlugin.h"
#include "QtnRibbonWorkspaceDsgnPlugin.h"
#include "QtnStatusBarDsgnPlugin.h"
#include "QtnRibbonBackstageViewDsgnPlugin.h"
#include "QtnRibbonStyleDsgnPlugin.h"
#include "QtnRibbonPageDsgnPlugin.h"

QTITAN_USE_NAMESPACE

/* QtitanRibbonDsgnPlugins */
QtitanRibbonDsgnPlugins::QtitanRibbonDsgnPlugins(QObject *parent)
    : QObject(parent)
{
    m_plugins << new RibbonMainWindowDsgnPlugin(this)
        << new DsgnRibbonBarPlugin(this)
        << new DsgnRibbonPagePlugin(this)
        << new DsgnRibbonGroupPlugin(this)
        << new RibbonWorkspaceDsgnPlugin(this)
        << new DsgnRibbonStatusBarPlugin(this)
        << new RibbonStyleDsgnPlugin(this)
        << new RibbonBackstageButtonPlugin(this)
        << new RibbonBackstageSeparatorPlugin(this)
        << new RibbonBackstagePagePlugin(this)
        << new OfficeStyleDsgnPlugin(this);
}

QList<QDesignerCustomWidgetInterface*> QtitanRibbonDsgnPlugins::customWidgets() const
{ 
    return m_plugins;
}

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
Q_EXPORT_PLUGIN2(qtnribbondsgn, QtitanRibbonDsgnPlugins)
#endif
