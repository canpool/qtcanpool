/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "tst_catalogue.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QList>
#include <QtCore/QTranslator>
#include <QtCore/QStringList>
#include <QtCore/QXmlStreamReader>
#include <QtTest/QtTest>

namespace
{

struct Message {
    QString context;
    QString source;
    QString translation;
    bool finished;
};

/*! Messages that stay English on purpose. */
QStringList deliberatelyEnglish()
{
    // Diagnostics reach whoever integrated the library, through qDebug(), and
    // are looked up in English whatever language the UI runs in. They are the
    // only exemption the catalogue knows.
    return QStringList{QStringLiteral("Ribbon Warning !!!")};
}

bool isDeliberatelyEnglish(const QString &source)
{
    for (const QString &prefix : deliberatelyEnglish()) {
        if (source.startsWith(prefix)) {
            return true;
        }
    }
    return false;
}

QList<Message> readCatalogue(const QString &tsFile, QStringList *contexts, QString *error)
{
    QList<Message> messages;

    QFile file(tsFile);
    if (!file.open(QIODevice::ReadOnly)) {
        *error = QStringLiteral("cannot read '%1'").arg(tsFile);
        return messages;
    }

    QXmlStreamReader xml(&file);
    QString context;
    Message pending;
    bool inMessage = false;

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            const QString name = xml.name().toString();
            if (name == QLatin1String("context")) {
                context.clear();
            } else if (name == QLatin1String("name") && context.isEmpty()) {
                context = xml.readElementText();
                if (!contexts->contains(context)) {
                    contexts->append(context);
                }
            } else if (name == QLatin1String("message")) {
                if (!context.isEmpty() && !contexts->contains(context)) {
                    contexts->append(context);
                }
                inMessage = true;
                pending = Message();
                pending.context = context;
            } else if (inMessage && name == QLatin1String("source")) {
                pending.source = xml.readElementText();
            } else if (inMessage && name == QLatin1String("translation")) {
                pending.finished = !xml.attributes().hasAttribute(QLatin1String("type"));
                pending.translation = xml.readElementText();
            }
        } else if (xml.isEndElement() && xml.name() == QLatin1String("message")) {
            if (inMessage) {
                messages.append(pending);
                inMessage = false;
            }
        }
    }

    if (xml.hasError()) {
        *error = QStringLiteral("malformed '%1': %2").arg(tsFile, xml.errorString());
    }
    return messages;
}

}   // namespace

bool catalogueAvailable(const QString &catalogue)
{
#ifndef QTCANPOOL_TRANSLATIONS_DIR
    Q_UNUSED(catalogue);
    // lrelease was missing at configure time, so nothing was compiled.
    return false;
#else
    const QFileInfo qm(QStringLiteral(QTCANPOOL_TRANSLATIONS_DIR) + QLatin1Char('/') + catalogue +
                       QStringLiteral("_zh_CN.qm"));
    return qm.exists();
#endif
}

void checkCatalogue(const QString &catalogue)
{
#ifdef QTCANPOOL_TRANSLATIONS_DIR
    // A catalogue that is never installed is a catalogue that never answers, so
    // the test loads exactly one: the one the build compiled.
    QTranslator translator;
    const QString qmFile =
        QStringLiteral(QTCANPOOL_TRANSLATIONS_DIR) + QLatin1Char('/') + catalogue + QStringLiteral("_zh_CN.qm");
    QVERIFY2(translator.load(qmFile), qPrintable(QStringLiteral("cannot load '%1'").arg(qmFile)));
    QVERIFY(QCoreApplication::installTranslator(&translator));

    QStringList contexts;
    QString error;
    const QString tsFile = QStringLiteral(QTCANPOOL_SOURCE_DIR) + QStringLiteral("/src/libs/") + catalogue +
                           QLatin1Char('/') + catalogue + QStringLiteral("_zh_CN.ts");
    const QList<Message> messages = readCatalogue(tsFile, &contexts, &error);
    QVERIFY2(error.isEmpty(), qPrintable(error));
    QVERIFY2(!messages.isEmpty(), qPrintable(QStringLiteral("no messages in '%1'").arg(tsFile)));

    // The problems are collected and reported together, which is not just
    // friendlier: a QVERIFY2 that hands failures back one at a time would have
    // to return from here with the translator still installed, and destroying
    // an installed QTranslator leaves QCoreApplication holding a pointer to
    // something that is gone. Whatever happens below, the uninstall is reached.
    QStringList problems;

    for (const QString &context : contexts) {
        // A class without Q_OBJECT never defines tr(), so nothing asks for a
        // context named after it: lupdate files such messages by lexical scope
        // while tr() answers with whatever Q_OBJECT it inherits, and the two
        // names never meet. "RibbonBarPrivate" and "RibbonBackstageViewPrivate"
        // used to sit here, holding two strings that no amount of translating
        // could ever put on screen.
        if (context.endsWith(QLatin1String("Private"))) {
            problems << QStringLiteral("context '%1' is a private class, so no tr() call can produce it").arg(context);
        }
    }

    for (const Message &message : messages) {
        const QString looked = QCoreApplication::translate(qPrintable(message.context), qPrintable(message.source));
        if (message.finished) {
            if (looked != message.translation) {
                problems << QStringLiteral("context '%1' source '%2' resolves to '%3', catalogued as '%4'")
                                .arg(message.context, message.source, looked, message.translation);
            }
        } else if (!isDeliberatelyEnglish(message.source)) {
            // Left untranslated on purpose, or simply forgotten? Only the
            // diagnostics list may say the former.
            problems << QStringLiteral("'%1' in context '%2' has no translation").arg(message.source, message.context);
        }
    }

    QVERIFY(QCoreApplication::removeTranslator(&translator));
    QVERIFY2(problems.isEmpty(), qPrintable(QStringLiteral("%1 in '%2':\n    %3\n")
                                                .arg(QString::number(problems.size()), catalogue,
                                                     problems.join(QStringLiteral("\n    ")))));
#else
    Q_UNUSED(catalogue);
#endif
}
