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
#include <QApplication>
#include <QAction>
#include <QStatusBar>


#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerWidgetFactoryInterface>
#include <QtDesigner/QDesignerMetaDataBaseInterface>
#include <QtDesigner/QDesignerWidgetBoxInterface>
#include <QtDesigner/QExtensionManager>

#include "QtnRibbonPlugins.h"
#include "QtnMainWindowDsgnPlugin.h"
#include "QtnMainWindowDsgnContainer.h"
#include "QtnMainWindowDsgnTaskMenu.h"
//#include "QtnRibbonStyle.h"
#include "QtnRibbonBar.h"
#include "QtnRibbonMainWindow.h"


using namespace Qtitan;


/* MainWindowEventFilter */
class MainWindowEventFilter : public QObject
{
public:
    explicit MainWindowEventFilter(QWidget* widget);

public:
    static void install(QWidget* widget);

public:
    virtual bool eventFilter (QObject* watched, QEvent* event);

private:
    QWidget* m_mainWindow;
};

MainWindowEventFilter::MainWindowEventFilter(QWidget* widget)
    : QObject(widget)
    , m_mainWindow(widget)
{
}

void MainWindowEventFilter::install(QWidget* widget)
{
    MainWindowEventFilter* tf = new MainWindowEventFilter(widget);
    widget->installEventFilter(tf);
    widget->setAcceptDrops(true);
    widget->setMouseTracking(true);
}

bool MainWindowEventFilter::eventFilter(QObject* watched, QEvent* event)
{
    if (watched != m_mainWindow)
        return QObject::eventFilter(watched, event);

    switch (event->type()) 
    {
        case QEvent::ChildAdded: 
            {
/*
                // Children should not interact with the mouse
                const QChildEvent* ce = static_cast<const QChildEvent *>(event);
                if (QWidget* w = qobject_cast<QWidget *>(ce->child())) 
                {
                    if (w->metaObject()->className() != QString("Qtitan::RibbonStyle") &&
                        w->metaObject()->className() != QString("Qtitan::OfficeStyle"))
                    {
                        w->installEventFilter(this);
                        w->setAttribute(Qt::WA_TransparentForMouseEvents, true);
                        w->setFocusPolicy(Qt::NoFocus);
                        Qtitan::setOfficeStyle(*w);
                    }
                }
*/
            }
            break;
        default:
            break;
    }
    return QObject::eventFilter(watched, event);
}


/* MainWindowDsgnPlugin */
MainWindowDsgnPlugin::MainWindowDsgnPlugin(QObject *parent)
    : QObject(parent)
{
    initialized = false;
    m_mainWindow = Q_NULL;
    m_core = Q_NULL;
}

MainWindowDsgnPlugin::~MainWindowDsgnPlugin()
{
}

QString MainWindowDsgnPlugin::name() const
{
    return QLatin1String("Qtitan::RibbonMainWindow");
}

QString MainWindowDsgnPlugin::group() const
{
    return QLatin1String("[invisible]");
}

QString MainWindowDsgnPlugin::toolTip() const
{
    return "Advanced RibbonMainWindow for Nokia-Qt";
}

QString MainWindowDsgnPlugin::whatsThis() const
{
    return "Advanced RibbonMainWindow for Nokia-Qt";
}

QString MainWindowDsgnPlugin::includeFile() const
{
    return QLatin1String("QtnRibbonMainWindow.h");
}

QIcon MainWindowDsgnPlugin::icon() const
{
    return QIcon();
}

bool MainWindowDsgnPlugin::isContainer() const
{
    return true;
}

QWidget* MainWindowDsgnPlugin::createWidget(QWidget* parent)
{
    RibbonMainWindow* mainWindow = new RibbonMainWindow(parent);
    if (parent)
    {
        m_mainWindow = mainWindow;
        QDesignerFormWindowInterface* formWindow = QDesignerFormWindowInterface::findFormWindow(parent);
        connect(formWindow, SIGNAL(fileNameChanged(const QString&)), this, SLOT(fileNameChanged(const QString&)));
    }

    MainWindowEventFilter::install(mainWindow);
    return mainWindow;
}

bool MainWindowDsgnPlugin::isInitialized() const
{
    return initialized;
}

void MainWindowDsgnPlugin::initialize(QDesignerFormEditorInterface* formEditor)
{
    Q_UNUSED(formEditor);
    if (initialized)
        return;
    initialized = true;

    QExtensionManager* manager = formEditor->extensionManager();
    Q_ASSERT(manager != Q_NULL);
    manager->registerExtensions(new MainWindowMenuExFactory(manager), Q_TYPEID(QDesignerTaskMenuExtension));
    manager->registerExtensions(new MainWindowContainerFactory(manager), Q_TYPEID(QDesignerContainerExtension));

    m_core = formEditor;
}

void MainWindowDsgnPlugin::fileNameChanged(const QString& fileName)
{
    if (fileName.isEmpty() && m_mainWindow)
    {
        QDesignerContainerExtension* c = qt_extension<QDesignerContainerExtension*>(m_core->extensionManager(), m_mainWindow);
        QDesignerFormWindowInterface* formWindow = QDesignerFormWindowInterface::findFormWindow(m_mainWindow->parentWidget());
        disconnect(formWindow, SIGNAL(fileNameChanged(const QString&)), this, SLOT(fileNameChanged(const QString&)));

        if (QWidget* ribbonBar = qobject_cast<QWidget*>(m_core->widgetFactory()->createWidget(QLatin1String("Qtitan::RibbonBar"), m_mainWindow)))
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

        if (QWidget* style = m_core->widgetFactory()->createWidget(QLatin1String("Qtitan::RibbonStyle"), m_mainWindow))
        {
            m_core->widgetFactory()->initialize(style);
            c->addWidget(style);
            m_core->metaDataBase()->add(style);
            style->setObjectName(QLatin1String("ribbonStyle"));
            formWindow->ensureUniqueObjectName(style);
        }
    }
}

QString MainWindowDsgnPlugin::codeTemplate() const
{ 
    return QString(); 
}

QString MainWindowDsgnPlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
        " <widget class=\"Qtitan::RibbonMainWindow\" name=\"mainWindow\">\n"
        "  <property name=\"geometry\">\n"
        "   <rect>\n"
        "    <x>0</x>\n"
        "    <y>0</y>\n"
        "    <width>800</width>\n"
        "    <height>600</height>\n"
        "   </rect>\n"
        "  </property>\n"
        "  <widget class=\"QWidget\" name=\"centralwidget\"/>\n"
        " </widget>\n"
        "</ui>\n";
}



