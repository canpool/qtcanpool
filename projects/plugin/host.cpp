/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

// The second half of the acceptance. Building the plugin proves it links;
// this proves it is actually discovered, cast and initialized - which is a
// different failure mode, and the one a plugin project is most likely to get
// wrong (an IID that does not match, metadata moc did not embed, a QObject
// subclass that loads but never casts).
//
// It is also the shortest honest answer to "what does a host have to write?".
// Everything host-specific is a no-op here; the surface a plugin can reach is
// the whole of QxPluginContext, and a real host fills it in.

#include "qxplugin/qxplugincontext.h"
#include "qxplugin/qxpluginmanager.h"
#include "qxplugin/qxpluginspec.h"

#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QTextStream>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

using namespace QxPlugin;

namespace
{

class HeadlessContext : public QxPluginContext
{
public:
    explicit HeadlessContext(const QString &settingsFile)
        : QxPluginContext()
        , m_settings(settingsFile, QSettings::IniFormat)
    {
    }

    void addPage(const QString &, const QIcon &, const QString &, QWidget *) override
    {
    }
    void setCurrentPage(const QString &) override
    {
    }
    QString currentPageId() const override
    {
        return QString();
    }
    QWidget *addDock(DockArea, const QString &, const QString &, QWidget *widget) override
    {
        return widget;
    }
    void setStatusMessage(const QString &) override
    {
    }
    void setBusy(bool) override
    {
    }
    void setProgressRange(int, int) override
    {
    }
    void setProgress(int) override
    {
    }
    void clearProgress() override
    {
    }
    void showToast(const QString &, ToastLevel, int) override
    {
    }
    QxCore::QxSettings *settings() const override
    {
        return &m_settings;
    }
private:
    mutable QxCore::QxSettings m_settings;
};

}   // namespace

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    const QDir pluginDir(QStringLiteral(QX_ACCEPTANCE_PLUGIN_DIR));
    HeadlessContext context(QDir::temp().filePath(QStringLiteral("qxplugin-acceptance.ini")));

    QxPluginManager manager;
    manager.setPluginPaths({pluginDir.absolutePath()});
    manager.setContext(&context);
    manager.loadPlugins();

    QxPluginSpec *spec = manager.spec(QStringLiteral("acceptance"));
    const bool found = spec != Q_NULLPTR;
    const bool started = manager.plugin(QStringLiteral("acceptance")) != Q_NULLPTR;

    QTextStream out(stdout);
    out << "plugin dir: " << pluginDir.absolutePath() << '\n';
    out << "found:   " << (found ? "yes" : "no") << '\n';
    out << "started: " << (started ? "yes" : "no") << '\n';
    if (found && !spec->error().isEmpty()) {
        out << "error:   " << spec->error() << '\n';
    }
    if (manager.hasError()) {
        out << "manager: " << manager.errorString() << '\n';
    }
    out.flush();

    return (found && started && !manager.hasError()) ? 0 : 1;
}
