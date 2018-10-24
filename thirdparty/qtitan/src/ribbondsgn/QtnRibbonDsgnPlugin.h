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

#ifndef QTN_RIBBON_DSGN_H
#define QTN_RIBBON_DSGN_H

#include <QtDesigner/QExtensionFactory>
#include <QDesignerCustomWidgetInterface>

#include "QtnRibbonBar.h"
#include "QtitanDef.h"

class QAction;
class QDesignerFormWindowInterface;
class QContextMenuEvent;

namespace Qtitan
{
    /* RibbonEventFilter */
    class RibbonEventFilter : public QObject
    {
        Q_OBJECT
    public:
        explicit RibbonEventFilter(QWidget* pWidget);
        virtual ~RibbonEventFilter();
        static RibbonEventFilter* rf; 
        static void install(QWidget* pWidget);
        static void uninstall(QWidget* pWidget, bool deleteThis = true);

    protected:
        QDesignerFormWindowInterface* formWindow() const;

    private slots:
        void slotRemoveRibbonBar();
        void slotAddNewPage();
        void slotRemoveCurrentPage();

    private:
        virtual bool eventFilter (QObject* watched, QEvent* event);
        bool handleContextMenuEvent(QContextMenuEvent* event);

    private:
        QWidget* m_pRibbonBar;
        QAction* m_removeRibbonBar;
        QAction* m_addRibbonPage;
        QAction* m_deleteRibbonPage;
        Q_DISABLE_COPY(RibbonEventFilter)
    };


    /* RibbonBarDsgnPlugin */
    class RibbonBarDsgnPlugin : public QObject, public QDesignerCustomWidgetInterface
    {                                               
        Q_OBJECT
        Q_INTERFACES(QDesignerCustomWidgetInterface)
    public:
        RibbonBarDsgnPlugin(QObject* parent = Q_NULL);

    public:
        bool isContainer() const;
        bool isInitialized() const;
        QIcon icon() const;
        QString domXml() const;
        QString group() const;
        QString includeFile() const;
        QString name() const;
        QString toolTip() const;
        QString whatsThis() const;
        QWidget* createWidget(QWidget* parent);
        void initialize(QDesignerFormEditorInterface* core);

    private slots:
        void currentPageChanged(int index);
        void formWindowAdded(QDesignerFormWindowInterface *formWindow);
        void formWindowRemoved(QDesignerFormWindowInterface *formWindow);
        void widgetManaged(QWidget* widget);
    private:
        bool initialized;
        QDesignerFormEditorInterface* m_core;
        Q_DISABLE_COPY(RibbonBarDsgnPlugin)
    };

}; //namespace Qtitan



#endif //QTN_RIBBON_DSGN_H
