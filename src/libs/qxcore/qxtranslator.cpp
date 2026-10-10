/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "qxtranslator.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QLibraryInfo>
#include <QtCore/QTranslator>

QX_CORE_BEGIN_NAMESPACE

/*!
 * Qt keeps its own translations ("qtbase_\<language\>.qm") next to the Qt
 * installation rather than with the application, so a build that does not copy
 * them still has to be able to find them. QLibraryInfo::path() is Qt 6 only,
 * hence the split.
 */
static QString qtTranslationsPath()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return QLibraryInfo::path(QLibraryInfo::TranslationsPath);
#else
    return QLibraryInfo::location(QLibraryInfo::TranslationsPath);
#endif
}

/*!
 * "app_zh_CN.qm" -> "zh_CN". The cut is at the first underscore and not at the
 * last one because the language may carry a territory of its own ("zh_CN",
 * "pt_BR") while a prefix never does. Names that carry no language at all -
 * no underscore, or nothing after it - yield an empty string.
 */
static QString languageFromFileName(const QString &fileName)
{
    if (!fileName.endsWith(QLatin1String(".qm"), Qt::CaseInsensitive)) {
        return QString();
    }

    const QString stem = fileName.left(fileName.length() - 3);
    const int separator = stem.indexOf(QLatin1Char('_'));
    if (separator <= 0 || separator == stem.length() - 1) {
        return QString();
    }

    return stem.mid(separator + 1);
}

class QxTranslatorPrivate
{
public:
    /*! Uninstalls and destroys everything that is currently installed. */
    void clear()
    {
        for (QTranslator *translator : m_installed) {
            QCoreApplication::removeTranslator(translator);
            delete translator;
        }

        m_installed.clear();
        m_loadedFiles.clear();
    }
public:
    QString m_path;
    QString m_language;
    QList<QTranslator *> m_installed;
    QStringList m_loadedFiles;
};

QxTranslator::QxTranslator(QObject *parent)
    : QObject(parent)
    , d_ptr(new QxTranslatorPrivate())
{
}

QxTranslator::~QxTranslator()
{
    Q_D(QxTranslator);
    d->clear();
    QX_FINI_PRIVATE();
}

QString QxTranslator::translationPath() const
{
    Q_D(const QxTranslator);
    return d->m_path;
}

void QxTranslator::setTranslationPath(const QString &path)
{
    Q_D(QxTranslator);
    d->m_path = path;
}

QStringList QxTranslator::availableLanguages() const
{
    Q_D(const QxTranslator);
    if (d->m_path.isEmpty()) {
        return QStringList();
    }

    const QDir dir(d->m_path);
    if (!dir.exists()) {
        return QStringList();
    }

    QStringList languages;
    const QStringList entries = dir.entryList(QStringList{QStringLiteral("*.qm")}, QDir::Files);
    for (const QString &entry : entries) {
        const QString language = languageFromFileName(entry);
        if (!language.isEmpty() && !languages.contains(language)) {
            languages.append(language);
        }
    }

    languages.sort();
    return languages;
}

QString QxTranslator::language() const
{
    Q_D(const QxTranslator);
    return d->m_language;
}

bool QxTranslator::setLanguage(const QString &language)
{
    Q_D(QxTranslator);
    if (language.isEmpty()) {
        return false;
    }

    if (!QCoreApplication::instance()) {
        qWarning("QxTranslator: no QCoreApplication instance to install '%s' into", qPrintable(language));
        return false;
    }

    QList<QTranslator *> translators;
    QStringList files;

    // Everything the application deployed, whatever prefix it happens to use.
    const QDir dir(d->m_path);
    if (!d->m_path.isEmpty() && dir.exists()) {
        const QStringList entries =
            dir.entryList(QStringList{QStringLiteral("*_%1.qm").arg(language)}, QDir::Files, QDir::Name);
        for (const QString &entry : entries) {
            const QString file = dir.absoluteFilePath(entry);
            QTranslator *translator = new QTranslator();
            if (translator->load(file)) {
                translators.append(translator);
                files.append(file);
            } else {
                qWarning("QxTranslator: '%s' is not a usable translation", qPrintable(file));
                delete translator;
            }
        }
    }

    // Nothing loaded means this language is not installed here. The swap below
    // is the only thing that touches state, so returning now leaves the
    // application exactly as it was.
    if (translators.isEmpty()) {
        return false;
    }

    // Qt's own strings, on top. This is the one optional part, and it is not
    // searched again when the application's directory happens to be the one Qt
    // keeps them in - the scan above already picked them up.
    const QString qtPath = qtTranslationsPath();
    if (!qtPath.isEmpty() && qtPath != d->m_path) {
        const QStringList qtPrefixes{QStringLiteral("qtbase"), QStringLiteral("qt")};
        for (const QString &prefix : qtPrefixes) {
            const QString name = prefix + QLatin1Char('_') + language;
            QTranslator *translator = new QTranslator();
            if (translator->load(name, qtPath)) {
                translators.append(translator);
                files.append(QDir(qtPath).absoluteFilePath(name + QStringLiteral(".qm")));
            } else {
                delete translator;
            }
        }
    }

    // The swap happens only once the replacement is complete, so a language
    // that cannot be installed leaves the application translated as it was
    // rather than untranslated.
    d->clear();
    for (QTranslator *translator : translators) {
        QCoreApplication::installTranslator(translator);
        d->m_installed.append(translator);
    }

    d->m_loadedFiles = files;
    d->m_language = language;

    emit languageChanged(language);
    return true;
}

QStringList QxTranslator::loadedFiles() const
{
    Q_D(const QxTranslator);
    return d->m_loadedFiles;
}

QX_CORE_END_NAMESPACE
