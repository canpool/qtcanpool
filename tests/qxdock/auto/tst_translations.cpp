/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "tst_catalogue.h"

#include <QtCore/QCoreApplication>

class tst_DockTranslations : public QObject
{
    Q_OBJECT

private slots:
    void catalogue();
};

void tst_DockTranslations::catalogue()
{
    const QString catalogue = QStringLiteral("qxdock");
    if (!catalogueAvailable(catalogue)) {
        QSKIP("no compiled catalogue: lrelease was not found");
    }

    checkCatalogue(catalogue);
}

TEST_ADD(tst_DockTranslations)

#include "tst_translations.moc"
