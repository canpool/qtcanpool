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

#ifndef QTITAN_MAINWINDOWTASKMENU_DSGN_H
#define QTITAN_MAINWINDOWTASKMENU_DSGN_H

#include <QtDesigner/QExtensionFactory>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QDesignerTaskMenuExtension>

#include "QtitanDef.h"

namespace Qtitan
{
    class MainWindowDsgnTaskMenuPrivate;
    /* MainWindowDsgnTaskMenu */
    class MainWindowDsgnTaskMenu : public QObject, public QDesignerTaskMenuExtension
    {
        Q_OBJECT
        Q_INTERFACES(QDesignerTaskMenuExtension)
    public:
        MainWindowDsgnTaskMenu(QWidget* pWidget, QObject* parent);

    public:
        QWidget *widget() const;
        QList<QAction*> taskActions() const;

    private slots:
        void createRibbonBar();

    protected:
        QDesignerFormWindowInterface* formWindow() const;

    private:
        QTN_DECLARE_PRIVATE(MainWindowDsgnTaskMenu)
        Q_DISABLE_COPY(MainWindowDsgnTaskMenu)
    };

    /* MainWindowMenuExFactory */
    class MainWindowMenuExFactory : public QExtensionFactory
    {
        Q_OBJECT
    public:
        MainWindowMenuExFactory(QExtensionManager *parent = Q_NULL);

    protected:
        QObject *createExtension(QObject* object, const QString& iid, QObject* parent) const;
    };

}; //namespace Qtitan

#endif //QTITAN_MAINWINDOWTASKMENU_DSGN_H
