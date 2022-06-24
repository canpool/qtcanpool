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

#ifndef QTN_RIBBONGROUP_DSGN_H
#define QTN_RIBBONGROUP_DSGN_H

#include <QExtensionFactory>

#if QT_VERSION >= 0x050500
    #include <QtUiPlugin/QDesignerCustomWidgetInterface>
#else
    #include <QtDesigner/QDesignerCustomWidgetInterface>
#endif
#include <QDesignerContainerExtension>
#include "QtnRibbonPage.h"

namespace Qtitan
{
    /* DsgnRibbonGroupPlugin */
    class DsgnRibbonGroupPlugin: public QObject, public QDesignerCustomWidgetInterface
    {
        Q_OBJECT
        Q_INTERFACES(QDesignerCustomWidgetInterface)
    public:
        DsgnRibbonGroupPlugin(QObject* parent = 0);
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
    private Q_SLOTS:
        void titleChanged(const QString& title);
    private:
        bool initialized;
        QDesignerFormEditorInterface* m_core;
    };

    /* DsgnRibbonPageContainer */
    class DsgnRibbonPageContainer: public QObject, public QDesignerContainerExtension
    {
        Q_OBJECT
        Q_INTERFACES(QDesignerContainerExtension)
    public:
        DsgnRibbonPageContainer(RibbonPage* widget, QObject* parent);
    public:
        void addWidget(QWidget *widget);
        int count() const;
        int currentIndex() const;
        void insertWidget(int index, QWidget *widget);
        void remove(int index);
        void setCurrentIndex(int index);
        QWidget *widget(int index) const;
    private:
        RibbonPage* m_ribbonPage;
    };

    /* DsgnRibbonPageContainerFactory */
    class DsgnRibbonPageContainerFactory : public QExtensionFactory
    {
        Q_OBJECT
    public:
        explicit DsgnRibbonPageContainerFactory(QExtensionManager* parent);
    protected:
        virtual QObject *createExtension(QObject* object, const QString& iid, QObject* parent) const;
    };
}; //namespace Qtitan

#endif //QTN_RIBBONGROUP_DSGN_H

