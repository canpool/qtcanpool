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
#ifndef QTITAN_RIBBONBACKSTAGE_DSGN_H
#define QTITAN_RIBBONBACKSTAGE_DSGN_H

#include <QtGlobal>

#if QT_VERSION >= 0x050500
    #include <QtUiPlugin/QDesignerCustomWidgetInterface>
#else
    #include <QtDesigner/QDesignerCustomWidgetInterface>
#endif

#include <QExtensionFactory>
#include <QDesignerTaskMenuExtension>
#include <QDesignerContainerExtension>
#include "QtitanDef.h"


namespace Qtitan
{
    class RibbonStyle;
    /* RibbonBackstageSeparatorPlugin */
    class RibbonBackstageSeparatorPlugin : public QObject, public QDesignerCustomWidgetInterface
    {
        Q_OBJECT
        Q_INTERFACES(QDesignerCustomWidgetInterface)
    public:
        explicit RibbonBackstageSeparatorPlugin(QObject* parent);
        virtual ~RibbonBackstageSeparatorPlugin();

    public:
        virtual QString name() const;
        virtual QString group() const;
        virtual QString toolTip() const;
        virtual QString whatsThis() const;
        virtual QString includeFile() const;
        virtual QIcon icon() const;
        virtual bool isContainer() const;
        virtual QWidget *createWidget(QWidget* parent);
        virtual bool isInitialized() const;
        virtual void initialize(QDesignerFormEditorInterface* formEditor);
        virtual QString codeTemplate() const;
        QString domXml() const;

    protected:
        bool initialized;
        RibbonStyle* m_pStyle;
    };


    /* RibbonBackstageButtonPlugin */
    class RibbonBackstageButtonPlugin : public QObject, public QDesignerCustomWidgetInterface
    {
        Q_OBJECT
        Q_INTERFACES(QDesignerCustomWidgetInterface)
    public:
        explicit RibbonBackstageButtonPlugin(QObject *parent = 0);
        virtual ~RibbonBackstageButtonPlugin();

    public:
        virtual QString name() const;
        virtual QString group() const;
        virtual QString toolTip() const;
        virtual QString whatsThis() const;
        virtual QString includeFile() const;
        virtual QIcon icon() const;
        virtual bool isContainer() const;
        virtual QWidget *createWidget(QWidget *parent);
        virtual bool isInitialized() const;
        virtual void initialize(QDesignerFormEditorInterface* formEditor);
        virtual QString codeTemplate() const;
        QString domXml() const;

    private:
        bool initialized;
        RibbonStyle* m_pStyle;
    };

    /* RibbonBackstagePagePlugin */
    class RibbonBackstagePagePlugin : public QObject, public QDesignerCustomWidgetInterface
    {
        Q_OBJECT
        Q_INTERFACES(QDesignerCustomWidgetInterface)
    public:
        explicit RibbonBackstagePagePlugin(QObject *parent = 0);
        virtual ~RibbonBackstagePagePlugin();

    public:
        virtual QString name() const;
        virtual QString group() const;
        virtual QString toolTip() const;
        virtual QString whatsThis() const;
        virtual QString includeFile() const;
        virtual QIcon icon() const;
        bool isContainer() const;
        virtual QWidget *createWidget(QWidget *parent);
        virtual bool isInitialized() const;
        virtual void initialize(QDesignerFormEditorInterface* formEditor);
        QString domXml() const;

    private:
        bool initialized;
        RibbonStyle* m_pStyle;
    };
}; //namespace Qtitan

#endif // QTITAN_RIBBONBACKSTAGE_DSGN_H
