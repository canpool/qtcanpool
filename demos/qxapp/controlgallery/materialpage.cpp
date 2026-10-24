/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialpage.h"

#include "qxmaterial/materialaccordion.h"
#include "qxmaterial/materialautocomplete.h"
#include "qxmaterial/materialavatar.h"
#include "qxmaterial/materialbadge.h"
#include "qxmaterial/materialbottomnavigation.h"
#include "qxmaterial/materialbreadcrumbs.h"
#include "qxmaterial/materialbutton.h"
#include "qxmaterial/materialcard.h"
#include "qxmaterial/materialcheckbox.h"
#include "qxmaterial/materialchip.h"
#include "qxmaterial/materialdatepicker.h"
#include "qxmaterial/materialdialog.h"
#include "qxmaterial/materialdivider.h"
#include "qxmaterial/materialdrawer.h"
#include "qxmaterial/materialfab.h"
#include "qxmaterial/materialimagelist.h"
#include "qxmaterial/materiallist.h"
#include "qxmaterial/materiallistitem.h"
#include "qxmaterial/materialmenu.h"
#include "qxmaterial/materialpagination.h"
#include "qxmaterial/materialprogress.h"
#include "qxmaterial/materialprogressring.h"
#include "qxmaterial/materialradio.h"
#include "qxmaterial/materialrating.h"
#include "qxmaterial/materialripple.h"
#include "qxmaterial/materialsegmentedbutton.h"
#include "qxmaterial/materialselect.h"
#include "qxmaterial/materialskeleton.h"
#include "qxmaterial/materialslider.h"
#include "qxmaterial/materialsnackbar.h"
#include "qxmaterial/materialspeeddial.h"
#include "qxmaterial/materialstepper.h"
#include "qxmaterial/materialswitch.h"
#include "qxmaterial/materialtable.h"
#include "qxmaterial/materialtabs.h"
#include "qxmaterial/materialtextfield.h"
#include "qxmaterial/materialtimepicker.h"
#include "qxmaterial/materialtokens.h"
#include "qxmaterial/materialtogglebuttongroup.h"
#include "qxmaterial/materialtooltip.h"

#include <QApplication>
#include <QDate>
#include <QIcon>
#include <QPainter>
#include <QPushButton>
#include <QPixmap>
#include <QLabel>
#include <QHBoxLayout>
#include <QTime>
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

/*! A picture of one colour, for a grid that shows photographs: the gallery ships
 *  no images, and what a tile says about itself is its name rather than what it
 *  is a photograph of. */
