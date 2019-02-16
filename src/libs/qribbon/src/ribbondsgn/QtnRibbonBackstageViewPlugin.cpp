/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2011 Developer Machines (http://www.devmachines.com)
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
#include <QApplication>
#include <QAction>
#include <QLineEdit>

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QExtensionManager>
//#include <QtDesigner/QExtensionFactory>
#include <QtDesigner/QDesignerContainerExtension>

#include "QtnRibbonBackstageViewPlugin.h"
#include "QtnRibbonBackstageView.h"
#include "QtnRibbonStyle.h"

using namespace Qtitan;


/* RibbonBackstageButtonPlugin */
RibbonBackstageSeparatorPlugin::RibbonBackstageSeparatorPlugin(QObject* parent)
    : QObject(parent)
    , m_pStyle(new RibbonStyle())
{
    initialized = false;
}

RibbonBackstageSeparatorPlugin::~RibbonBackstageSeparatorPlugin()
{
    delete m_pStyle;
}

QString RibbonBackstageSeparatorPlugin::name() const
{
    return QLatin1String("Qtitan::RibbonBackstageSeparator");
}

QString RibbonBackstageSeparatorPlugin::group() const
{
     return "Developer Machines: Components";
}

QString RibbonBackstageSeparatorPlugin::toolTip() const
{
    return tr("Backstage separator for Ribbon Interface");
}

QString RibbonBackstageSeparatorPlugin::whatsThis() const
{
    return tr("Backstage separator for Ribbon Interface");
}

QString RibbonBackstageSeparatorPlugin::includeFile() const
{
    return QLatin1String("QtnRibbonBackstageView.h");
}

QIcon RibbonBackstageSeparatorPlugin::icon() const
{
    return QIcon(":res/ribbonbackstageseparator_icon.png");
}

bool RibbonBackstageSeparatorPlugin::isContainer() const
{
    return false;
}

QWidget* RibbonBackstageSeparatorPlugin::createWidget(QWidget* parent)
{
    RibbonBackstageSeparator* separator = new RibbonBackstageSeparator(parent);
    separator->setStyle(m_pStyle);
    return separator;
}

bool RibbonBackstageSeparatorPlugin::isInitialized() const
{
    return initialized;
}

void RibbonBackstageSeparatorPlugin::initialize(QDesignerFormEditorInterface* formEditor)
{
    Q_UNUSED(formEditor);
    if (initialized)
        return;
    initialized = true;

//    QExtensionManager* manager = formEditor->extensionManager();
//    Q_ASSERT(manager != Q_NULL);
    //    pManager->registerExtensions(new MainWindowContainerFactory(m_pStyle, pManager), Q_TYPEID(QDesignerContainerExtension));
}

QString RibbonBackstageSeparatorPlugin::codeTemplate() const
{ 
    return QString(); 
}

QString RibbonBackstageSeparatorPlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
        " <widget class=\"Qtitan::RibbonBackstageSeparator\" name=\"backstageSeparator\">\n"
        "  <property name=\"geometry\">\n"
        "   <rect>\n"
        "    <width>120</width>\n"
        "    <height>3</height>\n"
        "   </rect>\n"
        "  </property>\n"
        " </widget>\n"
        "</ui>\n";
}




/* RibbonBackstageButtonPlugin */
RibbonBackstageButtonPlugin::RibbonBackstageButtonPlugin(QObject *parent)
    : QObject(parent)
    , m_pStyle(new RibbonStyle())
{
    initialized = false;
}

RibbonBackstageButtonPlugin::~RibbonBackstageButtonPlugin()
{
    delete m_pStyle;
}

QString RibbonBackstageButtonPlugin::name() const
{
    return QLatin1String("Qtitan::RibbonBackstageButton");
}

QString RibbonBackstageButtonPlugin::group() const
{
    return "Developer Machines: Components";
}

QString RibbonBackstageButtonPlugin::toolTip() const
{
    return tr("Backstage Button for Ribbon Interface");
}

QString RibbonBackstageButtonPlugin::whatsThis() const
{
    return tr("Backstage Button for Ribbon Interface");
}

QString RibbonBackstageButtonPlugin::includeFile() const
{
    return QLatin1String("QtnRibbonBackstageView.h");
}

