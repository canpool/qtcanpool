/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialdivider.h"

#include <QPalette>

QX_MATERIAL_USE_NAMESPACE

namespace
{
constexpr int kIndent = 16;

/*! Where the line runs: the x of its first and last ink, along the middle of
 *  the one row the divider is. */
QPair<int, int> lineSpan(QWidget *divider)
{
    const QImage image = divider->grab().toImage();
    const QColor page = divider->palette().color(QPalette::Window);
    int first = -1;
    int last = -1;
    const int row = image.height() / 2;
    for (int x = 0; x < image.width(); ++x) {
        if (image.pixelColor(x, row) != page) {
            if (first < 0) {
                first = x;
            }
            last = x;
        }
    }
    return qMakePair(first, last);
}
}   // namespace

class tst_MaterialDivider : public QObject
{
    Q_OBJECT
private slots:
    void aLineIsOneRowTall();
    void theVariantsSayWhereTheLineRuns();
};

/*! A hairline is a hairline: the whole of the drawing is one row. */
void tst_MaterialDivider::aLineIsOneRowTall()
{
    MaterialDivider divider;
    divider.resize(120, 1);
    const QImage image = divider.grab().toImage();
    const QColor page = divider.palette().color(QPalette::Window);
    int inkedRows = 0;
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            if (image.pixelColor(x, y) != page) {
                ++inkedRows;
                break;
            }
        }
    }
    QCOMPARE(inkedRows, 1);
}

/*! Full runs the width; inset starts where the content starts; middle
 *  touches neither end. */
void tst_MaterialDivider::theVariantsSayWhereTheLineRuns()
{
    MaterialDivider divider;
    divider.resize(120, 1);

    divider.setVariant(MaterialDivider::Variant::Full);
    QCOMPARE(lineSpan(&divider), qMakePair(0, 119));

    divider.setVariant(MaterialDivider::Variant::Inset);
    QCOMPARE(lineSpan(&divider), qMakePair(kIndent, 119));

    divider.setVariant(MaterialDivider::Variant::Middle);
    QCOMPARE(lineSpan(&divider), qMakePair(kIndent, 119 - kIndent));
}

TEST_ADD(tst_MaterialDivider)

#include "tst_materialdivider.moc"
