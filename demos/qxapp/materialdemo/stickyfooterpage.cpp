/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "stickyfooterpage.h"

#include "qxmaterial/materialavatar.h"
#include "qxmaterial/materialdivider.h"
#include "qxmaterial/materialpagination.h"
#include "qxmaterial/materialtokens.h"

#include <QtCore/QCoreApplication>
#include <QtGui/QPalette>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QX_MATERIAL_USE_NAMESPACE

namespace
{
// The width the words are read at: a paragraph stretched across a wide window is
// a line the eye loses its place in on the way back to the left.
constexpr int kReadingWidth = 640;
constexpr int kGap = 12;
constexpr int kFooterGap = 16;
constexpr int kPages = 4;
constexpr int kPage = 1;

QLabel *line(const QString &text, MaterialType::Scale scale, bool strong, QWidget *parent)
{
    auto *label = new QLabel(text, parent);
    label->setFont(MaterialType::font(scale, strong));
    label->setWordWrap(true);
    return label;
}

/*! A paragraph, at the width a paragraph is read at, left-aligned in its row. */
QWidget *paragraph(const QString &text, QWidget *parent)
{
    auto *host = new QWidget(parent);
    auto *layout = new QHBoxLayout(host);
    layout->setContentsMargins(QMargins());
    QLabel *label = line(text, MaterialType::Scale::Body, false, host);
    label->setFixedWidth(kReadingWidth);
    layout->addWidget(label);
    layout->addStretch(1);
    return host;
}
}   // namespace

StickyFooterPage::StickyFooterPage(QWidget *parent)
    : DemoPage(tr("Footer"),
               tr("A page too short to fill its window, and the band at the bottom that says where the rest of "
                  "it is."),
               parent)
{
    addNote(tr("Everything above the footer is one article: a title, who wrote it and when, and three "
               "paragraphs. It does not fill a window this size, which is the whole point - what is left over is "
               "taken up between the article and the footer rather than under the footer, so the page ends at "
               "the bottom of the window instead of two thirds of the way down."));

    auto *article = new QWidget(this);
    auto *articleLayout = new QVBoxLayout(article);
    articleLayout->setContentsMargins(QMargins());
    articleLayout->setSpacing(kGap);

    articleLayout->addWidget(line(tr("A Sunday at the mill"), MaterialType::Scale::Title, true, article));

    auto *byline = new QWidget(article);
    auto *bylineLayout = new QHBoxLayout(byline);
    bylineLayout->setContentsMargins(QMargins());
    bylineLayout->setSpacing(kGap);
    auto *avatar = new MaterialAvatar(byline);
    avatar->setName(tr("Mara Ott"));
    avatar->setAvatarSize(MaterialAvatar::Size::Small);
    bylineLayout->addWidget(avatar);
    QLabel *when = line(tr("Mara Ott · 12 October · four minutes"), MaterialType::Scale::Label, false, byline);
    // A byline is one line: wrapped, a wrapped label reports the width of its
    // longest word and the row it stands in squeezes it to three.
    when->setWordWrap(false);
    when->setForegroundRole(QPalette::PlaceholderText);
    bylineLayout->addWidget(when);
    bylineLayout->addStretch(1);
    articleLayout->addWidget(byline);

    auto *rule = new MaterialDivider(article);
    rule->setVariant(MaterialDivider::Variant::Full);
    articleLayout->addWidget(rule);

    articleLayout->addWidget(paragraph(tr("The fog came up the valley before the light did, which is the one "
                                          "thing about a Sunday worth writing down: by seven there was nothing "
                                          "past the third field, and the mill was a noise rather than a building."),
                                       article));
    articleLayout->addWidget(paragraph(tr("Twelve photographs came of it - the wheel, the wheel again, a slice "
                                          "of the roof that turned out to be the good one. They are in the album, "
                                          "in the order they were taken, which is not the order they are worth "
                                          "looking at."),
                                       article));
    articleLayout->addWidget(paragraph(tr("What the ledger says about the day is shorter than this page: two "
                                          "orders, one refund, and the whole of the afternoon spent on a broken "
                                          "printer that is still broken."),
                                       article));
    addBlock(article);

    // The stretch is the page. Without it the footer sits under the last
    // paragraph, and a window taller than the article has a band of nothing at
    // the bottom that reads as a screen that stopped loading.
    addStretch();

    auto *footer = new QWidget(this);
    auto *footerLayout = new QVBoxLayout(footer);
    footerLayout->setContentsMargins(QMargins(0, kFooterGap, 0, 0));
    footerLayout->setSpacing(kGap);

    auto *edge = new MaterialDivider(footer);
    edge->setVariant(MaterialDivider::Variant::Full);
    footerLayout->addWidget(edge);

    auto *row = new QWidget(footer);
    auto *rowLayout = new QHBoxLayout(row);
    rowLayout->setContentsMargins(QMargins());
    rowLayout->setSpacing(kGap);
    m_pages = new MaterialPagination(row);
    m_pages->setCount(kPages);
    m_pages->setPage(kPage);
    rowLayout->addWidget(m_pages);
    rowLayout->addStretch(1);
    QLabel *small = line(tr("© 2026 Canpool · one of four"), MaterialType::Scale::Label, false, row);
    // A foot of a page is read in one glance, so its small print is one line.
    small->setWordWrap(false);
    small->setForegroundRole(QPalette::PlaceholderText);
    rowLayout->addWidget(small);
    footerLayout->addWidget(row);
    addBlock(footer);

    connect(m_pages, &MaterialPagination::pageChanged, this, [this](int page) {
        // The article does not change: this window has four of them written and
        // one of them on screen, and saying which is the honest answer.
        Q_EMIT messageRequested(tr("Page %1 of %2 - the article would change and this one does not, because "
                                   "the other three were never written.")
                                    .arg(page)
                                    .arg(kPages));
    });
}
