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
#include <QtDesigner/QDesignerFormEditorInterface>


#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include <QStatusBar>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerWidgetFactoryInterface>
#include <QtDesigner/QDesignerMetaDataBaseInterface>
#include <QtDesigner/QDesignerWidgetBoxInterface>
#endif

#include <QtDesigner/QExtensionManager>

#include "QtnRibbonMainWindow.h"
#include "QtnMainWindowDsgnContainer.h"
#include "QtnRibbonMainWindowDsgnPlugin.h"
#include "QtnMainWindowDsgnTaskMenu.h"

QTITAN_USE_NAMESPACE


/* RibbonMainWindowDsgnPlugin */
RibbonMainWindowDsgnPlugin::RibbonMainWindowDsgnPlugin(QObject *parent)
    : QObject(parent)
    , m_initialized(false)
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    , m_mainWindow(Q_NULL)
#endif
    , m_core(Q_NULL)
{
}

RibbonMainWindowDsgnPlugin::~RibbonMainWindowDsgnPlugin()
{
}

QString RibbonMainWindowDsgnPlugin::name() const
{
    return create_qtitan_className("RibbonMainWindow");
}

QString RibbonMainWindowDsgnPlugin::group() const
{
    return QLatin1String("[invisible]");
}

QString RibbonMainWindowDsgnPlugin::toolTip() const
{
    return "Advanced RibbonMainWindow for Qt.C++";
}

QString RibbonMainWindowDsgnPlugin::whatsThis() const
{
    return "Advanced RibbonMainWindow for Qt.C++";
}

QString RibbonMainWindowDsgnPlugin::includeFile() const
{
    return QLatin1String("QtnRibbonMainWindow.h");
}

QIcon RibbonMainWindowDsgnPlugin::icon() const
{
    return QIcon();
}

bool RibbonMainWindowDsgnPlugin::isContainer() const
{
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    return true;
#else
    return false;
#endif

}

QWidget* RibbonMainWindowDsgnPlugin::createWidget(QWidget* parent)
{
    RibbonMainWindow* mainWindow = new RibbonMainWindow(parent);
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    if (parent)
    {
        m_mainWindow = mainWindow;
        mainContainerChanged(m_mainWindow);
    }
#endif
    return mainWindow;
}

bool RibbonMainWindowDsgnPlugin::isInitialized() const
{
    return m_initialized;
}

void RibbonMainWindowDsgnPlugin::initialize(QDesignerFormEditorInterface* formEditor)
{
    Q_UNUSED(formEditor);
    if (m_initialized)
        return;

    QExtensionManager* manager = formEditor->extensionManager();
    Q_ASSERT(manager != Q_NULL);
    manager->registerExtensions(new MainWindowMenuExFactory(manager), Q_TYPEID(QDesignerTaskMenuExtension));
    manager->registerExtensions(new MainWindowContainerFactory(manager), Q_TYPEID(QDesignerContainerExtension));
    m_core = formEditor;

    m_initialized = true;
}

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
void RibbonMainWindowDsgnPlugin::mainContainerChanged(QWidget* mainContainer)
{
    if (m_mainWindow == mainContainer)
    {
        QDesignerContainerExtension* c = qt_extension<QDesignerContainerExtension*>(m_core->extensionManager(), m_mainWindow);
        QDesignerFormWindowInterface* formWindow = QDesignerFormWindowInterface::findFormWindow(m_mainWindow->parentWidget());
        disconnect(formWindow, SIGNAL(fileNameChanged(const QString&)), this, SLOT(fileNameChanged(const QString&)));

        if (QWidget* ribbonBar = qobject_cast<QWidget*>(m_core->widgetFactory()->createWidget(create_qtitan_className("RibbonBar"), m_mainWindow)))
        {
            m_core->widgetFactory()->initialize(ribbonBar);
            c->addWidget(ribbonBar);
            m_core->metaDataBase()->add(ribbonBar);
            ribbonBar->setObjectName(QLatin1String("ribbonBar"));
            formWindow->ensureUniqueObjectName(ribbonBar);
        }

        if (QWidget* statusBar = m_mainWindow->statusBar())
        {
            c->addWidget(statusBar);
            m_core->metaDataBase()->add(statusBar);
            statusBar->setObjectName(QLatin1String("statusBar"));
            formWindow->ensureUniqueObjectName(statusBar);
        }

        if (QWidget* style = m_core->widgetFactory()->createWidget(create_qtitan_className("RibbonStyle"), m_mainWindow))
        {
            m_core->widgetFactory()->initialize(style);
            c->addWidget(style);
            m_core->metaDataBase()->add(style);
            style->setObjectName(QLatin1String("ribbonStyle"));
            formWindow->ensureUniqueObjectName(style);
        }
    }
}
#endif

QString RibbonMainWindowDsgnPlugin::domXml() const
{
    return QString();
}



