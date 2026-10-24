/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "albumpage.h"

#include "loadingstate.h"

#include "qxmaterial/materialcard.h"
#include "qxmaterial/materialchip.h"
#include "qxmaterial/materialskeleton.h"
#include "qxmaterial/materialtokens.h"
#include "qxcontrols/qxcontrol.h"

#include <QtCore/QCoreApplication>
#include <QtGui/QLinearGradient>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QStackedWidget>
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

/*! How long the twelve take to arrive.
 *
 *  A duration the page picks, not a token from the family's motion vocabulary:
 *  nothing here is moving from one state to another, the pictures are simply
 *  not in hand yet. Long enough to be read as a wait and short enough that a
 *  reader who is only passing through sees the page full.
 */
constexpr int kFetchMs = 1200;

/*! The twelve photographs: what each one is, where it was taken, and the two
 *  colours it is painted in. */
struct Shot {
    QColor first;
    QColor second;
    const char *name;
    const char *place;
};

const Shot kShots[] = {
    {QColor(196, 120, 74), QColor(120, 64, 44), QT_TR_NOOP("Morning fog"), QT_TR_NOOP("Ridge road")},
    {QColor(84, 124, 146), QColor(40, 64, 84), QT_TR_NOOP("The long beach"), QT_TR_NOOP("North point")},
    {QColor(140, 150, 96), QColor(72, 84, 52), QT_TR_NOOP("Three fields"), QT_TR_NOOP("The valley")},
    {QColor(168, 96, 120), QColor(96, 52, 72), QT_TR_NOOP("Blossom"), QT_TR_NOOP("Old orchard")},
    {QColor(110, 116, 160), QColor(56, 60, 96), QT_TR_NOOP("Dusk over the mill"), QT_TR_NOOP("River bend")},
    {QColor(176, 152, 96), QColor(104, 84, 48), QT_TR_NOOP("Sand and stone"), QT_TR_NOOP("Low tide")}};

constexpr int kTileCount = int(sizeof(kShots) / sizeof(kShots[0]));

/*! The pieces of this page are built by free functions, so they have no tr() of
 *  their own. The context is named rather than borrowed, so the words on the
 *  page are found under the page's own name when the demo is translated. */
QString tr(const char *text)
{
    return QCoreApplication::translate("AlbumPage", text);
}

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

/*! The album's cover, drawn rather than photographed.
 *
 *  A demo has no photographs, and a gradient on its own is a rectangle that
 *  happens to be two colours: it says "picture" only because it is where a
 *  picture would be. What the page is about is a day in the hills, so the cover
 *  is that day - three ridges one behind the other, the far ones paler with the
 *  air between, and a low sun behind the nearest.
 *
 *  It is content and not a control, so like the tiles it does not follow the
 *  theme: a photograph in a dark window does not become a dark photograph.
 */
class RidgeCover : public QWidget
{
public:
    explicit RidgeCover(QWidget *parent)
        : QWidget(parent)
    {
        setMinimumSize(160, 110);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        const QRectF area = QRectF(rect()).adjusted(0.0, 0.0, -1.0, -1.0);
        const qreal radius = MaterialShape::radius(MaterialShape::Radius::Large);

        QPainterPath shape;
        shape.addRoundedRect(area, radius, radius);
        painter.setClipPath(shape);

        // Autumn light: cool at the top of the frame, warm where it meets the
        // hills, because that is where the sun is.
        QLinearGradient sky(area.topLeft(), area.bottomLeft());
        sky.setColorAt(0.0, QColor(92, 88, 124));
        sky.setColorAt(0.55, QColor(206, 148, 104));
        sky.setColorAt(1.0, QColor(238, 192, 138));
        painter.fillPath(shape, sky);

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 240, 206, 220));
        const QPointF sun(area.left() + area.width() * 0.7, area.top() + area.height() * 0.55);
        painter.drawEllipse(sun, area.height() * 0.1, area.height() * 0.1);

        // Three ridges, each a line across the frame closed down to the bottom.
        // The heights are fixed numbers rather than a random walk: a cover that
        // drew differently every time would make a screenshot of this page
        // useless as a record of what it looked like.
        const qreal far[] = {0.53, 0.47, 0.51, 0.44, 0.49, 0.54, 0.47, 0.52, 0.56};
        const qreal middle[] = {0.67, 0.61, 0.66, 0.58, 0.64, 0.69, 0.62, 0.67, 0.71};
        const qreal near[] = {0.84, 0.79, 0.82, 0.75, 0.81, 0.86, 0.79, 0.84, 0.9};

        paintRidge(&painter, area, far, 9, QColor(134, 116, 132));
        paintRidge(&painter, area, middle, 9, QColor(118, 82, 70));
        paintRidge(&painter, area, near, 9, QColor(74, 50, 44));
    }
private:
    /*! Fills the ground under one ridge, given its height at each of @p count
     *  columns across the frame. */
    static void paintRidge(QPainter *painter, const QRectF &area, const qreal *heights, int count, const QColor &colour)
    {
        QPainterPath ridge;
        ridge.moveTo(area.left(), area.top() + area.height() * heights[0]);
        for (int i = 1; i < count; ++i) {
            const qreal x = area.left() + area.width() * i / (count - 1);
            ridge.lineTo(x, area.top() + area.height() * heights[i]);
        }
        ridge.lineTo(area.right(), area.bottom());
        ridge.lineTo(area.left(), area.bottom());
        ridge.closeSubpath();
        painter->fillPath(ridge, colour);
    }
};

/*! A tile with the words that belong under it: the picture, its name, and the
 *  place it was taken. */
