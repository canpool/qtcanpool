/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "dockgroupmanager.h"
#include "dockgroupmanager_p.h"
#include "dockgroup.h"
#include "docksubwindow.h"

#include <QAction>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QSettings>

#define GROUP_PREFIX QString("Group")

QX_DOCK_BEGIN_NAMESPACE

// Partially bypass QxDock perspective management, store list here
// and then QxDock will only have one perspective loaded
// this is because all docking widgets must exist when a perspective is loaded
// we will guarantee that!
class PerspectiveInfo
{
public:
    std::shared_ptr<QSettings> settings;
    QMap<QString, QStringList> groups;
};

class DockGroupManagerPrivate
{
public:
    QX_DECLARE_PUBLIC(DockGroupManager)
public:
    DockGroupManagerPrivate();
public:
    QMap<QString, PerspectiveInfo> m_perspectives;
    QString m_perspectivesFolder;
};

DockGroupManagerPrivate::DockGroupManagerPrivate()
{
}

DockGroupManager::DockGroupManager(const QString &perspectivesFolder)
{
    QX_INIT_PRIVATE(DockGroupManager);
    Q_D(DockGroupManager);
    d->m_perspectivesFolder = perspectivesFolder;
}

DockGroupManager::~DockGroupManager()
{
    Q_D(DockGroupManager);
    // remove temp files:
    for (auto perspective : d->m_perspectives) {
        QString fileName = perspective.settings->fileName();
        perspective.settings.reset();
        QFile::remove(fileName);
    }
    QX_FINI_PRIVATE();
}

QStringList DockGroupManager::perspectiveNames() const
{
    Q_D(const DockGroupManager);
    return d->m_perspectives.keys();
}

void DockGroupManager::addPerspective(const QString &name, DockGroup *group)
{
    Q_D(DockGroupManager);
    if (!d->m_perspectivesFolder.isEmpty()) {
        d->m_perspectives[name].settings = getSettingsObject(getSettingsFileName(name, true));
        d->m_perspectives[name].groups = group->subWindow()->getGroupContents();

        // save perspective internally
        group->subWindow()->addPerspectiveRec(name);
        // store it in QSettings object
        group->subWindow()->savePerspectivesRec(*(d->m_perspectives[name].settings));
        // remove internal perspectives
        group->subWindow()->removePerspectives(group->subWindow()->perspectiveNames());
    } else {
        assert(false);
    }

    emit perspectivesListChanged();
}

DockWidget *findWidget(QString name, const std::vector<DockSubWindow *> &sws)
{
    for (auto sw : sws) {
        auto widget = sw->findDockWidget(name);
        if (widget)
            return widget;
    }
    return NULL;
}

void DockGroupManager::openPerspective(const QString &name, DockGroup *group)
{
    Q_D(DockGroupManager);
    assert(group->topLevelGroup() == group);

    if (!d->m_perspectivesFolder.isEmpty()) {
        if (d->m_perspectives.contains(name)) {
            emit openingPerspective();

            if (group->canCreateNewGroups()) {
                auto sws = group->subWindow()->allSubWindows(true, true);
                for (auto grpName : d->m_perspectives[name].groups.keys()) {
                    bool found = false;
                    for (auto sw : sws) {
                        if (sw->getPersistGroupName() == grpName) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        grpName = DockSubWindow::getGroupNameFromPersistGroupName(grpName);

                        // restore group in file but not in GUI yet
                        DockPanel *insertPos = nullptr;
                        group->createGroup(grpName, insertPos);
                    }
                }

                sws = group->subWindow()->allSubWindows(false, true);
                for (auto sw : sws) {
                    if (!d->m_perspectives[name].groups.keys().contains(sw->getPersistGroupName())) {
                        group->destroyGroup(sw->group());
                    }
                }
            }

            auto sws = group->subWindow()->allSubWindows(true, true);
            for (auto grpName : d->m_perspectives[name].groups.keys()) {
                for (auto sw : sws) {
                    if (sw->getPersistGroupName() == grpName) {
                        for (QString widgetName : d->m_perspectives[name].groups[grpName]) {
                            DockWidget *dw = findWidget(widgetName, {sw});
                            if (dw) {
                                // OK, widget is already in the good manager!
                            } else {
                                dw = findWidget(widgetName, sws);
                                if (dw) {
                                    // move dock widget in the same group as it used to be when perspective was saved
                                    // this guarantee load/open perspectives will work smartly
                                    MoveDockWidgetAction::move(dw, sw);
                                }
                            }
                        }
                    }
                }
            }

            // internally load perspectives from QSettings
            group->subWindow()->loadPerspectivesRec(*(d->m_perspectives[name].settings));
            // load perspective (update GUI)
            group->subWindow()->openPerspectiveRec(name);
            // remove internal perspectives
            group->subWindow()->removePerspectives(group->subWindow()->perspectiveNames());

            emit openedPerspective();
        }
    } else {
        assert(false);
    }
}

