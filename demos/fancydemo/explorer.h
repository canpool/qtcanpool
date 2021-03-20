#ifndef EXPLORER_H
#define EXPLORER_H

#include <QObject>
#include <QDir>

#define SETTINGS_ORGANIZATION   "Canpool"
#define SETTINGS_APPLICATION    "qtcanpool"

class Explorer : public QObject
{
    Q_OBJECT
public:
    explicit Explorer(QObject *parent = 0);

public:
    static void initialize();
    static void clear();

    static QString applicationDirPath();

    static QString defaultProjectPath();

//    static QString configFileName();

    static QFileInfoList getThemeFiles();

    static QString getTemplateDir();

    static QString getConfigDir();

    static QString getLogDir();
    static QString getReportDir();
};

#endif // EXPLORER_H
