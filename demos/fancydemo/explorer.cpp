#include "explorer.h"
#include <QCoreApplication>
#include <QSettings>

#define CID_PATH(p)         QString("%1/cid").arg(p)
#define PROJECT_PATH(p)     QString("%1/../project").arg(p)
#define LOG_PATH(p)         QString("%1/../log").arg(p)
#define SCD_PATH(p)         QString("%1/../scd").arg(p)
#define TEMPLATE_PATH(p)    QString("%1/../templates").arg(p)
#define REPORT_PATH(p)      QString("%1/../report").arg(p)

#define SCHEMA_PATH(p)      QString("%1/../share/qtproject/schema").arg(p)
#define APP_SCDSERVER(p)    QString("%1/../share/qtproject/scdserver/scdserver.exe").arg(p)
#define THEME_PATH(p)       QString("%1/../share/qtproject/theme").arg(p)

#define APP_CONFIG(p)       QString("%1/qtproject.ini").arg(p)
#define CONFIG_DIR(p)       QString("%1/config").arg(p)

#ifdef USE_CONFIG_FILE
static QString s_configFileName;
#else
static QString s_proFileName;
static QString s_scdFileName;
static QString s_tcdFileName;
static QString s_wcdFileName;
#endif

Explorer::Explorer(QObject *parent)
    : QObject(parent)
{

}

void Explorer::initialize()
{
    QString appDirPath = applicationDirPath();

    // make directory
    // scd,templates,project,
    QDir pdir(appDirPath);
    pdir.cd(appDirPath);
    pdir.cdUp();
    if(!pdir.exists("scd"))
        pdir.mkdir("scd");
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
#ifdef USE_CONFIG_FILE
    // project documents
    QSettings settings(s_configFileName, QSettings::IniFormat);
    settings.beginGroup("project");
    settings.setValue("pro", QString(""));
    settings.setValue("scd", QString(""));
    settings.setValue("tcd", QString(""));
    settings.setValue("wcd", QString(""));
    settings.endGroup();
#else
    s_proFileName = QString("");
    s_scdFileName = QString("");
    s_tcdFileName = QString("");
    s_wcdFileName = QString("");
#endif
}

QString Explorer::applicationDirPath()
{
    QString appDirPath = QCoreApplication::applicationDirPath();
    appDirPath = appDirPath.replace("\\","/");
    return appDirPath;
}

QString Explorer::getProjectFile()
{
#ifdef USE_CONFIG_FILE
    QSettings settings(s_configFileName, QSettings::IniFormat);
    settings.beginGroup("project");
    return settings.value("pro").toString();
#else
    return s_proFileName;
#endif
}

QString Explorer::getScdFile()
{
#ifdef USE_CONFIG_FILE
    QSettings settings(s_configFileName, QSettings::IniFormat);
    settings.beginGroup("project");
    return settings.value("scd").toString();
#else
    return s_scdFileName;
#endif
}

QString Explorer::getTestFile()
{
#ifdef USE_CONFIG_FILE
    QSettings settings(s_configFileName, QSettings::IniFormat);
    settings.beginGroup("project");
    return settings.value("tcd").toString();
#else
    return s_tcdFileName;
#endif
}

QString Explorer::getWarnFile()
{
#ifdef USE_CONFIG_FILE
    QSettings settings(s_configFileName, QSettings::IniFormat);
    settings.beginGroup("project");
    return settings.value("wcd").toString();
#else
    return s_wcdFileName;
#endif
}

void Explorer::setProjectFile(const QString &fileName)
{
#ifdef USE_CONFIG_FILE
    QSettings settings(s_configFileName, QSettings::IniFormat);
    settings.beginGroup("project");
    settings.setValue("pro", fileName);
    settings.endGroup();
#else
    s_proFileName = fileName;
#endif
}

