/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2013 Developer Machines (http://www.devmachines.com)
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
#include "QtnRibbonPlugins.h"

#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QStyleFactory>
#include <QStyle>
#include <QtPlugin>

#include "QtnMainWindowDsgnPlugin.h"
#include "QtnRibbonDsgnPlugin.h"
#include "QtnRibbonPageDsgnPlugin.h"
#include "QtnStatusBarDsgnPlugin.h"
#include "QtnRibbonBackstageViewPlugin.h"
#include "QtnRibbonStyleDsgnPlugin.h"

using namespace Qtitan;
/*
namespace Qtitan
{
    static QPointer<QStyle> s_ribbonStyle;
    void setOfficeStyle(QWidget& widget) 
    {
        if (!s_ribbonStyle)
            s_ribbonStyle = QStyleFactory::create("ribbonStyle");
        widget.setStyle(s_ribbonStyle);
        if (qobject_cast<QMainWindow*>(&widget))
            s_ribbonStyle->polish(qApp);
    }
    void setStyleChildren(QWidget& widget)
    {
        QList<QWidget*> child_list = qFindChildren<QWidget*>(&widget);
        for(int i = 0, count = child_list.count(); count > i; i++)
        {
            if (QWidget* pWd = child_list[i])
            {
                Qtitan::setOfficeStyle(*pWd);
                setStyleChildren(*pWd);
            }
        }
    }
} // namespace Qtitan;
*/
/* QtitanRibbonPlugins */
QtitanRibbonPlugins::QtitanRibbonPlugins(QObject *parent)
    : QObject(parent)
{
    m_plugins << new MainWindowDsgnPlugin(this) 
              << new RibbonBarDsgnPlugin(this) 
              << new RibbonPageDsgnPlugin(this)
              << new StatusBarDsgnPlugin(this)
              << new RibbonStyleDsgnPlugin(this)
              << new OfficeStyleDsgnPlugin(this)
              << new RibbonBackstageButtonPlugin(this)
              << new RibbonBackstageSeparatorPlugin(this)
              << new RibbonBackstagePagePlugin(this);
}

QList<QDesignerCustomWidgetInterface*> QtitanRibbonPlugins::customWidgets() const
{ 
    return m_plugins;
}

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
Q_EXPORT_PLUGIN2(qtnribbondsgn, QtitanRibbonPlugins)
#endif
