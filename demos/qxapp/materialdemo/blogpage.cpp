/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "blogpage.h"

#include "qxmaterial/materialcard.h"
#include "qxmaterial/materialchip.h"
#include "qxmaterial/materialtokens.h"
#include "qxcontrols/qxcontrol.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtCore/QtMath>
#include <QtGui/QTextDocument>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QX_CONTROLS_USE_NAMESPACE
QX_MATERIAL_USE_NAMESPACE

namespace
{
constexpr int kGap = 16;
constexpr int kCardPadding = 16;
constexpr int kPostWidth = 300;

QLabel *line(const QString &text, MaterialType::Scale scale, bool strong, QWidget *parent)
{
    auto *label = new QLabel(text, parent);
    label->setFont(MaterialType::font(scale, strong));
    label->setWordWrap(true);
    return label;
}

/*! The post itself: as tall as what it holds, and never scrolling on its own.
 *
 *  The page is already inside a scroll area, and two strips of scrolling about
 *  the same text are one too many - so the view takes the height of its
 *  document and the page grows instead. Rich text takes the document's width
 *  and not the widget's, so the height is asked for again whenever the width
 *  changes; it is set only when it differs, which is what keeps the two
 *  layouts from calling each other in a circle.
 */
class PostView : public QTextBrowser
{
public:
    explicit PostView(QWidget *parent)
        : QTextBrowser(parent)
    {
        setFrameShape(QFrame::NoFrame);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        // The document is the theme's, not the browser's: a post on a card that
        // painted its own paper would be a white rectangle on a dark page.
        setStyleSheet(QStringLiteral("QTextBrowser { background: transparent; color: palette(window-text); }"));
    }
protected:
    void resizeEvent(QResizeEvent *event) override
    {
        QTextBrowser::resizeEvent(event);
        document()->setTextWidth(viewport()->width());
        const int wanted = qCeil(document()->size().height());
        if (wanted > 0 && wanted != minimumHeight()) {
            setFixedHeight(wanted);
        }
    }
};

/*! What the post says.
 *
 *  It is local rich text in a C string because that is what the demo is: a
 *  document that is part of the screen and not fetched from anywhere. The tags
 *  are the ones a post really uses - a sub-heading, two paragraphs, a
 *  quotation, a list - and none of them is a demonstration of itself.
 */
QString postBody()
{
    return BlogPage::tr("<h3>Start from what the screen is for</h3>"
                        "<p>A review that changes nothing is a meeting. The way out of that is not a longer "
                        "agenda: it is to say, before anyone opens the file, which one question the screen "
                        "has to answer, and to look at the screen while asking it.</p>"
                        "<blockquote>&ldquo;Does this make the next thing the user has to do easier to "
                        "see?&rdquo;</blockquote>"
                        "<p>Everything else - the spacing, the weight of the heading, whether the card casts "
                        "a shadow - is a detail of that answer, and a detail argued on its own is a matter of "
                        "taste with a citation.</p>"
                        "<h3>What the three surfaces are for</h3>"
                        "<p>Material gives a card four ways to sit on the page, and they are not a palette of "
                        "styles to choose from:</p>"
                        "<ul><li>elevated - a thing that could be picked up;</li>"
                        "<li>filled - a thing that belongs to the page, one step in;</li>"
                        "<li>outlined - a thing that is only its edge.</li></ul>"
                        "<p>Pick one, and the screen has said how its parts relate before a word is read.</p>");
}
}   // namespace

BlogPage::BlogPage(QWidget *parent)
    : DemoPage(tr("Blog"), tr("A page that is text, and has to stay readable for the length of it."), parent)
{
    auto *head = new QWidget(this);
    auto *headLayout = new QHBoxLayout(head);
    headLayout->setContentsMargins(QMargins());
    headLayout->setSpacing(kGap);

    auto *said = new QWidget(head);
    auto *saidLayout = new QVBoxLayout(said);
    saidLayout->setContentsMargins(QMargins());
    saidLayout->setSpacing(4);
    saidLayout->addWidget(
        line(tr("Running a design review that changes something"), MaterialType::Scale::Headline, true, said));
    QLabel *by = line(tr("maminjie - six minutes"), MaterialType::Scale::Label, false, said);
    by->setForegroundRole(QPalette::PlaceholderText);
    saidLayout->addWidget(by);
    headLayout->addWidget(said, 1);

    auto *follow = new MaterialChip(MaterialChip::Kind::Assist, head);
    follow->setText(tr("Follow"));
    connect(follow, &QxControl::clicked, this, [this]() {
        Q_EMIT messageRequested(tr("Followed, in a window with nothing to send it to."));
    });
    headLayout->addWidget(follow, 0, Qt::AlignTop);
    addBlock(head);

    auto *post = new MaterialCard(MaterialCard::Variant::Outlined, this);
    auto *postLayout = new QVBoxLayout(post);
    const int inset = MaterialCard::shadowMargin(MaterialCard::Variant::Outlined) + kCardPadding;
    postLayout->setContentsMargins(inset, inset, inset, inset);
    postLayout->setSpacing(0);

    auto *browser = new PostView(post);
    browser->setOpenExternalLinks(false);
    // The document's own font, because rich text takes the document's and not
    // the widget's: without this the post is read at whatever Qt's default is,
    // which is smaller than anything else on the screen.
    browser->document()->setDefaultFont(MaterialType::font(MaterialType::Scale::Body));
    browser->setHtml(postBody());
    postLayout->addWidget(browser);
    addBlock(post);

    addSection(tr("More to read"));

    auto *more = new QWidget(this);
    auto *moreLayout = new QHBoxLayout(more);
    moreLayout->setContentsMargins(QMargins());
    moreLayout->setSpacing(kGap);
    struct Other {
        const char *title;
        const char *line;
    };
    const Other others[] = {{QT_TR_NOOP("A navigation rail that stays put"),
                             QT_TR_NOOP("Five destinations, one column, and no question about which is in "
                                        "front.")},
                            {QT_TR_NOOP("The one thing a screen is for"),
                             QT_TR_NOOP("Why a floating action button is not a toolbar with a round button "
                                        "on it.")}};
    for (const Other &other : others) {
        auto *card = new MaterialCard(MaterialCard::Variant::Filled, more);
        card->setFixedWidth(kPostWidth);
        auto *layout = new QVBoxLayout(card);
        const int cardInset = MaterialCard::shadowMargin(MaterialCard::Variant::Filled) + kCardPadding;
        layout->setContentsMargins(cardInset, cardInset, cardInset, cardInset);
        layout->setSpacing(6);
        layout->addWidget(line(tr(other.title), MaterialType::Scale::Title, true, card));
        layout->addWidget(line(tr(other.line), MaterialType::Scale::Body, false, card));
        layout->addStretch(1);
        auto *read = new MaterialChip(MaterialChip::Kind::Assist, card);
        read->setText(tr("Read"));
        connect(read, &QxControl::clicked, this, [this, other]() {
            Q_EMIT messageRequested(
                tr("%1 is a title on a card: the post behind it does not exist.").arg(tr(other.title)));
        });
        layout->addWidget(read, 0, Qt::AlignLeft);
        moreLayout->addWidget(card);
    }
    moreLayout->addStretch(1);
    addBlock(more);

    addStretch();
}