void DockGroupManager::removePerspectives()
{
    Q_D(DockGroupManager);
    d->m_perspectives.clear();
    emit perspectivesListChanged();
}

void DockGroupManager::removePerspective(const QString &name)
{
    Q_D(DockGroupManager);
    d->m_perspectives.remove(name);
    emit perspectivesListChanged();
}

void DockGroupManager::loadPerspectives()
{
    Q_D(DockGroupManager);
    if (!d->m_perspectivesFolder.isEmpty()) {
        QDir().mkpath(d->m_perspectivesFolder);

        d->m_perspectives.clear();

        auto mainSettings = getSettingsObject(getSettingsFileName("", false));
        std::string debug = mainSettings->fileName().toStdString();

        int Size = mainSettings->beginReadArray("Perspectives");

        for (int i = 0; i < Size; ++i) {
            mainSettings->setArrayIndex(i);
            QString perspective = mainSettings->value("Name").toString();

            if (!perspective.isEmpty()) {
                // load perspective file:
                auto toLoad = getSettingsFileName(perspective, false);
                auto loaded = getSettingsFileName(perspective, true);

#ifdef _DEBUG
                std::string debug1 = loaded.toStdString();
                std::string debug2 = toLoad.toStdString();
#endif

                QFile::remove(loaded);
                if (!QFile::copy(toLoad, loaded))
                    assert(false);

                d->m_perspectives[perspective].settings = getSettingsObject(loaded);

                // load group info:
                mainSettings->beginGroup(GROUP_PREFIX);
                for (const auto &key : mainSettings->allKeys()) {
                    d->m_perspectives[perspective].groups[key] = mainSettings->value(key).toStringList();
                }
                mainSettings->endGroup();
            } else {
                assert(false);
            }
        }

        mainSettings->endArray();
    }

    emit perspectivesListChanged();
}

void DockGroupManager::savePerspectives() const
{
    Q_D(const DockGroupManager);
    if (!d->m_perspectivesFolder.isEmpty()) {
        auto mainSettings = getSettingsObject(getSettingsFileName("", false));

        // Save list of perspective and group organization
        mainSettings->beginWriteArray("Perspectives", d->m_perspectives.size());
        int i = 0;
        for (auto perspective : d->m_perspectives.keys()) {
            mainSettings->setArrayIndex(i);
            mainSettings->setValue("Name", perspective);
            mainSettings->beginGroup(GROUP_PREFIX);
            for (auto group : d->m_perspectives[perspective].groups.keys()) {
                mainSettings->setValue(group, d->m_perspectives[perspective].groups[group]);
            }
            mainSettings->endGroup();
            ++i;
        }
        mainSettings->endArray();

        // Save perspectives themselves
        for (auto perspectiveName : d->m_perspectives.keys()) {
            auto toSave = getSettingsFileName(perspectiveName, false);
            QSettings &settings = *(d->m_perspectives[perspectiveName].settings);
            settings.sync();

#ifdef _DEBUG
            std::string debug1 = settings.fileName().toStdString();
            std::string debug2 = toSave.toStdString();
#endif

            QFile::remove(toSave);
            if (!QFile::copy(settings.fileName(), toSave))
                assert(false);
        }
    }
}

