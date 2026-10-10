/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXTRANSLATOR_H
#define QXTRANSLATOR_H

#include "qxcore_global.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>

QX_CORE_BEGIN_NAMESPACE

class QxTranslatorPrivate;

/*!
 * Loads, installs and switches the translations of an application at run time.
 *
 * Qt already owns the hard part: install a QTranslator and every top-level
 * widget receives a QEvent::LanguageChange, which is what translated widgets
 * retranslate themselves on. What is missing around that is the bookkeeping -
 * which languages exist, which files were really loaded, and how to reach the
 * consumers that are not widgets at all. A model or a controller never sees
 * QEvent::LanguageChange, and languageChanged() exists for exactly those;
 * connecting a slot to it is how a non-widget retranslates.
 *
 * Persisting the chosen language is left to the caller on purpose. It is a
 * single value and an application already has a QxSettings to hold it, so the
 * two-line recipe in the example below beats a dependency here:
 *
 * @code
 * QxTranslator translator;
 * translator.setTranslationPath(dir);
 * connect(&translator, &QxTranslator::languageChanged, &settings, [&settings](const QString &language) {
 *     settings.setValue("ui/language", language);
 * });
 * translator.setLanguage(settings.stringValue("ui/language", QLocale::system().name()));
 * @endcode
 */
class QX_CORE_EXPORT QxTranslator : public QObject
{
    Q_OBJECT
public:
    explicit QxTranslator(QObject *parent = nullptr);
    ~QxTranslator() override;

    /*! Directory scanned for "\<prefix\>_\<language\>.qm"; empty by default. */
    QString translationPath() const;
    /*!
     * Points the loader at \a path. The language in use is not reloaded - the
     * next setLanguage() call is what reads the new directory.
     */
    void setTranslationPath(const QString &path);

    /*!
     * The languages present in translationPath(), read from the file names:
     * "app_zh_CN.qm" contributes "zh_CN". The prefix is whatever the files
     * carry, so an application's own translations and the ones copied next to
     * them are enumerated together. Sorted and without duplicates.
     */
    QStringList availableLanguages() const;

    /*! The language currently applied, or empty while none is. */
    QString language() const;

    /*!
     * Installs the translations for \a language and returns true.
     *
     * One of the application's own catalogues has to load - every
     * "\<prefix\>_\<language\>.qm" in translationPath() is tried, which is
     * exactly the set availableLanguages() reports, so the two stay in step.
     * "qtbase_\<language\>.qm" from Qt's own translations directory is then
     * loaded on top when it is there, which is how a build that does not deploy
     * Qt's translations still gets Qt's built-in strings. Qt's catalogues are a
     * supplement: their absence is not a failure, and their presence alone is
     * not a success.
     *
     * When nothing can be loaded nothing changes, so a language that is not
     * installed leaves the application in the one it was already using. An
     * empty \a language is rejected the same way.
     */
    bool setLanguage(const QString &language);

    /*! The files installed by the last successful setLanguage() call. */
    QStringList loadedFiles() const;

Q_SIGNALS:
    /*!
     * Emitted once \a language is installed, so a slot may call translate()
     * immediately. Widgets do not need it: Qt sends them
     * QEvent::LanguageChange.
     */
    void languageChanged(const QString &language);
private:
    Q_DISABLE_COPY(QxTranslator)
    QX_DECLARE_PRIVATE(QxTranslator)
};

QX_CORE_END_NAMESPACE

#endif   // QXTRANSLATOR_H