QPixmap swatch(const QColor &color)
{
    QPixmap pixmap(48, 48);
    pixmap.fill(color);
    return pixmap;
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

    auto *button = addControl<MaterialButton>(QStringLiteral("materialButton"), QStringLiteral("MaterialButton"),
                                              new MaterialButton(this));
    button->setText(tr("Filled"));

    auto *check = addControl<MaterialCheckBox>(QStringLiteral("materialCheckBox"), QStringLiteral("MaterialCheckBox"),
                                               new MaterialCheckBox(this));
    check->setText(tr("Keep me posted"));
    check->setChecked(true);

    // Two radios side by side: a choice of one among others is the whole of what
    // a radio says, and one alone in a row says nothing.
    auto *radioHost = new QWidget(this);
    auto *radioLayout = new QHBoxLayout(radioHost);
    radioLayout->setContentsMargins(0, 0, 0, 0);
    radioLayout->setSpacing(24);
    auto *everyDay = new MaterialRadio(radioHost);
    everyDay->setText(tr("Day"));
    everyDay->setChecked(true);
    auto *everyWeek = new MaterialRadio(radioHost);
    everyWeek->setText(tr("Week"));
    radioLayout->addWidget(everyDay);
    radioLayout->addWidget(everyWeek);
    addControl<QWidget>(QStringLiteral("materialRadio"), QStringLiteral("MaterialRadio"), radioHost);

    auto *select = addControl<MaterialSelect>(QStringLiteral("materialSelect"), QStringLiteral("MaterialSelect"),
                                              new MaterialSelect(this));
    select->setItems(QStringList{tr("Day"), tr("Week"), tr("Month")});
    select->setCurrentIndex(1);

    auto *toggle = addControl<MaterialToggleButtonGroup>(QStringLiteral("materialToggleButtonGroup"),
                                                         QStringLiteral("MaterialToggleButtonGroup"),
                                                         new MaterialToggleButtonGroup(this));
    toggle->addButton(tr("Day"));
    toggle->addButton(tr("Week"));
    toggle->setChecked(0, true);

    auto *suggest = addControl<MaterialAutocomplete>(
        QStringLiteral("materialAutocomplete"), QStringLiteral("MaterialAutocomplete"), new MaterialAutocomplete(this));
    suggest->setOptions(QStringList{tr("Apple"), tr("Apricot"), tr("Banana")});

    auto *dayField = addControl<MaterialDatePicker>(QStringLiteral("materialDatePicker"),
                                                    QStringLiteral("MaterialDatePicker"), new MaterialDatePicker(this));
    dayField->setDate(QDate(2026, 10, 24));

    auto *timeField = addControl<MaterialTimePicker>(
        QStringLiteral("materialTimePicker"), QStringLiteral("MaterialTimePicker"), new MaterialTimePicker(this));
    timeField->setTime(QTime(9, 30));

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

    auto *avatar = addControl<MaterialAvatar>(QStringLiteral("materialAvatar"), QStringLiteral("MaterialAvatar"),
                                              new MaterialAvatar(this));
    avatar->setName(tr("Ada Lovelace"));

    auto *divider = addControl<MaterialDivider>(QStringLiteral("materialDivider"), QStringLiteral("MaterialDivider"),
                                                new MaterialDivider(this));
    divider->setFixedWidth(240);

    auto *accordion = addControl<MaterialAccordion>(QStringLiteral("materialAccordion"),
                                                    QStringLiteral("MaterialAccordion"), new MaterialAccordion(this));
    auto *accordionBody = new QLabel(tr("A body the header is the summary of."));
    accordionBody->setMinimumHeight(48);
    accordion->addSection(tr("What an accordion hides"), accordionBody);
    accordion->setSectionExpanded(0, true);

    auto *list = addControl<MaterialList>(QStringLiteral("materialList"), QStringLiteral("MaterialList"),
                                          new MaterialList(this));
    const QStringList listNames{tr("Album"), tr("Artist"), tr("Genre")};
    for (const QString &name : listNames) {
        auto *item = new MaterialListItem(list);
        item->setText(name);
        list->addItem(item);
    }

    auto *table = addControl<MaterialTable>(QStringLiteral("materialTable"), QStringLiteral("MaterialTable"),
                                            new MaterialTable(this));
    table->setHeaders(QStringList{tr("Name"), tr("Size")});
    table->addRow(QStringList{tr("Album"), QStringLiteral("12")});
    table->addRow(QStringList{tr("Single"), QStringLiteral("4")});
    table->setFixedWidth(320);

    // A grid of photographs, with the photographs drawn here: the gallery ships
    // no resources, and a tile says what it is by its name rather than by what
    // its picture is of.
    auto *images = addControl<MaterialImageList>(QStringLiteral("materialImageList"),
                                                 QStringLiteral("MaterialImageList"), new MaterialImageList(this));
    images->setColumns(2);
    images->setRowHeight(72);
    images->addItem(swatch(QColor(90, 140, 200)), tr("Sea"));
    images->addItem(swatch(QColor(200, 120, 90)), tr("Dunes"));
    images->setFixedWidth(320);

    addSection(tr("Feedback"));

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

    auto *progress = addControl<MaterialProgress>(QStringLiteral("materialProgress"),
                                                  QStringLiteral("MaterialProgress"), new MaterialProgress(this));
    progress->setMinimum(0);
    progress->setMaximum(100);
    progress->setValue(60);

    // A tooltip is a window that arrives over the page, so the row is the thing
    // it waits beside: hovering the button shows it, and a capture of the page
    // cannot hold a second window - the same bargain the dialog row makes.
    auto *tipAnchor = addControl<QPushButton>(QStringLiteral("materialTooltip"), QStringLiteral("MaterialTooltip"),
                                              new QPushButton(tr("Hover me"), this));
    auto *tooltip = new MaterialTooltip(this);
    tooltip->setAnchor(tipAnchor);
    tooltip->setText(tr("A tooltip says a word about the control it waits beside."));

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

    auto *dial = addControl<MaterialSpeedDial>(QStringLiteral("materialSpeedDial"), QStringLiteral("MaterialSpeedDial"),
                                               new MaterialSpeedDial(this));
    dial->addAction(tr("Upload a file"), plusIcon());
    dial->addAction(tr("Note"), plusIcon());

    auto *menuButton = addControl<QPushButton>(QStringLiteral("materialMenu"), QStringLiteral("MaterialMenu"),
                                               new QPushButton(tr("Open the menu"), this));
    auto *menu = new MaterialMenu(menuButton);
    menu->addAction(tr("Rename"));
    menu->addCheckable(tr("Show hidden"), true);
    menu->addSeparator();
    menu->addAction(tr("Delete"));
    connect(menuButton, &QPushButton::clicked, menu, &MaterialMenu::open);

    auto *bottomBar = addControl<MaterialBottomNavigation>(QStringLiteral("materialBottomNavigation"),
                                                           QStringLiteral("MaterialBottomNavigation"),
                                                           new MaterialBottomNavigation(this));
    bottomBar->addDestination(tr("Home"));
    bottomBar->addDestination(tr("Search"));
    bottomBar->addDestination(tr("Profile"));
    bottomBar->setCurrentIndex(0);

    // A drawer takes its room from what it is on, or covers it, so the row is a
    // host of a known size rather than the page it would otherwise cover.
    auto *drawerHost = new QWidget(this);
    drawerHost->setFixedSize(320, 220);
    auto *drawer = new MaterialDrawer(drawerHost);
    drawer->setVariant(MaterialDrawer::Variant::Persistent);
    drawer->addDestination(tr("Inbox"));
    drawer->addDestination(tr("Starred"));
    drawer->addDestination(tr("Sent"));
    addControl<QWidget>(QStringLiteral("materialDrawer"), QStringLiteral("MaterialDrawer"), drawerHost);

    auto *pages = addControl<MaterialPagination>(QStringLiteral("materialPagination"),
                                                 QStringLiteral("MaterialPagination"), new MaterialPagination(this));
    pages->setCount(10);
    pages->setPage(3);

    auto *crumbs = addControl<MaterialBreadcrumbs>(
        QStringLiteral("materialBreadcrumbs"), QStringLiteral("MaterialBreadcrumbs"), new MaterialBreadcrumbs(this));
    crumbs->setItems(QStringList{tr("Home"), tr("Library"), tr("Album")});

    auto *steps = addControl<MaterialStepper>(QStringLiteral("materialStepper"), QStringLiteral("MaterialStepper"),
                                              new MaterialStepper(this));
    steps->addStep(tr("Cart"), tr("One album"));
    steps->addStep(tr("Pay"), tr("Card"));
    steps->addStep(tr("Done"), tr("Shipped"));
    steps->setCurrentStep(1);

    auto *tabs = addControl<MaterialTabs>(QStringLiteral("materialTabs"), QStringLiteral("MaterialTabs"),
                                          new MaterialTabs(this));
    tabs->addTab(tr("Album"));
    tabs->addTab(tr("Artist"));
    tabs->addTab(tr("Genre"));
    tabs->setCurrentIndex(0);

    addControl<RippleSurface>(QStringLiteral("materialRipple"), QStringLiteral("MaterialRipple"),
                              new RippleSurface(this));
}
