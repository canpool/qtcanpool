/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentpage.h"

#include "qxfluent/fluentbadge.h"
#include "qxfluent/fluentbreadcrumbbar.h"
#include "qxfluent/fluentcard.h"
#include "qxfluent/fluentcommandbar.h"
#include "qxfluent/fluentexpander.h"
#include "qxfluent/fluentinfobar.h"
#include "qxfluent/fluentprogressring.h"
#include "qxfluent/fluentrating.h"
#include "qxfluent/fluentreveal.h"
#include "qxfluent/fluentslider.h"
#include "qxfluent/fluentswitch.h"
#include "qxfluent/fluentteachingtip.h"
#include "qxfluent/fluenttokens.h"
#include "qxfluent/fluentwindowmaterial.h"

#include <QtGui/QPainter>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QVBoxLayout>

QX_FLUENT_USE_NAMESPACE

namespace
{
/*!
 * A surface for a FluentReveal to sit on.
 *
 * A reveal is an effect and not a widget: it listens to whatever it was
 * attached to and is painted by that thing, so a gallery made of controls has
 * nowhere to put one - every control paints its own wash already. This is that
 * place, and it is the smallest thing that can hold one: a rounded surface and
 * nothing else.
 */
class RevealSurface : public QWidget
{
public:
    explicit RevealSurface(QWidget *parent)
        : QWidget(parent)
        , m_reveal(new FluentReveal(this))
    {
        setFixedSize(240, 72);
    }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        // Half a pixel in and one and a half out: the stroke sits inside the
        // widget rather than straddling its edge, which is what keeps a rounded
        // corner from being clipped by it.
        const QRectF area = QRectF(rect()).adjusted(0.5, 0.5, -1.5, -1.5);
        const qreal radius = FluentShape::radius(FluentShape::Radius::Medium);
        painter.setPen(QPen(palette().color(QPalette::Mid), 1));
        painter.setBrush(palette().color(QPalette::Base));
        painter.drawRoundedRect(area, radius, radius);
        // The reveal is the hover wash, so it goes exactly where the flat one
        // would have gone, and the flat one is not painted at all - wearing both
        // would be two answers to the same question.
        painter.setPen(Qt::NoPen);
        m_reveal->paint(&painter, area, radius, palette().color(QPalette::Text));
    }
private:
    FluentReveal *m_reveal = Q_NULLPTR;
};
}   // namespace

