#include "mainwindow.h"

#include "qxapp/qxproperty.h"
#include "qxapp/qxpropertyeditor.h"
#include "qxapp/qxsettingsdialog.h"
#include "qxcore/qxsettings.h"

#include <QAction>
#include <QIcon>
#include <QList>
#include <QMenuBar>
#include <QSettings>

QX_APP_USE_NAMESPACE
using QxCore::QxSettings;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(640, 420);
    setWindowTitle(tr("QxProperty / QxPropertyEditor / QxSettingsDialog"));

    // A file-backed settings object so every edit survives a restart and is
    // visible in qtcanpool_example.ini next to the executable.
    m_settings = new QxSettings(QStringLiteral("qtcanpool_example.ini"), QSettings::IniFormat, this);

    auto *settingsAction = new QAction(tr("Settings..."), this);
    connect(settingsAction, &QAction::triggered, this, &MainWindow::openSettings);
    menuBar()->addAction(settingsAction);

    buildSettingsDialog();
}

MainWindow::~MainWindow() = default;

void MainWindow::buildSettingsDialog()
{
    m_dialog = new QxSettingsDialog(m_settings, this);

    QxProperty theme;
    theme.key = QStringLiteral("appearance/theme");
    theme.label = tr("Theme");
    theme.type = QxProperty::Enum;
    theme.choices = QStringList{tr("Light"), tr("Dark")};
    theme.defaultValue = QStringLiteral("Light");

    QxProperty showSplash;
    showSplash.key = QStringLiteral("appearance/showSplash");
    showSplash.label = tr("Show splash screen");
    showSplash.type = QxProperty::Bool;
    showSplash.defaultValue = true;

    QxProperty fontSize;
    fontSize.key = QStringLiteral("editor/fontSize");
    fontSize.label = tr("Font size");
    fontSize.type = QxProperty::Int;
    fontSize.defaultValue = 12;

    QxProperty lineNumbers;
    lineNumbers.key = QStringLiteral("editor/lineNumbers");
    lineNumbers.label = tr("Show line numbers");
    lineNumbers.type = QxProperty::Bool;
    lineNumbers.defaultValue = true;

    m_dialog->addPage(QStringLiteral("general"), QIcon(), tr("General"), QList<QxProperty>{theme, showSplash});
    m_dialog->addPage(QStringLiteral("editor"), QIcon(), tr("Editor"), QList<QxProperty>{fontSize, lineNumbers});
}

void MainWindow::openSettings()
{
    m_dialog->exec();
}
