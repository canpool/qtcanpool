/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialpage.h"

#include "qxmaterial/materialbadge.h"
#include "qxmaterial/materialcard.h"
#include "qxmaterial/materialchip.h"
#include "qxmaterial/materialdialog.h"
#include "qxmaterial/materialfab.h"
#include "qxmaterial/materialprogressring.h"
#include "qxmaterial/materialripple.h"
#include "qxmaterial/materialsegmentedbutton.h"
#include "qxmaterial/materialskeleton.h"
#include "qxmaterial/materialslider.h"
#include "qxmaterial/materialsnackbar.h"
#include "qxmaterial/materialswitch.h"
#include "qxmaterial/materialtextfield.h"
#include "qxmaterial/materialtokens.h"

#include <QApplication>
#include <QIcon>
#include <QPainter>
#include <QPushButton>
#include <QPixmap>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

QX_MATERIAL_USE_NAMESPACE

namespace
{
/*!
 * The glyph the floating action button wears.
 *
 * The gallery ships no resources - there is not an icon file anywhere in it -
 * and a FAB carries an icon and nothing else, so the one picture it needs is
 * drawn here. It is drawn in the palette's own answer to the accent, which is
 * the same pair a theme publishes as on-accent: a button filled with the accent
 * has to put that colour on it, and a glyph that shipped black would read as
 * "the icon is still black" exactly the way the dock's did.
 */
QIcon plusIcon()
{
    QPixmap pixmap(24, 24);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(qApp->palette().color(QPalette::HighlightedText), 3, Qt::SolidLine, Qt::RoundCap));
    const QPointF middle(12, 12);
    painter.drawLine(middle - QPointF(7, 0), middle + QPointF(7, 0));
    painter.drawLine(middle - QPointF(0, 7), middle + QPointF(0, 7));
    return QIcon(pixmap);
}

/*!
 * A surface for a MaterialRipple to sit on; see the Fluent page's twin.
 *
 * The ripple is what a press gets, so unlike the reveal this one is not
 * waiting for a pointer to arrive - it is waiting for a press, and the press
 * is the reader's. The label says so, because a surface that paints nothing
 * until it is pressed looks like a surface that painted nothing.
 */
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
        const QRectF area = QRectF(rect()).adjusted(0.5, 0.5, -1.5, -1.5);
        const qreal radius = MaterialShape::radius(MaterialShape::Radius::Medium);
        painter.setPen(QPen(palette().color(QPalette::Mid), 1));
        painter.setBrush(palette().color(QPalette::Base));
        painter.drawRoundedRect(area, radius, radius);
        painter.setPen(Qt::NoPen);
        m_ripple->paint(&painter, area, radius);
    }
private:
    MaterialRipple *m_ripple = Q_NULLPTR;
};
}   // namespace