FluentPage::FluentPage(QWidget *parent)
    : GalleryPage(QStringLiteral("fluent"), parent)
{
    addSection(tr("Input"));

    auto *onOff = addControl<FluentSwitch>(QStringLiteral("fluentSwitch"), QStringLiteral("FluentSwitch"),
                                           new FluentSwitch(this));
    onOff->setText(tr("Wi-Fi"));
    onOff->setOn(true);

    auto *slider = addControl<FluentSlider>(QStringLiteral("fluentSlider"), QStringLiteral("FluentSlider"),
                                            new FluentSlider(this));
    slider->setRange(0, 100);
    slider->setTickInterval(25);
    slider->setValue(40);

    auto *rating = addControl<FluentRating>(QStringLiteral("fluentRating"), QStringLiteral("FluentRating"),
                                            new FluentRating(this));
    rating->setValue(3);

    addSection(tr("Display"));

    auto *badge =
        addControl<FluentBadge>(QStringLiteral("fluentBadge"), QStringLiteral("FluentBadge"), new FluentBadge(this));
    badge->setCount(12);
    badge->setMaximumCount(99);

    auto *card =
        addControl<FluentCard>(QStringLiteral("fluentCard"), QStringLiteral("FluentCard"), new FluentCard(this));
    card->setElevation(FluentElevation::Level::Depth8);
    card->setFixedSize(240, 96);
    // A card is a surface, and a surface with nothing on it is a shape rather
    // than an answer - so it carries one line of text, inset by the margin the
    // shadow is drawn in, which is the only part of the rectangle the card's
    // contents may use.
    auto *cardLayout = new QVBoxLayout(card);
    const int cardInset = FluentCard::shadowMargin(FluentElevation::Level::Depth8);
    cardLayout->setContentsMargins(cardInset, cardInset, cardInset, cardInset);
    cardLayout->addWidget(new QLabel(tr("A card lifts what is on it."), card));

    addSection(tr("Feedback"));

    auto *ring = addControl<FluentProgressRing>(QStringLiteral("fluentProgressRing"),
                                                QStringLiteral("FluentProgressRing"), new FluentProgressRing(this));
    ring->setValue(65);

    auto *info = addControl<FluentInfoBar>(QStringLiteral("fluentInfoBar"), QStringLiteral("FluentInfoBar"),
                                           new FluentInfoBar(this));
    info->setSeverity(FluentInfoBar::Severity::Success);
    info->setTitle(tr("Saved"));
    info->setMessage(tr("The gallery wrote nothing to disk."));
    info->setOpen(true);

    addSection(tr("Navigation"));

    auto *expander = addControl<FluentExpander>(QStringLiteral("fluentExpander"), QStringLiteral("FluentExpander"),
                                                new FluentExpander(this));
    expander->setTitle(tr("What an expander hides"));
    auto *body = new QLabel(tr("A body the header is the summary of."));
    body->setMinimumHeight(48);
    expander->setContent(body);
    expander->setExpanded(true);

    auto *crumbs = addControl<FluentBreadcrumbBar>(
        QStringLiteral("fluentBreadcrumbBar"), QStringLiteral("FluentBreadcrumbBar"), new FluentBreadcrumbBar(this));
    crumbs->setItems(QStringList{tr("Home"), tr("Library"), tr("Album")});

    auto *commands = addControl<FluentCommandBar>(QStringLiteral("fluentCommandBar"),
                                                  QStringLiteral("FluentCommandBar"), new FluentCommandBar(this));
    commands->addCommand(FluentCommandBar::ItemRole::Button, tr("Save"));
    commands->addCommand(FluentCommandBar::ItemRole::Toggle, tr("Pin"));
    QAction *sort = commands->addCommand(FluentCommandBar::ItemRole::DropDown, tr("Sort"));
    auto *sortMenu = new QMenu(commands);
    sortMenu->addAction(tr("By name"));
    sortMenu->addAction(tr("By date"));
    sort->setMenu(sortMenu);

    addSection(tr("Effects and transients"));

    addControl<RevealSurface>(QStringLiteral("fluentReveal"), QStringLiteral("FluentReveal"), new RevealSurface(this));

    // A teaching tip is a popup of its own, so the row is what opens it rather
    // than the tip: a tip that was open would be a second top-level window, and
    // a window capture cannot hold two.
    auto *tipButton =
        addControl<QPushButton>(QStringLiteral("fluentTeachingTipLauncher"), QStringLiteral("FluentTeachingTip"),
                                new QPushButton(tr("Point at me"), this));
    connect(tipButton, &QPushButton::clicked, this, [this, tipButton]() {
        if (!m_tip) {
            m_tip = new FluentTeachingTip(this);
            m_tip->setTitle(tr("A teaching tip"));
            m_tip->setMessage(tr("It points at the thing it is about, and says what that thing is."));
        }
        m_tip->setTarget(tipButton);
        m_tip->setPreferredSide(FluentTeachingTip::Side::Bottom);
        m_tip->setOpen(true);
    });

    // A window material is not a control and has no rectangle of its own: it is
    // a choice about the window everything else sits in, asked of qxwindow. The
    // row is the switch that asks, and the label says what the platform answered
    // - which on anything other than Windows is "no", and that is worth showing
    // rather than hiding.
    auto *materialRow = new QWidget(this);
    auto *materialLayout = new QHBoxLayout(materialRow);
    materialLayout->setContentsMargins(0, 0, 0, 0);
    auto *materialBox = new QComboBox(materialRow);
    materialBox->addItem(tr("None"), int(FluentWindowMaterial::Material::None));
    materialBox->addItem(tr("Mica"), int(FluentWindowMaterial::Material::Mica));
    materialBox->addItem(tr("Mica Alt"), int(FluentWindowMaterial::Material::MicaAlt));
    materialBox->addItem(tr("Acrylic"), int(FluentWindowMaterial::Material::Acrylic));
    auto *materialState = new QLabel(tr("not applied"), materialRow);
    materialLayout->addWidget(materialBox);
    materialLayout->addWidget(materialState);
    addControl<QWidget>(QStringLiteral("fluentWindowMaterial"), QStringLiteral("FluentWindowMaterial"), materialRow);

    connect(materialBox, QOverload<int>::of(&QComboBox::activated), this,
            [this, materialBox, materialState](int index) {
                if (!m_windowMaterial) {
                    // Made here and not in the constructor: the page is built
                    // before it belongs to a window, and a window material is
                    // asked of a window.
                    m_windowMaterial = new FluentWindowMaterial(window());
                }
                const auto wanted = static_cast<FluentWindowMaterial::Material>(materialBox->itemData(index).toInt());
                materialState->setText(m_windowMaterial->apply(wanted) ? tr("applied") : tr("not applied"));
            });
}
