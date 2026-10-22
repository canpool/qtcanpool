/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "homepage.h"

#include "qxmaterial/materialcard.h"
#include "qxmaterial/materialripple.h"
#include "qxmaterial/materialtokens.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>

QX_MATERIAL_USE_NAMESPACE

namespace
{
// The three cards stand in a row, and a row of them is what a dashboard is
// made of; two hundred and twenty is wide enough for a sentence and narrow
// enough that three of them fit beside each other.
constexpr int kCardWidth = 220;
constexpr int kCardHeight = 104;

/*! One card at one variant, carrying one line.
 *
 *  A card is a surface, and a surface with nothing on it is a shape rather than
 *  an answer - so each of these says what it is for, inset by the margin the
 *  shadow is drawn in, which is the only part of the rectangle the contents may
 *  use. */
MaterialCard *card(MaterialCard::Variant variant, const QString &text, QWidget *parent)
{
    auto *surface = new MaterialCard(variant, parent);
    surface->setFixedSize(kCardWidth, kCardHeight);
    auto *layout = new QVBoxLayout(surface);
    const int inset = MaterialCard::shadowMargin(variant);
    layout->setContentsMargins(inset, inset, inset, inset);
    auto *label = new QLabel(text, surface);
    label->setWordWrap(true);
    label->setFont(MaterialType::font(MaterialType::Scale::Body));
    layout->addWidget(label);
    return surface;
}

/*! A surface for a MaterialRipple to sit on.
 *
 *  A ripple is an effect and not a widget: it listens to whatever it was
 *  attached to and is painted by that thing, so a screen made of controls has
 *  nowhere to put one - every control paints its own answer already. This is
 *  that place, and it is the smallest thing that can hold one. */
class RippleSurface : public QWidget
{
public:
    explicit RippleSurface(QWidget *parent)
        : QWidget(parent)
        , m_ripple(new MaterialRipple(this))
    {
        setFixedSize(240, 72);
    }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        // Half a pixel in and one and a half out: the stroke sits inside the
        // widget rather than straddling its edge, which keeps a rounded corner
        // from being clipped by it.
        const QRectF area = QRectF(rect()).adjusted(0.5, 0.5, -1.5, -1.5);
        const qreal radius = MaterialShape::radius(MaterialShape::Radius::Medium);
        painter.setPen(QPen(palette().color(QPalette::Mid), 1));
        painter.setBrush(palette().color(QPalette::Base));
        painter.drawRoundedRect(area, radius, radius);
        // The ripple is the press answer, so it goes exactly where the flat
        // one would have gone, and the flat one is not painted at all - a
        // surface that wears both gives two answers to one press.
        painter.setPen(Qt::NoPen);
        m_ripple->paint(&painter, area, radius);
    }
private:
    MaterialRipple *m_ripple = Q_NULLPTR;
};
}   // namespace

HomePage::HomePage(QWidget *parent)
    : DemoPage(tr("Home"),
               tr("An application built out of QxMaterial: a top app bar, a navigation rail, and every "
                  "control the library ships, on a screen that uses them for something."),
               parent)
{
    addSection(tr("What this is"));

    auto *cards = new QWidget(this);
    auto *cardLayout = new QHBoxLayout(cards);
    cardLayout->setContentsMargins(QMargins());
    cardLayout->setSpacing(16);
    cardLayout->addWidget(card(MaterialCard::Variant::Elevated,
                               tr("Elevated: it casts a shadow, and is the "
                                  "page's own colour."),
                               cards));
    cardLayout->addWidget(card(MaterialCard::Variant::Filled,
                               tr("Filled: one step from the page, and casts "
                                  "nothing."),
                               cards));
    cardLayout->addWidget(card(MaterialCard::Variant::Outlined,
                               tr("Outlined: the line is the whole of what "
                                  "makes it a surface."),
                               cards));
    cardLayout->addStretch(1);
    addBlock(cards);

    addSection(tr("The answer a surface gives"));

    addNote(tr("A press in most toolkits is a flat tint that switches on, which says that this was pressed "
               "and nothing about where. A ripple starts where the pointer landed, spreads to the furthest "
               "corner and fades as it goes - so the surface answers from the place it was touched. Press "
               "this one and watch it."));
    addBlock(new RippleSurface(this));

    addStretch();
}
