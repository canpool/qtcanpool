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
#ifndef QTITAN_MAINWINDOWTASKMENU_DSGN_H
#define QTITAN_MAINWINDOWTASKMENU_DSGN_H

#include <QPointer>

#include <QtDesigner/QExtensionFactory>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QDesignerTaskMenuExtension>

#include "QtitanDef.h"

namespace Qtitan
{
    /* MainWindowDsgnTaskMenu */
    class MainWindowDsgnTaskMenu : public QObject, public QDesignerTaskMenuExtension
    {
        Q_OBJECT
        Q_INTERFACES(QDesignerTaskMenuExtension)
    public:
        explicit MainWindowDsgnTaskMenu(QWidget* widget, QObject* parent);
        virtual ~MainWindowDsgnTaskMenu();

    public:
        QWidget *widget() const;
        QList<QAction*> taskActions() const;

    private Q_SLOTS:
        void slotCreateRibbonBar();

    protected:
        QDesignerFormWindowInterface* formWindow() const;

    private:
        QPointer<QWidget> m_widget;
        QAction* m_addRibbonBar;
    private:
        Q_DISABLE_COPY(MainWindowDsgnTaskMenu)
    };

    /* MainWindowMenuExFactory */
    class MainWindowMenuExFactory : public QExtensionFactory
    {
        Q_OBJECT
    public:
        explicit MainWindowMenuExFactory(QExtensionManager* parent = Q_NULL);
        virtual ~MainWindowMenuExFactory();

    protected:
        QObject* createExtension(QObject* object, const QString& iid, QObject* parent) const;
    };

}; //namespace Qtitan

#endif //QTITAN_MAINWINDOWTASKMENU_DSGN_H
