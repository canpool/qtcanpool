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
#include <QStyle>
#include <QMessageBox>
#include <QUndoStack>
#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerFormWindowManagerInterface>
#include <QtDesigner/QDesignerContainerExtension>
#include <QtDesigner/QDesignerMetaDataBaseInterface>
#include <QtDesigner/QExtensionManager>

#include <QVariant>

#include "QtnStatusBarDsgnPlugin.h"
#include "QtnRibbonStatusBar.h"
#include "QtnRibbonMainWindow.h"

QTITAN_USE_NAMESPACE


/* DsgnRibbonStatusBarPlugin */
DsgnRibbonStatusBarPlugin::DsgnRibbonStatusBarPlugin(QObject* parent)
    : QObject(parent), m_core(0)
{
    initialized = false;
}

void DsgnRibbonStatusBarPlugin::initialize(QDesignerFormEditorInterface* core)
{
    if (initialized)
        return;

    initialized = true;
    m_core = core;
    connect(core->formWindowManager(), SIGNAL(formWindowAdded(QDesignerFormWindowInterface*)), 
        this, SLOT(formWindowAdded(QDesignerFormWindowInterface*)));
}

bool DsgnRibbonStatusBarPlugin::isInitialized() const
{
    return initialized;
}

void DsgnRibbonStatusBarPlugin::formWindowAdded(QDesignerFormWindowInterface *formWindow)
{
    connect(formWindow, SIGNAL(widgetManaged(QWidget*)), this, SLOT(widgetManaged(QWidget*)));
}

void DsgnRibbonStatusBarPlugin::formWindowRemoved(QDesignerFormWindowInterface *formWindow)
{
    disconnect(formWindow, SIGNAL(widgetManaged(QWidget*)), this, SLOT(widgetManaged(QWidget*)));
}

void DsgnRibbonStatusBarPlugin::widgetManaged(QWidget* widget)
{
    if (widget->metaObject()->className() == create_qtitan_className("RibbonStatusBar"))
    {
        QDesignerFormWindowInterface* formWindow = static_cast<QDesignerFormWindowInterface *>(sender());
        QDesignerFormEditorInterface* core = formWindow->core();
        QDesignerContainerExtension* container = qt_extension<QDesignerContainerExtension*>(core->extensionManager(), formWindow->mainContainer());
        formWindow->unmanageWidget(widget);

        QUndoStack* stack = formWindow->commandHistory();
        if (!stack->isClean())
        {
            //This code check the InsertWidget command on the stack.
            const QUndoCommand* command = stack->command(stack->index());
            if (command->childCount() == 0)
                return;
        }

        if (qobject_cast<QMainWindow *>(formWindow->mainContainer()) == 0)
        {
                QMessageBox::critical(
                    formWindow->mainContainer(),
                    tr("Can't add Ribbon StatusBar"), 
                    tr("You can add Ribbon StatusBar onto RibbonMainWindow or QMainWindow only."));
                widget->deleteLater();
                return;
        }

        for (int i = 0; i < container->count(); ++i)
        {
            QWidget* w = container->widget(i);
            if (w->metaObject()->className() == create_qtitan_className("RibbonStatusBar") ||
                w->metaObject()->className() == QString("QStatusBar"))
            {
                QMessageBox::critical(
                    formWindow->mainContainer(),
                    tr("Can't add Ribbon StatusBar"), 
                    tr("Only one instance of the Ribbon StatusBar can be adding to the main form."));
                widget->deleteLater();
                return;
            }
        }

        container->addWidget(widget);
        formWindow->core()->metaDataBase()->add(widget);
    }
}

QWidget* DsgnRibbonStatusBarPlugin::createWidget(QWidget* pParent)
{
    QWidget* ptr = new RibbonStatusBar(pParent);
#if 0
    if (pParent)
    {
        QStyle* pStyle = pParent->style();
        ptr->setStyle(pStyle);
        pStyle->polish(ptr);
    }
#endif
    return ptr;
}

QString DsgnRibbonStatusBarPlugin::name() const
{
    return create_qtitan_className("RibbonStatusBar");
}

QString DsgnRibbonStatusBarPlugin::group() const
{
    return "Developer Machines: Components";
}

QIcon DsgnRibbonStatusBarPlugin::icon() const
{
    return QIcon(":res/statusbar_icon.png");
}

QString DsgnRibbonStatusBarPlugin::toolTip() const
{
    return "StatusBar for Ribbon Interface";
}

QString DsgnRibbonStatusBarPlugin::whatsThis() const
{
    return "StatusBar for Ribbon Interface";
}

bool DsgnRibbonStatusBarPlugin::isContainer() const
{
    return false;
}

QString DsgnRibbonStatusBarPlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
        " <widget class=\"" + create_qtitan_className("RibbonStatusBar") + "\" name=\"statusBar\"/>\n"
        " <customwidgets>\n"
        "   <customwidget>\n"
        "       <class>" + create_qtitan_className("RibbonStatusBar") + "</class>\n"
        "       <extends>QStatusBar</extends>\n"
        "   </customwidget>\n"
        " </customwidgets>\n"
        "</ui>\n";
}

QString DsgnRibbonStatusBarPlugin::includeFile() const
{
    return "QtnRibbonStatusBar.h";
}


