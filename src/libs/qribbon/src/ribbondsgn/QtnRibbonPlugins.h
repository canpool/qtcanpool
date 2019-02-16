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

#ifndef QTITAN_RIBBON_DSGN_H
#define QTITAN_RIBBON_DSGN_H

//#include <QAccessibleWidgetEx>
#include <QDesignerCustomWidgetInterface>

class QWidget;
namespace Qtitan
{
    /* QtitanRibbonPlugins */
    class QtitanRibbonPlugins: public QObject, public QDesignerCustomWidgetCollectionInterface
    {
        Q_OBJECT
        Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        Q_PLUGIN_METADATA(IID "org.devmachines.Qt.QDesignerCustomWidgetCollectionInterface")
#endif
    public:
        QtitanRibbonPlugins(QObject *parent = 0);
        QList<QDesignerCustomWidgetInterface*> customWidgets() const;

    private:
        QList<QDesignerCustomWidgetInterface *> m_plugins;
    };
    // -----
//    void setOfficeStyle(QWidget& widget);
//    void setStyleChildren(QWidget& widget);

}; //namespace Qtitan

#endif //QTITAN_RIBBON_DSGN_H