void Explorer::setTestFile(const QString &fileName)
{
#ifdef USE_CONFIG_FILE
    QSettings settings(s_configFileName, QSettings::IniFormat);
    settings.beginGroup("project");
    settings.setValue("tcd", fileName);
    settings.endGroup();
#else
    s_tcdFileName = fileName;
#endif
}

void Explorer::setWarnFile(const QString &fileName)
{
#ifdef USE_CONFIG_FILE
    QSettings settings(s_configFileName, QSettings::IniFormat);
    settings.beginGroup("project");
    settings.setValue("wcd", fileName);
    settings.endGroup();
#else
    s_wcdFileName = fileName;
#endif
}

void Explorer::setScdFile(const QString &fileName)
{
#ifdef USE_CONFIG_FILE
    QSettings settings(s_configFileName, QSettings::IniFormat);
    settings.beginGroup("project");
    settings.setValue("scd", fileName);
    settings.endGroup();
#else
    s_scdFileName = fileName;
#endif
}

QFileInfoList Explorer::getCidFiles()
{
    QFileInfoList cidFileInfoList;

    QFileInfo proFileInfo(getProjectFile());
    if(!proFileInfo.isFile())
        return cidFileInfoList;

    QString cidDirPath = CID_PATH(proFileInfo.absolutePath());
    QDir dir(cidDirPath);

    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

    foreach(QFileInfo fileinfo, fileInfoList){
        QString suffix = fileinfo.suffix().toLower();
        if( suffix != "cid"){
            continue;
        }
        cidFileInfoList.append(fileinfo);
    }
    return cidFileInfoList;
}

QFileInfoList Explorer::getDcdFiles()
{
    QFileInfo projectFileInfo(getProjectFile());
    QString projectDirPath = projectFileInfo.absolutePath();

    QDir dir(projectDirPath);
    QStringList typeList;
    typeList<<"*.dcd"<<"*.DCD";
    QFileInfoList fileInfoList = dir.entryInfoList(typeList,QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    return fileInfoList;
}

QFileInfoList Explorer::getScdFiles()
{
    QString appDirPath = applicationDirPath();
    QString scdDirPath = SCD_PATH(appDirPath);
    scdDirPath = QDir::cleanPath(scdDirPath);

    QDir dir(scdDirPath);

    QFileInfoList scdFileInfoList;
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    foreach(QFileInfo fileinfo, fileInfoList)
    {
      //_filter = _fileinfo.completeSuffix();
      QString suffix = fileinfo.suffix().toLower();    //?????
      if( suffix != "scd" ){
          continue;
      }
      scdFileInfoList.append(fileinfo);
    }
    return scdFileInfoList;
}

QFileInfoList Explorer::getWctFiles()
{
    QString appDirPath = applicationDirPath();
    QString templateDirPath = TEMPLATE_PATH(appDirPath);
    templateDirPath = QDir::cleanPath(templateDirPath);

    QDir dir(templateDirPath);

    QFileInfoList wctFileInfoList;
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    foreach(QFileInfo fileinfo, fileInfoList)
    {
      //_filter = _fileinfo.completeSuffix();
      QString suffix = fileinfo.suffix().toLower();    //?????
      if( suffix != "wct" ){
          continue;
      }
      wctFileInfoList.append(fileinfo);
    }
    return wctFileInfoList;
}

QString Explorer::defaultProjectPath()
{
    QString path = applicationDirPath();
    path = PROJECT_PATH(path);
    path = QDir::cleanPath(path);
    return path;
}

QString Explorer::getCidPath()
{
    QFileInfo proFileInfo(getProjectFile());
    if(!proFileInfo.isFile())
        return QString("");

    QString cidDirPath = CID_PATH(proFileInfo.absolutePath());
    return cidDirPath;
}

QString Explorer::getSchemaPath()
{
    QString path = applicationDirPath();
    path = SCHEMA_PATH(path);
    path = QDir::cleanPath(path);
    return path;
}

QString Explorer::getScdserver()
{
    QString path = applicationDirPath();
    path = APP_SCDSERVER(path);
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

