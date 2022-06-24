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
#ifndef QTITAN_RIBBONTASKMENU_DSGN_H
#define QTITAN_RIBBONTASKMENU_DSGN_H

#include <QDesignerTaskMenuExtension>
#include <QtDesigner/QExtensionFactory>
#include "QtnRibbonPage.h"


namespace Qtitan
{
    class RibbonBar;

    /* DsgnRibbonTaskMenu for RibbonBar*/
    class DsgnRibbonTaskMenu : public QObject, public QDesignerTaskMenuExtension
    {
        Q_OBJECT
        Q_INTERFACES(QDesignerTaskMenuExtension)
    public:
        DsgnRibbonTaskMenu(RibbonBar* pRibbon, QObject* parent);
        virtual ~DsgnRibbonTaskMenu();
    public:
        virtual QAction* preferredEditAction() const;
        virtual QList<QAction*> taskActions() const;

    private Q_SLOTS:
        void slot_removeRibbonBar();
        void slot_addNewPage();
        void slot_removeCurrentPage();
    private:
        QAction* m_removeRebbonBar;
        QAction* m_addRibbonPage;
        QAction* m_deleteRibbonPage;
        QAction* m_separator;
        RibbonBar* m_pRibbonBar;
    };

    class RibbonStyleWidget;
    /* DsgnRibbonTaskMenu for Styles*/
    class StyleDsgnTaskMenu : public QObject, public QDesignerTaskMenuExtension
    {
        Q_OBJECT
        Q_INTERFACES(QDesignerTaskMenuExtension)
    public:
        StyleDsgnTaskMenu(QWidget* parentStyle, QObject* parent);
        virtual ~StyleDsgnTaskMenu();
    public:
        virtual QAction* preferredEditAction() const;
        virtual QList<QAction*> taskActions() const;

    private Q_SLOTS:
        void slotRemoveStyle();
    private:
        QAction* m_removeStyle;
        QWidget* m_parentStyle;
    };

    class DsgnRibbonPageTaskMenu: public QObject, public QDesignerTaskMenuExtension
    {
        Q_OBJECT
        Q_INTERFACES(QDesignerTaskMenuExtension)
    public:
        DsgnRibbonPageTaskMenu(RibbonPage* page, QObject* parent);
        virtual ~DsgnRibbonPageTaskMenu();
    public:
        virtual QAction* preferredEditAction() const;
        virtual QList<QAction*> taskActions() const;

    private Q_SLOTS:
        void contextMenuRequested(QMenu *menu, QWidget *widget);
        void slotAddGroup();
        void slotRemovePage();
    private:
        RibbonPage* m_ribbonPage;
        QAction* m_separatorAction;
        QAction* m_addGroupAction;
        QAction* m_removePageAction;
    };

    class DsgnRibbonGroupTaskMenu: public QObject, public QDesignerTaskMenuExtension
    {
        Q_OBJECT
        Q_INTERFACES(QDesignerTaskMenuExtension)
    public:
        DsgnRibbonGroupTaskMenu(RibbonGroup* group, QObject* parent);
    public:
        virtual QAction* preferredEditAction() const;
        virtual QList<QAction*> taskActions() const;

    private Q_SLOTS:
        void contextMenuRequested(QMenu *menu, QWidget *widget);
        void slotRemoveRibbonGroup();
        void slotRemoveRibbonPage();
    private:
        QAction* m_removeGroupAction;
        QAction* m_separatorAction;
        QAction* m_removePageAction;
        RibbonGroup* m_ribbonGroup;
    };

    /* DsgnRibbonTaskMenuFactory */
    class DsgnRibbonTaskMenuFactory: public QExtensionFactory
    {
        Q_OBJECT
    public:
        DsgnRibbonTaskMenuFactory(QExtensionManager *parent = 0);
    protected:
        QObject* createExtension(QObject* object, const QString& iid, QObject* parent) const;
    };
}; //namespace Qtitan

#endif // QTITAN_RIBBONTASKMENU_DSGN_H