QIcon RibbonBackstageButtonPlugin::icon() const
{
    return QIcon(":res/ribbonbackstagebutton_icon.png");
}

bool RibbonBackstageButtonPlugin::isContainer() const
{
    return false;
}

QWidget* RibbonBackstageButtonPlugin::createWidget(QWidget* parent)
{
    RibbonBackstageButton* button = new RibbonBackstageButton(parent);
    button->setStyle(m_pStyle);
    return button;
}

bool RibbonBackstageButtonPlugin::isInitialized() const
{
    return initialized;
}

void RibbonBackstageButtonPlugin::initialize(QDesignerFormEditorInterface* formEditor)
{
    Q_UNUSED(formEditor);
    if (initialized)
        return;
    initialized = true;

//    QExtensionManager* manager = formEditor->extensionManager();
//    Q_ASSERT(manager != Q_NULL);
//    pManager->registerExtensions(new MainWindowContainerFactory(m_pStyle, pManager), Q_TYPEID(QDesignerContainerExtension));
}

QString RibbonBackstageButtonPlugin::codeTemplate() const
{ 
    return QString(); 
}

QString RibbonBackstageButtonPlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
        " <widget class=\"Qtitan::RibbonBackstageButton\" name=\"backstageButton\">\n"
        "  <property name=\"geometry\">\n"
        "   <rect>\n"
        "    <width>100</width>\n"
        "    <height>100</height>\n"
        "   </rect>\n"
        "  </property>\n"
        " </widget>\n"
        "</ui>\n";
}



/* RibbonBackstageButtonPlugin */
RibbonBackstagePagePlugin::RibbonBackstagePagePlugin(QObject *parent)
    : QObject(parent)
    , m_pStyle(new RibbonStyle())
{
    initialized = false;
}

RibbonBackstagePagePlugin::~RibbonBackstagePagePlugin()
{
    delete m_pStyle;
}

QString RibbonBackstagePagePlugin::name() const
{
    return QLatin1String("Qtitan::RibbonBackstagePage");
}

QString RibbonBackstagePagePlugin::group() const
{
    return QLatin1String("[invisible]");
}

QString RibbonBackstagePagePlugin::toolTip() const
{
    return "Advanced RibbonBackstagePage for Nokia-Qt";
}

QString RibbonBackstagePagePlugin::whatsThis() const
{
//    return "Advanced RibbonBackstagePage for Nokia-Qt";
    return QString();
}

QString RibbonBackstagePagePlugin::includeFile() const
{
    return QLatin1String("QtnRibbonBackstageView.h");
}

QIcon RibbonBackstagePagePlugin::icon() const
{
    return QIcon(":res/ribbonbackstagepage_icon.png");
}

bool RibbonBackstagePagePlugin::isContainer() const
{
    return true;
}

QWidget* RibbonBackstagePagePlugin::createWidget(QWidget* parent)
{
    RibbonBackstagePage* page = new RibbonBackstagePage(parent);
    page->setStyle(m_pStyle);
    return page;
}

bool RibbonBackstagePagePlugin::isInitialized() const
{
    return initialized;
}

void RibbonBackstagePagePlugin::initialize(QDesignerFormEditorInterface* formEditor)
{
    Q_UNUSED(formEditor);
    if (initialized)
        return;
    initialized = true;

//    QExtensionManager* manager = formEditor->extensionManager();
//    Q_ASSERT(manager != Q_NULL);
    //    pManager->registerExtensions(new MainWindowContainerFactory(m_pStyle, pManager), Q_TYPEID(QDesignerContainerExtension));
}

QString RibbonBackstagePagePlugin::codeTemplate() const
{ 
    return QString(); 
}

QString RibbonBackstagePagePlugin::domXml() const
{
    return QLatin1String("\
                         <ui language=\"c++\">\
                             <widget class=\"Qtitan::RibbonBackstagePage\" name=\"backstagePage\">\
                                <widget class=\"QWidget\" name=\"page\" />\
                             </widget>\
                             <customwidgets>\
                                 <customwidget>\
                                     <class>Qtitan::RibbonBackstagePage</class>\
                                     <extends>QWidget</extends>\
                                 </customwidget>\
                             </customwidgets>\
                         </ui>");
}