QString DockGroupManager::getSettingsFileName(const QString &perspective, bool temp) const
{
    Q_D(const DockGroupManager);
    auto name = (perspective.isEmpty()) ? QStringLiteral("perspectives.ini")
                                        : QStringLiteral("perspective_") + perspective + (temp ? ".tmp" : ".ini");

    return d->m_perspectivesFolder + "/" + name;
}

std::shared_ptr<QSettings> DockGroupManager::getSettingsObject(const QString &filePath) const
{
    return std::make_shared<QSettings>(filePath, QSettings::IniFormat);
}

// CreateChildDockAction
CreateChildDockAction::CreateChildDockAction(DockGroup *group, QMenu *menu)
    : QAction("New group...", menu)
    , m_group(group)
{
    connect(this, SIGNAL(triggered()), this, SLOT(createGroup()));
}

void CreateChildDockAction::createGroup()
{
    QString name = "";
    while (true) {
        bool ok = false;
        name = QInputDialog::getText(NULL, this->text(), "Enter group name", QLineEdit::Normal, name, &ok);
        if (ok) {
            QString error = "";
            if (m_group->topLevelGroup())
                error = m_group->topLevelGroup()->getGroupNameError(name);
            else
                assert(false);

            if (error.isEmpty()) {
                DockPanel *insertPos = NULL;
                m_group->createGroup(name, insertPos);
                break;
            } else {
                QMessageBox::critical(NULL, "Error", error);
                continue;
            }
        } else {
            break;
        }
    }
}

// DestroyGroupAction
DestroyGroupAction::DestroyGroupAction(DockGroup *group, QMenu *menu)
    : QAction("Destroy " + group->subWindow()->getGroupName(), menu)
    , m_group(group)
{
    connect(this, SIGNAL(triggered()), this, SLOT(destroyGroup()));
}

void DestroyGroupAction::destroyGroup()
{
    m_group->topLevelGroup()->destroyGroup(m_group);
}

// MoveDockWidgetAction
MoveDockWidgetAction::MoveDockWidgetAction(DockWidget *widget, DockSubWindow *moveTo, QMenu *menu)
    : QAction(menu)
    , m_widget(widget)
    , m_moveTo(moveTo)
{
    if (moveTo->group()->isTopLevel()) {
        setText("To top");
    } else {
        setText("To " + moveTo->group()->objectName());
    }
    connect(this, SIGNAL(triggered()), this, SLOT(move()));
}

void MoveDockWidgetAction::move()
{
    move(m_widget, m_moveTo);
}

void MoveDockWidgetAction::move(DockWidget *widget, DockSubWindow *moveTo)
{
    if (widget && moveTo) {
        widget->dockWindow()->removeDockWidget(widget);
        moveTo->addDockWidget(Qx::CenterDockWidgetArea, widget, moveTo->getInsertDefaultPos());
    } else {
        assert(false);
    }
}

// LoadPerspectiveAction
LoadPerspectiveAction::LoadPerspectiveAction(QMenu *parent, const QString &name, DockGroup *group)
    : QAction(name, parent)
    , name(name)
    , m_group(group)
{
    connect(this, SIGNAL(triggered()), this, SLOT(load()));
}

void LoadPerspectiveAction::load()
{
    m_group->groupManager()->openPerspective(name, m_group);
}

// RemovePerspectiveAction
RemovePerspectiveAction::RemovePerspectiveAction(QMenu *parent, const QString &name, DockGroup *group)
    : QAction(name, parent)
    , name(name)
    , m_group(group)
{
    connect(this, SIGNAL(triggered()), this, SLOT(remove()));
}

void RemovePerspectiveAction::remove()
{
    m_group->groupManager()->removePerspective(name);
}

QX_DOCK_END_NAMESPACE
