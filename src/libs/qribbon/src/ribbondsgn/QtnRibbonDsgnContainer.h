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
#ifndef QTITAN_RIBBONCONTAINER_DSGN_H
#define QTITAN_RIBBONCONTAINER_DSGN_H

#include <QtDesigner/QExtensionFactory>
#include <QtDesigner/QDesignerContainerExtension>

#include "QtitanDef.h"

class QExtensionManager;


namespace Qtitan
{
    class RibbonBar;
    class RibbonPage;
    class RibbonTabBar;

    /* RibbonDsgnContainer */
    class RibbonTabBarFilter : public QObject
    {
        Q_OBJECT
    public:    
        explicit RibbonTabBarFilter(RibbonTabBar* pWidget);
        virtual ~RibbonTabBarFilter();
        static RibbonTabBarFilter* tf;
        static void install(QWidget* pWidget);
        static void uninstall(QWidget* pWidget, bool deleteThis = true);

    private:
        virtual bool eventFilter (QObject* watched, QEvent* event);

    private:
        RibbonTabBar* m_ribbonTabBar;
    };

    /* DesignerRibbonBarContainer */
    class DesignerRibbonBarContainer: public QObject, public QDesignerContainerExtension
    {
        Q_OBJECT
        Q_INTERFACES(QDesignerContainerExtension)
    public:
        DesignerRibbonBarContainer(RibbonBar* widget, QObject* parent);
    public:
        void addWidget(QWidget *widget);
        int count() const;
        int currentIndex() const;
        void insertWidget(int index, QWidget *widget);
        void remove(int index);
        void setCurrentIndex(int index);
        QWidget *widget(int index) const;
    private:
        RibbonBar* m_pRibbon;
    };

    /* DesignerRibbonBarContainerFactory */
    class DesignerRibbonBarContainerFactory: public QExtensionFactory
    {
        Q_OBJECT
    public:
        DesignerRibbonBarContainerFactory(QExtensionManager *parent = Q_NULL);
    protected:
        QObject *createExtension(QObject* object, const QString& iid, QObject* parent) const;
    };

}; //namespace Qtitan

#endif // QTITAN_RIBBONCONTAINER_DSGN_H
