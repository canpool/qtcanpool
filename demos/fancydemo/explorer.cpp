#include "explorer.h"
#include <QCoreApplication>
#include <QSettings>

#define PROJECT_PATH(p)     QString("%1/../project").arg(p)
#define LOG_PATH(p)         QString("%1/../log").arg(p)
#define TEMPLATE_PATH(p)    QString("%1/../templates").arg(p)
#define REPORT_PATH(p)      QString("%1/../report").arg(p)

#define THEME_PATH(p)       QString("%1/../share/qtproject/theme").arg(p)

#define APP_CONFIG(p)       QString("%1/qtproject.ini").arg(p)
#define CONFIG_DIR(p)       QString("%1/config").arg(p)


Explorer::Explorer(QObject *parent)
    : QObject(parent)
{

}

void Explorer::initialize()
{
    QString appDirPath = applicationDirPath();

    // make directory
    QDir pdir(appDirPath);
    pdir.cd(appDirPath);
    pdir.cdUp();
    if(!pdir.exists("templates"))
        pdir.mkdir("templates");
    if(!pdir.exists("project"))
        pdir.mkdir("project");
    if(!pdir.exists("log"))
        pdir.mkdir("log");
    if(!pdir.exists("report"))
        pdir.mkdir("report");

#ifdef USE_CONFIG_FILE
    s_configFileName = APP_CONFIG(appDirPath);
#endif
}

void Explorer::clear()
{
}

QString Explorer::applicationDirPath()
{
    QString appDirPath = QCoreApplication::applicationDirPath();
    appDirPath = appDirPath.replace("\\","/");
    return appDirPath;
}

QString Explorer::defaultProjectPath()
{
    QString path = applicationDirPath();
    path = PROJECT_PATH(path);
    path = QDir::cleanPath(path);
    return path;
}

QFileInfoList Explorer::getThemeFiles()
{
    QString path = applicationDirPath();
    path = THEME_PATH(path);
    path = QDir::cleanPath(path);

    QDir dir(path);
    QStringList typeList;
    typeList<<"*.cptheme"<<"*.ini";
    QFileInfoList fileInfoList = dir.entryInfoList(typeList,QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    return fileInfoList;
}

QString Explorer::getTemplateDir()
{
    QString path = applicationDirPath();
    path = TEMPLATE_PATH(path);
    path = QDir::cleanPath(path);
    return path;
}

QString Explorer::getConfigDir()
{
    QString path = applicationDirPath();
    path = CONFIG_DIR(path);
    path = QDir::cleanPath(path);
    return path;
}

QString Explorer::getLogDir()
{
    QString path = applicationDirPath();
    path = LOG_PATH(path);
    path = QDir::cleanPath(path);
    return path;
}

QString Explorer::getReportDir()
{
    QString path = applicationDirPath();
    path = REPORT_PATH(path);
    path = QDir::cleanPath(path);
    return path;
}

//QString Explorer::configFileName()
//{
//    return s_configFileName;
//}

