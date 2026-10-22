/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "albumpage.h"

#include "qxmaterial/materialcard.h"
#include "qxmaterial/materialchip.h"
#include "qxmaterial/materialtokens.h"
#include "qxcontrols/qxcontrol.h"

#include <QtGui/QLinearGradient>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QX_CONTROLS_USE_NAMESPACE
QX_MATERIAL_USE_NAMESPACE

namespace
{
constexpr int kGap = 16;
constexpr int kHeroHeight = 176;
constexpr int kTileHeight = 150;
constexpr int kTileWidth = 216;
constexpr int kColumns = 3;

QLabel *line(const QString &text, MaterialType::Scale scale, bool strong, QWidget *parent)
{
    auto *label = new QLabel(text, parent);
    label->setFont(MaterialType::font(scale, strong));
    label->setWordWrap(true);
    return label;
}

/*! One photograph.
 *
 *  A photograph is content and not a control, so it does not follow the theme:
 *  what the theme owns here is the frame around it and the words under it. The
 *  picture is two flat colours and two soft discs, which is what there is room
 *  for in a demo and enough to make a grid of them read as a grid of pictures
 *  rather than as a grid of coloured rectangles.
 */
class PhotoTile : public QWidget
{
public:
    PhotoTile(const QColor &first, const QColor &second, QWidget *parent)
        : QWidget(parent)
        , m_first(first)
        , m_second(second)
    {
        setMinimumSize(120, 90);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        // Half a pixel in and one out: the edge sits inside the widget instead
        // of straddling it, which is what keeps the round corners round.
        const QRectF area = QRectF(rect()).adjusted(0.0, 0.0, -1.0, -1.0);
        const qreal radius = MaterialShape::radius(MaterialShape::Radius::Large);

        QLinearGradient sky(area.topLeft(), area.bottomRight());
        sky.setColorAt(0.0, m_first);
        sky.setColorAt(1.0, m_second);

        QPainterPath shape;
        shape.addRoundedRect(area, radius, radius);
        painter.setClipPath(shape);
        painter.fillPath(shape, sky);

        painter.setPen(Qt::NoPen);
        QColor glow(255, 255, 255, 42);
        painter.setBrush(glow);
        painter.drawEllipse(QPointF(area.left() + area.width() * 0.28, area.bottom()), area.height() * 0.62,
                            area.height() * 0.62);
        glow.setAlpha(28);
        painter.setBrush(glow);
        painter.drawEllipse(QPointF(area.right(), area.top() + area.height() * 0.18), area.height() * 0.34,
                            area.height() * 0.34);
    }
private:
    QColor m_first;
    QColor m_second;
};

/*! A tile with the words that belong under it: the picture, its name, and the
 *  place it was taken. */
QWidget *cell(const QColor &first, const QColor &second, const QString &name, const QString &place, QWidget *parent)
{
    auto *holder = new QWidget(parent);
    auto *layout = new QVBoxLayout(holder);
    layout->setContentsMargins(QMargins());
    layout->setSpacing(6);
    auto *photo = new PhotoTile(first, second, holder);
    photo->setFixedHeight(kTileHeight);
    layout->addWidget(photo);
    layout->addWidget(line(name, MaterialType::Scale::Body, true, holder));
    QLabel *where = line(place, MaterialType::Scale::Label, false, holder);
    where->setForegroundRole(QPalette::PlaceholderText);
    layout->addWidget(where);
    return holder;
}
}   // namespace

