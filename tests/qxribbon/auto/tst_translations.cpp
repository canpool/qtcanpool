/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "tst_catalogue.h"

#include <QtCore/QCoreApplication>

class tst_RibbonTranslations : public QObject
{
    Q_OBJECT

private slots:
    void catalogue();
};

void tst_RibbonTranslations::catalogue()
{
    const QString catalogue = QStringLiteral("qxribbon");
    if (!catalogueAvailable(catalogue)) {
        QSKIP("no compiled catalogue: lrelease was not found");
    }

    checkCatalogue(catalogue);
}

TEST_ADD(tst_RibbonTranslations)

#include "tst_translations.moc"
