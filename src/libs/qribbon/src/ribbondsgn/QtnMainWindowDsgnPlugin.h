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

#ifndef QTITAN_MAINWINDOW_DSGN_H
#define QTITAN_MAINWINDOW_DSGN_H

#include <QDesignerCustomWidgetInterface>

#include "QtitanDef.h"

namespace Qtitan
{
//    class RibbonStyle;
    class RibbonMainWindow;
    /* MainWindowDsgnPlugin */
    class MainWindowDsgnPlugin : public QObject, public QDesignerCustomWidgetInterface
    {
        Q_OBJECT
        Q_INTERFACES(QDesignerCustomWidgetInterface)
    public:
        MainWindowDsgnPlugin(QObject *parent = 0);
        virtual ~MainWindowDsgnPlugin();

    public:
        QString name() const;
        QString group() const;
        QString toolTip() const;
        QString whatsThis() const;
        QString includeFile() const;
        QIcon icon() const;
        bool isContainer() const;
        virtual QWidget *createWidget(QWidget *parent);
        virtual bool isInitialized() const;
        virtual void initialize(QDesignerFormEditorInterface* formEditor);
        virtual QString codeTemplate() const;
        QString domXml() const;

    private slots:
        void fileNameChanged(const QString &fileName);
    private:
        bool initialized;
        RibbonMainWindow* m_mainWindow;
        QDesignerFormEditorInterface* m_core;
    };

}; //namespace Qtitan

#endif //QTITAN_MAINWINDOW_DSGN_H