AlbumPage::AlbumPage(QWidget *parent)
    : DemoPage(tr("Album"), tr("A page whose subject is a picture, and whose job is to get out of the way of it."),
               parent)
{
    // The hero is the whole of the page's first screen: what this is, who made
    // it, and the one way in - with the picture beside the words rather than
    // under them, because the page is about the picture.
    auto *hero = new MaterialCard(MaterialCard::Variant::Elevated);
    hero->setFixedHeight(kHeroHeight);
    auto *heroLayout = new QHBoxLayout(hero);
    const int inset = MaterialCard::shadowMargin(MaterialCard::Variant::Elevated) + 8;
    heroLayout->setContentsMargins(inset, inset, inset, inset);
    heroLayout->setSpacing(kGap);

    auto *said = new QWidget(hero);
    auto *saidLayout = new QVBoxLayout(said);
    saidLayout->setContentsMargins(QMargins());
    saidLayout->setSpacing(6);
    saidLayout->addWidget(line(tr("Autumn in the hills"), MaterialType::Scale::Headline, true, said));
    saidLayout->addWidget(line(tr("Twelve photographs taken over three days, on the way up and on the way "
                                  "back down."),
                               MaterialType::Scale::Body, false, said));
    auto *view = new MaterialChip(MaterialChip::Kind::Assist, said);
    view->setText(tr("View the album"));
    connect(view, &QxControl::clicked, this, [this]() {
        Q_EMIT messageRequested(tr("The album is twelve pictures that are not here: a template shows the "
                                   "shape of a screen, and this one has no photographs behind it."));
    });
    saidLayout->addWidget(view, 0, Qt::AlignLeft);
    saidLayout->addStretch(1);
    heroLayout->addWidget(said, 1);

    auto *cover = new PhotoTile(QColor(184, 118, 68), QColor(92, 56, 44), hero);
    cover->setFixedWidth(kTileWidth + 80);
    heroLayout->addWidget(cover);

    addBlock(hero);

    addSection(tr("The pieces"));

    addNote(tr("A grid is the page's whole answer to \"how many\": three across is what fits in the window "
               "this demo opens at, and the tiles take whatever height they are given, because a photograph "
               "does not have a size of its own."));

    auto *grid = new QWidget(this);
    auto *gridLayout = new QGridLayout(grid);
    gridLayout->setContentsMargins(QMargins());
    gridLayout->setHorizontalSpacing(kGap);
    gridLayout->setVerticalSpacing(kGap);
    struct Shot {
        QColor first;
        QColor second;
        const char *name;
        const char *place;
    };
    const Shot shots[] = {
        {QColor(196, 120, 74), QColor(120, 64, 44), QT_TR_NOOP("Morning fog"), QT_TR_NOOP("Ridge road")},
        {QColor(84, 124, 146), QColor(40, 64, 84), QT_TR_NOOP("The long beach"), QT_TR_NOOP("North point")},
        {QColor(140, 150, 96), QColor(72, 84, 52), QT_TR_NOOP("Three fields"), QT_TR_NOOP("The valley")},
        {QColor(168, 96, 120), QColor(96, 52, 72), QT_TR_NOOP("Blossom"), QT_TR_NOOP("Old orchard")},
        {QColor(110, 116, 160), QColor(56, 60, 96), QT_TR_NOOP("Dusk over the mill"), QT_TR_NOOP("River bend")},
        {QColor(176, 152, 96), QColor(104, 84, 48), QT_TR_NOOP("Sand and stone"), QT_TR_NOOP("Low tide")}};
    int index = 0;
    for (const Shot &shot : shots) {
        gridLayout->addWidget(cell(shot.first, shot.second, tr(shot.name), tr(shot.place), grid), index / kColumns,
                              index % kColumns);
        ++index;
    }
    // The columns share the width and nothing else gets a say: a grid whose
    // columns are as wide as their widest tile drifts as soon as one caption is
    // longer than the rest.
    for (int column = 0; column < kColumns; ++column) {
        gridLayout->setColumnStretch(column, 1);
    }
    addBlock(grid);

    addSection(tr("Where to go next"));

    // A footer is not a section: it is the last line of the page, at the size a
    // page uses for things it is not saying loudly.
    auto *footer = new QWidget(this);
    auto *footerLayout = new QHBoxLayout(footer);
    footerLayout->setContentsMargins(QMargins());
    footerLayout->setSpacing(kGap);
    const char *ways[] = {QT_TR_NOOP("About"), QT_TR_NOOP("Archive"), QT_TR_NOOP("Contact")};
    for (const char *way : ways) {
        QLabel *link = line(tr(way), MaterialType::Scale::Label, false, footer);
        link->setForegroundRole(QPalette::PlaceholderText);
        footerLayout->addWidget(link);
    }
    footerLayout->addStretch(1);
    footerLayout->addWidget(line(tr("Twelve photographs, 2026"), MaterialType::Scale::Label, false, footer));
    addBlock(footer);

    addStretch();
}
