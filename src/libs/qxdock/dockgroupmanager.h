/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include <QObject>

class QSettings;

QX_DOCK_BEGIN_NAMESPACE

class DockGroup;

class DockGroupManagerPrivate;

class QX_DOCK_EXPORT DockGroupManager : public QObject
{
    Q_OBJECT
public:
    explicit DockGroupManager(const QString &perspectivesFolder);
    virtual ~DockGroupManager();

    QStringList perspectiveNames() const;

    void addPerspective(const QString &name, DockGroup *group);
    void openPerspective(const QString &name, DockGroup *group);
    void removePerspectives();
    void removePerspective(const QString &name);
    void loadPerspectives();
    void savePerspectives() const;
signals:
    void perspectivesListChanged();
    void openingPerspective();
    void openedPerspective();
private:
    QString getSettingsFileName(const QString &perspective, bool temp) const;
    std::shared_ptr<QSettings> getSettingsObject(const QString &filePath) const;
private:
    QX_DECLARE_PRIVATE(DockGroupManager)
};

QX_DOCK_END_NAMESPACE