MaterialPage::MaterialPage(QWidget *parent)
    : GalleryPage(QStringLiteral("material"), parent)
{
    addSection(tr("Input"));

    auto *onOff = addControl<MaterialSwitch>(QStringLiteral("materialSwitch"), QStringLiteral("MaterialSwitch"),
                                             new MaterialSwitch(this));
    onOff->setText(tr("Wi-Fi"));
    onOff->setOn(true);

    auto *slider = addControl<MaterialSlider>(QStringLiteral("materialSlider"), QStringLiteral("MaterialSlider"),
                                              new MaterialSlider(this));
    slider->setRange(0, 100);
    slider->setValue(40);

    auto *segmented = addControl<MaterialSegmentedButton>(QStringLiteral("materialSegmentedButton"),
                                                          QStringLiteral("MaterialSegmentedButton"),
                                                          new MaterialSegmentedButton(this));
    segmented->setItems(QStringList{tr("Day"), tr("Week"), tr("Month")});
    segmented->setCurrentIndex(1);

    // The two variants are the two answers to where a field ends, so they are
    // shown side by side rather than one per row: a page that grows a row per
    // variant is a page whose last control sits below the capture.
    auto *fieldHost = new QWidget(this);
    auto *fieldLayout = new QHBoxLayout(fieldHost);
    fieldLayout->setContentsMargins(0, 0, 0, 0);
    fieldLayout->setSpacing(16);
    auto *filledField = new MaterialTextField(fieldHost);
    filledField->setVariant(MaterialTextField::Variant::Filled);
    filledField->setLabel(tr("Email"));
    auto *outlinedField = new MaterialTextField(fieldHost);
    outlinedField->setVariant(MaterialTextField::Variant::Outlined);
    outlinedField->setLabel(tr("Search"));
    outlinedField->setText(QStringLiteral("Material"));
    fieldLayout->addWidget(filledField);
    fieldLayout->addWidget(outlinedField);
    addControl<QWidget>(QStringLiteral("materialTextField"), QStringLiteral("MaterialTextField"), fieldHost);

    addSection(tr("Display"));

    auto *badge = addControl<MaterialBadge>(QStringLiteral("materialBadge"), QStringLiteral("MaterialBadge"),
                                            new MaterialBadge(this));
    badge->setCount(12);
    badge->setMaximumCount(99);

    auto *chip = addControl<MaterialChip>(QStringLiteral("materialChip"), QStringLiteral("MaterialChip (assist)"),
                                          new MaterialChip(MaterialChip::Kind::Assist, this));
    chip->setText(tr("Assist"));

    auto *filter =
        addControl<MaterialChip>(QStringLiteral("materialChipFilter"), QStringLiteral("MaterialChip (filter)"),
                                 new MaterialChip(MaterialChip::Kind::Filter, this));
    filter->setText(tr("Filter"));
    filter->setChecked(true);

    // All three variants, because a card's variant is the whole of what it says:
    // a shadow, a step in the surface, or a line round the page's own colour.
    const MaterialCard::Variant variants[] = {MaterialCard::Variant::Elevated, MaterialCard::Variant::Filled,
                                              MaterialCard::Variant::Outlined};
    const QString variantIds[] = {QStringLiteral("materialCardElevated"), QStringLiteral("materialCardFilled"),
                                  QStringLiteral("materialCardOutlined")};
    const QString variantNames[] = {QStringLiteral("MaterialCard (elevated)"), QStringLiteral("MaterialCard (filled)"),
                                    QStringLiteral("MaterialCard (outlined)")};
    for (int i = 0; i < 3; ++i) {
        auto *card = addControl<MaterialCard>(variantIds[i], variantNames[i], new MaterialCard(variants[i], this));
        card->setFixedSize(240, 76);
        auto *cardLayout = new QVBoxLayout(card);
        const int inset = MaterialCard::shadowMargin(variants[i]) + 12;
        cardLayout->setContentsMargins(inset, inset, inset, inset);
        cardLayout->addWidget(new QLabel(tr("A card holds one thing."), card));
    }

    // A skeleton is a shape waiting for content, so its size is the content's
    // and not its own: it is given the width a line of text would have been.
    auto *line = addControl<MaterialSkeleton>(QStringLiteral("materialSkeleton"), QStringLiteral("MaterialSkeleton"),
                                              new MaterialSkeleton(MaterialSkeleton::Shape::Rectangle, this));
    line->setFixedSize(240, 16);
    addControl<MaterialSkeleton>(QStringLiteral("materialSkeletonCircle"), QStringLiteral("MaterialSkeleton (circle)"),
                                 new MaterialSkeleton(MaterialSkeleton::Shape::Circle, this));

    addSection(tr("Feedback"));

    auto *ring = addControl<MaterialProgressRing>(
        QStringLiteral("materialProgressRing"), QStringLiteral("MaterialProgressRing"), new MaterialProgressRing(this));
    ring->setValue(65);

    // A snackbar belongs to the bottom of whatever it is on, and asks its parent
    // for that - so the row is a host of a known height rather than the page,
    // whose bottom is somewhere the reader would have to scroll to.
    auto *snackbarHost = new QWidget(this);
    snackbarHost->setFixedSize(320, 76);
    auto *snackbar = new MaterialSnackbar(snackbarHost);
    snackbar->setActionText(tr("Undo"));
    // A gallery row is not a message and must not leave: duration 0 is the one
    // value that starts no timer, so the snackbar stays where it rose to.
    snackbar->setDuration(0);
    snackbar->showMessage(tr("A snackbar says what happened."));
    addControl<QWidget>(QStringLiteral("materialSnackbar"), QStringLiteral("MaterialSnackbar"), snackbarHost);

    // A dialog is a window that covers the page it was opened over, so the row
    // is what opens it rather than the dialog - a window capture cannot hold
    // two, which is the same bargain the teaching tip row on the Fluent page
    // makes.
    auto *dialogButton =
        addControl<QPushButton>(QStringLiteral("materialDialogLauncher"), QStringLiteral("MaterialDialog"),
                                new QPushButton(tr("Ask me something"), this));
    connect(dialogButton, &QPushButton::clicked, this, [this]() {
        if (!m_dialog) {
            m_dialog = new MaterialDialog(this);
            m_dialog->setTitle(tr("Discard draft?"));
            m_dialog->setMessage(tr("The draft cannot be brought back."));
            m_dialog->setButtonText(MaterialDialog::Role::Primary, tr("Discard"));
            m_dialog->setButtonText(MaterialDialog::Role::Secondary, tr("Cancel"));
            // The dialog asks and the page answers: a text button only says
            // which answer it was, so these two connections are the whole of
            // what pressing one does here.
            connect(m_dialog, &MaterialDialog::buttonClicked, m_dialog, [this](MaterialDialog::Role role) {
                if (role == MaterialDialog::Role::Primary) {
                    m_dialog->accept();
                } else {
                    m_dialog->reject();
                }
            });
        }
        m_dialog->open();
    });

    addSection(tr("Actions and effects"));

    auto *fab =
        addControl<MaterialFAB>(QStringLiteral("materialFAB"), QStringLiteral("MaterialFAB"), new MaterialFAB(this));
    // A FAB carries an icon and nothing else, and the gallery ships no resources
    // - so the glyph is drawn here rather than shipped, and recoloured on the
    // way in the way an application's own icon would be.
    fab->setIcon(plusIcon());
    fab->setText(tr("Add"));

    addControl<RippleSurface>(QStringLiteral("materialRipple"), QStringLiteral("MaterialRipple"),
                              new RippleSurface(this));
}