QWidget *cell(const Shot &shot, QWidget *parent)
{
    auto *holder = new QWidget(parent);
    auto *layout = new QVBoxLayout(holder);
    layout->setContentsMargins(QMargins());
    layout->setSpacing(6);
    auto *photo = new PhotoTile(shot.first, shot.second, holder);
    photo->setFixedHeight(kTileHeight);
    layout->addWidget(photo);
    layout->addWidget(line(tr(shot.name), MaterialType::Scale::Body, true, holder));
    QLabel *where = line(tr(shot.place), MaterialType::Scale::Label, false, holder);
    where->setForegroundRole(QPalette::PlaceholderText);
    layout->addWidget(where);
    return holder;
}

/*! A tile that is not here yet: the shape of one, in the place it will take.
 *
 *  The three blocks are the three things the arriving tile will hold - a
 *  picture, a name, a place - at the heights those will be, so that the grid
 *  does not move when it fills. That is the whole point of a skeleton: the
 *  reader starts looking in the right place before there is anything to see.
 */
QWidget *waitingCell(QWidget *parent)
{
    auto *holder = new QWidget(parent);
    auto *layout = new QVBoxLayout(holder);
    layout->setContentsMargins(QMargins());
    layout->setSpacing(6);

    auto *photo = new MaterialSkeleton(MaterialSkeleton::Shape::Rectangle, holder);
    photo->setFixedHeight(kTileHeight);
    layout->addWidget(photo);

    auto *name = new MaterialSkeleton(MaterialSkeleton::Shape::Rectangle, holder);
    name->setFixedSize(112, 15);
    layout->addWidget(name, 0, Qt::AlignLeft);

    auto *where = new MaterialSkeleton(MaterialSkeleton::Shape::Rectangle, holder);
    where->setFixedSize(72, 12);
    layout->addWidget(where, 0, Qt::AlignLeft);

    return holder;
}

/*! The twelve, laid out three across.
 *
 *  The columns share the width and nothing else gets a say: a grid whose
 *  columns are as wide as their widest tile drifts as soon as one caption is
 *  longer than the rest.
 */
QWidget *photoGrid(QWidget *parent)
{
    auto *grid = new QWidget(parent);
    auto *layout = new QGridLayout(grid);
    layout->setContentsMargins(QMargins());
    layout->setHorizontalSpacing(kGap);
    layout->setVerticalSpacing(kGap);

    int index = 0;
    for (const Shot &shot : kShots) {
        layout->addWidget(cell(shot, grid), index / kColumns, index % kColumns);
        ++index;
    }
    for (int column = 0; column < kColumns; ++column) {
        layout->setColumnStretch(column, 1);
    }
    return grid;
}

/*! The same grid, with the twelve still on their way. */
QWidget *waitingGrid(QWidget *parent)
{
    auto *grid = new QWidget(parent);
    auto *layout = new QGridLayout(grid);
    layout->setContentsMargins(QMargins());
    layout->setHorizontalSpacing(kGap);
    layout->setVerticalSpacing(kGap);

    for (int index = 0; index < kTileCount; ++index) {
        layout->addWidget(waitingCell(grid), index / kColumns, index % kColumns);
    }
    for (int column = 0; column < kColumns; ++column) {
        layout->setColumnStretch(column, 1);
    }
    return grid;
}
}   // namespace

AlbumPage::AlbumPage(QWidget *parent)
    : DemoPage(tr("Album"), tr("A page whose subject is a picture, and whose job is to get out of the way of it."),
               parent)
{
    // The hero is the whole of the page's first screen: what this is, who made
    // it, and the two ways in - with the picture beside the words rather than
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

    auto *chips = new QWidget(said);
    auto *chipLayout = new QHBoxLayout(chips);
    chipLayout->setContentsMargins(QMargins());
    chipLayout->setSpacing(8);
    auto *view = new MaterialChip(MaterialChip::Kind::Assist, chips);
    view->setText(tr("View the album"));
    chipLayout->addWidget(view);
    // The way to watch the page wait again. A demo of a wait that could only be
    // seen once, by arriving at start-up, would be a demo nobody could check.
    auto *again = new MaterialChip(MaterialChip::Kind::Assist, chips);
    again->setText(tr("Fetch them again"));
    chipLayout->addWidget(again);
    chipLayout->addStretch(1);
    saidLayout->addWidget(chips);
    saidLayout->addStretch(1);

    connect(view, &QxControl::clicked, this, [this]() {
        Q_EMIT messageRequested(tr("The album is twelve pictures that are not here: a template shows the "
                                   "shape of a screen, and this one has no photographs behind it."));
    });
    heroLayout->addWidget(said, 1);

    auto *cover = new RidgeCover(hero);
    cover->setFixedWidth(kTileWidth + 80);
    heroLayout->addWidget(cover);

    addBlock(hero);

    addSection(tr("The pieces"));

    addNote(tr("A grid is the page's whole answer to \"how many\": three across is what fits in the window "
               "this demo opens at, and the tiles take whatever height they are given, because a photograph "
               "does not have a size of its own. The twelve arrive after the page does, so what stands in "
               "for them until then is the shape of a tile in the place a tile will be."));

    // The cover is here and the twelve are not, which is what a portfolio looks
    // like while it is being fetched: the page has something to show from its
    // first frame, and the part that is missing waits in the shape it will take.
    auto *pieces = new QStackedWidget(this);
    auto *waiting = waitingGrid(pieces);
    auto *arrived = photoGrid(pieces);
    pieces->addWidget(waiting);
    pieces->addWidget(arrived);
    addBlock(pieces);

    auto *loading = new LoadingState(pieces, waiting, arrived, this);
    loading->startOnFirstShow(this, kFetchMs);
    connect(again, &QxControl::clicked, this, [loading]() {
        loading->start(kFetchMs);
    });

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
