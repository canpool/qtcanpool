#ifndef EXPLORER_H
#define EXPLORER_H

#include <QObject>
#include <QDir>

class Explorer : public QObject
{
    Q_OBJECT
public:
    explicit Explorer(QObject *parent = 0);

public:
    static void initialize();
    static void clear();

    static QString applicationDirPath();

    static QString getProjectFile();
    static QString getTestFile();
    static QString getWarnFile();
    static QString getScdFile();

    static void setProjectFile(const QString &fileName);
    static void setTestFile(const QString &fileName);
    static void setWarnFile(const QString &fileName);
    static void setScdFile(const QString &fileName);

    static QFileInfoList getCidFiles();
    static QFileInfoList getDcdFiles();
    static QFileInfoList getScdFiles();
    static QFileInfoList getWctFiles();

    static QString defaultProjectPath();
    static QString getCidPath();

//    static QString configFileName();

    static QString getSchemaPath();
    static QString getScdserver();
    static QFileInfoList getThemeFiles();

    static QString getTemplateDir();

    static QString getConfigDir();

    static QString getLogDir();
    static QString getReportDir();
};

#endif // EXPLORER_H
